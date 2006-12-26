#pragma once

/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgDogTitles class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "ARBDogTitle.h"
#include "ColumnOrder.h"
#include "DlgBasePropertyPage.h"
#include "ListCtrl.h"
#include "ListData.h"
class CAgilityBookDoc;

class CDlgDogTitles : public CDlgBasePropertyPage
{
	friend class CDlgDog;
public:
	CDlgDogTitles(
			CAgilityBookDoc* pDoc,
			ARBDogTitleList const& titles);

private:
// Dialog Data
	//{{AFX_DATA(CDlgDogTitles)
	enum { IDD = IDD_DOG_TITLES };
	CListCtrl2	m_ctrlTitles;
	CButton	m_ctrlTitleEdit;
	CButton	m_ctrlTitleDelete;
	CButton	m_ctrlHidden;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	CColumnOrder m_sortTitles;
	CImageList m_ImageList;
	int m_imgEmpty;
	int m_imgTitled;
	int m_imgTitledReceived;
	ARBDogTitleList m_Titles;

	//{{AFX_VIRTUAL(CDlgDogTitles)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	CListPtrData<ARBDogTitlePtr>* GetTitleData(int index) const;
	void SetColumnTitleHeaders();
	void ListTitles();
	void UpdateButtons();

// Implementation
protected:
	//{{AFX_MSG(CDlgDogTitles)
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTitleNew();
	afx_msg void OnTitleEdit();
	afx_msg void OnTitleDelete();
	afx_msg void OnBnClickedHidden();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
