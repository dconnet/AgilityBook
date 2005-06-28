#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgDogNumbers class
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "ARBDogRegNum.h"
#include "ColumnOrder.h"
#include "DlgBasePropertyPage.h"
#include "ListCtrl.h"
class ARBDog;
class CAgilityBookDoc;

/////////////////////////////////////////////////////////////////////////////

class CDlgDogNumbers : public CDlgBasePropertyPage
{
	friend class CDlgDog;
public:
	CDlgDogNumbers(
			CAgilityBookDoc* pDoc,
			ARBDogRegNumList const& regnums);

private:
// Dialog Data
	//{{AFX_DATA(CDlgDogNumbers)
	enum { IDD = IDD_DOG_NUMBERS };
	CButton	m_ctrlRegEdit;
	CButton	m_ctrlRegDelete;
	CListCtrl2	m_ctrlRegNums;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	CColumnOrder m_sortRegNums;
	ARBDogRegNumList m_RegNums;

	//{{AFX_VIRTUAL(CDlgDogNumbers)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetColumnRegNumHeaders();
	void ListRegNums();
	void UpdateButtons();

// Implementation
protected:
	//{{AFX_MSG(CDlgDogNumbers)
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedRegNums(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRegNew();
	afx_msg void OnRegEdit();
	afx_msg void OnRegDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
