#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigEvent class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-18 Added a time fault multiplier.
 * 2004-11-15 Added time fault computation option on T+F.
 */

#include "ARB/ARBConfigScoring.h"
#include "LibARBWin/Logger.h"


namespace dconSoft
{
namespace ARBWin
{
class CSpellCheckCtrl;
class CTextCtrl;
} // namespace ARBWin
class CConfigEventDataLifetimePoints;
class CConfigEventDataPlaceInfo;
class CConfigEventDataScoring;
class CConfigEventDataTitlePoints;


class CDlgConfigEvent : public wxDialog
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgConfigEvent)
public:
	CDlgConfigEvent(
		bool bNewEntry,
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigEventPtr const& inEvent,
		wxWindow* pParent = nullptr);
	~CDlgConfigEvent();

	void GetFixups(std::vector<ARB::ARBConfigActionPtr>& ioFixups);

private:
	void ClearFixups();
	CConfigEventDataScoring* GetScoringData(int index) const;
	CConfigEventDataTitlePoints* GetTitleData(int index) const;
	CConfigEventDataLifetimePoints* GetLifetimeData(int index) const;
	CConfigEventDataPlaceInfo* GetPlacementData(int index) const;
	wxString GetListName(ARB::ARBConfigScoringPtr const& inScoring) const;
	void EnableSubnameControls();
	void EditSubname();
	void UpdateSubnames();
	void EnablePointsControls();
	void ShowSubNames(bool bShow);
	void FillControls();
	void FillMethodList();
	void FillTitlePoints(ARB::ARBConfigScoringPtr const& inScoring);
	bool SaveControls();
	void EditMethod();
	void EditPoints();

	bool m_bNewEntry;
	ARB::ARBConfigVenuePtr m_pVenue;
	ARB::ARBConfigEventPtr m_pEvent;
	std::vector<ARB::ARBConfigActionPtr> m_DlgFixup;
	ARB::ARBConfigScoringList m_Scorings;
	wxString m_Name;
	bool m_bHasPartners;
	wxString m_Desc;
	ARBWin::CTextCtrl* m_ctrlName;
	wxCheckBox* m_ctrlTable;
	wxCheckBox* m_ctrlHasSubnames;
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
	ARBWin::CSpellCheckCtrl* m_ctrlNote;
	int m_idxMethod;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnClickedTable(wxCommandEvent& evt);
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

} // namespace dconSoft
