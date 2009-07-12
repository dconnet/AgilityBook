#pragma once

/*
 * Copyright © 2006-2009 David Connet. All Rights Reserved.
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
class wxDatePickerCtrl;
class wxListEvent;

class CDlgConfigureDataPlacement;
typedef tr1::shared_ptr<CDlgConfigureDataPlacement> CDlgConfigureDataPlacementPtr;


class CDlgConfigEventMethod : public wxDialog
{
	friend int wxCALLBACK ComparePlacement(long item1, long item2, long sortData);
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
	wxTextCtrl* m_ctrlMultiply;
	wxStaticText* m_ctrlPlacementText;
	CReportListCtrl* m_ctrlPlacement;
	wxButton* m_ctrlPlacementNew;
	wxButton* m_ctrlPlacementEdit;
	wxButton* m_ctrlPlacementDelete;
	wxCheckBox* m_ctrlTimeFaultsCleanQ;
	wxCheckBox* m_ctrlTimeFaultsUnder;
	wxCheckBox* m_ctrlTimeFaultsOver;
	wxCheckBox* m_ctrlSubtractTimeFaults;
	wxStaticText* m_ctrlPointsOpeningText;
	wxTextCtrl* m_ctrlPointsOpening;
	wxStaticText* m_ctrlPointsClosingText;
	wxTextCtrl* m_ctrlPointsClosing;
	ARBDate m_dateFrom;
	ARBDate m_dateTo;
	bool m_DropFractions;
	bool m_Bonus;
	bool m_SuperQ;
	bool m_SpeedPts;
	short m_Multiply;
	bool m_TimeFaultsCleanQ;
	bool m_TimeFaultsUnder;
	bool m_TimeFaultsOver;
	bool m_SubtractTimeFaults;
	short m_OpeningPts;
	short m_ClosingPts;

	DECLARE_EVENT_TABLE()
	void OnSelchangeDivision(wxCommandEvent& evt);
	void OnValidFrom(wxCommandEvent& evt);
	void OnValidTo(wxCommandEvent& evt);
	void OnSelchangeType(wxCommandEvent& evt);
	void OnSpeedPoints(wxCommandEvent& evt);
	void OnItemchangedPlacement(wxListEvent& evt);
	void OnItemActivatedPlacement(wxListEvent& evt);
	void OnKeydownPlacement(wxKeyEvent& evt);
	void OnPlacementNew(wxCommandEvent& evt);
	void OnPlacementEdit(wxCommandEvent& evt);
	void OnPlacementDelete(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
