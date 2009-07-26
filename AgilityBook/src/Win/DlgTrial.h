#pragma once

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
 * @brief interface of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Make 'Notes' button change selection.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2004-12-19 DRC Added Location/Club note information.
 */

#include "ARBTypes.h"
#include "ARBDogClub.h"
class CAgilityBookDoc;
class CNoteButton;
class CReportListCtrl;
class CRichEditCtrl2;
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
	wxString m_Location;
	wxComboBox* m_ctrlLocation;
	bool m_Verified;
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
			bool bLocation,
			bool bClub);
	void ListLocations();
	void ListClubs(ARBDogClubPtr* inClub = NULL);
	void EditClub();

	DECLARE_EVENT_TABLE()
	void OnSelchangeLocation(wxCommandEvent& evt);
	void OnKillfocusLocation(wxFocusEvent& evt);
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
