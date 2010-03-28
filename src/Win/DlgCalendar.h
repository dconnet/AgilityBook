#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-01-21 DRC Added Location/Club info fields.
 */

#include "ARBDate.h"
#include "ARBTypes.h"
class CAgilityBookDoc;
class CNoteButton;
class CRichEditCtrl2;
class CTextCtrl;
class CVenueComboBox;
class wxDateEvent;
class wxDatePickerCtrl;


class CDlgCalendar : public wxDialog
{
public:
	CDlgCalendar(
			ARBCalendarPtr pCal,
			CAgilityBookDoc* pDoc,
			wxWindow* pParent = NULL);

private:
	void UpdateLocationInfo(wxChar const* pLocation);
	void ListLocations();
	void UpdateClubInfo(wxChar const* pClub);
	void ListClubs();

	ARBCalendarPtr m_pCal;
	CAgilityBookDoc* m_pDoc;
	int m_Span;
	ARBDate m_dateStart;
	ARBDate m_dateEnd;
	bool m_bTentative;
	ARBDate m_dateOpens;
	bool m_bOpeningUnknown;
	ARBDate m_dateDraws;
	bool m_bDrawingUnknown;
	ARBDate m_dateCloses;
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
	CTextCtrl* m_ctrlOnlineUrl;
	wxRadioButton* m_ctrlAccomNot;
	wxRadioButton* m_ctrlAccomNeeded;
	wxRadioButton* m_ctrlAccomMade;
	CTextCtrl* m_ctrlConfirmation;
	wxButton* m_ctrlPremiumEntry;
	CTextCtrl* m_ctrlPremiumUrl;
	wxButton* m_ctrlEMailSec;
	wxComboBox* m_ctrlEMailSecAddr;
	CVenueComboBox* m_ctrlVenue;
	wxComboBox* m_ctrlClub;
	CNoteButton* m_ctrlClubNotes;
	CRichEditCtrl2* m_ctrlClubInfo;
	wxComboBox* m_ctrlLocation;
	CNoteButton* m_ctrlLocationNotes;
	CRichEditCtrl2* m_ctrlLocationInfo;

	DECLARE_EVENT_TABLE()
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
	void OnKillfocusClub(wxFocusEvent& evt);
	void OnClubNotes(wxCommandEvent& evt);
	void OnSelchangeLocation(wxCommandEvent& evt);
	void OnKillfocusLocation(wxFocusEvent& evt);
	void OnLocationNotes(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
