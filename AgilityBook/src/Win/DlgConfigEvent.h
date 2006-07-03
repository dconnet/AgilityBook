#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-11-15 DRC Added time fault computation option on T+F.
 */

#include "ARBConfigScoring.h"
#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "ListBox.h"
#include "ListData.h"
class ARBAgilityRecordBook;
class CDlgFixup;

class CDlgConfigEvent : public CDlgBaseDialog
{
public:
	CDlgConfigEvent(
			CAgilityBookDoc* pDoc,
			ARBAgilityRecordBook const* book,
			ARBConfigVenuePtr pVenue,
			ARBConfigEventPtr pEvent,
			CWnd* pParent = NULL);
	virtual ~CDlgConfigEvent();
	void GetFixups(std::vector<CDlgFixup*>& ioFixups);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigEvent)
	enum { IDD = IDD_CONFIG_EVENT };
	CString m_Name;
	BOOL	m_bHasTable;
	BOOL	m_bHasPartners;
	BOOL	m_bHasSubNames;
	CListBox2	m_ctrlSubNames;
	CButton m_ctrlSubNamesNew;
	CButton m_ctrlSubNamesDelete;
	CString	m_Desc;
	CButton m_ctrlNew;
	CButton m_ctrlCopy;
	CButton m_ctrlDelete;
	CButton m_ctrlUp;
	CButton m_ctrlDown;
	CListBox2 m_ctrlMethods;
	CListBox2 m_ctrlUnused;
	CButton	m_ctrlValidFrom;
	CDateTimeCtrl	m_ctrlDateFrom;
	CButton	m_ctrlValidTo;
	CDateTimeCtrl	m_ctrlDateTo;
	CComboBox2 m_ctrlDivision;
	CComboBox2 m_ctrlLevel;
	CComboBox2 m_ctrlType;
	CStatic	m_ctrlPointsOpeningText;
	CEdit	m_ctrlPointsOpening;
	short	m_OpeningPts;
	CStatic	m_ctrlPointsClosingText;
	CEdit	m_ctrlPointsClosing;
	short	m_ClosingPts;
	CButton	m_ctrlDropFractions;
	CButton	m_ctrlTimeFaultsCleanQ;
	CButton	m_ctrlSubtractTimeFaults;
	CButton	m_ctrlTimeFaultsUnder;
	CButton	m_ctrlTimeFaultsOver;
	CEdit	m_ctrlNote;
	CButton m_ctrlSpeedPts;
	CButton m_ctrlSuperQ;
	CButton	m_ctrlBonus;
	CStatic	m_ctrlMultiplyText;
	CEdit	m_ctrlMultiply;
	short	m_Multiply;
	CListBox2	m_ctrlPointsList;
	CButton	m_ctrlPointsNew;
	CButton	m_ctrlPointsEdit;
	CButton	m_ctrlPointsDelete;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBAgilityRecordBook const* m_Book;
	ARBConfigVenuePtr m_pVenue;
	ARBConfigEventPtr m_pEvent;
	std::vector<CDlgFixup*> m_DlgFixup;
	ARBConfigScoringList m_Scorings;
	int m_idxMethod;
	CString m_strOpening[2];

	CListPtrData<ARBConfigScoringPtr>* GetScoringData(int index) const;
	CListPtrData<ARBConfigTitlePointsPtr>* GetTitleData(int index) const;
	CListPtrData<ARBConfigLifetimePointsPtr>* GetLifetimeData(int index) const;
	void ClearFixups();
	CString GetListName(ARBConfigScoringPtr pScoring) const;
	void FillControls();
	void FillSubNames(bool bInit = false);
	void FillMethodList();
	void FillDivisionList();
	void FillLevelList();
	void FillRequiredPoints();
	void FillTitlePoints(ARBConfigScoringPtr pScoring);
	bool SaveControls();

	//{{AFX_VIRTUAL(CDlgConfigEvent)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgConfigEvent)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSubNames();
	afx_msg void OnLbnSelchangeSubnames();
	afx_msg void OnBnClickedSubNamesNew();
	afx_msg void OnBnClickedSubNamesDelete();
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnValidFrom();
	afx_msg void OnValidTo();
	afx_msg void OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeMethods();
	afx_msg void OnCbnSelchangeDivision();
	afx_msg void OnCbnSelchangeLevel();
	afx_msg void OnSelchangeType();
	afx_msg void OnSelchangePoints();
	afx_msg void OnDblclkPoints();
	afx_msg void OnPointsNew();
	afx_msg void OnPointsEdit();
	afx_msg void OnPointsDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
