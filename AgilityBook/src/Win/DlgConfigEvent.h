#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
#include "ListBox.h"
class ARBConfigEvent;
class ARBConfigVenue;

class CDlgConfigEvent : public CDialog
{
public:
	CDlgConfigEvent(ARBConfigVenue* pVenue, ARBConfigEvent* pEvent, CWnd* pParent = NULL);
	virtual ~CDlgConfigEvent();

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigEvent)
	enum { IDD = IDD_CONFIG_EVENT };
	CString m_Name;
	BOOL m_bHasPartners;
	CString	m_Desc;
	CButton m_ctrlNew;
	CButton m_ctrlDelete;
	CListBox2 m_ctrlMethods;
	CListBox2 m_ctrlUnused;
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
	CListBox2	m_ctrlTitleList;
	CButton	m_ctrlTitleNew;
	CButton	m_ctrlTitleEdit;
	CButton	m_ctrlTitleDelete;
	//}}AFX_DATA
	ARBConfigVenue* m_pVenue;
	ARBConfigEvent* m_pEvent;
	ARBConfigScoringList m_Scorings;
	int m_idxMethod;
	CString m_strOpening[2];

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
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLbnSelchangeMethods();
	afx_msg void OnCbnSelchangeDivision();
	afx_msg void OnCbnSelchangeLevel();
	afx_msg void OnSelchangeType();
	afx_msg void OnSelchangeList();
	afx_msg void OnDblclkList();
	afx_msg void OnTitleNew();
	afx_msg void OnTitleEdit();
	afx_msg void OnTitleDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
