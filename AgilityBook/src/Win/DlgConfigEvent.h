#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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

	DECLARE_ON_INIT()
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
	void OnDblclickConfigInfo(wxCommandEvent& evt);
	void OnSelchangePoints(wxCommandEvent& evt);
	void OnDblclkPoints(wxCommandEvent& evt);
	void OnPointsNew(wxCommandEvent& evt);
	void OnPointsEdit(wxCommandEvent& evt);
	void OnPointsDelete(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
