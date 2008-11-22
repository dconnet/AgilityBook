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
 * @li 2007-02-26 DRC Fix a problem redrawing list columns.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-30 DRC Allow restricted selection in multi-select lists.
 * @li 2004-10-04 DRC Added div-by-0 tests.
 * @li 2004-09-16 DRC Override SetColumnWidth to auto-fix tooltip rectangles.
 * @li 2004-09-09 DRC Added tooltips to header control.
 * @li 2004-09-03 DRC Using the sort icon forced alignment to left.
 * @li 2004-08-26 DRC Added GetPrintLine to CListCtrl2.
 * @li 2003-11-22 DRC When copying, only create a header if more than 1 line
 *                    is selected (or if only one line exists).
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "ListCtrl.h"

#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define IDC_IPEDIT	999

// Based on http://www.codeguru.com/cpp/controls/listview/editingitemsandsubitem/article.php/c923/

class CInPlaceEdit : public CEdit
{
public:
	CInPlaceEdit(int iItem, int iSubItem, CString const& sInitText)
		: m_sInitText(sInitText)
	{
		m_iItem = iItem;
		m_iSubItem = iSubItem;
		m_bESC = FALSE;
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC;	 	// To indicate whether ESC key was pressed
};


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN
		|| pMsg->wParam == VK_DELETE
		|| pMsg->wParam == VK_ESCAPE
		|| GetKeyState(VK_CONTROL)
		)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE; // DO NOT process further
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}


int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText(m_sInitText);
	SetFocus();
	SetSel(0, -1);
	return 0;
}


void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();
	delete this;
}


void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_sInitText) : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = m_bESC ? m_sInitText.GetLength() : str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY,
			GetParent()->GetDlgCtrlID(),
			(LPARAM)&dispinfo);

	DestroyWindow();
}


void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;

	GetWindowText(str);
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject(pFont);
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(pFontDC);
	size.cx += 5; // add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect(&rect);
	GetParent()->GetClientRect(&parentrect);

	// Transform rect to parent coordinates
	ClientToScreen(&rect);
	GetParent()->ScreenToClient(&rect);

	// Check whether control needs to be resized
	// and whether there is space to grow
	if (size.cx > rect.Width())
	{
		if (size.cx + rect.left < parentrect.right)
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow(&rect);
	}
}

/////////////////////////////////////////////////////////////////////////////

class CInPlaceCombo : public CComboBox
{
public:
	CInPlaceCombo(int iItem, int iSubItem, CString const& sInitText)
		: m_sInitText(sInitText)
	{
		m_iItem = iItem;
		m_iSubItem = iSubItem;
		m_bESC = FALSE;
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCloseup();
	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC;	 	// To indicate whether ESC key was pressed
};


BEGIN_MESSAGE_MAP(CInPlaceCombo, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceCombo)
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CInPlaceCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE; // DO NOT process further
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}


int CInPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetFocus();

	return 0;
}


void CInPlaceCombo::OnNcDestroy()
{
	CComboBox::OnNcDestroy();
	delete this;
}


void CInPlaceCombo::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);
	if (pNewWnd)
	{
		// The edit control of the combo is getting focus
		if (pNewWnd->GetParent() == this)
			return;
	}

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_sInitText) : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = m_bESC ? m_sInitText.GetLength() : str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY,
			GetParent()->GetDlgCtrlID(),
			(LPARAM)&dispinfo);

	DestroyWindow();
}


void CInPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}


void CInPlaceCombo::OnCloseup()
{
	// Set the focus to the parent list control
	GetParent()->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrl2

BEGIN_MESSAGE_MAP(CHeaderCtrl2, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrl2)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGED, OnHdnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CHeaderCtrl2::CHeaderCtrl2()
	: fBufferSize(300)
	, fpBuffer(NULL)
{
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 0);
	m_sortAscending = m_ImageList.Add(theApp.LoadIcon(IDI_HEADER_UP));
	m_sortDescending = m_ImageList.Add(theApp.LoadIcon(IDI_HEADER_DOWN));
	fpBuffer = new TCHAR[fBufferSize];
}


CHeaderCtrl2::~CHeaderCtrl2()
{
	delete [] fpBuffer;
}


void CHeaderCtrl2::PreSubclassWindow()
{
	CHeaderCtrl::PreSubclassWindow();
	SetImageList(&m_ImageList);
#if _MSC_VER >= 1300
	SetBitmapMargin(0);
#endif
	m_ToolTip.Create(this);
	CRect rMargin(0,0,0,0);
	m_ToolTip.SetMargin(&rMargin);
}


BOOL CHeaderCtrl2::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CHeaderCtrl::PreTranslateMessage(pMsg);
}


void CHeaderCtrl2::OnDestroy()
{
	CHeaderCtrl::OnDestroy();
	m_ToolTip.DestroyWindow();
}


void CHeaderCtrl2::OnSize(UINT nType, int cx, int cy)
{
	CHeaderCtrl::OnSize(nType, cx, cy);
	FixTooltips();
}


BOOL CHeaderCtrl2::OnHdnItemChanged(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// In theory, we could just adjust the column that changed. But that
	// also affects any columns to the right. So let's just keep in simple!
	// Over-optimizing to only do exactly what work is required is not worth it.
	if (phdr->pitem->mask & HDI_WIDTH)
		FixTooltips();
	*pResult = 0;
	return FALSE; // Allow parent to handle also
}


void CHeaderCtrl2::FixTooltips()
{
	if (!::IsWindow(m_ToolTip.GetSafeHwnd()) || !::IsWindow(GetSafeHwnd()))
		return;

	// Get rid of all existing tips.
	int nCols = m_ToolTip.GetToolCount();
	while (0 < nCols)
	{
		m_ToolTip.DelTool(this, nCols);
		--nCols;
	}

	// Now add them back.
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	nCols = GetItemCount();
	for (int iCol = 0; iCol < nCols; ++iCol)
	{
		HDITEM item;
		item.mask = HDI_TEXT | HDI_FORMAT;
		item.pszText = fpBuffer;
		item.cchTextMax = fBufferSize;
		bool bDelTip = true;
		if (GetItem(iCol, &item))
		{
			// Get all the text a header may have.
			while (lstrlen(fpBuffer) == fBufferSize - 1)
			{
				delete [] fpBuffer;
				fBufferSize *= 2;
				fpBuffer = new TCHAR[fBufferSize];
				item.pszText = fpBuffer;
				item.cchTextMax = fBufferSize;
				GetItem(iCol, &item);
			}
			if (fpBuffer && *fpBuffer)
			{
				CRect rColumn;
				GetItemRect(iCol, rColumn);
				CRect rAdjusted(rColumn);
#if _MSC_VER >= 1300
				rAdjusted.right -= GetBitmapMargin();
#endif
				rAdjusted.right -= 12; // Deflate to allow for the padding. 6 is just an observed number on each side
				if (item.fmt & HDF_IMAGE)
					rAdjusted.right -= 16; // Subtract icon.
				CRect rText(rAdjusted);
				CString str(fpBuffer);
				// It doesn't matter if the column is left/center/right adjusted.
				// We only care if the text exceeds the allowed area.
				dc.DrawText(str, rText, DT_CALCRECT | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
				if (rText.Width() > rAdjusted.Width())
				{
					// A tooltip can only display 260 chars.
					if (str.GetLength() >= 260)
						str = _T("...") + str.Right(256);
					m_ToolTip.AddTool(this, str, rColumn, iCol+1);
					bDelTip = false;
				}
			}
		}
		if (bDelTip)
			m_ToolTip.DelTool(this, iCol+1);
	}
	dc.SelectObject(pOldFont);
}


CHeaderCtrl2::SortOrder CHeaderCtrl2::GetSortOrder(int iCol) const
{
	if (0 > iCol || iCol >= GetItemCount())
		return eNoSort;
	HDITEM item;
	item.mask = HDI_FORMAT | HDI_IMAGE;
	GetItem(iCol, &item);
	if (item.fmt & HDF_IMAGE)
	{
		if (item.iImage == m_sortAscending)
			return eAscending;
		else if (item.iImage == m_sortDescending)
			return eDescending;
	}
	return eNoSort;
}


void CHeaderCtrl2::Sort(
		int iCol,
		SortOrder eOrder)
{
	if (0 > iCol || iCol >= GetItemCount())
		return;
	int nIndex = -1;
	switch (eOrder)
	{
	case eAscending: nIndex = m_sortAscending; break;
	case eDescending: nIndex = m_sortDescending; break;
	}
	HDITEM item;
	item.mask = HDI_FORMAT;
	GetItem(iCol, &item);
	if (0 <= nIndex)
	{
		item.mask |= HDI_IMAGE;
		item.fmt |= HDF_IMAGE;
		item.iImage = nIndex;
	}
	else
		item.fmt &= ~HDF_IMAGE;
	SetItem(iCol, &item);
	FixTooltips();
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx(CListCtrl* pList, bool bAutoDelete)
	: m_List(pList)
	, m_SortHeader()
	, m_bAutoDelete(bAutoDelete)
	, m_bAllowEdit(false)
	, m_editControl(true)
	, m_Items()
{
}


/// Returns whether we ran the initialization.
bool CListCtrlEx::Init()
{
	if (!IsSafe())
		return false;
	if (::IsWindow(m_List->GetHeaderCtrl()->GetSafeHwnd())
	&& (!m_SortHeader.GetSafeHwnd() || !::IsWindow(m_SortHeader.GetSafeHwnd())))
	{
		m_SortHeader.SubclassWindow(m_List->GetHeaderCtrl()->GetSafeHwnd());
		m_SortHeader.FixTooltips();
		return true;
	}
	return false;
}


int CListCtrlEx::HitTestEx(CPoint& point, int& col)
{
	if (!IsSafe())
		return -1;
	int row = m_List->HitTest(point, NULL);
	col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_List->m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return row;

	// Get the top and bottom row visible
	row = m_List->GetTopIndex();
	int bottom = row + m_List->GetCountPerPage();
	if (bottom > m_List->GetItemCount())
		bottom = m_List->GetItemCount();

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_List->GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for ( ;row <= bottom; ++row)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		m_List->GetItemRect(row, &rect, LVIR_BOUNDS);
		if (rect.PtInRect(point))
		{
			// Now find the column
			for (int colnum = 0; colnum < nColumnCount; ++colnum)
			{
				int colwidth = m_List->GetColumnWidth(colnum);
				if (point.x >= rect.left
				&& point.x <= (rect.left + colwidth))
				{
					col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}


void CListCtrlEx::FixTooltips()
{
	if (!IsSafe())
		return;
	if (!Init())
		m_SortHeader.FixTooltips();
}


int CListCtrlEx::HeaderItemCount()
{
	if (!IsSafe())
		return 0;
	Init();
	return m_SortHeader.GetItemCount();
}


CHeaderCtrl2::SortOrder CListCtrlEx::HeaderSortOrder(int iCol) const
{
	if (!IsSafe())
		return CHeaderCtrl2::eNoSort;
	const_cast<CListCtrlEx*>(this)->Init();
	return m_SortHeader.GetSortOrder(iCol);
}


void CListCtrlEx::HeaderSort(
		int iCol,
		CHeaderCtrl2::SortOrder eOrder)
{
	if (!IsSafe())
		return;
	Init();
	m_SortHeader.Sort(iCol, eOrder);
}


DWORD CListCtrlEx::GetExtendedStyle() const
{
	return IsSafe() ? m_List->GetExtendedStyle() : 0;
}


DWORD CListCtrlEx::SetExtendedStyle(DWORD dwNewStyle)
{
	return IsSafe() ? m_List->SetExtendedStyle(dwNewStyle): 0;
}


CHeaderCtrl* CListCtrlEx::GetHeaderCtrl() const
{
	return IsSafe() ? m_List->GetHeaderCtrl() : NULL;
}


CImageList* CListCtrlEx::GetImageList(int nImageList) const
{
	return IsSafe() ? m_List->GetImageList(nImageList) : NULL;
}


CImageList* CListCtrlEx::SetImageList(
		CImageList* pImageList,
		int nImageList)
{
	return IsSafe() ? m_List->SetImageList(pImageList, nImageList) : NULL;
}


BOOL CListCtrlEx::GetColumn(
		int nCol,
		LVCOLUMN* pColumn) const
{
	return IsSafe() ? m_List->GetColumn(nCol, pColumn) : FALSE;
}


BOOL CListCtrlEx::SetColumn(
		int nCol,
		const LVCOLUMN* pColumn)
{
	return IsSafe() ? m_List->SetColumn(nCol, pColumn) : FALSE;
}


int CListCtrlEx::InsertColumn(
		int nCol,
		LVCOLUMN const* pColumn)
{
	if (!IsSafe())
		return -1;
	int rc = m_List->InsertColumn(nCol, pColumn);
	if (0 <= rc && !Init())
		m_SortHeader.FixTooltips();
	return rc;
}


int CListCtrlEx::InsertColumn(
		int nCol,
		LPCTSTR lpszColumnHeading,
		int nFormat,
		int nWidth,
		int nSubItem)
{
	if (!IsSafe())
		return -1;
	int rc = m_List->InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	if (0 <= rc && !Init())
		m_SortHeader.FixTooltips();
	return rc;
}


BOOL CListCtrlEx::SetColumnWidth(
		int nCol,
		int cx)
{
	if (!IsSafe())
		return FALSE;
	BOOL rc = m_List->SetColumnWidth(nCol, cx);
	if (rc && !Init())
		m_SortHeader.FixTooltips();
	return rc;
}


BOOL CListCtrlEx::DeleteColumn(int nCol)
{
	if (!IsSafe())
		return FALSE;
	BOOL rc = m_List->DeleteColumn(nCol);
	if (rc && !Init())
		m_SortHeader.FixTooltips();
	return rc;
}


int CListCtrlEx::GetItemCount() const
{
	return IsSafe() ? m_List->GetItemCount() : 0;
}


UINT CListCtrlEx::GetSelectedCount() const
{
	return IsSafe() ? m_List->GetSelectedCount() : 0;
}


BOOL CListCtrlEx::EnsureVisible(
		int nItem,
		BOOL bPartialOK)
{
	return IsSafe() ? m_List->EnsureVisible(nItem, bPartialOK) : FALSE;
}


BOOL CListCtrlEx::GetCheck(int nItem) const
{
	return IsSafe() ? m_List->GetCheck(nItem) : FALSE;
}


DWORD_PTR CListCtrlEx::GetItemData(int nItem) const
{
	return IsSafe() ? m_List->GetItemData(nItem) : 0;
}


BOOL CListCtrlEx::GetItemRect(
		int nItem,
		LPRECT lpRect,
		UINT nCode) const
{
	return IsSafe() ? m_List->GetItemRect(nItem, lpRect, nCode) : FALSE;
}


BOOL CListCtrlEx::GetSubItemRect(
		int iItem,
		int iSubItem,
		int nArea,
		CRect& ref)
{
	return IsSafe() ? m_List->GetSubItemRect(iItem, iSubItem, nArea, ref) : FALSE;
}


UINT CListCtrlEx::GetItemState(
		int nItem,
		UINT nMask) const
{
	return IsSafe() ? m_List->GetItemState(nItem, nMask) : 0;
}


CString CListCtrlEx::GetItemText(
		int nItem,
		int nSubItem) const
{
	return IsSafe() ? m_List->GetItemText(nItem, nSubItem) : CString();
}


int CListCtrlEx::GetItemText(
		int nItem,
		int nSubItem,
		LPTSTR lpszText,
		int nLen) const
{
	return IsSafe() ? m_List->GetItemText(nItem, nSubItem, lpszText, nLen) : 0;
}


BOOL CListCtrlEx::SetCheck(
		int nItem,
		BOOL fCheck)
{
	return IsSafe() ? m_List->SetCheck(nItem, fCheck) : FALSE;
}


BOOL CListCtrlEx::SetItem(const LVITEM* pItem)
{
	return IsSafe() ? m_List->SetItem(pItem) : FALSE;
}


BOOL CListCtrlEx::SetItem(
		int nItem,
		int nSubItem,
		UINT nMask,
		LPCTSTR lpszItem,
		int nImage,
		UINT nState,
		UINT nStateMask,
		LPARAM lParam)
{
	return IsSafe() ? m_List->SetItem(nItem, nSubItem, nMask, lpszItem, nImage, nState, nStateMask, lParam) : FALSE;
}


#if _MSC_VER >= 1300
BOOL CListCtrlEx::SetItem(
		int nItem,
		int nSubItem,
		UINT nMask,
		LPCTSTR lpszItem,
		int nImage,
		UINT nState,
		UINT nStateMask,
		LPARAM lParam,
		int nIndent)
{
	return IsSafe() ? m_List->SetItem(nItem, nSubItem, nMask, lpszItem, nImage, nState, nStateMask, lParam, nIndent) : FALSE;
}
#endif


BOOL CListCtrlEx::SetItemData(
		int nItem,
		DWORD_PTR dwData)
{
	return IsSafe() ? m_List->SetItemData(nItem, dwData) : FALSE;
}


BOOL CListCtrlEx::SetItemState(
		int nItem,
		LVITEM* pItem)
{
	return IsSafe() ? m_List->SetItemState(nItem, pItem) : FALSE;
}


BOOL CListCtrlEx::SetItemState(
		int nItem,
		UINT nState,
		UINT nMask)
{
	return IsSafe() ? m_List->SetItemState(nItem, nState, nMask) : FALSE;
}


BOOL CListCtrlEx::SetItemText(
		int nItem,
		int nSubItem,
		LPCTSTR lpszText)
{
	return IsSafe() ? m_List->SetItemText(nItem, nSubItem, lpszText) : FALSE;
}


BOOL CListCtrlEx::DeleteAllItems()
{
	return IsSafe() ? m_List->DeleteAllItems() : FALSE;
}


BOOL CListCtrlEx::DeleteItem(int nItem)
{
	return IsSafe() ? m_List->DeleteItem(nItem) : FALSE;
}


int CListCtrlEx::InsertItem(const LVITEM* pItem)
{
	return IsSafe() ? m_List->InsertItem(pItem) : -1;
}


int CListCtrlEx::InsertItem(
		int nItem,
		LPCTSTR lpszItem)
{
	return IsSafe() ? m_List->InsertItem(nItem, lpszItem) : -1;
}


int CListCtrlEx::InsertItem(
		int nItem,
		LPCTSTR lpszItem,
		int nImage)
{
	return IsSafe() ? m_List->InsertItem(nItem, lpszItem, nImage) : -1;
}


int CListCtrlEx::InsertItem(
		UINT nMask,
		int nItem,
		LPCTSTR lpszItem,
		UINT nState,
		UINT nStateMask,
		int nImage,
		LPARAM lParam)
{
	return IsSafe() ? m_List->InsertItem(nMask, nItem, lpszItem, nState, nStateMask, nImage, lParam) : -1;
}


BOOL CListCtrlEx::SortItems(
		PFNLVCOMPARE pfnCompare,
		DWORD_PTR dwData)
{
	return IsSafe() ? m_List->SortItems(pfnCompare, dwData) : FALSE;
}


CListData* CListCtrlEx::GetData(int index) const
{
	if (!IsSafe())
		return NULL;
	if (0 <= index && index < m_List->GetItemCount() && m_bAutoDelete)
		return reinterpret_cast<CListData*>(m_List->GetItemData(index));
	return NULL;
}


void CListCtrlEx::SetData(int index, CListData* inData)
{
	if (!IsSafe())
		return;
	if (0 <= index && index < m_List->GetItemCount() && m_bAutoDelete)
	{
		CListData* pData = GetData(index);
		if (pData)
			delete pData;
		m_List->SetItemData(index, reinterpret_cast<LPARAM>(inData));
	}
}


int CListCtrlEx::GetSelection(bool bRestricted)
{
	if (!IsSafe())
		return -1;
	std::vector<int> indices;
	GetSelection(indices);
	bool bSingle = (bRestricted || (LVS_SINGLESEL == (m_List->GetStyle() & LVS_SINGLESEL)));
	if ((bSingle && 1 == indices.size())
	|| (!bSingle && 0 < indices.size()))
		return indices[0];
	else
		return -1;
}


size_t CListCtrlEx::GetSelection(std::vector<int>& indices)
{
	indices.clear();
	if (!IsSafe())
		return 0;
	POSITION pos = m_List->GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		indices.push_back(m_List->GetNextSelectedItem(pos));
	}
	return indices.size();
}


void CListCtrlEx::SetSelection(
		int index,
		bool bEnsureVisible)
{
	if (!IsSafe())
		return;
	std::vector<int> indices;
	indices.push_back(index);
	SetSelection(indices, bEnsureVisible);
}


void CListCtrlEx::SetSelection(
		std::vector<int>& indices,
		bool bEnsureVisible)
{
	if (!IsSafe())
		return;
	// Clear everything.
	POSITION pos = m_List->GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		int i = m_List->GetNextSelectedItem(pos);
		m_List->SetItemState(i, 0, LVIS_SELECTED|LVIS_FOCUSED);
	}
	// Set it.
	for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		int index = *iter;
		if (index >= 0 && index < m_List->GetItemCount())
		{
			m_List->SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			if (bEnsureVisible)
				m_List->EnsureVisible(index, FALSE);
		}
	}
}


// This allows a derived class to print a subset of columns if it wants.
void CListCtrlEx::GetPrintLine(
		int nItem,
		CStringArray& line)
{
	line.RemoveAll();
	if (!IsSafe())
		return;
	int nColumns = m_List->GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumns; ++i)
	{
		if (0 > nItem)
		{
			TCHAR buffer[1000];
			HDITEM hdr;
			hdr.mask = HDI_TEXT;
			hdr.pszText = buffer;
			hdr.cchTextMax = sizeof(buffer) / sizeof(TCHAR);
			m_List->GetHeaderCtrl()->GetItem(i, &hdr);
			line.Add(buffer);
		}
		else
			line.Add(m_List->GetItemText(nItem, i));
	}
}


void CListCtrlEx::SetEditList(std::vector<tstring> const& dropItems)
{
	if (!IsSafe())
		return;
	m_editControl = false;
	m_Items = dropItems;
}


// The returned pointer should not be saved
CWnd* CListCtrlEx::EditSubItem(int index, int nCol)
{
	// Make sure item is valid
	if (!IsSafe() || 0 > index || index >= m_List->GetItemCount())
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_List->GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || m_List->GetColumnWidth(nCol) < 5)
		return NULL;

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_List->m_hWnd;
	dispinfo.hdr.idFrom = m_List->GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;
	dispinfo.item.mask = 0;
	dispinfo.item.iItem = index;
	dispinfo.item.iSubItem = nCol;
	if (m_List->GetParent()->SendMessage(WM_NOTIFY, m_List->GetDlgCtrlID(), (LPARAM)&dispinfo))
		return NULL;

	// Make sure that the item is visible
	if (!m_List->EnsureVisible(index, TRUE))
		return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; ++i)
		offset += m_List->GetColumnWidth(i);

	CRect rect;
	m_List->GetItemRect(index, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	m_List->GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		m_List->Scroll(size);
		rect.left -= size.cx;
	}

	// Additional offsets determined by looking at normal edit of col0.
	rect.left += offset + 4;
	rect.right = rect.left + m_List->GetColumnWidth(nCol) + 5;
	if (rect.right > rcClient.right)
		rect.right = rcClient.right;
	--rect.top;
	rect.bottom += 2;

	// Get Column alignment
	CWnd* pWnd = NULL;
	CString text = m_List->GetItemText(index, nCol);
	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE;
	if (m_editControl)
	{
		LV_COLUMN lvcol;
		lvcol.mask = LVCF_FMT;
		m_List->GetColumn(nCol, &lvcol);
		dwStyle |= ES_AUTOHSCROLL;
		if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
			dwStyle |= ES_LEFT;
		else if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
			dwStyle |= ES_RIGHT;
		else
			dwStyle |= ES_CENTER;
		CEdit* pEdit = new CInPlaceEdit(index, nCol, text);
		pEdit->Create(dwStyle, rect, m_List, IDC_IPEDIT);
		pWnd = pEdit;
	}
	else
	{
		dwStyle |= CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL;
		CComboBox* pCombo = new CInPlaceCombo(index, nCol, text);
		rect.bottom += 300;
		pCombo->Create(dwStyle, rect, m_List, IDC_IPEDIT);
		for (std::vector<tstring>::iterator i = m_Items.begin(); i != m_Items.end(); ++i)
		{
			int idx = pCombo->AddString((*i).c_str());
			if (text == (*i).c_str())
				pCombo->SetCurSel(idx);
		}
	}

	m_editControl = true;
	m_Items.clear();

	return pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrl2

BEGIN_MESSAGE_MAP(CListCtrl2, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrl2)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnDeleteitem)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#pragma warning ( push )
#pragma warning ( disable : 4355 )
CListCtrl2::CListCtrl2(bool bAutoDelete)
	: CListCtrlEx(this, bAutoDelete)
{
}
#pragma warning ( pop )


CListCtrl2::~CListCtrl2()
{
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrl2 message handlers

void CListCtrl2::OnDestroy()
{
	DeleteAllItems();
	CListCtrl::OnDestroy();
}


BOOL CListCtrl2::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView && m_bAutoDelete)
	{
		CListData* pData = reinterpret_cast<CListData*>(pNMListView->lParam);
		delete pData;
		pNMListView->lParam = 0;
	}
	*pResult = 0;
	return FALSE; // Allow parent to handle also
}


void CListCtrl2::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool bDefault = true;
	if (m_bAllowEdit)
	//if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
	{
		int index;
		int colnum;
		if ((index = HitTestEx(point, colnum)) != -1)
		{
			UINT flag = LVIS_FOCUSED;
			if ((GetItemState(index, flag) & flag) == flag)
			{
				bDefault = false;
				EditSubItem(index, colnum);
			}
		}
	}
	if (bDefault)
		CListCtrl::OnLButtonDown(nFlags, point);
}


BOOL CListCtrl2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetFocus() != this)
		SetFocus();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


void CListCtrl2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// For subitem editing support
	if (GetFocus() != this)
		SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CListCtrl2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// For subitem editing support
	if (GetFocus() != this)
		SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


/////////////////////////////////////////////////////////////////////////////
// CListView2

IMPLEMENT_DYNCREATE(CListView2, CListView)


BEGIN_MESSAGE_MAP(CListView2, CListView)
	//{{AFX_MSG_MAP(CListView2)
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnDeleteitem)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE, OnUpdateNotSupported)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateNotSupported)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateNotSupported)
	ON_UPDATE_COMMAND_UI(ID_REORDER, OnUpdateNotSupported)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()


CListView2::CListView2()
	: CListCtrlEx(&GetListCtrl())
{
}


CListView2::~CListView2()
{
}

/////////////////////////////////////////////////////////////////////////////
// CListView2 message handlers

void CListView2::OnDestroy()
{
	GetListCtrl().DeleteAllItems();
	CListView::OnDestroy();
}


void CListView2::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CListView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	InitMenuPopup(this, pPopupMenu, nIndex, bSysMenu);
}


BOOL CListView2::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView && m_bAutoDelete)
	{
		CListData* pData = reinterpret_cast<CListData*>(pNMListView->lParam);
		delete pData;
		pNMListView->lParam = 0;
	}
	return FALSE; // Allow parent to handle also
}


void CListView2::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool bDefault = true;
	if (m_bAllowEdit)
	//if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
	{
		int index;
		int colnum;
		if ((index = HitTestEx(point, colnum)) != -1)
		{
			UINT flag = LVIS_FOCUSED;
			if ((GetListCtrl().GetItemState(index, flag) & flag) == flag && colnum > 0)
			{
				bDefault = false;
				EditSubItem(index, colnum);
			}
		}
	}
	if (bDefault)
		CListView::OnLButtonDown(nFlags, point);
}


BOOL CListView2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetFocus() != this)
		SetFocus();
	return CListView::OnMouseWheel(nFlags, zDelta, pt);
}


void CListView2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// For subitem editing support
	if (GetFocus() != this)
		SetFocus();
	CListView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CListView2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// For subitem editing support
	if (GetFocus() != this)
		SetFocus();
	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
}


struct CListPrintData
{
	CRect r;
	int nHeight;
	int nLinesPerPage;
	int nPages;
	std::vector<int> nMaxWidth;
};


BOOL CListView2::OnPreparePrinting(CPrintInfo* pInfo)
{
	// We can't set the number of pages here cause we need the DC to determine
	// how much we can fit, so the computation is defered until BeginPrint.
	// However, that has the side effect of not populating the print dialog
	// with the page range correctly. For that to show up, it must be set here.
	// So, you must first answer the chicken and egg question...
	// Which is - disable the page selection stuff!
	// We allow the page numbers, but as noted, the max page is not set.
	pInfo->m_pPD->m_pd.Flags |= PD_NOSELECTION;
	return DoPreparePrinting(pInfo);
}


void CListView2::OnBeginPrinting(
		CDC* pDC,
		CPrintInfo* pInfo)
{
	CListPrintData* pData = new CListPrintData();
	pInfo->m_lpUserData = reinterpret_cast<void*>(pData);

	pDC->SetMapMode(MM_LOENGLISH);

	// Set the font
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	CFont font;
	fontInfo.CreateFont(font);
	pDC->SelectObject(&font);

	CSize szDevice(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	pData->r = CRect(CPoint(0,0), szDevice);
	pDC->DPtoLP(pData->r);
	CRect margins;
	CAgilityBookOptions::GetPrinterMargins(margins);
	pDC->DPtoLP(margins);
	pData->r.left += margins.left;
	pData->r.top += margins.top;
	pData->r.right -= margins.right;
	pData->r.bottom -= margins.bottom;

	int nMaxItem = GetListCtrl().GetItemCount();
	for (int i = -1; i < nMaxItem; ++i)
	{
		CStringArray line;
		GetPrintLine(i, line);
		for (size_t nCol = 0; nCol < static_cast<size_t>(line.GetSize()); ++nCol)
		{
			line[nCol] += _T(" ");
			if (nCol == pData->nMaxWidth.size())
				pData->nMaxWidth.push_back(0);
			CRect r(0,0,0,0);
			pDC->DrawText(line[nCol], &r, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
			if (r.Width() > pData->nMaxWidth[nCol])
				pData->nMaxWidth[nCol] = r.Width();
		}
	}

	CRect rTest(0,0,0,0);
	CString strTextForHeight(_T("Testing for height"));
	pDC->DrawText(strTextForHeight, &rTest, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
	pData->nHeight = 4 * abs(rTest.Height()) / 3;
	if (1 > pData->nHeight)
		pData->nHeight = 1;
	pData->nLinesPerPage = abs(pData->r.Height()) / pData->nHeight;
	if (1 > pData->nLinesPerPage)
		pData->nLinesPerPage = 1;
	pData->nPages = (GetListCtrl().GetItemCount() + 1) / pData->nLinesPerPage + 1;
	//TRACE(_T("Lines per page: %d\nLines: %d\nPages: %d\n"),
	//	pData->nLinesPerPage,
	//	GetListCtrl().GetItemCount(),
	//	pData->nPages);
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(pData->nPages);
}


void CListView2::OnEndPrinting(
		CDC* pDC,
		CPrintInfo* pInfo)
{
	CListPrintData* pData = reinterpret_cast<CListPrintData*>(pInfo->m_lpUserData);
	delete pData;
}


void CListView2::OnPrint(
		CDC* pDC,
		CPrintInfo* pInfo)
{
	pDC->SetMapMode(MM_LOENGLISH);

	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	CFont font;
	fontInfo.CreateFont(font);
	pDC->SelectObject(&font);

	CListPrintData* pData = reinterpret_cast<CListPrintData*>(pInfo->m_lpUserData);
//	pDC->Rectangle(pData->r);

	int nMaxItem = GetListCtrl().GetItemCount();
	// ListCtrl starts with item '-1'. This gets the header information.
	int nStartItem = pData->nLinesPerPage * (pInfo->m_nCurPage - 1) - 1;
	for (int nItem = nStartItem; nItem < nMaxItem && nItem - nStartItem < pData->nLinesPerPage; ++nItem)
	{
		CStringArray line;
		GetPrintLine(nItem, line);
		CRect r = pData->r;
		r.top -= (nItem - nStartItem) * pData->nHeight;
		for (int i = 0; i < line.GetSize(); ++i)
		{
			r.right = r.left + pData->nMaxWidth[i];
			pDC->DrawText(line[i], r, DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
			r.left = r.right;
		}
	}
}


void CListView2::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}


void CListView2::OnEditCopy()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		CClipboardDataWriter clpData;
		if (!clpData.isOkay())
			return;

		CString data;
		CString html;
		CClipboardDataTable table(data, html);

		// Take care of the header, but only if more than one line is selected.
		if (1 < indices.size()
		|| indices.size() == static_cast<size_t>(GetListCtrl().GetItemCount()))
		{
			CStringArray line;
			GetPrintLine(-1, line);
			table.StartLine();
			for (int i = 0; i < line.GetSize(); ++i)
			{
				table.Cell(i, line[i]);
			}
			table.EndLine();
		}

		// Now all the data.
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CStringArray line;
			GetPrintLine((*iter), line);
			table.StartLine();
			for (int i = 0; i < line.GetSize(); ++i)
			{
				table.Cell(i, line[i]);
			}
			table.EndLine();
		}

		clpData.SetData(table);
	}
}


void CListView2::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetItemCount() && (LVS_SINGLESEL != (GetStyle() & LVS_SINGLESEL)))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}


void CListView2::OnEditSelectAll()
{
	int nIndices = GetListCtrl().GetItemCount();
	if (0 < nIndices)
	{
		std::vector<int> indices;
		indices.reserve(nIndices);
		for (int i = 0; i < nIndices; ++i)
			indices.push_back(i);
		SetSelection(indices, false);
	}
}


void CListView2::OnUpdateNotSupported(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
