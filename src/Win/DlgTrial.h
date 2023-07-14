#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2009-02-09 Ported to wxWidgets.
 * 2008-02-01 Make 'Notes' button change selection.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2004-12-19 Added Location/Club note information.
 */

#include "ARB/ARBDogClub.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/Logger.h"
class wxDatePickerCtrl;
class wxListEvent;
class wxTextUrlEvent;


namespace dconSoft
{
namespace ARBWin
{
class CReportListCtrl;
class CRichEditCtrl2;
} // namespace ARBWin
class CAgilityBookDoc;
class CNoteButton;


class CDlgTrial : public wxDialog
{
public:
	CDlgTrial(CAgilityBookDoc* pDoc, ARB::ARBDogTrialPtr const& inTrial, wxWindow* pParent = nullptr);

	bool RunsWereDeleted() const
	{
		return m_bRunsDeleted;
	}

private:
	ARB::ARBDogClubPtr GetClubData(long index) const;
	void UpdateNotes(wxString const& location, bool bLocation, bool bClub);
	void ListLocations();
	void ListClubs(ARB::ARBDogClubPtr const* inClub = nullptr);
	void EditClub();

	wxDatePickerCtrl* m_ctrlStart;
	ARBCommon::ARBDate m_dateStart;
	bool m_Verified;
	wxString m_Location;
	wxComboBox* m_ctrlLocation;
	wxString m_Notes;
	CNoteButton* m_ctrlLocationNotes;
	ARBWin::CRichEditCtrl2* m_ctrlLocationInfo;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDn;
	ARBWin::CReportListCtrl* m_ctrlClubs;
	CNoteButton* m_ctrlClubNotes;
	ARBWin::CRichEditCtrl2* m_ctrlClubInfo;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogTrialPtr m_pTrial;
	ARB::ARBDogClubList m_Clubs;
	bool m_bShowCoSanction;
	bool m_bFixup;
	bool m_bRunsDeleted;
	ARBWin::CStackLogger m_trace;

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
	void OnClubMoveUp(wxCommandEvent& evt);
	void OnClubMoveDown(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
};

} // namespace dconSoft
