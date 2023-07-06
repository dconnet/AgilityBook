/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-05-22 Removed killfocus handlers, not needed.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Set focus to first control.
 * 2011-12-30 Fixed CGenericValidator.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix killfocus handling.
 * 2009-07-14 Fixed group box creation order.
 * 2009-02-09 Ported to wxWidgets.
 * 2008-02-01 Make 'Notes' button change selection.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-21 Added Location/Club info fields.
 */

#include "stdafx.h"
#include "DlgCalendar.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ComboBoxVenue.h"
#include "DlgInfoNote.h"
#include "NoteButton.h"

#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfig.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

wxBEGIN_EVENT_TABLE(CDlgCalendar, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgCalendar::OnOk)
wxEND_EVENT_TABLE()


CDlgCalendar::CDlgCalendar(ARBCalendarPtr const& inCal, CAgilityBookDoc* pDoc, wxWindow* pParent)
	: wxDialog()
	, m_pCal(inCal)
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
	, m_OnlineUrl(StringUtil::stringWX(m_pCal->GetOnlineURL()))
	, m_Confirmation()
	, m_PremiumUrl(StringUtil::stringWX(m_pCal->GetPremiumURL()))
	, m_EMailSecAddr(StringUtil::stringWX(m_pCal->GetSecEmail()))
	, m_Venue(StringUtil::stringWX(m_pCal->GetVenue()))
	, m_Club(StringUtil::stringWX(m_pCal->GetClub()))
	, m_Location(StringUtil::stringWX(m_pCal->GetLocation()))
	, m_Notes(StringUtil::stringWX(m_pCal->GetNote()))
	, m_ctrlEnd(nullptr)
	, m_ctrlOpens(nullptr)
	, m_ctrlDraws(nullptr)
	, m_ctrlCloses(nullptr)
	, m_ctrlEntryNot(nullptr)
	, m_ctrlEntryPlan(nullptr)
	, m_ctrlEntryPending(nullptr)
	, m_ctrlEntryEntered(nullptr)
	, m_ctrlOnlineUrlEntry(nullptr)
	, m_ctrlOnlineUrl(nullptr)
	, m_ctrlAccomNot(nullptr)
	, m_ctrlAccomNeeded(nullptr)
	, m_ctrlAccomMade(nullptr)
	, m_ctrlConfirmation(nullptr)
	, m_ctrlPremiumEntry(nullptr)
	, m_ctrlPremiumUrl(nullptr)
	, m_ctrlEMailSec(nullptr)
	, m_ctrlEMailSecAddr(nullptr)
	, m_ctrlVenue(nullptr)
	, m_ctrlClub(nullptr)
	, m_ctrlClubNotes(nullptr)
	, m_ctrlClubInfo(nullptr)
	, m_ctrlLocation(nullptr)
	, m_ctrlLocationNotes(nullptr)
	, m_ctrlLocationInfo(nullptr)
{
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

	wxStaticText* textStart
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_DATE_START"), wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	wxDatePickerCtrl* ctrlStart = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateStart));
	ctrlStart->Bind(wxEVT_DATE_CHANGED, &CDlgCalendar::OnDatetimechangeStart, this);
	ctrlStart->SetHelpText(_("HIDC_CAL_DATE_START"));
	ctrlStart->SetToolTip(_("HIDC_CAL_DATE_START"));

	wxStaticText* textEnd
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_DATE_END"), wxDefaultPosition, wxDefaultSize, 0);
	textEnd->Wrap(-1);

	m_ctrlEnd = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateEnd));
	m_ctrlEnd->SetHelpText(_("HIDC_CAL_DATE_END"));
	m_ctrlEnd->SetToolTip(_("HIDC_CAL_DATE_END"));

	wxCheckBox* ctrlTentative = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CAL_TENTATIVE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bTentative));
	ctrlTentative->SetHelpText(_("HIDC_CAL_TENTATIVE"));
	ctrlTentative->SetToolTip(_("HIDC_CAL_TENTATIVE"));

	wxStaticText* textOpens
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_DATE_OPENS"), wxDefaultPosition, wxDefaultSize, 0);
	textOpens->Wrap(-1);

	m_ctrlOpens = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateOpens));
	m_ctrlOpens->SetHelpText(_("HIDC_CAL_DATE_OPENS"));
	m_ctrlOpens->SetToolTip(_("HIDC_CAL_DATE_OPENS"));
	m_ctrlOpens->Enable(!m_bOpeningUnknown);

	wxCheckBox* ctrlOpensUnknown = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CAL_DATE_OPENS_UNKNOWN"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bOpeningUnknown));
	ctrlOpensUnknown->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgCalendar::OnDateOpensUnknown, this);
	ctrlOpensUnknown->SetHelpText(_("HIDC_CAL_DATE_OPENS_UNKNOWN"));
	ctrlOpensUnknown->SetToolTip(_("HIDC_CAL_DATE_OPENS_UNKNOWN"));

	wxStaticText* textDraws
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_DATE_DRAWS"), wxDefaultPosition, wxDefaultSize, 0);
	textDraws->Wrap(-1);

	m_ctrlDraws = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateDraws));
	m_ctrlDraws->SetHelpText(_("HIDC_CAL_DATE_DRAWS"));
	m_ctrlDraws->SetToolTip(_("HIDC_CAL_DATE_DRAWS"));
	m_ctrlDraws->Enable(!m_bDrawingUnknown);

	wxCheckBox* ctrDrawsUnknown = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CAL_DATE_DRAWS_UNKNOWN"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bDrawingUnknown));
	ctrDrawsUnknown->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgCalendar::OnDateDrawsUnknown, this);
	ctrDrawsUnknown->SetHelpText(_("HIDC_CAL_DATE_DRAWS_UNKNOWN"));
	ctrDrawsUnknown->SetToolTip(_("HIDC_CAL_DATE_DRAWS_UNKNOWN"));

	wxStaticText* textCloses
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_DATE_CLOSES"), wxDefaultPosition, wxDefaultSize, 0);
	textCloses->Wrap(-1);

	m_ctrlCloses = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateCloses));
	m_ctrlCloses->SetHelpText(_("HIDC_CAL_DATE_CLOSES"));
	m_ctrlCloses->SetToolTip(_("HIDC_CAL_DATE_CLOSES"));
	m_ctrlCloses->Enable(!m_bClosingUnknown);

	wxCheckBox* ctrlClosesUnknown = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CAL_DATE_CLOSES_UNKNOWN"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bClosingUnknown));
	ctrlClosesUnknown->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgCalendar::OnDateClosesUnknown, this);
	ctrlClosesUnknown->SetHelpText(_("HIDC_CAL_DATE_CLOSES_UNKNOWN"));
	ctrlClosesUnknown->SetToolTip(_("HIDC_CAL_DATE_CLOSES_UNKNOWN"));

	wxStaticBox* boxEntry = new wxStaticBox(this, wxID_ANY, _("IDC_CAL_ENTER"));

	m_ctrlEntryNot
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ENTER_NOT"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlEntryNot->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnCalEntry, this);
	m_ctrlEntryNot->SetHelpText(_("HIDC_CAL_ENTER_NOT"));
	m_ctrlEntryNot->SetToolTip(_("HIDC_CAL_ENTER_NOT"));

	m_ctrlEntryPlan
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ENTER_PLANNING"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEntryPlan->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnCalEntry, this);
	m_ctrlEntryPlan->SetHelpText(_("HIDC_CAL_ENTER_PLANNING"));
	m_ctrlEntryPlan->SetToolTip(_("HIDC_CAL_ENTER_PLANNING"));

	m_ctrlEntryPending
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ENTER_PENDING"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEntryPending->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnCalEntry, this);
	m_ctrlEntryPending->SetHelpText(_("HIDC_CAL_ENTER_PENDING"));
	m_ctrlEntryPending->SetToolTip(_("HIDC_CAL_ENTER_PENDING"));

	m_ctrlEntryEntered
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ENTER_ENTERED"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEntryEntered->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnCalEntry, this);
	m_ctrlEntryEntered->SetHelpText(_("HIDC_CAL_ENTER_ENTERED"));
	m_ctrlEntryEntered->SetToolTip(_("HIDC_CAL_ENTER_ENTERED"));

	switch (m_pCal->GetEntered())
	{
	case ARBCalendarEntry::Not:
		m_ctrlEntryNot->SetValue(true);
		break;
	case ARBCalendarEntry::Planning:
		m_ctrlEntryPlan->SetValue(true);
		break;
	case ARBCalendarEntry::Pending:
		m_ctrlEntryPending->SetValue(true);
		break;
	case ARBCalendarEntry::Entered:
		m_ctrlEntryEntered->SetValue(true);
		break;
	}

	m_ctrlOnlineUrlEntry
		= new wxButton(this, wxID_ANY, _("IDC_CAL_ONLINE_ENTRY"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlOnlineUrlEntry->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendar::OnOnlineEntry, this);
	m_ctrlOnlineUrlEntry->SetHelpText(_("HIDC_CAL_ONLINE_ENTRY"));
	m_ctrlOnlineUrlEntry->SetToolTip(_("HIDC_CAL_ONLINE_ENTRY"));
	if (ARBCalendarEntry::Planning != m_pCal->GetEntered() || m_OnlineUrl.empty())
		m_ctrlOnlineUrlEntry->Enable(false);

	m_ctrlOnlineUrl = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_OnlineUrl, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlOnlineUrl->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgCalendar::OnEnChangeCalOnlineUrl, this);
	m_ctrlOnlineUrl->SetHelpText(_("HIDC_CAL_ONLINE_URL"));
	m_ctrlOnlineUrl->SetToolTip(_("HIDC_CAL_ONLINE_URL"));

	wxStaticBox* boxAccom = new wxStaticBox(this, wxID_ANY, _("IDC_CAL_ACCOM"));

	m_ctrlAccomNot
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ACCOM_NONE"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlAccomNot->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnAccommodation, this);
	m_ctrlAccomNot->SetHelpText(_("HIDC_CAL_ACCOM_NONE"));
	m_ctrlAccomNot->SetToolTip(_("HIDC_CAL_ACCOM_NONE"));

	m_ctrlAccomNeeded
		= new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ACCOM_NEEDED"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAccomNeeded->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnAccommodation, this);
	m_ctrlAccomNeeded->SetHelpText(_("HIDC_CAL_ACCOM_NEEDED"));
	m_ctrlAccomNeeded->SetToolTip(_("HIDC_CAL_ACCOM_NEEDED"));

	m_ctrlAccomMade = new wxRadioButton(this, wxID_ANY, _("IDC_CAL_ACCOM_MADE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAccomMade->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgCalendar::OnAccommodation, this);
	m_ctrlAccomMade->SetHelpText(_("HIDC_CAL_ACCOM_MADE"));
	m_ctrlAccomMade->SetToolTip(_("HIDC_CAL_ACCOM_MADE"));

	switch (m_pCal->GetAccommodation())
	{
	case ARBAccommodations::None:
		m_ctrlAccomNot->SetValue(true);
		break;
	case ARBAccommodations::Todo:
		m_ctrlAccomNeeded->SetValue(true);
		break;
	case ARBAccommodations::Confirmed:
		m_ctrlAccomMade->SetValue(true);
		m_Confirmation = StringUtil::stringWX(m_pCal->GetConfirmation());
		break;
	}

	m_ctrlConfirmation = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_Confirmation, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlConfirmation->SetHelpText(_("HIDC_CAL_ACCOM_CONFIRMATION"));
	m_ctrlConfirmation->SetToolTip(_("HIDC_CAL_ACCOM_CONFIRMATION"));
	m_ctrlConfirmation->Enable(ARBAccommodations::Confirmed == m_pCal->GetAccommodation());

	m_ctrlPremiumEntry = new wxButton(this, wxID_ANY, _("IDC_CAL_PREMIUM_ENTRY"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPremiumEntry->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendar::OnPremiumEntry, this);
	m_ctrlPremiumEntry->SetHelpText(_("HIDC_CAL_PREMIUM_ENTRY"));
	m_ctrlPremiumEntry->SetToolTip(_("HIDC_CAL_PREMIUM_ENTRY"));
	if (m_PremiumUrl.empty())
		m_ctrlPremiumEntry->Enable(false);

	m_ctrlPremiumUrl = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_PremiumUrl, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlPremiumUrl->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgCalendar::OnEnChangeCalPremiumUrl, this);
	m_ctrlPremiumUrl->SetHelpText(_("HIDC_CAL_PREMIUM_URL"));
	m_ctrlPremiumUrl->SetToolTip(_("HIDC_CAL_PREMIUM_URL"));

	m_ctrlEMailSec = new wxButton(this, wxID_ANY, _("IDC_CAL_EMAIL_SEC"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEMailSec->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendar::OnEmailSec, this);
	m_ctrlEMailSec->SetHelpText(_("HIDC_CAL_EMAIL_SEC"));
	m_ctrlEMailSec->SetToolTip(_("HIDC_CAL_EMAIL_SEC"));
	if (m_EMailSecAddr.empty())
		m_ctrlEMailSec->Enable(false);

	std::set<std::wstring> addrs;
	for (auto const& cal : m_pDoc->Book().GetCalendar())
	{
		addrs.insert(cal->GetSecEmail());
	}
	wxArrayString items;
	for (auto const& addr : addrs)
	{
		if (!addr.empty())
			items.Add(addr);
	}

	m_ctrlEMailSecAddr = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_EMailSecAddr, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlEMailSecAddr->AutoComplete(items);
	m_ctrlEMailSecAddr->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgCalendar::OnEnChangeCalEmailSecAddr, this);
	m_ctrlEMailSecAddr->SetHelpText(_("HIDC_CAL_EMAIL_SEC_ADDR"));
	m_ctrlEMailSecAddr->SetToolTip(_("HIDC_CAL_EMAIL_SEC_ADDR"));

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY, _("IDC_CAL_VENUE"), wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenue = new CVenueComboBox(
		this,
		m_pDoc->Book().GetConfig().GetVenues(),
		m_Venue,
		false,
		CTrimValidator(&m_Venue, TRIMVALIDATOR_TRIM_BOTH),
		true);
	m_ctrlVenue->SetHelpText(_("HIDC_CAL_VENUE"));
	m_ctrlVenue->SetToolTip(_("HIDC_CAL_VENUE"));

	wxStaticText* textClub = new wxStaticText(this, wxID_ANY, _("IDC_CAL_CLUB"), wxDefaultPosition, wxDefaultSize, 0);
	textClub->Wrap(-1);

	m_ctrlClub = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Club, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlClub->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgCalendar::OnSelchangeClub, this);
	m_ctrlClub->SetHelpText(_("HIDC_CAL_CLUB"));
	m_ctrlClub->SetToolTip(_("HIDC_CAL_CLUB"));

	m_ctrlClubNotes = new CNoteButton(this);
	m_ctrlClubNotes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendar::OnClubNotes, this);
	m_ctrlClubNotes->SetHelpText(_("HIDC_CAL_CLUB_NOTES"));
	m_ctrlClubNotes->SetToolTip(_("HIDC_CAL_CLUB_NOTES"));

	m_ctrlClubInfo = new CRichEditCtrl2(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 40)),
		true);
	m_ctrlClubInfo->SetHelpText(_("HIDC_CAL_CLUB_NOTE"));
	m_ctrlClubInfo->SetToolTip(_("HIDC_CAL_CLUB_NOTE"));

	wxStaticText* textLocation
		= new wxStaticText(this, wxID_ANY, _("IDC_CAL_LOCATION"), wxDefaultPosition, wxDefaultSize, 0);
	textLocation->Wrap(-1);

	m_ctrlLocation = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Location, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlLocation->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgCalendar::OnSelchangeLocation, this);
	m_ctrlLocation->SetHelpText(_("HIDC_CAL_LOCATION"));
	m_ctrlLocation->SetToolTip(_("HIDC_CAL_LOCATION"));

	m_ctrlLocationNotes = new CNoteButton(this);
	m_ctrlLocationNotes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendar::OnLocationNotes, this);
	m_ctrlLocationNotes->SetHelpText(_("HIDC_CAL_LOCATION_NOTES"));
	m_ctrlLocationNotes->SetToolTip(_("HIDC_CAL_LOCATION_NOTES"));

	m_ctrlLocationInfo = new CRichEditCtrl2(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 40)),
		true);
	m_ctrlLocationInfo->SetHelpText(_("HIDC_CAL_LOCATION_NOTE"));
	m_ctrlLocationInfo->SetToolTip(_("HIDC_CAL_LOCATION_NOTE"));

	wxStaticText* textNotes = new wxStaticText(this, wxID_ANY, _("IDC_CAL_NOTES"), wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CSpellCheckCtrl* ctrlNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 40)),
		wxTE_MULTILINE,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_CAL_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_CAL_NOTES"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDates = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerTrialInfo = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerDates1 = new wxFlexGridSizer(2, 2, padding.Inner(), padding.Inner());
	sizerDates1->SetFlexibleDirection(wxBOTH);
	sizerDates1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates1->Add(textStart, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates1->Add(ctrlStart, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates1->Add(textEnd, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates1->Add(m_ctrlEnd, 0, wxALIGN_CENTER_VERTICAL);

	sizerTrialInfo->Add(sizerDates1, 0, wxEXPAND | wxBOTTOM, padding.Controls());
	sizerTrialInfo->Add(ctrlTentative);

	sizerDates->Add(sizerTrialInfo, 0, wxEXPAND | wxRIGHT, padding.Controls());

	wxFlexGridSizer* sizerDates2 = new wxFlexGridSizer(3, 3, padding.Inner(), padding.Inner());
	sizerDates2->SetFlexibleDirection(wxBOTH);
	sizerDates2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates2->Add(textOpens, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates2->Add(m_ctrlOpens, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates2->Add(ctrlOpensUnknown, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates2->Add(textDraws, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates2->Add(m_ctrlDraws, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates2->Add(ctrDrawsUnknown, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates2->Add(textCloses, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates2->Add(m_ctrlCloses, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates2->Add(ctrlClosesUnknown, 0, wxALIGN_CENTER_VERTICAL);

	sizerDates->Add(sizerDates2, 0, wxEXPAND);

	bSizer->Add(sizerDates, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerContent = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerEntry = new wxStaticBoxSizer(boxEntry, wxVERTICAL);
	sizerEntry->Add(m_ctrlEntryNot, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerEntry->Add(m_ctrlEntryPlan, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());

	wxBoxSizer* sizerOnline = new wxBoxSizer(wxHORIZONTAL);
	sizerOnline->AddSpacer(padding.CheckboxOffset() + padding.Inner());
	sizerOnline->Add(m_ctrlOnlineUrlEntry, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerOnline->Add(m_ctrlOnlineUrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());

	sizerEntry->Add(sizerOnline, 0, wxEXPAND | wxTOP, padding.Tight());
	sizerEntry->Add(m_ctrlEntryPending, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerEntry->Add(m_ctrlEntryEntered, 0, wxALL, padding.Inner());

	sizer1->Add(sizerEntry, 0, wxEXPAND);

	wxBoxSizer* sizerPremium = new wxBoxSizer(wxHORIZONTAL);
	sizerPremium->Add(m_ctrlPremiumEntry, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerPremium->Add(m_ctrlPremiumUrl, 1, wxALIGN_CENTER_VERTICAL);

	sizer1->Add(sizerPremium, 0, wxEXPAND | wxTOP, padding.Controls());

	wxBoxSizer* sizerSec = new wxBoxSizer(wxHORIZONTAL);
	sizerSec->Add(m_ctrlEMailSec, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerSec->Add(m_ctrlEMailSecAddr, 1, wxALIGN_CENTER_VERTICAL);

	sizer1->Add(sizerSec, 0, wxEXPAND | wxTOP, padding.Controls());

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerVenue->Add(m_ctrlVenue, 0, wxALIGN_CENTER_VERTICAL);

	sizer1->Add(sizerVenue, 0, wxEXPAND | wxTOP, padding.Controls());

	wxBoxSizer* sizerClub = new wxBoxSizer(wxHORIZONTAL);
	sizerClub->Add(textClub, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerClub->Add(m_ctrlClub, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Tight());
	sizerClub->Add(m_ctrlClubNotes, 0, wxALIGN_CENTER_VERTICAL);

	sizer1->Add(sizerClub, 0, wxEXPAND | wxTOP, padding.Controls());
	sizer1->Add(m_ctrlClubInfo, 0, wxEXPAND | wxTOP, padding.Inner());

	sizerContent->Add(sizer1, 3, wxEXPAND | wxRIGHT, padding.Controls());

	wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerAccom = new wxStaticBoxSizer(boxAccom, wxVERTICAL);
	sizerAccom->Add(m_ctrlAccomNot, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerAccom->Add(m_ctrlAccomNeeded, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());

	wxBoxSizer* sizerMade = new wxBoxSizer(wxHORIZONTAL);
	sizerMade->Add(m_ctrlAccomMade, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Tight());
	sizerMade->Add(m_ctrlConfirmation, 1, wxALIGN_CENTER_VERTICAL);

	sizerAccom->Add(sizerMade, 0, wxEXPAND | wxALL, padding.Inner());

	sizer2->Add(sizerAccom, 0, wxEXPAND);
	sizer2->AddStretchSpacer();
	sizer2->Add(textLocation);

	wxBoxSizer* sizerLocation = new wxBoxSizer(wxHORIZONTAL);
	sizerLocation->Add(m_ctrlLocation, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Tight());
	sizerLocation->Add(m_ctrlLocationNotes, 0, wxALIGN_CENTER_VERTICAL);

	sizer2->Add(sizerLocation, 0, wxEXPAND | wxBOTTOM, padding.Inner());
	sizer2->Add(m_ctrlLocationInfo, 0, wxEXPAND);

	sizerContent->Add(sizer2, 2, wxEXPAND);
	bSizer->Add(sizerContent, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerNotes = new wxBoxSizer(wxVERTICAL);
	sizerNotes->Add(textNotes, 0, wxBOTTOM, padding.Tight());
	sizerNotes->Add(ctrlNotes, 1, wxEXPAND);

	bSizer->Add(sizerNotes, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	ListLocations();
	ListClubs();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	ctrlStart->SetFocus();
}


void CDlgCalendar::UpdateLocationInfo(wxString const& location)
{
	wxString str;
	if (!location.empty())
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->Book().GetInfo().GetInfo(ARBInfoType::Location).FindItem(StringUtil::stringW(location), &pItem))
		{
			str = StringUtil::stringWX(pItem->GetComment());
		}
	}
	m_ctrlLocationInfo->SetValue(str);
}


void CDlgCalendar::ListLocations()
{
	std::set<std::wstring> locations;
	m_pDoc->Book().GetAllTrialLocations(locations, true, true);
	if (!m_pCal->GetLocation().empty())
		locations.insert(m_pCal->GetLocation());
	std::wstring loc(m_Location);
	if (m_Location.empty())
		loc = m_pCal->GetLocation();
	m_ctrlLocation->Clear();
	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	for (auto const& itLoc : locations)
	{
		items.Add(itLoc);
		if (itLoc == loc)
		{
			idxCur = static_cast<int>(items.size()) - 1;
			UpdateLocationInfo(itLoc);
		}
	}
	m_ctrlLocation->Append(items);
	m_ctrlLocation->SetSelection(idxCur);
	m_ctrlLocation->AutoComplete(items);
}


void CDlgCalendar::UpdateClubInfo(wxString const& club)
{
	wxString str;
	if (!club.empty())
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->Book().GetInfo().GetInfo(ARBInfoType::Club).FindItem(StringUtil::stringW(club), &pItem))
		{
			str = StringUtil::stringWX(pItem->GetComment());
		}
	}
	m_ctrlClubInfo->SetValue(str);
}


void CDlgCalendar::ListClubs()
{
	std::set<std::wstring> clubs;
	m_pDoc->Book().GetAllClubNames(clubs, true, true);
	if (!m_pCal->GetClub().empty())
		clubs.insert(m_pCal->GetClub());
	std::wstring club(m_Club);
	if (m_Club.empty())
		club = m_pCal->GetClub();
	m_ctrlClub->Clear();
	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	for (auto const& itClub : clubs)
	{
		items.Add(itClub);
		if (itClub == club)
		{
			idxCur = static_cast<int>(items.size()) - 1;
			UpdateClubInfo(itClub);
		}
	}
	m_ctrlClub->Append(items);
	m_ctrlClub->SetSelection(idxCur);
	m_ctrlClub->AutoComplete(items);
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
	TransferDataFromWindow();
	m_ctrlOnlineUrlEntry->Enable(m_ctrlEntryPlan->GetValue() && !m_OnlineUrl.empty());
}


void CDlgCalendar::OnEnChangeCalOnlineUrl(wxCommandEvent& evt)
{
	// Note: Do not call TransferDataFromWindow from any text change event.
	// wxGenericValidator uses SetValue to change a value which generates
	// another event. During dialog initialization, this means we would then
	// force the data from the controls into the variables at the same time
	// we're updating the controls from the variables. This is really just
	// an issue during dialog startup.
	wxString s = m_ctrlOnlineUrl->GetValue();
	m_ctrlOnlineUrlEntry->Enable(m_ctrlEntryPlan->GetValue() && !s.empty());
}


void CDlgCalendar::OnOnlineEntry(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxLaunchDefaultBrowser(m_OnlineUrl);
}


void CDlgCalendar::OnEnChangeCalPremiumUrl(wxCommandEvent& evt)
{
	wxString s = m_ctrlPremiumUrl->GetValue();
	m_ctrlPremiumEntry->Enable(!s.empty());
}


void CDlgCalendar::OnPremiumEntry(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxLaunchDefaultBrowser(m_PremiumUrl);
}


void CDlgCalendar::OnEnChangeCalEmailSecAddr(wxCommandEvent& evt)
{
	wxString s = m_ctrlEMailSecAddr->GetValue();
	m_ctrlEMailSec->Enable(!s.empty());
}


void CDlgCalendar::OnEmailSec(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	wxString s(m_EMailSecAddr);
	if (0 != m_EMailSecAddr.Find(L"mailto:"))
		s = L"mailto:" + m_EMailSecAddr;
	wxLaunchDefaultBrowser(s);
}


void CDlgCalendar::OnAccommodation(wxCommandEvent& evt)
{
	m_ctrlConfirmation->Enable(m_ctrlAccomMade->GetValue());
}


void CDlgCalendar::OnSelchangeClub(wxCommandEvent& evt)
{
	wxString s = m_ctrlClub->GetValue();
	UpdateClubInfo(s);
}


void CDlgCalendar::OnClubNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfoType::Club, StringUtil::stringW(m_Club), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Club = StringUtil::stringWX(dlg.CurrentSelection());
		ListClubs();
	}
}


void CDlgCalendar::OnSelchangeLocation(wxCommandEvent& evt)
{
	wxString s = m_ctrlLocation->GetValue();
	UpdateLocationInfo(s);
}


void CDlgCalendar::OnLocationNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfoType::Location, StringUtil::stringW(m_Location), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Location = StringUtil::stringWX(dlg.CurrentSelection());
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
			wxMessageBox(_("IDS_BAD_DRAWDATE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
			m_ctrlDraws->SetFocus();
			return;
		}
		if (!m_bClosingUnknown && m_dateDraws > m_dateCloses)
		{
			wxMessageBox(_("IDS_BAD_DRAWDATE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
			m_ctrlDraws->SetFocus();
			return;
		}
	}
	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	if (CAgilityBookOptions::AutoDeleteCalendarEntries() && m_dateEnd < today)
	{
		if (wxYES
			!= wxMessageBox(
				_("IDS_AUTODELETE_CAL"),
				_("Agility Record Book"),
				wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_WARNING))
			return;
	}

	m_pCal->SetStartDate(m_dateStart);
	m_pCal->SetEndDate(m_dateEnd);
	if (m_ctrlEntryNot->GetValue())
		m_pCal->SetEntered(ARBCalendarEntry::Not);
	else if (m_ctrlEntryPlan->GetValue())
		m_pCal->SetEntered(ARBCalendarEntry::Planning);
	else if (m_ctrlEntryPending->GetValue())
		m_pCal->SetEntered(ARBCalendarEntry::Pending);
	else if (m_ctrlEntryEntered->GetValue())
		m_pCal->SetEntered(ARBCalendarEntry::Entered);
	m_pCal->SetIsTentative(m_bTentative);
	m_pCal->SetLocation(StringUtil::stringW(m_Location));
	m_pCal->SetVenue(StringUtil::stringW(m_Venue));
	m_pCal->SetClub(StringUtil::stringW(m_Club));
	if (m_bOpeningUnknown)
		m_dateOpens.clear();
	if (m_bDrawingUnknown)
		m_dateDraws.clear();
	if (m_bClosingUnknown)
		m_dateCloses.clear();
	m_pCal->SetOpeningDate(m_dateOpens);
	m_pCal->SetDrawDate(m_dateDraws);
	m_pCal->SetClosingDate(m_dateCloses);
	m_pCal->SetOnlineURL(StringUtil::stringW(m_OnlineUrl));
	m_pCal->SetPremiumURL(StringUtil::stringW(m_PremiumUrl));
	m_pCal->SetSecEmail(StringUtil::stringW(m_EMailSecAddr));
	if (m_ctrlAccomNot->GetValue())
		m_pCal->SetAccommodation(ARBAccommodations::None);
	else if (m_ctrlAccomNeeded->GetValue())
		m_pCal->SetAccommodation(ARBAccommodations::Todo);
	else if (m_ctrlAccomMade->GetValue())
	{
		m_pCal->SetAccommodation(ARBAccommodations::Confirmed);
		m_pCal->SetConfirmation(StringUtil::stringW(m_Confirmation));
	}
	m_pCal->SetNote(StringUtil::stringW(m_Notes));

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}

} // namespace dconSoft
