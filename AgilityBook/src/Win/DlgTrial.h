#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2012-05-22 DRC Change KillFocus handler to text change handler.
 * @li 2012-05-07 DRC Added autocompletion to combo boxes.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Make 'Notes' button change selection.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2004-12-19 DRC Added Location/Club note information.
 */

#include "ARB/ARBTypes2.h"
#include "ARB/ARBDogClub.h"
#include "ARBCommon/ARBDate.h"
class CAgilityBookDoc;
class CAutoFillComboBox;
class CNoteButton;
class CReportListCtrl;
class CRichEditCtrl2;
class wxDatePickerCtrl;
class wxListEvent;
class wxTextUrlEvent;


class CDlgTrial : public wxDialog
{
public:
	CDlgTrial(
			CAgilityBookDoc* pDoc,
			ARBDogTrialPtr pTrial,
			wxWindow* pParent = NULL);

	bool RunsWereDeleted() const		{return m_bRunsDeleted;}

private:
	wxDatePickerCtrl* m_ctrlStart;
	ARBDate m_dateStart;
	bool m_Verified;
	wxString m_Location;
	CAutoFillComboBox* m_ctrlLocation;
	wxString m_Notes;
	CNoteButton* m_ctrlLocationNotes;
	CRichEditCtrl2* m_ctrlLocationInfo;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	CReportListCtrl* m_ctrlClubs;
	CNoteButton* m_ctrlClubNotes;
	CRichEditCtrl2* m_ctrlClubInfo;
	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogClubList m_Clubs;
	bool m_bRunsDeleted;

private:
	ARBDogClubPtr GetClubData(long index) const;
	void UpdateNotes(
			wxString const& location,
			bool bLocation,
			bool bClub);
	void ListLocations();
	void ListClubs(ARBDogClubPtr* inClub = NULL);
	void EditClub();

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnEnChangeLocation(wxCommandEvent& evt);
	void OnSelchangeLocation(wxCommandEvent& evt);
	void OnItemSelectedClubs(wxListEvent& evt);
	void OnItemActivatedClubs(wxListEvent& evt);
	void OnKeydownClubs(wxKeyEvent& evt);
	void OnLocationNotes(wxCommandEvent& evt);
	void OnClubNotes(wxCommandEvent& evt);
	void OnClubNew(wxCommandEvent& evt);
	void OnClubEdit(wxCommandEvent& evt);
	void OnClubDelete(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
