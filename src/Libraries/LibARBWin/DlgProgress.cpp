/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Progress dialog
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2017-12-19 Tweaked default width a little.
 * 2014-12-31 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-11 Ported to wxWidgets.
 * 2008-09-05 Put progress dialog into separate UI thread
 * 2004-10-01 Created
 */

#include "stdafx.h"
#include "LibARBWin/DlgProgress.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/TaskbarProgress.h"
#include <wx/thread.h>
#include <wx/utils.h>
#include <vector>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

namespace
{
constexpr int _defWidth = 250;
} // namespace

/////////////////////////////////////////////////////////////////////////////
// I used the wxProgressDialog as a guide

class CDlgProgress
	: public wxDialog
	, public IDlgProgress
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgProgress)
public:
	CDlgProgress(short nBars, wxWindow* parent);
	~CDlgProgress();

	bool Show(bool show = true) override;

	void SetCaption(std::wstring const& inCaption) override;
	void SetMessage(std::wstring const& inMessage) override;
	void SetRange(short inBar, int inRange) override;
	void SetStep(short inBar, int inStep) override;
	void StepIt(short inBar) override;
	void OffsetPos(short inBar, int inDelta) override;
	void SetPos(short inBar, int inPos) override;
	int GetPos(short inBar) override;
	bool EnableCancel(bool bEnable = true) override;
	bool HasCanceled() const override;
	void ShowProgress(bool bShow = true) override;
	void SetForegroundWindow() override;
	void Dismiss() override;

private:
	struct GaugeData
	{
		GaugeData()
			: gauge(nullptr)
			, step(1)
			, pos(0)
		{
		}
		GaugeData(wxGauge* inGauge)
			: gauge(inGauge)
			, step(1)
			, pos(0)
		{
		}
		wxGauge* gauge;
		int step;
		int pos;
	};
	wxGauge* GetBar(short inBar)
	{
		if (1 <= inBar && static_cast<size_t>(inBar) <= m_ctrlBars.size())
			return m_ctrlBars[inBar - 1].gauge;
		return nullptr;
	}
	void ReenableOtherWindows();

	mutable wxCriticalSection m_crit;
	wxWindow* m_parentTop;
	CTaskbarProgressPtr m_pTaskbar;
	wxStaticText* m_ctrlMessage;
	std::vector<GaugeData> m_ctrlBars;
	wxButton* m_ctrlCancel;
	wxWindowDisabler* m_winDisabler;
	bool m_HasCanceled;

	void OnCancel(wxCommandEvent& evt);
};


CDlgProgress::CDlgProgress(short nBars, wxWindow* parent)
	: wxDialog()
	, m_parentTop(wxGetTopLevelParent(parent))
	, m_pTaskbar(nullptr)
	, m_ctrlMessage(nullptr)
	, m_ctrlBars()
	, m_ctrlCancel(nullptr)
	, m_winDisabler(nullptr)
	, m_HasCanceled(false)
{
	if (1 > nBars)
		nBars = 1;
	SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);
	Create(parent, wxID_ANY, _("Progress"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	WXWidget handle = m_parentTop ? m_parentTop->GetHandle() : GetHandle();
	if (wxTheApp && wxTheApp->GetTopWindow())
		handle = wxTheApp->GetTopWindow()->GetHandle();
	m_pTaskbar = CTaskbarProgress::Get(handle);

	// Controls (these are done first to control tab order)

	m_ctrlMessage = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlMessage->Wrap(-1);

	m_ctrlBars.push_back(GaugeData(new wxGauge(
		this,
		wxID_ANY,
		10,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(_defWidth, -1)),
		wxGA_HORIZONTAL | wxGA_SMOOTH)));
	for (int nBar = 1; nBar < nBars; ++nBar)
		m_ctrlBars.push_back(GaugeData(new wxGauge(
			this,
			wxID_ANY,
			10,
			wxDefaultPosition,
			wxDLG_UNIT(this, wxSize(_defWidth, -1)),
			wxGA_HORIZONTAL | wxGA_SMOOTH)));

	// Sizers

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlMessage, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	for (size_t i = 0; i < m_ctrlBars.size(); ++i)
		bSizer->Add(m_ctrlBars[i].gauge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));
	m_ctrlCancel = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_CANCEL), wxButton);
	m_ctrlCancel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgProgress::OnCancel, this);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	// if (appmodal)
	m_winDisabler = new wxWindowDisabler(this);
	// else
	//{
	//	if (m_parentTop)
	//		m_parentTop->Disable();
	//	m_winDisabler = nullptr;
	//}
	Show();
	Enable();
}


CDlgProgress::~CDlgProgress()
{
	ReenableOtherWindows();
}


bool CDlgProgress::Show(bool show)
{
	if (!show)
		ReenableOtherWindows();
	else if (m_pTaskbar)
		m_pTaskbar->SetProgressState(TBPF_NORMAL);
	return wxDialog::Show(show);
}


void CDlgProgress::ReenableOtherWindows()
{
	if (m_pTaskbar)
		m_pTaskbar->SetProgressState(TBPF_NOPROGRESS);
	// if (appmodal)
	{
		delete m_winDisabler;
		m_winDisabler = nullptr;
	}
	// else
	//{
	//	if (m_parentTop)
	//		m_parentTop->Enable();
	//}
}


void CDlgProgress::OnCancel(wxCommandEvent& evt)
{
	{
		wxCriticalSectionLocker lock(m_crit);
		m_HasCanceled = true;
	}
	m_ctrlCancel->Enable(false);
}

// IDlgProgress interface

void CDlgProgress::SetCaption(std::wstring const& inCaption)
{
	assert(wxIsMainThread());
	SetLabel(StringUtil::stringWX(inCaption));
}


void CDlgProgress::SetMessage(std::wstring const& inMessage)
{
	assert(wxIsMainThread());
	wxString msg(StringUtil::stringWX(inMessage));
	if (!inMessage.empty() && msg != m_ctrlMessage->GetLabel())
	{
		m_ctrlMessage->SetLabel(msg);
		// TODO: This needs to be reworked with threads. The download should happen
		// on a worker thread that messages back here. Stnce the GUI would no longer
		// be locked by a busy process, we would no longer need this kludge.
		wxYieldIfNeeded();
	}
}


void CDlgProgress::SetRange(short inBar, int inRange)
{
	assert(wxIsMainThread());
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->SetRange(inRange);
		Update();
		wxYieldIfNeeded(); // See nasty comment above
	}
}


void CDlgProgress::SetStep(short inBar, int inStep)
{
	assert(wxIsMainThread());
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar - 1].step = inStep;
	}
}


void CDlgProgress::StepIt(short inBar)
{
	assert(wxIsMainThread());
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar - 1].pos += m_ctrlBars[inBar - 1].step;
		if (m_pTaskbar)
			m_pTaskbar->SetProgressValue(m_ctrlBars[inBar - 1].pos, m_ctrlBars[inBar - 1].gauge->GetRange());
		pBar->SetValue(m_ctrlBars[inBar - 1].pos);
		Update();
		wxYieldIfNeeded(); // See nasty comment above
	}
}


void CDlgProgress::OffsetPos(short inBar, int inDelta)
{
	assert(wxIsMainThread());
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar - 1].pos += inDelta;
		if (m_pTaskbar)
			m_pTaskbar->SetProgressValue(m_ctrlBars[inBar - 1].pos, m_ctrlBars[inBar - 1].gauge->GetRange());
		pBar->SetValue(m_ctrlBars[inBar - 1].pos);
		Update();
		wxYieldIfNeeded(); // See nasty comment above
	}
}


void CDlgProgress::SetPos(short inBar, int inPos)
{
	assert(wxIsMainThread());
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar - 1].pos = inPos;
		if (m_pTaskbar)
			m_pTaskbar->SetProgressValue(m_ctrlBars[inBar - 1].pos, m_ctrlBars[inBar - 1].gauge->GetRange());
		pBar->SetValue(m_ctrlBars[inBar - 1].pos);
		Update();
		wxYieldIfNeeded(); // See nasty comment above
	}
}


int CDlgProgress::GetPos(short inBar)
{
	assert(wxIsMainThread());
	int pos = 0;
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		pos = pBar->GetValue();
		m_ctrlBars[inBar - 1].pos = pos;
	}
	return pos;
}


bool CDlgProgress::EnableCancel(bool bEnable)
{
	assert(wxIsMainThread());
	bool hasCanceled = false;
	{
		wxCriticalSectionLocker lock(m_crit);
		hasCanceled = m_HasCanceled;
	}
	bool bCancelEnabled = m_ctrlCancel ? m_ctrlCancel->IsEnabled() : false;
	if (!hasCanceled)
	{
		if (m_ctrlCancel)
			m_ctrlCancel->Enable(bEnable);
		Update();
	}
	return bCancelEnabled;
}


bool CDlgProgress::HasCanceled() const
{
	wxCriticalSectionLocker lock(m_crit);
	return m_HasCanceled;
}


void CDlgProgress::ShowProgress(bool bShow)
{
	assert(wxIsMainThread());
	Show(bShow);
}


void CDlgProgress::SetForegroundWindow()
{
	assert(wxIsMainThread());
	SetFocus();
	Update();
}


void CDlgProgress::Dismiss()
{
	assert(wxIsMainThread());
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////

IDlgProgress* IDlgProgress::CreateProgress(short nBars, wxWindow* parent)
{
	CDlgProgress* pDlg = new CDlgProgress(nBars, parent);
	return pDlg;
}
