#pragma once

/*
 * Copyright © 2006-2008 David Connet. All Rights Reserved.
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
 * @li 2006-11-04 DRC Created.
 */

#include "ARBConfigScoring.h"
#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "ListBox.h"
#include "ListCtrl.h"
#include "ListData.h"
class ARBAgilityRecordBook;
class CDlgFixup;

class CDlgConfigEventMethod : public CDlgBaseDialog
{
public:
	CDlgConfigEventMethod(
			ARBConfigVenuePtr pVenue,
			ARBConfigScoringPtr pScoring,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigEventMethod)
	enum { IDD = IDD_CONFIG_EVENT_METHOD };
	CComboBox2	m_ctrlDivision;
	CComboBox2	m_ctrlLevel;
	CButton	m_ctrlValidFrom;
	CDateTimeCtrl	m_ctrlDateFrom;
	CButton	m_ctrlValidTo;
	CDateTimeCtrl	m_ctrlDateTo;
	CComboBox2 	m_ctrlType;
	CButton	m_ctrlDropFractions;
	BOOL	m_DropFractions;
	CButton	m_ctrlBonus;
	BOOL	m_Bonus;
	CButton	m_ctrlSuperQ;
	BOOL	m_SuperQ;
	CButton	m_ctrlSpeedPts;
	BOOL	m_SpeedPts;
	CStatic	m_ctrlMultiplyText;
	CEdit	m_ctrlMultiply;
	short	m_Multiply;
	CStatic	m_ctrlPlacementText;
	CListCtrl2	m_ctrlPlacement;
	CButton	m_ctrlPlacementNew;
	CButton	m_ctrlPlacementEdit;
	CButton	m_ctrlPlacementDelete;
	CButton	m_ctrlTimeFaultsCleanQ;
	BOOL	m_TimeFaultsCleanQ;
	CButton	m_ctrlTimeFaultsUnder;
	BOOL	m_TimeFaultsUnder;
	CButton	m_ctrlTimeFaultsOver;
	BOOL	m_TimeFaultsOver;
	CButton	m_ctrlSubtractTimeFaults;
	BOOL	m_SubtractTimeFaults;
	CStatic	m_ctrlPointsOpeningText;
	CEdit	m_ctrlPointsOpening;
	short	m_OpeningPts;
	CStatic	m_ctrlPointsClosingText;
	CEdit	m_ctrlPointsClosing;
	short	m_ClosingPts;
	//}}AFX_DATA
	ARBConfigVenuePtr m_pVenue;
	ARBConfigScoringPtr m_pScoring;
	ARBConfigPlaceInfoList m_PlaceInfo;
	CString m_strOpening[2];

	void UpdateButtons();
	void FillDivisionList();
	void FillLevelList();

	//{{AFX_VIRTUAL(CDlgConfigEventMethod)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgConfigEventMethod)
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeDivision();
	afx_msg void OnValidFrom();
	afx_msg void OnValidTo();
	afx_msg void OnSelchangeType();
	afx_msg void OnSpeedPoints();
	afx_msg void OnGetdispinfoPlacement(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPlacement(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownPlacement(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPlacement(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlacementNew();
	afx_msg void OnPlacementEdit();
	afx_msg void OnPlacementDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
