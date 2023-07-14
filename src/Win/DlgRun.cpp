/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2019-09-17 Fix last-entered on hidden fields. Fix run's club on creation.
 * 2019-08-18 Fix Bonus points (couldn't enter double).
 *            Auto-select event when there's only one.
 * 2018-12-16 Convert to fmt.
 * 2017-11-21 Update title points when InClass changes.
 * 2015-11-01 Compute score for NA runs also.
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-05-19 Make last div/level/height/handler dog-aware.
 * 2012-12-29 Fix pasting metafiles.
 * 2012-11-17 Allow judge to be empty.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Fix initial focus.
 * 2012-01-03 Fix field updating (some fields didn't have validator
 *            associated variables tied to them)
 * 2012-01-02 Change validator to support default value on empty field.
 * 2012-01-01 Add validation dialogs, page change vetoing.
 * 2011-12-30 Fixed CGenericValidator.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-02-12 Add DnD support for linked files.
 * 2010-01-02 Fix setting of required points with level changes.
 * 2009-10-18 Fix prepending of '0' to title points.
 * 2009-10-14 Add dog's name to dialog caption.
 * 2009-10-14 Fix initialization of opening pts in eScoreThenTime.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fixed division/level initialization.
 * 2009-03-16 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2003-10-13 Make ref run dlg default to perfect score.
 */

#include "stdafx.h"
#include "DlgRun.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "DlgRunPanelCRCD.h"
#include "DlgRunPanelComments.h"
#include "DlgRunPanelLinks.h"
#include "DlgRunPanelRefRuns.h"
#include "DlgRunPanelScore.h"

#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

wxBEGIN_EVENT_TABLE(CDlgRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgRun::OnOk)
wxEND_EVENT_TABLE()


CDlgRun::CDlgRun(
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* pParent,
	int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pTrial(inTrial)
	, m_pRealRun(inRun)
	, m_Run(inRun->Clone())
	, m_panelScore(nullptr)
	, m_panels()
	, m_trace("CDlgRun")
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		StringUtil::stringWX(pDoc->AddDogToCaption(StringUtil::stringW(_("IDS_RUN_PROPERTIES")))),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, &CDlgRun::OnPageChanging, this);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, &CDlgRun::OnPageChanged, this);

	// Score

	m_panelScore = new CDlgRunPanelScore(this, pDoc, inDog, m_pTrial, m_Run, notebook);
	notebook->AddPage(m_panelScore, _("IDD_RUN_SCORE"), false);
	m_panels.push_back(m_panelScore);

	// Comments

	auto panelComments = new CDlgRunPanelComments(this, pDoc, inDog, m_pTrial, m_Run, notebook);
	notebook->AddPage(panelComments, _("IDD_RUN_COMMENTS"), false);
	m_panels.push_back(panelComments);

	// RefRuns

	auto panelRefRuns = new CDlgRunPanelRefRuns(this, pDoc, inDog, m_pTrial, m_Run, notebook);
	notebook->AddPage(panelRefRuns, _("IDD_RUN_REFERENCE"), false);
	m_panels.push_back(panelRefRuns);

	// CRCD

	auto panelCRCD = new CDlgRunPanelCRCD(this, pDoc, inDog, m_pTrial, m_Run, notebook);
	notebook->AddPage(panelCRCD, _("IDD_RUN_CRCD"), false);
	m_panels.push_back(panelCRCD);

	// Links

	auto panelLinks = new CDlgRunPanelLinks(this, pDoc, inDog, m_pTrial, m_Run, notebook);
	notebook->AddPage(panelLinks, _("IDD_RUN_LINK"), false);
	m_panels.push_back(panelLinks);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(notebook, 1, wxEXPAND | wxALL, padding.ButtonSizer());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	std::vector<wxWindow*> ctrls;
	for (auto panel : m_panels)
		ctrls.push_back(panel->GetInitialControl());
	IMPLEMENT_ON_INIT(CDlgRun, ctrls[iSelectPage])

	notebook->ChangeSelection(iSelectPage);
}


std::wstring CDlgRun::GetCurrentVenueName() const
{
	return m_panelScore->GetCurrentVenueName();
}


void CDlgRun::OnPageChanging(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
	{
		if (!Validate() || !TransferDataFromWindow())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


void CDlgRun::OnPageChanged(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
		TransferDataFromWindow();
	auto sel = evt.GetSelection();
	if (0 <= sel && sel < static_cast<int>(m_panels.size()))
	{
		m_panels[sel]->OnActivation();
	}
	evt.Skip();
}


void CDlgRun::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	for (auto panel : m_panels)
		if (!panel->Validate())
			return;

	for (auto panel : m_panels)
		panel->Save();

	*m_pRealRun = *m_Run;
	m_pTrial->SetMultiQs(m_pDoc->Book().GetConfig()); // Note, when adding a new run, this is actually too soon to call
													  // - the run isn't in the trial yet

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}

} // namespace dconSoft
