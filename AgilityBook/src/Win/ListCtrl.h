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
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * @li 2004-08-26 DRC Added GetPrintLine to CListCtrl2.
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include <vector>

/////////////////////////////////////////////////////////////////////////////

/**
 * Header control designed for subclassing for use in a ListCtrl.
 */
class CHeaderCtrl2 : public CHeaderCtrl
{
// Construction
public:
	CHeaderCtrl2();
	virtual ~CHeaderCtrl2();

	typedef enum { eNoSort, eAscending, eDescending } SortOrder;
	SortOrder GetSortOrder(int iCol) const;
	void Sort(int iCol, SortOrder eOrder);

protected:
// Overrides
	//{{AFX_VIRTUAL(CHeaderCtrl2)
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	CImageList m_ImageList;
	int m_sortAscending;
	int m_sortDescending;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderCtrl2)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CListCtrl2 : public CListCtrl
{
public:
	CListCtrl2();
	virtual ~CListCtrl2();

	// Header functions
	int HeaderItemCount();
	CHeaderCtrl2::SortOrder HeaderSortOrder(int iCol) const;
	void HeaderSort(int iCol, CHeaderCtrl2::SortOrder eOrder);

	/**
	 * Returns the first selected item.
	 */
	int GetSelection();
	/**
	 * Returns the number of selected items.
	 */
	size_t GetSelection(std::vector<int>& indices);
	void SetSelection(int index, bool bEnsureVisible = false);
	void SetSelection(std::vector<int>& indices, bool bEnsureVisible = false);
	/**
	 * Returns the data required to print/copy a line.
	 */
	virtual void GetPrintLine(int nItem, CStringArray& line);

protected:
	void Init();
	bool m_bInit;
	CHeaderCtrl2 m_SortHeader;
// Overrides
	//{{AFX_VIRTUAL(CListCtrl2)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CListCtrl2)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CListView2 : public CListView
{
	DECLARE_DYNCREATE(CListView2)
public:
	CListView2();
	virtual ~CListView2();

	// Header functions
	int HeaderItemCount();
	CHeaderCtrl2::SortOrder HeaderSortOrder(int iCol) const;
	void HeaderSort(int iCol, CHeaderCtrl2::SortOrder eOrder);

	/**
	 * Returns the first selected item.
	 */
	int GetSelection();
	/**
	 * Returns the number of selected items.
	 */
	size_t GetSelection(std::vector<int>& indices);
	void SetSelection(int index, bool bEnsureVisible = false);
	void SetSelection(std::vector<int>& indices, bool bEnsureVisible = false);
	/**
	 * Returns the data required to print/copy a line.
	 */
	virtual void GetPrintLine(int nItem, CStringArray& line);

protected:
	void Init();
	bool m_bInit;
	CHeaderCtrl2 m_SortHeader;
	//{{AFX_VIRTUAL(CListView2)
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CListView2)
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	//}}AFX_MSG
	afx_msg void OnUpdateNotSupported(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};
