/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 */

#include "stdafx.h"
#include "DlgRun.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBDogRun.h"
#include "ComboBoxes.h"
#include "NoteButton.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>


BEGIN_EVENT_TABLE(CDlgRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgRun::OnOk)
END_EVENT_TABLE()


CDlgRun::CDlgRun(
		CAgilityBookDoc* pDoc,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pTrial(pTrial)
	, m_pRealRun(pRun)
	, m_Run(pRun->Clone())
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	Create(pParent, wxID_ANY, _("IDS_RUN_PROPERTIES"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	ARBDogClubPtr pClub;
	pTrial->GetClubs().GetPrimaryClub(&pClub);
	assert(NULL != pClub.get());
	ARBConfigVenuePtr pVenue;
	pDoc->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue);
	assert(NULL != pVenue.get());

	//m_pageScore = new CDlgRunScore(pDoc, pVenue, pTrial, m_pRealRun, m_Run);
	//m_pageComments = new CDlgRunComments(pDoc, m_Run);
	//m_pageReference = new CDlgRunReference(pDoc, pVenue, m_Run);
	//m_pageCRCD = new CDlgRunCRCD(m_Run);
	//m_pageLink = new CDlgRunLink(pDoc, m_Run);

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, 0);

	// Score

	wxPanel* panelScore = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textDate = new wxStaticText(panelScore, wxID_ANY,
		wxT("Date"), wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	wxDatePickerCtrl* ctrlDate = new wxDatePickerCtrl(panelScore, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	ctrlDate->SetHelpText(_("HIDC_RUNSCORE_DATE"));
	ctrlDate->SetToolTip(_("HIDC_RUNSCORE_DATE"));

	wxStaticText* textVenue = new wxStaticText(panelScore, wxID_ANY,
		pVenue->GetName().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textVenue->Wrap(-1);

	wxStaticText* textClub = new wxStaticText(panelScore, wxID_ANY,
		pClub->GetName().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textClub->Wrap(-1);

	wxStaticText* textLocation = new wxStaticText(panelScore, wxID_ANY,
		pTrial->GetLocation().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textLocation->Wrap(-1);

	wxStaticText* textDiv = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DIVISION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	wxComboBox* ctrlDivision = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	ctrlDivision->SetHelpText(_("HIDC_RUNSCORE_DIVISION"));
	ctrlDivision->SetToolTip(_("HIDC_RUNSCORE_DIVISION"));

	wxStaticText* textLevel = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_LEVEL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	wxComboBox* m_ctrlLevel = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlLevel->SetHelpText(_("HIDC_RUNSCORE_LEVEL"));
	m_ctrlLevel->SetToolTip(_("HIDC_RUNSCORE_LEVEL"));

	wxStaticText* textEvent = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	wxComboBox* m_ctrlEvent = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlEvent->SetHelpText(_("HIDC_RUNSCORE_EVENT"));
	m_ctrlEvent->SetToolTip(_("HIDC_RUNSCORE_EVENT"));

	wxStaticText* m_textSubName = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SUBNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textSubName->Wrap(-1);

	wxComboBox* m_ctrlSubName = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlSubName->SetHelpText(_("HIDC_RUNSCORE_SUBNAME"));
	m_ctrlSubName->SetToolTip(_("HIDC_RUNSCORE_SUBNAME"));

	wxCheckBox* ctrlTable = new wxCheckBox(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	ctrlTable->SetHelpText(_("HIDC_RUNSCORE_TABLE"));
	ctrlTable->SetToolTip(_("HIDC_RUNSCORE_TABLE"));

	wxStaticText* textHeight = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);

	wxComboBox* ctrlHeight = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(50, -1),
		0, NULL, wxCB_DROPDOWN);
	ctrlHeight->SetHelpText(_("HIDC_RUNSCORE_HEIGHT"));
	ctrlHeight->SetToolTip(_("HIDC_RUNSCORE_HEIGHT"));

	wxStaticText* textJudge = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_JUDGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textJudge->Wrap(-1);

	wxComboBox* ctrlJudge = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	ctrlJudge->SetHelpText(_("HIDC_RUNSCORE_JUDGE"));
	ctrlJudge->SetToolTip(_("HIDC_RUNSCORE_JUDGE"));

	CNoteButton* ctrlJudgeNote = new CNoteButton(panelScore);
	ctrlJudgeNote->SetHelpText(_("HIDC_RUNSCORE_JUDGE_NOTES"));
	ctrlJudgeNote->SetToolTip(_("HIDC_RUNSCORE_JUDGE_NOTES"));

	wxStaticText* textHandler = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HANDLER"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	wxComboBox* ctrlHandler = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	ctrlHandler->SetHelpText(_("HIDC_RUNSCORE_HANDLER"));
	ctrlHandler->SetToolTip(_("HIDC_RUNSCORE_HANDLER"));

	wxStaticText* textConditions = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CONDITIONS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textConditions->Wrap(-1);

	wxTextCtrl* ctrlConditions = new wxTextCtrl(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(-1, 50),
		wxTE_MULTILINE|wxTE_WORDWRAP);
	ctrlConditions->SetHelpText(_("HIDC_RUNSCORE_CONDITIONS"));
	ctrlConditions->SetToolTip(_("HIDC_RUNSCORE_CONDITIONS"));

	wxTextCtrl* m_ctrlNote = new wxTextCtrl(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(-1, 70),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP);
	m_ctrlNote->SetHelpText(_("HIDC_RUNSCORE_DESC"));
	m_ctrlNote->SetToolTip(_("HIDC_RUNSCORE_DESC"));

	wxButton* m_btnPartner = new wxButton(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PARTNERS_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNERS_EDIT"));
	m_btnPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNERS_EDIT"));

	wxTextCtrl* m_ctrlPartner = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(200, -1), wxTE_READONLY);
	m_ctrlPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNER"));
	m_ctrlPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNER"));

	wxStaticText* m_textSCT = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textSCT->Wrap(-1);

	wxTextCtrl* m_ctrlSCT = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlSCT->SetHelpText(_("HIDC_RUNSCORE_SCT"));
	m_ctrlSCT->SetToolTip(_("HIDC_RUNSCORE_SCT"));

	wxStaticText* textTime = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TIME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTime->Wrap(-1);

	wxTextCtrl* ctrlTime = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlTime->SetHelpText(_("HIDC_RUNSCORE_TIME"));
	ctrlTime->SetToolTip(_("HIDC_RUNSCORE_TIME"));

	wxStaticText* textPlace = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PLACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPlace->Wrap(-1);

	wxTextCtrl* ctrlPlace = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(30, -1), 0);
	ctrlPlace->SetHelpText(_("HIDC_RUNSCORE_PLACE"));
	ctrlPlace->SetToolTip(_("HIDC_RUNSCORE_PLACE"));

	wxStaticText* textPlaceOf = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_IN_CLASS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPlaceOf->Wrap(-1);

	wxTextCtrl* ctrlPlaceTotal = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(30, -1), 0);
	ctrlPlaceTotal->SetHelpText(_("HIDC_RUNSCORE_IN_CLASS"));
	ctrlPlaceTotal->SetToolTip(_("HIDC_RUNSCORE_IN_CLASS"));

	wxStaticText* m_textBonus = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_BONUSPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textBonus->Wrap(-1);

	wxTextCtrl* m_ctrlBonus = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlBonus->SetHelpText(_("HIDC_RUNSCORE_BONUSPTS"));
	m_ctrlBonus->SetToolTip(_("HIDC_RUNSCORE_BONUSPTS"));

	wxStaticText* m_textYardsReqOpeningPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPENING_PTS"),
		//_("IDC_RUNSCORE_YARDS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYardsReqOpeningPts->Wrap(-1);

	wxTextCtrl* m_ctrlYardsReqOpeningPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
	m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
	//m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
	//m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));

	wxStaticText* textFaults = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textFaults->Wrap(-1);

	wxTextCtrl* ctrlFaults = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlFaults->SetHelpText(_("HIDC_RUNSCORE_FAULTS"));
	ctrlFaults->SetToolTip(_("HIDC_RUNSCORE_FAULTS"));

	wxStaticText* textDogsQd = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DOGS_QD"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDogsQd->Wrap(-1);

	wxTextCtrl* ctrlDogsQd = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlDogsQd->SetHelpText(_("HIDC_RUNSCORE_DOGS_QD"));
	ctrlDogsQd->SetToolTip(_("HIDC_RUNSCORE_DOGS_QD"));

	wxStaticText* m_textSpeed = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SPEEDPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textSpeed->Wrap(-1);

	wxStaticText* m_ctrlSpeed = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* m_textMinYPSClosingTime = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_MIN_YPS"),
		//_("IDC_RUNSCORE_SCT2"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textMinYPSClosingTime->Wrap(-1);

	wxTextCtrl* m_ctrlMinYPSClosingTime = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
	m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));

	wxStaticText* m_textYPSOpeningPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPEN_PTS"),
		//_("IDC_RUNSCORE_YPS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYPSOpeningPts->Wrap(-1);

	wxTextCtrl* m_ctrlYPSOpeningPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
	m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));

	wxStaticText* textQ = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	CQualifyingComboBox* ctrlQ = new CQualifyingComboBox(panelScore, m_Run);
	ctrlQ->SetHelpText(_("HIDC_RUNSCORE_Q"));
	ctrlQ->SetToolTip(_("HIDC_RUNSCORE_Q"));

	wxStaticText* textTitlePts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TITLE_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTitlePts->Wrap(-1);

	wxStaticText* ctrlTitlePts = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* m_textReqClosingPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSING_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textReqClosingPts->Wrap(-1);

	wxTextCtrl* m_ctrlReqClosingPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlReqClosingPts->SetHelpText(_("HIDC_RUNSCORE_CLOSING_PTS"));
	m_ctrlReqClosingPts->SetToolTip(_("HIDC_RUNSCORE_CLOSING_PTS"));

	wxStaticText* m_textClosingPtsTotalFaults = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSE_PTS"),
		//_("IDC_RUNSCORE_TOTAL_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textClosingPtsTotalFaults->Wrap(-1);

	wxTextCtrl* m_ctrlClosingPtsTotalFaults = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
	m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));

	wxStaticText* textScore = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCORE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	wxStaticText* ctrlScore = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* textNumObs = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNumObs->Wrap(-1);

	wxTextCtrl* ctrlNumObs = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlNumObs->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES"));
	ctrlNumObs->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES"));

	wxStaticText* textObsSec = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES_PER_SEC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textObsSec->Wrap(-1);

	wxStaticText* m_ctrlObsSec = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlObsSec->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	m_ctrlObsSec->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));

	wxButton* btnOtherPoints = new wxButton(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OTHERPOINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnOtherPoints->SetHelpText(_("HIDC_RUNSCORE_OTHERPOINTS"));
	btnOtherPoints->SetToolTip(_("HIDC_RUNSCORE_OTHERPOINTS"));

	// Comments

	wxPanel* panelComments = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxTextCtrl* ctrlComments = new wxTextCtrl(panelComments, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
	ctrlComments->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS"));
	ctrlComments->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS"));

	wxListBox* ctrlFaultsList = new wxListBox(panelComments, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	ctrlFaultsList->SetHelpText(_("HIDC_RUNCOMMENT_FAULTS_LIST"));
	ctrlFaultsList->SetToolTip(_("HIDC_RUNCOMMENT_FAULTS_LIST"));

	wxButton* btnFaults = new wxButton(panelComments, wxID_ANY,
		_("IDC_RUNCOMMENT_COMMENTS_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnFaults->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));
	btnFaults->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));

	// RefRuns

	wxPanel* panelRefRuns = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxListCtrl* ctrlRefRuns = new wxListCtrl(panelRefRuns, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	ctrlRefRuns->SetHelpText(_("HIDC_RUNREF_REF_RUNS"));
	ctrlRefRuns->SetToolTip(_("HIDC_RUNREF_REF_RUNS"));

	wxButton* btnRefNew = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRefNew->SetHelpText(_("HIDC_RUNREF_NEW"));
	btnRefNew->SetToolTip(_("HIDC_RUNREF_NEW"));

	wxButton* btnRefAddMe = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_ADDDOG"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRefAddMe->SetHelpText(_("HIDC_RUNREF_ADDDOG"));
	btnRefAddMe->SetToolTip(_("HIDC_RUNREF_ADDDOG"));

	wxButton* btnRefEdit = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRefEdit->SetHelpText(_("HIDC_RUNREF_EDIT"));
	btnRefEdit->SetToolTip(_("HIDC_RUNREF_EDIT"));

	wxButton* btnRefDelete = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRefDelete->SetHelpText(_("HIDC_RUNREF_DELETE"));
	btnRefDelete->SetToolTip(_("HIDC_RUNREF_DELETE"));

	// CRCD

	wxPanel* panelCRCD = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textCRCD = new wxStaticText(panelCRCD, wxID_ANY,
		_("IDC_RUN_CRCD"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCRCD->Wrap(500);

	wxStaticText* textDisplay = new wxStaticText(panelCRCD, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textDisplay->Wrap(-1);

	wxButton* btnCourse = new wxButton(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnCourse->SetHelpText(_("HIDC_RUNCRCD_EDIT"));
	btnCourse->SetToolTip(_("HIDC_RUNCRCD_EDIT"));

	wxButton* btnView = new wxButton(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_VIEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnView->SetHelpText(_("HIDC_RUNCRCD_VIEW"));
	btnView->SetToolTip(_("HIDC_RUNCRCD_VIEW"));

	wxButton* btnCopyCRCD = new wxButton(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnCopyCRCD->SetHelpText(_("HIDC_RUNCRCD_COPY"));
	btnCopyCRCD->SetToolTip(_("HIDC_RUNCRCD_COPY"));

	wxCheckBox* ctrlIncImage = new wxCheckBox(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_IMAGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	ctrlIncImage->SetHelpText(_("HIDC_RUNCRCD_IMAGE"));
	ctrlIncImage->SetToolTip(_("HIDC_RUNCRCD_IMAGE"));

	wxStaticText* textImageDesc = new wxStaticText(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_IMAGE_TEXT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textImageDesc->Wrap(80);

	// Links

	wxPanel* panelLinks = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxListBox* ctrlLinks = new wxListBox(panelLinks, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	ctrlLinks->SetHelpText(_("HIDC_RUNLINK_LIST"));
	ctrlLinks->SetToolTip(_("HIDC_RUNLINK_LIST"));

	wxButton* btnLinkNew = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkNew->SetHelpText(_("HIDC_RUNLINK_NEW"));
	btnLinkNew->SetToolTip(_("HIDC_RUNLINK_NEW"));

	wxButton* btnLinkEdit = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkEdit->SetHelpText(_("HIDC_RUNLINK_EDIT"));
	btnLinkEdit->SetToolTip(_("HIDC_RUNLINK_EDIT"));

	wxButton* btnLinkDelete = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkDelete->SetHelpText(_("HIDC_RUNLINK_DELETE"));
	btnLinkDelete->SetToolTip(_("HIDC_RUNLINK_DELETE"));

	wxButton* btnLinkOpen = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_OPEN"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkOpen->SetHelpText(_("HIDC_RUNLINK_OPEN"));
	btnLinkOpen->SetToolTip(_("HIDC_RUNLINK_OPEN"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerScorePanel = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textDate, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerDate->Add(ctrlDate, 0, wxALL, 5);
	sizerDate->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(textClub, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerScorePanel->Add(sizerDate, 0, wxEXPAND, 5);

	wxBoxSizer* sizerDivHt = new wxBoxSizer(wxHORIZONTAL);

	wxFlexGridSizer* sizerEvent = new wxFlexGridSizer(5, 2, 0, 0);
	sizerEvent->SetFlexibleDirection(wxBOTH);
	sizerEvent->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerEvent->Add(textDiv, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(ctrlDivision, 0, wxALL, 5);
	sizerEvent->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlLevel, 0, wxALL, 5);
	sizerEvent->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlEvent, 0, wxALL, 5);
	sizerEvent->Add(m_textSubName, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlSubName, 0, wxALL, 5);
	sizerEvent->Add(0, 0, 1, wxEXPAND, 5);
	sizerEvent->Add(ctrlTable, 0, wxALL, 5);

	sizerDivHt->Add(sizerEvent, 0, wxEXPAND, 5);

	wxBoxSizer* sizerHtCond = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerHt = new wxBoxSizer(wxHORIZONTAL);
	sizerHt->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(ctrlHeight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHt->Add(textJudge, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(ctrlJudge, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHt->Add(ctrlJudgeNote, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxTOP, 5);
	sizerHt->Add(textHandler, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(ctrlHandler, 0, wxALL, 5);

	sizerHtCond->Add(sizerHt, 0, wxEXPAND, 5);

	wxBoxSizer* sizerCond = new wxBoxSizer(wxHORIZONTAL);
	sizerCond->Add(textConditions, 0, wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerCond->Add(ctrlConditions, 1, wxALL|wxEXPAND, 5);

	sizerHtCond->Add(sizerCond, 1, wxEXPAND, 5);

	wxBoxSizer* sizerComments = new wxBoxSizer(wxHORIZONTAL);
	sizerComments->Add(m_ctrlNote, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerPartner = new wxBoxSizer(wxVERTICAL);
	sizerPartner->Add(m_btnPartner, 0, wxALL, 5);
	sizerPartner->Add(m_ctrlPartner, 0, wxALL, 5);

	sizerComments->Add(sizerPartner, 0, wxEXPAND, 5);

	sizerHtCond->Add(sizerComments, 0, wxEXPAND, 5);

	sizerDivHt->Add(sizerHtCond, 0, wxEXPAND, 5);

	sizerScorePanel->Add(sizerDivHt, 0, wxEXPAND, 5);

	wxFlexGridSizer* sizerResults = new wxFlexGridSizer(5, 4, 0, 0);
	sizerResults->SetFlexibleDirection(wxBOTH);
	sizerResults->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerSCT = new wxBoxSizer(wxHORIZONTAL);
	sizerSCT->Add(m_textSCT, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerSCT->Add(m_ctrlSCT, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerSCT, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerTime = new wxBoxSizer(wxHORIZONTAL);
	sizerTime->Add(textTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerTime->Add(ctrlTime, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerTime, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerPlace = new wxBoxSizer(wxHORIZONTAL);
	sizerPlace->Add(textPlace, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerPlace->Add(ctrlPlace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPlace->Add(textPlaceOf, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxTOP, 5);
	sizerPlace->Add(ctrlPlaceTotal, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerPlace, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerBonus = new wxBoxSizer(wxHORIZONTAL);
	sizerBonus->Add(m_textBonus, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerBonus->Add(m_ctrlBonus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerBonus, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerYardsReqOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYardsReqOpeningPts->Add(m_textYardsReqOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerYardsReqOpeningPts->Add(m_ctrlYardsReqOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerYardsReqOpeningPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerFaults->Add(textFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerFaults->Add(ctrlFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerFaults, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerDogsQd = new wxBoxSizer(wxHORIZONTAL);
	sizerDogsQd->Add(textDogsQd, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerDogsQd->Add(ctrlDogsQd, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerDogsQd, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerSpeed = new wxBoxSizer(wxHORIZONTAL);
	sizerSpeed->Add(m_textSpeed, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerSpeed->Add(m_ctrlSpeed, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerSpeed, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerMinYPSClosingTime = new wxBoxSizer(wxHORIZONTAL);
	sizerMinYPSClosingTime->Add(m_textMinYPSClosingTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerMinYPSClosingTime->Add(m_ctrlMinYPSClosingTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerMinYPSClosingTime, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerYPSOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYPSOpeningPts->Add(m_textYPSOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerYPSOpeningPts->Add(m_ctrlYPSOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerYPSOpeningPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerQ = new wxBoxSizer(wxHORIZONTAL);
	sizerQ->Add(textQ, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerQ->Add(ctrlQ, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerQ, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerTitlePts = new wxBoxSizer(wxHORIZONTAL);
	sizerTitlePts->Add(textTitlePts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerTitlePts->Add(ctrlTitlePts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerTitlePts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerReqClosingPts = new wxBoxSizer(wxHORIZONTAL);
	sizerReqClosingPts->Add(m_textReqClosingPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerReqClosingPts->Add(m_ctrlReqClosingPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerReqClosingPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerClosingPtsTotalFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerClosingPtsTotalFaults->Add(m_textClosingPtsTotalFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerClosingPtsTotalFaults->Add(m_ctrlClosingPtsTotalFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerClosingPtsTotalFaults, 0, wxALIGN_RIGHT, 5);

	sizerResults->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* sizerScore = new wxBoxSizer(wxHORIZONTAL);
	sizerScore->Add(textScore, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerScore->Add(ctrlScore, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerScore, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerNumObs = new wxBoxSizer(wxHORIZONTAL);
	sizerNumObs->Add(textNumObs, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerNumObs->Add(ctrlNumObs, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerNumObs, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerObsSec = new wxBoxSizer(wxHORIZONTAL);
	sizerObsSec->Add(textObsSec, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerObsSec->Add(m_ctrlObsSec, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerObsSec, 0, wxALIGN_RIGHT, 5);
	sizerResults->Add(0, 0, 1, wxEXPAND, 5);
	sizerResults->Add(btnOtherPoints, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	sizerScorePanel->Add(sizerResults, 1, wxEXPAND, 5);

	panelScore->SetSizer(sizerScorePanel);
	panelScore->Layout();
	sizerScorePanel->Fit(panelScore);
	notebook->AddPage(panelScore, _("IDD_RUN_SCORE"), true);

	wxBoxSizer* sizerCommentsFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerCommentsFaults->Add(ctrlComments, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerFaultsList = new wxBoxSizer(wxVERTICAL);
	sizerFaultsList->Add(ctrlFaultsList, 1, wxALL, 5);
	sizerFaultsList->Add(btnFaults, 0, wxALL, 5);

	sizerCommentsFaults->Add(sizerFaultsList, 0, wxEXPAND, 5);

	panelComments->SetSizer(sizerCommentsFaults);
	panelComments->Layout();
	sizerCommentsFaults->Fit(panelComments);
	notebook->AddPage(panelComments, _("IDD_RUN_COMMENTS"), false);

	wxBoxSizer* sizerRefRuns = new wxBoxSizer(wxVERTICAL);
	sizerRefRuns->Add(ctrlRefRuns, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerRefBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRefBtns->Add(btnRefNew, 0, wxALL, 5);
	sizerRefBtns->Add(btnRefAddMe, 0, wxALL, 5);
	sizerRefBtns->Add(btnRefEdit, 0, wxALL, 5);
	sizerRefBtns->Add(btnRefDelete, 0, wxALL, 5);
	sizerRefRuns->Add(sizerRefBtns, 0, wxEXPAND, 5);

	panelRefRuns->SetSizer(sizerRefRuns);
	panelRefRuns->Layout();
	sizerRefRuns->Fit(panelRefRuns);
	notebook->AddPage(panelRefRuns, _("IDD_RUN_REFERENCE"), false);

	wxBoxSizer* sizerCRCD = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDisplay = new wxBoxSizer(wxVERTICAL);
	sizerDisplay->Add(textCRCD, 0, wxALL, 5);
	sizerDisplay->Add(textDisplay, 1, wxALL|wxEXPAND, 5);

	sizerCRCD->Add(sizerDisplay, 1, wxEXPAND, 5);

	wxBoxSizer* sizerBtnsCRCD = new wxBoxSizer(wxVERTICAL);
	sizerBtnsCRCD->Add(btnCourse, 0, wxALL, 5);
	sizerBtnsCRCD->Add(btnView, 0, wxALL, 5);
	sizerBtnsCRCD->Add(btnCopyCRCD, 0, wxALL, 5);
	sizerBtnsCRCD->Add(ctrlIncImage, 0, wxALL, 5);
	sizerBtnsCRCD->Add(textImageDesc, 0, wxALL, 5);

	sizerCRCD->Add(sizerBtnsCRCD, 0, wxEXPAND, 5);

	panelCRCD->SetSizer(sizerCRCD);
	panelCRCD->Layout();
	sizerCRCD->Fit(panelCRCD);
	notebook->AddPage(panelCRCD, _("IDD_RUN_CRCD"), false);

	wxBoxSizer* sizerLinks = new wxBoxSizer(wxVERTICAL);
	sizerLinks->Add(ctrlLinks, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerLinkBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerLinkBtns->Add(btnLinkNew, 0, wxALL, 5);
	sizerLinkBtns->Add(btnLinkEdit, 0, wxALL, 5);
	sizerLinkBtns->Add(btnLinkDelete, 0, wxALL, 5);
	sizerLinkBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerLinkBtns->Add(btnLinkOpen, 0, wxALL, 5);

	sizerLinks->Add(sizerLinkBtns, 0, wxEXPAND, 5);

	panelLinks->SetSizer(sizerLinks);
	panelLinks->Layout();
	sizerLinks->Fit(panelLinks);
	notebook->AddPage(panelLinks, _("IDD_RUN_LINK"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxSizer* sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	notebook->SetSelection(iSelectPage);
}


void CDlgRun::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	//TODO: Remove debugging code
#ifdef _DEBUG
	{
		ARBDogClubPtr pClub;
		m_pTrial->GetClubs().GetPrimaryClub(&pClub);
		assert(NULL != pClub.get());
		ARBConfigVenuePtr pVenue;
		m_pDoc->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue);
		assert(NULL != pVenue.get());
		ARBConfigEventPtr pEvent;
		pVenue->GetEvents().FindEvent(m_Run->GetEvent(), &pEvent);
		assert(NULL != pEvent.get());
		if (!pEvent->HasTable())
			if (m_Run->GetScoring().HasTable())
				wxMessageBox(wxT("Poof!"), wxMessageBoxCaptionStr, wxCENTRE);
	}
#endif
	//End TODO

	*m_pRealRun = *m_Run;
	m_pTrial->SetMultiQs(m_pDoc->Book().GetConfig()); // Note, when adding a new run, this is actually too soon to call - the run isn't in the trial yet
	CAgilityBookOptions::SetLastEnteredDivision(m_Run->GetDivision().c_str());
	CAgilityBookOptions::SetLastEnteredLevel(m_Run->GetLevel().c_str());
	CAgilityBookOptions::SetLastEnteredHeight(m_Run->GetHeight().c_str());
	CAgilityBookOptions::SetLastEnteredJudge(m_Run->GetJudge().c_str());
	CAgilityBookOptions::SetLastEnteredHandler(m_Run->GetHandler().c_str());
	EndDialog(wxID_OK);
}
