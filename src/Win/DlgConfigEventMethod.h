#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigEventMethod class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-04 Created.
 */

#include "ARB/ARBConfigScoring.h"
#include "ARBCommon/ARBDate.h"
class wxDatePickerCtrl;
class wxListEvent;


namespace dconSoft
{
namespace ARB
{
class ARBAgilityRecordBook;
} // namespace ARB
namespace ARBWin
{
class CReportListCtrl;
class CTextCtrl;
} // namespace ARBWin
class CDlgFixup;

class CDlgConfigureDataPlacement;
typedef std::shared_ptr<CDlgConfigureDataPlacement> CDlgConfigureDataPlacementPtr;


class CDlgConfigEventMethod : public wxDialog
{
public:
	CDlgConfigEventMethod(
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigScoringPtr const& inScoring,
		wxWindow* pParent = nullptr);

private:
	ARB::ARBScoringStyle GetType(int index) const;
	CDlgConfigureDataPlacementPtr GetPlacementData(int index) const;
	void UpdateButtons();
	void UpdateControls();
	void FillLevelList();
	void DoPlacementEdit();

	ARB::ARBConfigVenuePtr m_pVenue;
	ARB::ARBConfigScoringPtr m_pScoring;
	ARB::ARBConfigPlaceInfoList m_PlaceInfo;
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
	ARBWin::CTextCtrl* m_ctrlMultiply;
	wxStaticText* m_ctrlPlacementText;
	ARBWin::CReportListCtrl* m_ctrlPlacement;
	wxButton* m_ctrlPlacementNew;
	wxButton* m_ctrlPlacementEdit;
	wxButton* m_ctrlPlacementDelete;
	wxCheckBox* m_ctrlTimeFaultsCleanQ;
	wxCheckBox* m_ctrlSubtractTimeFaults;
	wxCheckBox* m_ctrlTimeFaultsUnder;
	wxCheckBox* m_ctrlTimeFaultsOver;
	wxCheckBox* m_ctrlTimeFaultsTitlingPts;
	wxStaticText* m_ctrlPointsOpeningText;
	ARBWin::CTextCtrl* m_ctrlPointsOpening;
	wxStaticText* m_ctrlPointsClosingText;
	ARBWin::CTextCtrl* m_ctrlPointsClosing;
	ARBCommon::ARBDate m_dateFrom;
	ARBCommon::ARBDate m_dateTo;
	bool m_DropFractions;
	bool m_Bonus;
	bool m_SuperQ;
	bool m_SpeedPts;
	short m_Multiply;
	bool m_TimeFaultsCleanQ;
	bool m_SubtractTimeFaults;
	bool m_TimeFaultsUnder;
	bool m_TimeFaultsOver;
	bool m_TitlingPointsRawFaults;
	short m_OpeningPts;
	short m_ClosingPts;

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

} // namespace dconSoft
