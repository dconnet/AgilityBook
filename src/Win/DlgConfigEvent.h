#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 */

#include "ARBConfigScoring.h"
#include "DlgBaseDialog.h"
#include "ListBox.h"
class ARBAgilityRecordBook;
class ARBConfig;
class ARBConfigEvent;
class ARBConfigVenue;
class CDlgFixup;

class CDlgConfigEvent : public CDlgBaseDialog
{
public:
	CDlgConfigEvent(CAgilityBookDoc* pDoc,
		ARBAgilityRecordBook* book, ARBConfig* config,
		ARBConfigVenue* pVenue, ARBConfigEvent* pEvent, CWnd* pParent = NULL);
	virtual ~CDlgConfigEvent();
	void GetFixups(std::vector<CDlgFixup*>& ioFixups);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigEvent)
	enum { IDD = IDD_CONFIG_EVENT };
	CString m_Name;
	BOOL	m_bHasTable;
	BOOL	m_bHasPartners;
	CListBox2	m_ctrlSubCat;
	CButton	m_ctrlSubCatNew;
	CButton	m_ctrlSubCatEdit;
	CButton	m_ctrlSubCatDelete;
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
	CComboBox m_ctrlDivision;
	CComboBox m_ctrlLevel;
	CComboBox m_ctrlType;
	CButton	m_ctrlDropFractions;
	CStatic	m_ctrlPointsOpeningText;
	CEdit	m_ctrlPointsOpening;
	short	m_OpeningPts;
	CStatic	m_ctrlPointsClosingText;
	CEdit	m_ctrlPointsClosing;
	short	m_ClosingPts;
	CEdit	m_ctrlNote;
	CButton m_ctrlSuperQ;
	CButton m_ctrlMachPts;
	CButton m_ctrlDoubleQ;
	CListBox2	m_ctrlPointsList;
	CButton	m_ctrlPointsNew;
	CButton	m_ctrlPointsEdit;
	CButton	m_ctrlPointsDelete;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBAgilityRecordBook* m_Book;
	ARBConfig* m_Config;
	ARBConfigVenue* m_pVenue;
	ARBConfigEvent* m_pEvent;
	std::vector<CDlgFixup*> m_DlgFixup;
	ARBConfigScoringList m_Scorings;
	int m_idxMethod;
	CString m_strOpening[2];

	void ClearFixups();
	CString GetListName(ARBConfigScoring* pScoring) const;
	void FillControls();
	void FillMethodList();
	void FillDivisionList();
	void FillLevelList();
	void FillRequiredPoints();
	void FillTitlePoints(ARBConfigScoring* pScoring);
	bool SaveControls();

	//{{AFX_VIRTUAL(CDlgConfigEvent)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgConfigEvent)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSubCat();
	afx_msg void OnDblclkSubCat();
	afx_msg void OnSubCatNew();
	afx_msg void OnSubCatEdit();
	afx_msg void OnSubCatDelete();
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
