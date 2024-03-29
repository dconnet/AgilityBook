#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-01-21 Added Location/Club info fields.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/Logger.h"
class wxDateEvent;
class wxDatePickerCtrl;


namespace dconSoft
{
namespace ARBWin
{
class CRichEditCtrl2;
class CTextCtrl;
} // namespace ARBWin
class CAgilityBookDoc;
class CNoteButton;
class CVenueComboBox;


class CDlgCalendar : public wxDialog
{
public:
	CDlgCalendar(ARB::ARBCalendarPtr const& inCal, CAgilityBookDoc* pDoc, wxWindow* pParent = nullptr);

private:
	void UpdateLocationInfo(wxString const& location);
	void ListLocations();
	void UpdateClubInfo(wxString const& club);
	void ListClubs();

	ARB::ARBCalendarPtr m_pCal;
	CAgilityBookDoc* m_pDoc;
	int m_Span;
	ARBCommon::ARBDate m_dateStart;
	ARBCommon::ARBDate m_dateEnd;
	bool m_bTentative;
	ARBCommon::ARBDate m_dateOpens;
	bool m_bOpeningUnknown;
	ARBCommon::ARBDate m_dateDraws;
	bool m_bDrawingUnknown;
	ARBCommon::ARBDate m_dateCloses;
	bool m_bClosingUnknown;
	wxString m_OnlineUrl;
	wxString m_Confirmation;
	wxString m_PremiumUrl;
	wxString m_EMailSecAddr;
	wxString m_Venue;
	wxString m_Club;
	wxString m_Location;
	wxString m_Notes;
	wxDatePickerCtrl* m_ctrlEnd;
	wxDatePickerCtrl* m_ctrlOpens;
	wxDatePickerCtrl* m_ctrlDraws;
	wxDatePickerCtrl* m_ctrlCloses;
	wxRadioButton* m_ctrlEntryNot;
	wxRadioButton* m_ctrlEntryPlan;
	wxRadioButton* m_ctrlEntryPending;
	wxRadioButton* m_ctrlEntryEntered;
	wxButton* m_ctrlOnlineUrlEntry;
	ARBWin::CTextCtrl* m_ctrlOnlineUrl;
	wxRadioButton* m_ctrlAccomNot;
	wxRadioButton* m_ctrlAccomNeeded;
	wxRadioButton* m_ctrlAccomMade;
	ARBWin::CTextCtrl* m_ctrlConfirmation;
	wxButton* m_ctrlPremiumEntry;
	ARBWin::CTextCtrl* m_ctrlPremiumUrl;
	wxButton* m_ctrlEMailSec;
	wxComboBox* m_ctrlEMailSecAddr;
	CVenueComboBox* m_ctrlVenue;
	wxComboBox* m_ctrlClub;
	CNoteButton* m_ctrlClubNotes;
	ARBWin::CRichEditCtrl2* m_ctrlClubInfo;
	wxComboBox* m_ctrlLocation;
	CNoteButton* m_ctrlLocationNotes;
	ARBWin::CRichEditCtrl2* m_ctrlLocationInfo;
	ARBWin::CStackLogger m_trace;

	void OnDatetimechangeStart(wxDateEvent& evt);
	void OnDateOpensUnknown(wxCommandEvent& evt);
	void OnDateDrawsUnknown(wxCommandEvent& evt);
	void OnDateClosesUnknown(wxCommandEvent& evt);
	void OnCalEntry(wxCommandEvent& evt);
	void OnEnChangeCalOnlineUrl(wxCommandEvent& evt);
	void OnOnlineEntry(wxCommandEvent& evt);
	void OnEnChangeCalPremiumUrl(wxCommandEvent& evt);
	void OnPremiumEntry(wxCommandEvent& evt);
	void OnEnChangeCalEmailSecAddr(wxCommandEvent& evt);
	void OnEmailSec(wxCommandEvent& evt);
	void OnAccommodation(wxCommandEvent& evt);
	void OnSelchangeClub(wxCommandEvent& evt);
	void OnClubNotes(wxCommandEvent& evt);
	void OnSelchangeLocation(wxCommandEvent& evt);
	void OnLocationNotes(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
