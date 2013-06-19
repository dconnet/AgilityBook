/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Progress dialog
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-09-05 DRC Put progress dialog into separate UI thread
 * @li 2004-10-01 DRC Created
 */

#include "stdafx.h"
#include "DlgProgress.h"

#include "Globals.h"

#include "ARBCommon/StringUtil.h"
#include <vector>
#include <wx/utils.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// I used the wxProgressDialog as a guide

class CDlgProgress : public wxDialog, public IDlgProgress
{
public:
	CDlgProgress(short nBars, wxWindow* parent);
	virtual ~CDlgProgress();

	virtual bool Show(bool show = true);

	virtual void SetCaption(std::wstring const& inCaption);
	virtual void SetMessage(std::wstring const& inMessage);
	virtual void SetRange(
			short inBar,
			int inRange);
	virtual void SetStep(
			short inBar,
			int inStep);
	virtual void StepIt(short inBar);
	virtual void OffsetPos(
			short inBar,
			int inDelta);
	virtual void SetPos(
			short inBar,
			int inPos);
	virtual int GetPos(short inBar);
	virtual bool EnableCancel(bool bEnable = true);
	virtual bool HasCanceled() const;
	virtual void ShowProgress(bool bShow = true);
	virtual void SetForegroundWindow();
	virtual void Dismiss();

private:
	struct GaugeData
	{
		GaugeData() : gauge(NULL), step(1), pos(0) {}
		GaugeData(wxGauge* inGauge) : gauge(inGauge), step(1), pos(0) {}
		wxGauge* gauge;
		int step;
		int pos;
	};
	wxGauge* GetBar(short inBar)
	{
		if (1 <= inBar && static_cast<size_t>(inBar) <= m_ctrlBars.size())
			return m_ctrlBars[inBar-1].gauge;
		return NULL;
	}
	void ReenableOtherWindows();

	wxWindow* m_parentTop;
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
	, m_ctrlMessage(NULL)
	, m_ctrlBars()
	, m_ctrlCancel(NULL)
	, m_winDisabler(NULL)
	, m_HasCanceled(false)
{
	if (1 > nBars)
		nBars = 1;
	SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);
	Create(parent, wxID_ANY, _("Progress"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlMessage = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlMessage->Wrap(-1);

	m_ctrlBars.push_back(GaugeData(new wxGauge(this, wxID_ANY, 10, wxDefaultPosition, wxSize(400 ,-1), wxGA_HORIZONTAL|wxGA_SMOOTH)));
	for (int nBar = 1; nBar < nBars; ++nBar)
		m_ctrlBars.push_back(GaugeData(new wxGauge(this, wxID_ANY, 10, wxDefaultPosition, wxSize(400 ,-1), wxGA_HORIZONTAL|wxGA_SMOOTH)));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlMessage, 0, wxALL|wxEXPAND, 5);
	for (size_t i = 0; i < m_ctrlBars.size(); ++i)
		bSizer->Add(m_ctrlBars[i].gauge, 0, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);
	m_ctrlCancel = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_CANCEL), wxButton);
	BIND_OR_CONNECT_CTRL(m_ctrlCancel, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgProgress::OnCancel);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	//if (appmodal)
		m_winDisabler = new wxWindowDisabler(this);
	//else
	//{
	//	if (m_parentTop)
	//		m_parentTop->Disable();
	//	m_winDisabler = NULL;
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
	return wxDialog::Show(show);
}


void CDlgProgress::ReenableOtherWindows()
{
	//if (appmodal)
	{
		delete m_winDisabler;
		m_winDisabler = NULL;
	}
	//else
	//{
	//	if (m_parentTop)
	//		m_parentTop->Enable();
	//}
}


void CDlgProgress::OnCancel(wxCommandEvent& evt)
{
	m_HasCanceled = true;
	m_ctrlCancel->Enable(false);
}


void CDlgProgress::SetCaption(std::wstring const& inCaption)
{
	SetLabel(StringUtil::stringWX(inCaption));
}


void CDlgProgress::SetMessage(std::wstring const& inMessage)
{
	wxString msg(StringUtil::stringWX(inMessage));
	if (!inMessage.empty() && msg != m_ctrlMessage->GetLabel())
	{
		m_ctrlMessage->SetLabel(msg);
		wxYieldIfNeeded();
	}
}


void CDlgProgress::SetRange(short inBar, int inRange)
{
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->SetRange(inRange);
		Update();
	}
}


void CDlgProgress::SetStep(short inBar, int inStep)
{
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar-1].step = inStep;
	}
}


void CDlgProgress::StepIt(short inBar)
{
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar-1].pos += m_ctrlBars[inBar-1].step;
		pBar->SetValue(m_ctrlBars[inBar-1].pos);
		Update();
	}
}


void CDlgProgress::OffsetPos(short inBar, int inDelta)
{
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar-1].pos += inDelta;
		pBar->SetValue(m_ctrlBars[inBar-1].pos);
		Update();
	}
}


void CDlgProgress::SetPos(short inBar, int inPos)
{
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		m_ctrlBars[inBar-1].pos = inPos;
		pBar->SetValue(m_ctrlBars[inBar-1].pos);
		Update();
	}
}


int CDlgProgress::GetPos(short inBar)
{
	int pos = 0;
	wxGauge* pBar = GetBar(inBar);
	if (pBar)
	{
		pos = pBar->GetValue();
		m_ctrlBars[inBar-1].pos = pos;
	}
	return pos;
}


bool CDlgProgress::EnableCancel(bool bEnable)
{
	bool bCancelEnabled = m_ctrlCancel ? m_ctrlCancel->IsEnabled() : false;
	if (!m_HasCanceled)
	{
		m_ctrlCancel->Enable(bEnable);
		Update();
	}
	return bCancelEnabled;
}


bool CDlgProgress::HasCanceled() const
{
	return m_HasCanceled;
}


void CDlgProgress::ShowProgress(bool bShow)
{
	Show(bShow);
}


void CDlgProgress::SetForegroundWindow()
{
	SetFocus();
	Update();
}


void CDlgProgress::Dismiss()
{
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////

IDlgProgress* IDlgProgress::CreateProgress(short nBars, wxWindow* parent)
{
	CDlgProgress* pDlg = new CDlgProgress(nBars, parent);
	return pDlg;
}


IDlgProgress::~IDlgProgress()
{
}
