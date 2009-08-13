/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-08-12 DRC Fix killfocus handling.
 * @li 2009-07-14 DRC Fixed group box creation order.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Make 'Notes' button change selection.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-21 DRC Added Location/Club info fields.
 */

#include "stdafx.h"
#include "DlgCalendar.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "ComboBoxes.h"
#include "DlgInfoNote.h"
#include "NoteButton.h"
#include "RichEditCtrl2.h"
#include "Validators.h"
#include <set>
#include <wx/datectrl.h>
#include <wx/dateevt.h>


BEGIN_EVENT_TABLE(CDlgCalendar, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgCalendar::OnOk)
END_EVENT_TABLE()


CDlgCalendar::CDlgCalendar(
		ARBCalendarPtr pCal,
		CAgilityBookDoc* pDoc,
		wxWindow* pParent)
	: wxDialog()
	, m_pCal(pCal)
	, m_pDoc(pDoc)
	, m_Span(0)
	, m_dateStart(ARBDate::Today())
	, m_dateEnd(ARBDate::Today() + 1)
	, m_bTentative(m_pCal->IsTentative())
	, m_dateOpens(ARBDate::Today())
	, m_bOpeningUnknown(true)
	, m_dateDraws(ARBDate::Today())
	, m_bDrawingUnknown(true)
	, m_dateCloses(ARBDate::Today())
	, m_bClosingUnknown(true)
	, m_OnlineUrl(m_pCal->GetOnlineURL().c_str())
	, m_Confirmation()
	, m_PremiumUrl(m_pCal->GetPremiumURL().c_str())
	, m_EMailSecAddr(m_pCal->GetSecEmail().c_str())
	, m_Venue(m_pCal->GetVenue().c_str())
	, m_Club(m_pCal->GetClub().c_str())
	, m_Location(m_pCal->GetLocation().c_str())
	, m_Notes(m_pCal->GetNote().c_str())
	, m_ctrlEnd(NULL)
	, m_ctrlOpens(NULL)
	, m_ctrlDraws(NULL)
	, m_ctrlCloses(NULL)
	, m_ctrlEntryNot(NULL)
	, m_ctrlEntryPlan(NULL)
	, m_ctrlEntryEntered(NULL)
	, m_ctrlOnlineUrlEntry(NULL)
	, m_ctrlOnlineUrl(NULL)
	, m_ctrlAccomNot(NULL)
	, m_ctrlAccomNeeded(NULL)
	, m_ctrlAccomMade(NULL)
	, m_ctrlConfirmation(NULL)
	, m_ctrlPremiumEntry(NULL)
	, m_ctrlPremiumUrl(NULL)
	, m_ctrlEMailSec(NULL)
	, m_ctrlEMailSecAddr(NULL)
	, m_ctrlVenue(NULL)
	, m_ctrlClub(NULL)
	, m_ctrlClubNotes(NULL)
	, m_ctrlClubInfo(NULL)
	, m_ctrlLocation(NULL)
	, m_ctrlLocationNotes(NULL)
	, m_ctrlLocationInfo(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CALENDAR"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	if (m_pCal->GetStartDate().IsValid())
	{
		m_dateStart = m_pCal->GetStartDate();
		if (m_pCal->GetEndDate().IsValid())
			m_Span = m_pCal->GetEndDate() - m_pCal->GetStartDate();
	}
	if (m_pCal->GetEndDate().IsValid())
		m_dateEnd = m_pCal->GetEndDate();
	if (m_pCal->GetOpeningDate().IsValid())
	{
		m_dateOpens = m_pCal->GetOpeningDate();
		m_bOpeningUnknown = false;
	}
	if (m_pCal->GetDrawDate().IsValid())
	{
		m_dateDraws = m_pCal->GetDrawDate();
		m_bDrawingUnknown = false;
	}
	if (m_pCal->GetClosingDate().IsValid())
	{
		m_dateCloses = m_pCal->GetClosingDate();
		m_bClosingUnknown = false;
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textStart = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_DATE_START"),
		wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	wxDatePickerCtrl* ctrlStart = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateStart));
	ctrlStart->Connect(wxEVT_DATE_CHANGED, wxDateEventHandler(CDlgCalendar::OnDatetimechangeStart), NULL, this);
	ctrlStart->SetHelpText(_("HIDC_CAL_DATE_START"));
	ctrlStart->SetToolTip(_("HIDC_CAL_DATE_START"));

	wxStaticText* textEnd = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_DATE_END"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEnd->Wrap(-1);

	m_ctrlEnd = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateEnd));
	m_ctrlEnd->SetHelpText(_("HIDC_CAL_DATE_END"));
	m_ctrlEnd->SetToolTip(_("HIDC_CAL_DATE_END"));

	wxCheckBox* ctrlTentative = new wxCheckBox(this, wxID_ANY,
		_("IDC_CAL_TENTATIVE"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bTentative));
	ctrlTentative->SetHelpText(_("HIDC_CAL_TENTATIVE"));
	ctrlTentative->SetToolTip(_("HIDC_CAL_TENTATIVE"));

	wxStaticText* textOpens = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_DATE_OPENS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textOpens->Wrap(-1);

	m_ctrlOpens = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateOpens));
	m_ctrlOpens->SetHelpText(_("HIDC_CAL_DATE_OPENS"));
	m_ctrlOpens->SetToolTip(_("HIDC_CAL_DATE_OPENS"));
	m_ctrlOpens->Enable(!m_bOpeningUnknown);

	wxCheckBox* ctrlOpensUnknown = new wxCheckBox(this, wxID_ANY,
		_("IDC_CAL_DATE_OPENS_UNKNOWN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bOpeningUnknown));
	ctrlOpensUnknown->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgCalendar::OnDateOpensUnknown), NULL, this);
	ctrlOpensUnknown->SetHelpText(_("HIDC_CAL_DATE_OPENS_UNKNOWN"));
	ctrlOpensUnknown->SetToolTip(_("HIDC_CAL_DATE_OPENS_UNKNOWN"));

	wxStaticText* textDraws = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_DATE_DRAWS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDraws->Wrap(-1);

	m_ctrlDraws = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateDraws));
	m_ctrlDraws->SetHelpText(_("HIDC_CAL_DATE_DRAWS"));
	m_ctrlDraws->SetToolTip(_("HIDC_CAL_DATE_DRAWS"));
	m_ctrlDraws->Enable(!m_bDrawingUnknown);

	wxCheckBox* ctrDrawsUnknown = new wxCheckBox(this, wxID_ANY,
		_("IDC_CAL_DATE_DRAWS_UNKNOWN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bDrawingUnknown));
	ctrDrawsUnknown->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgCalendar::OnDateDrawsUnknown), NULL, this);
	ctrDrawsUnknown->SetHelpText(_("HIDC_CAL_DATE_DRAWS_UNKNOWN"));
	ctrDrawsUnknown->SetToolTip(_("HIDC_CAL_DATE_DRAWS_UNKNOWN"));

	wxStaticText* textCloses = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_DATE_CLOSES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCloses->Wrap(-1);

	m_ctrlCloses = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateCloses));
	m_ctrlCloses->SetHelpText(_("HIDC_CAL_DATE_CLOSES"));
	m_ctrlCloses->SetToolTip(_("HIDC_CAL_DATE_CLOSES"));
	m_ctrlCloses->Enable(!m_bClosingUnknown);

	wxCheckBox* ctrlClosesUnknown = new wxCheckBox(this, wxID_ANY,
		_("IDC_CAL_DATE_CLOSES_UNKNOWN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bClosingUnknown));
	ctrlClosesUnknown->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgCalendar::OnDateClosesUnknown), NULL, this);
	ctrlClosesUnknown->SetHelpText(_("HIDC_CAL_DATE_CLOSES_UNKNOWN"));
	ctrlClosesUnknown->SetToolTip(_("HIDC_CAL_DATE_CLOSES_UNKNOWN"));

	wxStaticBox* boxEntry = new wxStaticBox(this, wxID_ANY, _("IDC_CAL_ENTER"));

	m_ctrlEntryNot = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ENTER_NOT"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlEntryNot->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnCalEntry), NULL, this);
	m_ctrlEntryNot->SetHelpText(_("HIDC_CAL_ENTER_NOT"));
	m_ctrlEntryNot->SetToolTip(_("HIDC_CAL_ENTER_NOT"));

	m_ctrlEntryPlan = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ENTER_PLANNING"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEntryPlan->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnCalEntry), NULL, this);
	m_ctrlEntryPlan->SetHelpText(_("HIDC_CAL_ENTER_PLANNING"));
	m_ctrlEntryPlan->SetToolTip(_("HIDC_CAL_ENTER_PLANNING"));

	m_ctrlEntryEntered = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ENTER_ENTERED"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEntryEntered->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnCalEntry), NULL, this);
	m_ctrlEntryEntered->SetHelpText(_("HIDC_CAL_ENTER_ENTERED"));
	m_ctrlEntryEntered->SetToolTip(_("HIDC_CAL_ENTER_ENTERED"));

	switch (m_pCal->GetEntered())
	{
	default:
		m_ctrlEntryNot->SetValue(true);
		break;
	case ARBCalendar::ePlanning:
		m_ctrlEntryPlan->SetValue(true);
		break;
	case ARBCalendar::eEntered:
		m_ctrlEntryEntered->SetValue(true);
		break;
	}

	m_ctrlOnlineUrlEntry = new wxButton(this, wxID_ANY,
		_("IDC_CAL_ONLINE_ENTRY"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlOnlineUrlEntry->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendar::OnOnlineEntry), NULL, this);
	m_ctrlOnlineUrlEntry->SetHelpText(_("HIDC_CAL_ONLINE_ENTRY"));
	m_ctrlOnlineUrlEntry->SetToolTip(_("HIDC_CAL_ONLINE_ENTRY"));
	if (ARBCalendar::ePlanning != m_pCal->GetEntered() || m_OnlineUrl.empty())
		m_ctrlOnlineUrlEntry->Enable(false);

	m_ctrlOnlineUrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_OnlineUrl, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlOnlineUrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CDlgCalendar::OnEnChangeCalOnlineUrl), NULL, this);
	m_ctrlOnlineUrl->SetHelpText(_("HIDC_CAL_ONLINE_URL"));
	m_ctrlOnlineUrl->SetToolTip(_("HIDC_CAL_ONLINE_URL"));

	wxStaticBox* boxAccom = new wxStaticBox(this, wxID_ANY, _("IDC_CAL_ACCOM"));

	m_ctrlAccomNot = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ACCOM_NONE"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlAccomNot->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnAccommodation), NULL, this);
	m_ctrlAccomNot->SetHelpText(_("HIDC_CAL_ACCOM_NONE"));
	m_ctrlAccomNot->SetToolTip(_("HIDC_CAL_ACCOM_NONE"));

	m_ctrlAccomNeeded = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ACCOM_NEEDED"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAccomNeeded->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnAccommodation), NULL, this);
	m_ctrlAccomNeeded->SetHelpText(_("HIDC_CAL_ACCOM_NEEDED"));
	m_ctrlAccomNeeded->SetToolTip(_("HIDC_CAL_ACCOM_NEEDED"));

	m_ctrlAccomMade = new wxRadioButton(this, wxID_ANY,
		_("IDC_CAL_ACCOM_MADE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAccomMade->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgCalendar::OnAccommodation), NULL, this);
	m_ctrlAccomMade->SetHelpText(_("HIDC_CAL_ACCOM_MADE"));
	m_ctrlAccomMade->SetToolTip(_("HIDC_CAL_ACCOM_MADE"));

	switch (m_pCal->GetAccommodation())
	{
	default:
		m_ctrlAccomNot->SetValue(true);
		break;
	case ARBCalendar::eAccomTodo:
		m_ctrlAccomNeeded->SetValue(true);
		break;
	case ARBCalendar::eAccomConfirmed:
		m_ctrlAccomMade->SetValue(true);
		m_Confirmation = m_pCal->GetConfirmation().c_str();
		break;
	}

	m_ctrlConfirmation = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Confirmation, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlConfirmation->SetHelpText(_("HIDC_CAL_ACCOM_CONFIRMATION"));
	m_ctrlConfirmation->SetToolTip(_("HIDC_CAL_ACCOM_CONFIRMATION"));
	m_ctrlConfirmation->Enable(ARBCalendar::eAccomConfirmed == m_pCal->GetAccommodation());

	m_ctrlPremiumEntry = new wxButton(this, wxID_ANY,
		_("IDC_CAL_PREMIUM_ENTRY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPremiumEntry->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendar::OnPremiumEntry), NULL, this);
	m_ctrlPremiumEntry->SetHelpText(_("HIDC_CAL_PREMIUM_ENTRY"));
	m_ctrlPremiumEntry->SetToolTip(_("HIDC_CAL_PREMIUM_ENTRY"));
	if (m_PremiumUrl.empty())
		m_ctrlPremiumEntry->Enable(false);

	m_ctrlPremiumUrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_PremiumUrl, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlPremiumUrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CDlgCalendar::OnEnChangeCalPremiumUrl), NULL, this);
	m_ctrlPremiumUrl->SetHelpText(_("HIDC_CAL_PREMIUM_URL"));
	m_ctrlPremiumUrl->SetToolTip(_("HIDC_CAL_PREMIUM_URL"));

	m_ctrlEMailSec = new wxButton(this, wxID_ANY,
		_("IDC_CAL_EMAIL_SEC"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEMailSec->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendar::OnEmailSec), NULL, this);
	m_ctrlEMailSec->SetHelpText(_("HIDC_CAL_EMAIL_SEC"));
	m_ctrlEMailSec->SetToolTip(_("HIDC_CAL_EMAIL_SEC"));
	if (m_EMailSecAddr.empty())
		m_ctrlEMailSec->Enable(false);

	m_ctrlEMailSecAddr = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_EMailSecAddr, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlEMailSecAddr->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CDlgCalendar::OnEnChangeCalEmailSecAddr), NULL, this);
	m_ctrlEMailSecAddr->SetHelpText(_("HIDC_CAL_EMAIL_SEC_ADDR"));
	m_ctrlEMailSecAddr->SetToolTip(_("HIDC_CAL_EMAIL_SEC_ADDR"));
	std::set<tstring> addrs;
	for (ARBCalendarList::const_iterator iCal = m_pDoc->Book().GetCalendar().begin();
		iCal != m_pDoc->Book().GetCalendar().end();
		++iCal)
	{
		addrs.insert((*iCal)->GetSecEmail());
	}
	for (std::set<tstring>::iterator i = addrs.begin(); i != addrs.end(); ++i)
	{
		if (!(*i).empty())
		{
			m_ctrlEMailSecAddr->Append((*i).c_str());
		}
	}

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_VENUE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenue = new CVenueComboBox(this, m_pDoc->Book().GetConfig().GetVenues(), m_Venue, false,
		CTrimValidator(&m_Venue, TRIMVALIDATOR_TRIM_BOTH), true);
	m_ctrlVenue->SetHelpText(_("HIDC_CAL_VENUE"));
	m_ctrlVenue->SetToolTip(_("HIDC_CAL_VENUE"));

	wxStaticText* textClub = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_CLUB"),
		wxDefaultPosition, wxDefaultSize, 0);
	textClub->Wrap(-1);

	m_ctrlClub = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Club, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlClub->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgCalendar::OnSelchangeClub), NULL, this);
	m_ctrlClub->SetHelpText(_("HIDC_CAL_CLUB"));
	m_ctrlClub->SetToolTip(_("HIDC_CAL_CLUB"));

	m_ctrlClubNotes = new CNoteButton(this);
	m_ctrlClubNotes->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendar::OnClubNotes), NULL, this);
	m_ctrlClubNotes->SetHelpText(_("HIDC_CAL_CLUB_NOTES"));
	m_ctrlClubNotes->SetToolTip(_("HIDC_CAL_CLUB_NOTES"));

	m_ctrlClubInfo = new CRichEditCtrl2(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 70), true);
	m_ctrlClubInfo->SetHelpText(_("HIDC_CAL_CLUB_NOTE"));
	m_ctrlClubInfo->SetToolTip(_("HIDC_CAL_CLUB_NOTE"));

	wxStaticText* textLocation = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_LOCATION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLocation->Wrap(-1);

	m_ctrlLocation = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Location, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlLocation->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgCalendar::OnSelchangeLocation), NULL, this);
	m_ctrlLocation->SetHelpText(_("HIDC_CAL_LOCATION"));
	m_ctrlLocation->SetToolTip(_("HIDC_CAL_LOCATION"));

	m_ctrlLocationNotes = new CNoteButton(this);
	m_ctrlLocationNotes->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendar::OnLocationNotes), NULL, this);
	m_ctrlLocationNotes->SetHelpText(_("HIDC_CAL_LOCATION_NOTES"));
	m_ctrlLocationNotes->SetToolTip(_("HIDC_CAL_LOCATION_NOTES"));

	m_ctrlLocationInfo = new CRichEditCtrl2(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 115), true);
	m_ctrlLocationInfo->SetHelpText(_("HIDC_CAL_LOCATION_NOTE"));
	m_ctrlLocationInfo->SetToolTip(_("HIDC_CAL_LOCATION_NOTE"));

	wxStaticText* textNotes = new wxStaticText(this, wxID_ANY,
		_("IDC_CAL_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	wxTextCtrl* ctrlNotes = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_CAL_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_CAL_NOTES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDates = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerTrialInfo = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerDates1 = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDates1->SetFlexibleDirection(wxBOTH);
	sizerDates1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates1->Add(textStart, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 2);
	sizerDates1->Add(ctrlStart, 0, wxALL, 3);
	sizerDates1->Add(textEnd, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	sizerDates1->Add(m_ctrlEnd, 0, wxALL, 3);

	sizerTrialInfo->Add(sizerDates1, 0, wxEXPAND, 5);
	sizerTrialInfo->Add(ctrlTentative, 0, wxALL, 5);

	sizerDates->Add(sizerTrialInfo, 0, wxEXPAND, 5);

	wxFlexGridSizer* sizerDates2 = new wxFlexGridSizer(3, 3, 0, 0);
	sizerDates2->SetFlexibleDirection(wxBOTH);
	sizerDates2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates2->Add(textOpens, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	sizerDates2->Add(m_ctrlOpens, 0, wxALL, 3);
	sizerDates2->Add(ctrlOpensUnknown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	sizerDates2->Add(textDraws, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	sizerDates2->Add(m_ctrlDraws, 0, wxALL, 3);
	sizerDates2->Add(ctrDrawsUnknown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	sizerDates2->Add(textCloses, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	sizerDates2->Add(m_ctrlCloses, 0, wxALL, 3);
	sizerDates2->Add(ctrlClosesUnknown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);

	sizerDates->Add(sizerDates2, 0, wxEXPAND, 5);

	bSizer->Add(sizerDates, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerContent = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerEntry = new wxStaticBoxSizer(boxEntry, wxVERTICAL);
	sizerEntry->Add(m_ctrlEntryNot, 0, wxALL, 3);
	sizerEntry->Add(m_ctrlEntryPlan, 0, wxALL, 3);

	wxBoxSizer* sizerOnline = new wxBoxSizer(wxHORIZONTAL);
	sizerOnline->Add(15, 0, 0, 0, 5);
	sizerOnline->Add(m_ctrlOnlineUrlEntry, 0, wxALL, 3);
	sizerOnline->Add(m_ctrlOnlineUrl, 1, wxALL, 5);

	sizerEntry->Add(sizerOnline, 0, wxEXPAND, 5);
	sizerEntry->Add(m_ctrlEntryEntered, 0, wxALL, 3);

	sizer1->Add(sizerEntry, 0, wxEXPAND, 5);

	wxBoxSizer* sizerPremium = new wxBoxSizer(wxHORIZONTAL);
	sizerPremium->Add(m_ctrlPremiumEntry, 0, wxALL, 3);
	sizerPremium->Add(m_ctrlPremiumUrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);

	sizer1->Add(sizerPremium, 0, wxEXPAND, 5);

	wxBoxSizer* sizerSec = new wxBoxSizer(wxHORIZONTAL);
	sizerSec->Add(m_ctrlEMailSec, 0, wxALL, 3);
	sizerSec->Add(m_ctrlEMailSecAddr, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);

	sizer1->Add(sizerSec, 0, wxEXPAND, 5);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(m_ctrlVenue, 0, wxALL, 3);

	sizer1->Add(sizerVenue, 0, wxEXPAND, 5);

	wxBoxSizer* sizerClub = new wxBoxSizer(wxHORIZONTAL);
	sizerClub->Add(textClub, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerClub->Add(m_ctrlClub, 0, wxALL, 3);
	sizerClub->Add(m_ctrlClubNotes, 0, wxALL, 3);

	sizer1->Add(sizerClub, 0, wxEXPAND, 5);
	sizer1->Add(m_ctrlClubInfo, 1, wxALL|wxEXPAND, 5);

	sizerContent->Add(sizer1, 3, wxEXPAND|wxLEFT|wxRIGHT, 5);

	wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerAccom = new wxStaticBoxSizer(boxAccom, wxVERTICAL);
	sizerAccom->Add(m_ctrlAccomNot, 0, wxALL, 3);
	sizerAccom->Add(m_ctrlAccomNeeded, 0, wxALL, 3);

	wxBoxSizer* sizerMade = new wxBoxSizer(wxHORIZONTAL);
	sizerMade->Add(m_ctrlAccomMade, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	sizerMade->Add(m_ctrlConfirmation, 1, wxALL, 5);

	sizerAccom->Add(sizerMade, 0, wxEXPAND, 5);

	sizer2->Add(sizerAccom, 0, wxEXPAND, 5);
	sizer2->Add(0, 0, 1, wxEXPAND, 5);
	sizer2->Add(textLocation, 0, wxLEFT|wxRIGHT, 5);

	wxBoxSizer* sizerLocation = new wxBoxSizer(wxHORIZONTAL);
	sizerLocation->Add(m_ctrlLocation, 1, wxALL, 5);
	sizerLocation->Add(m_ctrlLocationNotes, 0, wxALL, 5);

	sizer2->Add(sizerLocation, 0, wxEXPAND, 5);
	sizer2->Add(m_ctrlLocationInfo, 0, wxALL|wxEXPAND, 5);

	sizerContent->Add(sizer2, 2, wxEXPAND|wxLEFT|wxRIGHT, 5);

	bSizer->Add(sizerContent, 0, wxEXPAND, 5);
	bSizer->Add(textNotes, 0, wxLEFT|wxRIGHT, 7);
	bSizer->Add(ctrlNotes, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	ListLocations();
	ListClubs();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	// Connect killfocus handlers last
	m_ctrlClub->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CDlgCalendar::OnKillfocusClub), NULL, this);
	m_ctrlLocation->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CDlgCalendar::OnKillfocusLocation), NULL, this);
}


void CDlgCalendar::UpdateLocationInfo(wxChar const* pLocation)
{
	wxString str;
	if (pLocation && *pLocation)
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->Book().GetInfo().GetInfo(ARBInfo::eLocationInfo).FindItem(pLocation, &pItem))
		{
			str = pItem->GetComment().c_str();
		}
	}
	m_ctrlLocationInfo->SetValue(str);
}


void CDlgCalendar::ListLocations()
{
	std::set<tstring> locations;
	m_pDoc->Book().GetAllTrialLocations(locations, true, true);
	if (!m_pCal->GetLocation().empty())
		locations.insert(m_pCal->GetLocation());
	tstring loc(m_Location.c_str());
	if (m_Location.empty())
		loc = m_pCal->GetLocation();
	m_ctrlLocation->Clear();
	for (std::set<tstring>::const_iterator iter = locations.begin(); iter != locations.end(); ++iter)
	{
		int index = m_ctrlLocation->Append((*iter).c_str());
		if ((*iter) == loc)
		{
			m_ctrlLocation->SetSelection(index);
			UpdateLocationInfo((*iter).c_str());
		}
	}
}


void CDlgCalendar::UpdateClubInfo(wxChar const* pClub)
{
	wxString str;
	if (pClub && *pClub)
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->Book().GetInfo().GetInfo(ARBInfo::eClubInfo).FindItem(pClub, &pItem))
		{
			str = pItem->GetComment().c_str();
		}
	}
	m_ctrlClubInfo->SetValue(str);
}


void CDlgCalendar::ListClubs()
{
	std::set<tstring> clubs;
	m_pDoc->Book().GetAllClubNames(clubs, true, true);
	if (!m_pCal->GetClub().empty())
		clubs.insert(m_pCal->GetClub());
	tstring club(m_Club.c_str());
	if (m_Club.empty())
		club = m_pCal->GetClub();
	m_ctrlClub->Clear();
	for (std::set<tstring>::const_iterator iter = clubs.begin(); iter != clubs.end(); ++iter)
	{
		int index = m_ctrlClub->Append((*iter).c_str());
		if ((*iter) == club)
		{
			m_ctrlClub->SetSelection(index);
			UpdateClubInfo((*iter).c_str());
		}
	}
}


void CDlgCalendar::OnDatetimechangeStart(wxDateEvent& evt)
{
	if (0 < m_Span)
	{
		wxDateTime date = evt.GetDate();
		if (date.IsValid())
		{
			date.Add(wxDateSpan(0, 0, 0, m_Span));
			m_ctrlEnd->SetValue(date);
		}
	}
}


void CDlgCalendar::OnDateOpensUnknown(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlOpens->Enable(!m_bOpeningUnknown);
}


void CDlgCalendar::OnDateDrawsUnknown(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlDraws->Enable(!m_bDrawingUnknown);
}


void CDlgCalendar::OnDateClosesUnknown(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlCloses->Enable(!m_bClosingUnknown);
}


void CDlgCalendar::OnCalEntry(wxCommandEvent& evt)
{
	wxString s = m_ctrlOnlineUrl->GetValue();
	if (!m_ctrlEntryPlan->GetValue() || s.empty())
		m_ctrlOnlineUrlEntry->Enable(false);
	else
		m_ctrlOnlineUrlEntry->Enable(true);
}


void CDlgCalendar::OnEnChangeCalOnlineUrl(wxCommandEvent& evt)
{
	wxString s = m_ctrlOnlineUrl->GetValue();
	if (!m_ctrlEntryPlan->GetValue() || s.empty())
		m_ctrlOnlineUrlEntry->Enable(false);
	else
		m_ctrlOnlineUrlEntry->Enable(true);
}


void CDlgCalendar::OnOnlineEntry(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxLaunchDefaultBrowser(m_OnlineUrl);
}


void CDlgCalendar::OnEnChangeCalPremiumUrl(wxCommandEvent& evt)
{
	if (m_ctrlPremiumUrl->GetValue().empty())
		m_ctrlPremiumEntry->Enable(false);
	else
		m_ctrlPremiumEntry->Enable(true);
}


void CDlgCalendar::OnPremiumEntry(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxLaunchDefaultBrowser(m_PremiumUrl);
}


void CDlgCalendar::OnEnChangeCalEmailSecAddr(wxCommandEvent& evt)
{
	int idx = m_ctrlEMailSecAddr->GetSelection();
	wxString str;
	if (wxNOT_FOUND != idx)
		str = m_ctrlEMailSecAddr->GetString(idx);
	if (str.empty())
		m_ctrlEMailSec->Enable(false);
	else
		m_ctrlEMailSec->Enable(true);
}


void CDlgCalendar::OnEmailSec(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxString s(m_EMailSecAddr);
	if (0 != m_EMailSecAddr.Find(wxT("mailto:")))
		s = wxT("mailto:") + m_EMailSecAddr;
	wxLaunchDefaultBrowser(s);
}


void CDlgCalendar::OnAccommodation(wxCommandEvent& evt)
{
	m_ctrlConfirmation->Enable(m_ctrlAccomMade->GetValue());
}


void CDlgCalendar::OnSelchangeClub(wxCommandEvent& evt)
{
	int idx = m_ctrlClub->GetSelection();
	wxString str;
	if (wxNOT_FOUND != idx)
		str = m_ctrlClub->GetString(idx);
	UpdateClubInfo(str.c_str());
}


void CDlgCalendar::OnKillfocusClub(wxFocusEvent& evt)
{
	TransferDataFromWindow();
	UpdateClubInfo(m_Club.c_str());
	evt.Skip();
}


void CDlgCalendar::OnClubNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfo::eClubInfo, m_Club, this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Club = dlg.CurrentSelection();
		ListClubs();
	}
}


void CDlgCalendar::OnSelchangeLocation(wxCommandEvent& evt)
{
	int idx = m_ctrlLocation->GetSelection();
	wxString str;
	if (wxNOT_FOUND != idx)
		str = m_ctrlLocation->GetString(idx);
	UpdateLocationInfo(str.c_str());
}


void CDlgCalendar::OnKillfocusLocation(wxFocusEvent& evt)
{
	TransferDataFromWindow();
	UpdateLocationInfo(m_Location.c_str());
	evt.Skip();
}

void CDlgCalendar::OnLocationNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfo::eLocationInfo, m_Location, this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Location = dlg.CurrentSelection();
		ListLocations();
	}
}


void CDlgCalendar::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_dateStart > m_dateEnd)
	{
		ARBDate temp = m_dateStart;
		m_dateStart = m_dateEnd;
		m_dateEnd = temp;
	}
	if (!m_bOpeningUnknown && !m_bClosingUnknown && m_dateOpens > m_dateCloses)
	{
		ARBDate temp = m_dateOpens;
		m_dateOpens = m_dateCloses;
		m_dateCloses = temp;
	}
	if (!m_bDrawingUnknown)
	{
		if (!m_bOpeningUnknown && m_dateOpens > m_dateDraws)
		{
			wxMessageBox(_("IDS_BAD_DRAWDATE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
			m_ctrlDraws->SetFocus();
			return;
		}
		if (!m_bClosingUnknown && m_dateDraws > m_dateCloses)
		{
			wxMessageBox(_("IDS_BAD_DRAWDATE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
			m_ctrlDraws->SetFocus();
			return;
		}
	}
	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	if (CAgilityBookOptions::AutoDeleteCalendarEntries() && m_dateEnd < today)
	{
		if (wxYES != wxMessageBox(_("IDS_AUTODELETE_CAL"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING | wxYES_NO | wxNO_DEFAULT))
			return;
	}

	m_pCal->SetStartDate(m_dateStart);
	m_pCal->SetEndDate(m_dateEnd);
	if (m_ctrlEntryNot->GetValue())
		m_pCal->SetEntered(ARBCalendar::eNot);
	else if (m_ctrlEntryPlan->GetValue())
		m_pCal->SetEntered(ARBCalendar::ePlanning);
	else if (m_ctrlEntryEntered->GetValue())
		m_pCal->SetEntered(ARBCalendar::eEntered);
	m_pCal->SetIsTentative(m_bTentative);
	m_pCal->SetLocation(m_Location.c_str());
	m_pCal->SetVenue(m_Venue.c_str());
	m_pCal->SetClub(m_Club.c_str());
	if (m_bOpeningUnknown)
		m_dateOpens.clear();
	if (m_bDrawingUnknown)
		m_dateDraws.clear();
	if (m_bClosingUnknown)
		m_dateCloses.clear();
	m_pCal->SetOpeningDate(m_dateOpens);
	m_pCal->SetDrawDate(m_dateDraws);
	m_pCal->SetClosingDate(m_dateCloses);
	m_pCal->SetOnlineURL(m_OnlineUrl.c_str());
	m_pCal->SetPremiumURL(m_PremiumUrl.c_str());
	m_pCal->SetSecEmail(m_EMailSecAddr.c_str());
	if (m_ctrlAccomNot->GetValue())
		m_pCal->SetAccommodation(ARBCalendar::eAccomNone);
	else if (m_ctrlAccomNeeded->GetValue())
		m_pCal->SetAccommodation(ARBCalendar::eAccomTodo);
	else if (m_ctrlAccomMade->GetValue())
	{
		m_pCal->SetAccommodation(ARBCalendar::eAccomConfirmed);
		m_pCal->SetConfirmation(m_Confirmation.c_str());
	}
	m_pCal->SetNote(m_Notes.c_str());

	EndDialog(wxID_OK);
}
