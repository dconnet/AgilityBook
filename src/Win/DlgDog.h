#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 */

#include "ARBDogRegNum.h"
#include "ARBDogTitle.h"
#include "ColumnOrder.h"
#include "ListCtrl.h"
class ARBConfig;
class ARBDog;

/////////////////////////////////////////////////////////////////////////////

class CDlgDog : public CDialog
{
public:
	CDlgDog(ARBConfig& config, ARBDog* pDog, CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgDog)
	enum { IDD = IDD_DOG };
	CString	m_CallName;
	CDateTimeCtrl	m_ctrlDate;
	CString	m_Breed;
	CStatic	m_ctrlAge;
	CString	m_RegName;
	CButton	m_ctrlTitleEdit;
	CButton	m_ctrlTitleDelete;
	CListCtrl2	m_ctrlTitles;
	CButton	m_ctrlRegEdit;
	CButton	m_ctrlRegDelete;
	CListCtrl2	m_ctrlRegNums;
	CString	m_Notes;
	//}}AFX_DATA
	ARBConfig& m_Config;
	ARBDog* m_pDog;
	CColumnOrder m_sortTitles;
	CColumnOrder m_sortRegNums;
	ARBDogTitleList m_Titles;
	ARBDogRegNumList m_RegNums;

	//{{AFX_VIRTUAL(CDlgDog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetColumnTitleHeaders();
	void SetColumnRegNumHeaders();
	void ListTitles();
	void ListRegNums();
	void UpdateButtons();
	void UpdateAge();

// Implementation
protected:
	//{{AFX_MSG(CDlgDog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeDob(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTitleNew();
	afx_msg void OnTitleEdit();
	afx_msg void OnTitleDelete();
	afx_msg void OnRegNew();
	afx_msg void OnRegEdit();
	afx_msg void OnRegDelete();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
