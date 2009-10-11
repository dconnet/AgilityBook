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
 * @brief interface of the CDlgConfigEvent class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-11-15 DRC Added time fault computation option on T+F.
 */

#include "ARBConfigScoring.h"
class CConfigEventDataLifetimePoints;
class CConfigEventDataPlaceInfo;
class CConfigEventDataScoring;
class CConfigEventDataTitlePoints;
class CTextCtrl;


class CDlgConfigEvent : public wxDialog
{
public:
	CDlgConfigEvent(
			bool bNewEntry,
			ARBConfigVenuePtr pVenue,
			ARBConfigEventPtr pEvent,
			wxWindow* pParent = NULL);
	~CDlgConfigEvent();

	void GetFixups(std::vector<ARBConfigActionPtr>& ioFixups);

private:
	void ClearFixups();
	CConfigEventDataScoring* GetScoringData(int index) const;
	CConfigEventDataTitlePoints* GetTitleData(int index) const;
	CConfigEventDataLifetimePoints* GetLifetimeData(int index) const;
	CConfigEventDataPlaceInfo* GetPlacementData(int index) const;
	wxString GetListName(ARBConfigScoringPtr pScoring) const;
	void EnableSubnameControls();
	void EditSubname();
	void EnablePointsControls();
	void FillSubNames(bool bInit = false);
	void FillControls();
	void FillMethodList();
	void FillTitlePoints(ARBConfigScoringPtr pScoring);
	bool SaveControls();
	void EditMethod();
	void EditPoints();

	bool m_bNewEntry;
	ARBConfigVenuePtr m_pVenue;
	ARBConfigEventPtr m_pEvent;
	std::vector<ARBConfigActionPtr> m_DlgFixup;
	ARBConfigScoringList m_Scorings;
	wxString m_Name;
	bool m_bHasTable;
	bool m_bHasPartners;
	bool m_bHasSubNames;
	wxString m_Desc;
	CTextCtrl* m_ctrlName;
	wxListBox* m_ctrlSubNames;
	wxButton* m_ctrlSubNamesNew;
	wxButton* m_ctrlSubNamesEdit;
	wxButton* m_ctrlSubNamesDelete;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlCopy;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;
	wxListBox* m_ctrlMethods;
	wxListBox* m_ctrlUnused;
	wxStaticText* m_ctrlInfo;
	wxListBox* m_ctrlPointsList;
	wxButton* m_ctrlPointsNew;
	wxButton* m_ctrlPointsEdit;
	wxButton* m_ctrlPointsDelete;
	CTextCtrl* m_ctrlNote;
	int m_idxMethod;

	DECLARE_EVENT_TABLE()
	void OnClickedSubNames(wxCommandEvent& evt);
	void OnLbnSelchangeSubnames(wxCommandEvent& evt);
	void OnLbnDblclkSubnames(wxCommandEvent& evt);
	void OnBnClickedSubNamesNew(wxCommandEvent& evt);
	void OnBnClickedSubNamesEdit(wxCommandEvent& evt);
	void OnBnClickedSubNamesDelete(wxCommandEvent& evt);
	void OnLbnSelchangeMethods(wxCommandEvent& evt);
	void OnLbnDblclkMethods(wxCommandEvent& evt);
	void OnBnClickedNew(wxCommandEvent& evt);
	void OnBnClickedEdit(wxCommandEvent& evt);
	void OnBnClickedDelete(wxCommandEvent& evt);
	void OnBnClickedCopy(wxCommandEvent& evt);
	void OnBnClickedUp(wxCommandEvent& evt);
	void OnBnClickedDown(wxCommandEvent& evt);
	void OnDblclickConfigInfo(wxMouseEvent& evt);
	void OnSelchangePoints(wxCommandEvent& evt);
	void OnDblclkPoints(wxCommandEvent& evt);
	void OnPointsNew(wxCommandEvent& evt);
	void OnPointsEdit(wxCommandEvent& evt);
	void OnPointsDelete(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
