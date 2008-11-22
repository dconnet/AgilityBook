#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2008-02-20 DRC Added subitem editing to lists.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-30 DRC Allow restricted selection in multi-select lists.
 * @li 2004-09-16 DRC Override SetColumnWidth to auto-fix tooltip rectangles.
 * @li 2004-09-09 DRC Added tooltips to header control.
 * @li 2004-08-26 DRC Added GetPrintLine to CListCtrl2.
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include <vector>
class CListData;

/**
 * Header control designed for subclassing for use in a ListCtrl.
 */
class CHeaderCtrl2 : public CHeaderCtrl
{
// Construction
public:
	CHeaderCtrl2();
	virtual ~CHeaderCtrl2();

	/// This function must be called whenever columns are added/removed/sized
	void FixTooltips();

	typedef enum
	{
		eNoSort,
		eAscending,
		eDescending
	} SortOrder;
	SortOrder GetSortOrder(int iCol) const;
	void Sort(
			int iCol,
			SortOrder eOrder);

// Overrides
	//{{AFX_VIRTUAL(CHeaderCtrl2)
protected:
	virtual void PreSubclassWindow();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	CImageList m_ImageList;
	int m_sortAscending;
	int m_sortDescending;
	CToolTipCtrl m_ToolTip;
	int fBufferSize;
	TCHAR* fpBuffer;

protected:
	//{{AFX_MSG(CHeaderCtrl2)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnHdnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// There may be other APIs that need forwarding, but for now, this covers our need.
class CListCtrlEx
{
protected:
	CListCtrlEx(CListCtrl* pList, bool bAutoDelete = true);

public:
	bool IsSafe() const				{return m_List && ::IsWindow(m_List->GetSafeHwnd());}
	void AllowEdit()				{m_bAllowEdit = true;}

	// Header functions
	void FixTooltips();
	int HeaderItemCount();
	CHeaderCtrl2::SortOrder HeaderSortOrder(int iCol) const;
	void HeaderSort(
			int iCol,
			CHeaderCtrl2::SortOrder eOrder);

	// CListCtrl api
	DWORD GetExtendedStyle() const;
	DWORD SetExtendedStyle(DWORD dwNewStyle);
	CHeaderCtrl* GetHeaderCtrl() const;
	CImageList* GetImageList(int nImageList) const;
	CImageList* SetImageList(
			CImageList* pImageList,
			int nImageList);
	BOOL GetColumn(
			int nCol,
			LVCOLUMN* pColumn) const;
	BOOL SetColumn(
			int nCol,
			const LVCOLUMN* pColumn);
	int InsertColumn(
			int nCol,
			LVCOLUMN const* pColumn);
	int InsertColumn(
			int nCol,
			LPCTSTR lpszColumnHeading,
			int nFormat = LVCFMT_LEFT,
			int nWidth = -1,
			int nSubItem = -1);
	BOOL SetColumnWidth(
			int nCol,
			int cx);
	BOOL DeleteColumn(int nCol);
	int GetItemCount() const;
	UINT GetSelectedCount() const;
	BOOL EnsureVisible(
			int nItem,
			BOOL bPartialOK);
	BOOL GetCheck(int nItem) const;
	DWORD_PTR GetItemData(int nItem) const;
	BOOL GetItemRect(
			int nItem,
			LPRECT lpRect,
			UINT nCode) const;
	BOOL GetSubItemRect(
			int iItem,
			int iSubItem,
			int nArea,
			CRect& ref);
	UINT GetItemState(
			int nItem,
			UINT nMask) const;
	CString GetItemText(
			int nItem,
			int nSubItem) const;
	int GetItemText(
			int nItem,
			int nSubItem,
			LPTSTR lpszText,
			int nLen) const;
	BOOL SetCheck(
			int nItem,
			BOOL fCheck = TRUE);
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(
			int nItem,
			int nSubItem,
			UINT nMask,
			LPCTSTR lpszItem,
			int nImage,
			UINT nState,
			UINT nStateMask,
			LPARAM lParam);
#if _MSC_VER >= 1300
	BOOL SetItem(
			int nItem,
			int nSubItem,
			UINT nMask,
			LPCTSTR lpszItem,
			int nImage,
			UINT nState,
			UINT nStateMask,
			LPARAM lParam,
			int nIndent);
#endif
	BOOL SetItemData(
			int nItem,
			DWORD_PTR dwData);
	BOOL SetItemState(
			int nItem,
			LVITEM* pItem);
	BOOL SetItemState(
			int nItem,
			UINT nState,
			UINT nMask);
	BOOL SetItemText(
			int nItem,
			int nSubItem,
			LPCTSTR lpszText);
	BOOL DeleteAllItems();
	BOOL DeleteItem(int nItem);
	int InsertItem(const LVITEM* pItem);
	int InsertItem(
			int nItem,
			LPCTSTR lpszItem);
	int InsertItem(
			int nItem,
			LPCTSTR lpszItem,
			int nImage);
	int InsertItem(
			UINT nMask,
			int nItem,
			LPCTSTR lpszItem,
			UINT nState,
			UINT nStateMask,
			int nImage,
			LPARAM lParam);
	BOOL SortItems(
			PFNLVCOMPARE pfnCompare,
			DWORD_PTR dwData);

	/*
	 * Simple wrappers to control data access.
	 * Only valid if autodelete is set.
	 */
	CListData* GetData(int index) const;
	void SetData(
			int index,
			CListData* inData);

	/**
	 * Returns the first selected item.
	 * @param bRestricted In multi-select lists, only allow a single selection.
	 */
	int GetSelection(bool bRestricted = false);

	/**
	 * Returns the number of selected items.
	 */
	size_t GetSelection(std::vector<int>& indices);
	void SetSelection(
			int index,
			bool bEnsureVisible = false);
	void SetSelection(
			std::vector<int>& indices,
			bool bEnsureVisible = false);

	/**
	 * Returns the data required to print/copy a line.
	 */
	virtual void GetPrintLine(
			int nItem,
			CStringArray& line);

	/**
	 * Initialize the type of editing control.
	 * @note Call this during the LVN_BEGINLABELEDIT callback.
	 * @note Only droplist style is supported due to issues with the child edit control of a combobox.
	 * @param dropItems Items to initial combo
	 */
	void SetEditList(std::vector<tstring> const& dropItems);

	/**
	 * Edit a sub item.
	 * @note This will be either an edit control (default) or combobox,
	 * depending on whether SetEditList is called.
	 * @return Temporary handle to control (do not save)
	 */
	CWnd* EditSubItem(
			int index,
			int nCol);

protected:
	bool Init();
	int HitTestEx(CPoint& point, int& col);

	CListCtrl* m_List;
	CHeaderCtrl2 m_SortHeader;
	bool m_bAutoDelete;
	bool m_bAllowEdit;

	bool m_editControl;
	std::vector<tstring> m_Items;
};

/////////////////////////////////////////////////////////////////////////////

// Protect the listctrl so all access to it through CListCtrlEx.
class CListCtrl2 : protected CListCtrl, public CListCtrlEx
{
	friend void AFXAPI DDX_Control(CDataExchange* pDX, int nIDC, CListCtrl2& rControl);
public:
	CListCtrl2(bool bAutoDelete);
	virtual ~CListCtrl2();

	// Allow access to CWnd functionality
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{
		return CListCtrl::Create(dwStyle, rect, pParentWnd, nID);
	}
	HWND GetSafeHwnd() const
	{
		return CListCtrl::GetSafeHwnd();
	}
	void GetWindowRect(LPRECT lpRect) const
	{
		CListCtrl::GetWindowRect(lpRect);
	}
	void Invalidate(BOOL bErase = TRUE)
	{
		CListCtrl::Invalidate(bErase);
	}
	void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE)
	{
		CListCtrl::InvalidateRect(lpRect, bErase);
	}
	void SetRedraw(BOOL bRedraw = TRUE)
	{
		CListCtrl::SetRedraw(bRedraw);
	}
	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
	{
		return CListCtrl::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);
	}
	BOOL ShowWindow(int nCmdShow)
	{
		return CListCtrl::ShowWindow(nCmdShow);
	}

	// Pass all CListCtrl calls to CListCtrlEx
	DWORD GetExtendedStyle() const
	{
		return CListCtrlEx::GetExtendedStyle();
	}
	DWORD SetExtendedStyle(DWORD dwNewStyle)
	{
		return CListCtrlEx::SetExtendedStyle(dwNewStyle);
	}
	CHeaderCtrl* GetHeaderCtrl() const
	{
		return CListCtrlEx::GetHeaderCtrl();
	}
	CImageList* GetImageList(int nImageList) const
	{
		return CListCtrlEx::GetImageList(nImageList);
	}
	CImageList* SetImageList(CImageList* pImageList, int nImageList)
	{
		return CListCtrlEx::SetImageList(pImageList, nImageList);
	}
	BOOL GetColumn(int nCol, LVCOLUMN* pColumn) const
	{
		return CListCtrlEx::GetColumn(nCol, pColumn);
	}
	BOOL SetColumn(int nCol, const LVCOLUMN* pColumn)
	{
		return CListCtrlEx::SetColumn(nCol, pColumn);
	}
	int InsertColumn(int nCol, LVCOLUMN const* pColumn)
	{
		return CListCtrlEx::InsertColumn(nCol, pColumn);
	}
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1)
	{
		return CListCtrlEx::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	}
	BOOL SetColumnWidth(int nCol, int cx)
	{
		return CListCtrlEx::SetColumnWidth(nCol, cx);
	}
	BOOL DeleteColumn(int nCol)
	{
		return CListCtrlEx::DeleteColumn(nCol);
	}
	int GetItemCount() const
	{
		return CListCtrlEx::GetItemCount();
	}
	UINT GetSelectedCount() const
	{
		return CListCtrlEx::GetSelectedCount();
	}
	BOOL EnsureVisible(int nItem, BOOL bPartialOK)
	{
		return CListCtrlEx::EnsureVisible(nItem, bPartialOK);
	}
	BOOL GetCheck(int nItem) const
	{
		return CListCtrlEx::GetCheck(nItem);
	}
	DWORD_PTR GetItemData(int nItem) const
	{
		return CListCtrlEx::GetItemData(nItem);
	}
	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
	{
		return CListCtrlEx::GetItemRect(nItem, lpRect, nCode);
	}
	BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref)
	{
		return CListCtrlEx::GetSubItemRect(iItem, iSubItem, nArea, ref);
	}
	UINT GetItemState(int nItem, UINT nMask) const
	{
		return CListCtrlEx::GetItemState(nItem, nMask);
	}
	CString GetItemText(int nItem, int nSubItem) const
	{
		return CListCtrlEx::GetItemText(nItem, nSubItem);
	}
	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const
	{
		return CListCtrlEx::GetItemText(nItem, nSubItem, lpszText, nLen);
	}
	BOOL SetCheck(int nItem, BOOL fCheck = TRUE)
	{
		return CListCtrlEx::SetCheck(nItem, fCheck);
	}
	BOOL SetItem(const LVITEM* pItem)
	{
		return CListCtrlEx::SetItem(pItem);
	}
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
	{
		return CListCtrlEx::SetItem(nItem, nSubItem, nMask, lpszItem, nImage, nState, nStateMask, lParam);
	}
#if _MSC_VER >= 1300
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam, int nIndent)
	{
		return CListCtrlEx::SetItem(nItem, nSubItem, nMask, lpszItem, nImage, nState, nStateMask, lParam, nIndent);
	}
#endif
	BOOL SetItemData(int nItem, DWORD_PTR dwData)
	{
		return CListCtrlEx::SetItemData(nItem, dwData);
	}
	BOOL SetItemState(int nItem, LVITEM* pItem)
	{
		return CListCtrlEx::SetItemState(nItem, pItem);
	}
	BOOL SetItemState(int nItem, UINT nState, UINT nMask)
	{
		return CListCtrlEx::SetItemState(nItem, nState, nMask);
	}
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		return CListCtrlEx::SetItemText(nItem, nSubItem, lpszText);
	}
	BOOL DeleteAllItems()
	{
		return CListCtrlEx::DeleteAllItems();
	}
	BOOL DeleteItem(int nItem)
	{
		return CListCtrlEx::DeleteItem(nItem);
	}
	int InsertItem(const LVITEM* pItem)
	{
		return CListCtrlEx::InsertItem(pItem);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		return CListCtrlEx::InsertItem(nItem, lpszItem);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
	{
		return CListCtrlEx::InsertItem(nItem, lpszItem, nImage);
	}
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
	{
		return CListCtrlEx::InsertItem(nMask, nItem, lpszItem, nState, nStateMask, nImage, lParam);
	}
	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
	{
		return CListCtrlEx::SortItems(pfnCompare, dwData);
	}

protected:
	//{{AFX_MSG(CListCtrl2)
	afx_msg void OnDestroy();
	afx_msg BOOL OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline void AFXAPI DDX_Control(CDataExchange* pDX, int nIDC, CListCtrl2& rControl)
{
	DDX_Control(pDX, nIDC, (CWnd&)rControl);
}

/////////////////////////////////////////////////////////////////////////////

class CListView2 : public CListView, public CListCtrlEx
{
	DECLARE_DYNCREATE(CListView2)
public:
	CListView2();
	virtual ~CListView2();

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
	afx_msg void OnDestroy();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg BOOL OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	//}}AFX_MSG
	afx_msg void OnUpdateNotSupported(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
	// Hide it so users can't use it.
	CListCtrl& GetListCtrl() const	{return CListView::GetListCtrl();}
};
