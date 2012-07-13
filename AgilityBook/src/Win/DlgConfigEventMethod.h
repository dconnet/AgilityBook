#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgConfigEventMethod class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-11-04 DRC Created.
 */

#include "ARBConfigScoring.h"
class ARBAgilityRecordBook;
class CDlgFixup;
class CReportListCtrl;
class CTextCtrl;
class wxDatePickerCtrl;
class wxListEvent;

class CDlgConfigureDataPlacement;
typedef std::tr1::shared_ptr<CDlgConfigureDataPlacement> CDlgConfigureDataPlacementPtr;


class CDlgConfigEventMethod : public wxDialog
{
#if wxCHECK_VERSION(2, 9, 4)
	friend int wxCALLBACK ComparePlacement(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
#else
	friend int wxCALLBACK ComparePlacement(long item1, long item2, long sortData);
#endif
public:
	CDlgConfigEventMethod(
			ARBConfigVenuePtr pVenue,
			ARBConfigScoringPtr pScoring,
			wxWindow* pParent = NULL);

private:
	ARBConfigScoring::ScoringStyle GetType(int index) const;
	CDlgConfigureDataPlacementPtr GetPlacementData(int index) const;
	CDlgConfigureDataPlacementPtr GetPlacementDataByData(int index) const;
	void UpdateButtons();
	void UpdateControls();
	void FillLevelList();
	void DoPlacementEdit();

	ARBConfigVenuePtr m_pVenue;
	ARBConfigScoringPtr m_pScoring;
	ARBConfigPlaceInfoList m_PlaceInfo;
	wxComboBox* m_ctrlDivision;
	wxComboBox* m_ctrlLevel;
	wxCheckBox* m_ctrlValidFrom;
	wxDatePickerCtrl* m_ctrlDateFrom;
	wxCheckBox* m_ctrlValidTo;
	wxDatePickerCtrl* m_ctrlDateTo;
	wxComboBox* m_ctrlType;
	wxCheckBox* m_ctrlDropFractions;
	wxCheckBox* m_ctrlBonus;
	wxCheckBox* m_ctrlSuperQ;
	wxCheckBox* m_ctrlSpeedPts;
	wxStaticText* m_ctrlMultiplyText;
	CTextCtrl* m_ctrlMultiply;
	wxStaticText* m_ctrlPlacementText;
	CReportListCtrl* m_ctrlPlacement;
	wxButton* m_ctrlPlacementNew;
	wxButton* m_ctrlPlacementEdit;
	wxButton* m_ctrlPlacementDelete;
	wxCheckBox* m_ctrlTimeFaultsCleanQ;
	wxCheckBox* m_ctrlSubtractTimeFaults;
	wxCheckBox* m_ctrlTimeFaultsUnder;
	wxCheckBox* m_ctrlTimeFaultsOver;
	wxStaticText* m_ctrlPointsOpeningText;
	CTextCtrl* m_ctrlPointsOpening;
	wxStaticText* m_ctrlPointsClosingText;
	CTextCtrl* m_ctrlPointsClosing;
	ARBDate m_dateFrom;
	ARBDate m_dateTo;
	bool m_DropFractions;
	bool m_Bonus;
	bool m_SuperQ;
	bool m_SpeedPts;
	short m_Multiply;
	bool m_TimeFaultsCleanQ;
	bool m_SubtractTimeFaults;
	bool m_TimeFaultsUnder;
	bool m_TimeFaultsOver;
	short m_OpeningPts;
	short m_ClosingPts;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnSelchangeDivision(wxCommandEvent& evt);
	void OnValidFrom(wxCommandEvent& evt);
	void OnValidTo(wxCommandEvent& evt);
	void OnSelchangeType(wxCommandEvent& evt);
	void OnSpeedPoints(wxCommandEvent& evt);
	void OnComputeTimeFaults(wxCommandEvent& evt);
	void OnItemchangedPlacement(wxListEvent& evt);
	void OnItemActivatedPlacement(wxListEvent& evt);
	void OnKeydownPlacement(wxKeyEvent& evt);
	void OnPlacementNew(wxCommandEvent& evt);
	void OnPlacementEdit(wxCommandEvent& evt);
	void OnPlacementDelete(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
