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
 * @li 2004-09-09 DRC Added tooltips to header control.
 * @li 2004-09-03 DRC Using the sort icon forced alignment to left.
 * @li 2004-08-26 DRC Added GetPrintLine to CListCtrl2.
 * @li 2003-11-22 DRC When copying, only create a header if more than 1 line
 *                    is selected (or if only one line exists).
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include "stdafx.h"
#include "resource.h"
#include "ListCtrl.h"

#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static size_t GetSelection(CListCtrl& list, std::vector<int>& indices)
{
	indices.clear();
	POSITION pos = list.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		indices.push_back(list.GetNextSelectedItem(pos));
	}
	return indices.size();
}

static void SetSelection(CListCtrl& list, std::vector<int>& indices, bool bEnsureVisible)
{
	// Clear everything.
	POSITION pos = list.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		int i = list.GetNextSelectedItem(pos);
		list.SetItemState(i, 0, LVIS_SELECTED|LVIS_FOCUSED);
	}
	// Set it.
	for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		int index = *iter;
		if (index >= 0 && index < list.GetItemCount())
		{
			list.SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			if (bEnsureVisible)
				list.EnsureVisible(index, FALSE);
		}
	}
}

static void GetPrintLineImp(CListCtrl& list, int nItem, CStringArray& line)
{
	line.RemoveAll();
	int nColumns = list.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumns; ++i)
	{
		if (0 > nItem)
		{
			char buffer[1000];
			HDITEM hdr;
			hdr.mask = HDI_TEXT;
			hdr.pszText = buffer;
			hdr.cchTextMax = sizeof(buffer) / sizeof(char);
			list.GetHeaderCtrl()->GetItem(i, &hdr);
			line.Add(buffer);
		}
		else
			line.Add(list.GetItemText(nItem, i));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrl2

BEGIN_MESSAGE_MAP(CHeaderCtrl2, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrl2)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(HDN_ITEMCHANGED, OnHdnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CHeaderCtrl2::CHeaderCtrl2()
	: fBufferSize(300)
	, fpBuffer(NULL)
{
	m_ImageList.Create(16, 16, ILC_MASK, 2, 0);
	CWinApp* app = AfxGetApp();
	m_sortAscending = m_ImageList.Add(app->LoadIcon(IDI_HEADER_UP));
	m_sortDescending = m_ImageList.Add(app->LoadIcon(IDI_HEADER_DOWN));
	fpBuffer = new char[fBufferSize];
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
}

BOOL CHeaderCtrl2::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CHeaderCtrl::PreTranslateMessage(pMsg);
}

void CHeaderCtrl2::OnSize(UINT nType, int cx, int cy)
{
	CHeaderCtrl::OnSize(nType, cx, cy);
	FixTooltips();
}

void CHeaderCtrl2::OnHdnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->pitem->mask & HDI_WIDTH)
		SetColumnTip(phdr->iItem);
	*pResult = 0;
}

void CHeaderCtrl2::FixTooltips()
{
	if (::IsWindow(m_ToolTip.GetSafeHwnd()))
	{
		// Get rid of all existing tips.
		int nCols = m_ToolTip.GetToolCount();
		while (0 < nCols)
		{
			m_ToolTip.DelTool(this, nCols);
			--nCols;
		}
		// Now add them back.
		nCols = GetItemCount();
		for (int iCol = 0; iCol < nCols; ++iCol)
		{
			SetColumnTip(iCol);
		}
	}
}

void CHeaderCtrl2::SetColumnTip(int iCol)
{
	if (!::IsWindow(m_ToolTip.GetSafeHwnd()))
		return;

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());

	HDITEM item;
	item.mask = HDI_TEXT | HDI_FORMAT;
	item.pszText = fpBuffer;
	item.cchTextMax = fBufferSize;
	GetItem(iCol, &item);
	while (lstrlen(fpBuffer) == fBufferSize - 1)
	{
		delete [] fpBuffer;
		fBufferSize *= 2;
		fpBuffer = new char[fBufferSize];
		item.pszText = fpBuffer;
		item.cchTextMax = fBufferSize;
		GetItem(iCol, &item);
	}
	bool bDelTip = true;
	if (fpBuffer && *fpBuffer)
	{
		CRect rColumn;
		GetItemRect(iCol, rColumn);
		CRect rAdjusted(rColumn);
		rAdjusted.right -= GetBitmapMargin();
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
				str = "..." + str.Right(256);
			m_ToolTip.AddTool(this, str, rColumn, iCol+1);
			bDelTip = false;
		}
	}
	if (bDelTip)
		m_ToolTip.DelTool(this, iCol+1);

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

void CHeaderCtrl2::Sort(int iCol, SortOrder eOrder)
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
	SetColumnTip(iCol);
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrl2

BEGIN_MESSAGE_MAP(CListCtrl2, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrl2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CListCtrl2::CListCtrl2()
	: m_bInit(false)
{
}

CListCtrl2::~CListCtrl2()
{
}

void CListCtrl2::Init()
{
	if (!m_bInit)
	{
		if (::IsWindow(GetHeaderCtrl()->GetSafeHwnd()))
		{
			m_bInit = true;
			m_SortHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
			m_SortHeader.FixTooltips();
		}
	}
}

void CListCtrl2::FixTooltips()
{
	m_SortHeader.FixTooltips();
}

int CListCtrl2::HeaderItemCount()
{
	return GetHeaderCtrl()->GetItemCount();
}

CHeaderCtrl2::SortOrder CListCtrl2::HeaderSortOrder(int iCol) const
{
	const_cast<CListCtrl2*>(this)->Init();
	return m_SortHeader.GetSortOrder(iCol);
}

void CListCtrl2::HeaderSort(int iCol, CHeaderCtrl2::SortOrder eOrder)
{
	Init();
	m_SortHeader.Sort(iCol, eOrder);
}

int CListCtrl2::GetSelection()
{
	std::vector<int> indices;
	::GetSelection(*this, indices);
	bool bSingle = (LVS_SINGLESEL == (GetStyle() & LVS_SINGLESEL));
	if ((!bSingle && 1 == indices.size())
	|| (bSingle && 0 < indices.size()))
		return indices[0];
	else
		return -1;
}

size_t CListCtrl2::GetSelection(std::vector<int>& indices)
{
	return ::GetSelection(*this, indices);
}

void CListCtrl2::SetSelection(int index, bool bEnsureVisible)
{
	std::vector<int> indices;
	indices.push_back(index);
	::SetSelection(*this, indices, bEnsureVisible);
}

void CListCtrl2::SetSelection(std::vector<int>& indices, bool bEnsureVisible)
{
	::SetSelection(*this, indices, bEnsureVisible);
}

// This allows a derived class to print a subset of columns if it wants.
void CListCtrl2::GetPrintLine(int nItem, CStringArray& line)
{
	GetPrintLineImp(*this, nItem, line);
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrl2 message handlers


/////////////////////////////////////////////////////////////////////////////
// CListView2

IMPLEMENT_DYNCREATE(CListView2, CListView)

BEGIN_MESSAGE_MAP(CListView2, CListView)
	//{{AFX_MSG_MAP(CListView2)
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
	: m_bInit(false)
{
}

CListView2::~CListView2()
{
}

void CListView2::Init()
{
	if (!m_bInit)
	{
		if (::IsWindow(GetListCtrl().GetHeaderCtrl()->GetSafeHwnd()))
		{
			m_bInit = true;
			m_SortHeader.SubclassWindow(GetListCtrl().GetHeaderCtrl()->GetSafeHwnd());
			m_SortHeader.FixTooltips();
		}
	}
}

void CListView2::FixTooltips()
{
	m_SortHeader.FixTooltips();
}

int CListView2::HeaderItemCount()
{
	return GetListCtrl().GetHeaderCtrl()->GetItemCount();
}

CHeaderCtrl2::SortOrder CListView2::HeaderSortOrder(int iCol) const
{
	const_cast<CListView2*>(this)->Init();
	return m_SortHeader.GetSortOrder(iCol);
}

void CListView2::HeaderSort(int iCol, CHeaderCtrl2::SortOrder eOrder)
{
	Init();
	m_SortHeader.Sort(iCol, eOrder);
}

int CListView2::GetSelection()
{
	std::vector<int> indices;
	::GetSelection(GetListCtrl(), indices);
	bool bSingle = (LVS_SINGLESEL == (GetStyle() & LVS_SINGLESEL));
	if ((!bSingle && 1 == indices.size())
	|| (bSingle && 0 < indices.size()))
		return indices[0];
	else
		return -1;
}

size_t CListView2::GetSelection(std::vector<int>& indices)
{
	return ::GetSelection(GetListCtrl(), indices);
}

void CListView2::SetSelection(int index, bool bEnsureVisible)
{
	std::vector<int> indices;
	indices.push_back(index);
	::SetSelection(GetListCtrl(), indices, bEnsureVisible);
}

void CListView2::SetSelection(std::vector<int>& indices, bool bEnsureVisible)
{
	::SetSelection(GetListCtrl(), indices, bEnsureVisible);
}

// This allows a derived class to print a subset of columns if it wants.
void CListView2::GetPrintLine(int nItem, CStringArray& line)
{
	GetPrintLineImp(GetListCtrl(), nItem, line);
}

/////////////////////////////////////////////////////////////////////////////
// CListView2 message handlers

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

void CListView2::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CListPrintData *pData = new CListPrintData();
	pInfo->m_lpUserData = reinterpret_cast<void*>(pData);

	// Set the font
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	CFont font;
	fontInfo.CreateFont(font, pDC);
	pDC->SelectObject(&font);

	CSize szDevice(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	pData->r = CRect(CPoint(0,0), szDevice);
	pDC->DPtoLP(pData->r);

	int nMaxItem = GetListCtrl().GetItemCount();
	for (int i = -1; i < nMaxItem; ++i)
	{
		CStringArray line;
		GetPrintLine(i, line);
		for (size_t nCol = 0; nCol < static_cast<size_t>(line.GetSize()); ++nCol)
		{
			line[nCol] += " ";
			if (nCol == pData->nMaxWidth.size())
				pData->nMaxWidth.push_back(0);
			CRect r(0,0,0,0);
			pDC->DrawText(line[nCol], &r, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
			if (r.Width() > pData->nMaxWidth[nCol])
				pData->nMaxWidth[nCol] = r.Width();
		}
	}

	CRect rTest(0,0,0,0);
	CString strTextForHeight("Testing for height");
	pDC->DrawText(strTextForHeight, &rTest, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
	pData->nHeight = 4 * rTest.Height() / 3;
	pData->nLinesPerPage = pData->r.Height() / pData->nHeight;
	pData->nPages = (GetListCtrl().GetItemCount() + 1) / pData->nLinesPerPage + 1;
	//TRACE("Lines per page: %d\nLines: %d\nPages: %d\n",
	//	pData->nLinesPerPage,
	//	GetListCtrl().GetItemCount(),
	//	pData->nPages);
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(pData->nPages);
}

void CListView2::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CListPrintData* pData = reinterpret_cast<CListPrintData*>(pInfo->m_lpUserData);
	delete pData;
}

void CListView2::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	CFont font;
	fontInfo.CreateFont(font, pDC);
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
		r.top += (nItem - nStartItem) * pData->nHeight;
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
	if (0 < GetSelection(indices) && AfxGetMainWnd()->OpenClipboard())
	{
		EmptyClipboard();

		CString data;
		CStringArray line;

		// Take care of the header, but only if more than one line is selected.
		if (1 < indices.size()
		|| indices.size() == static_cast<size_t>(GetListCtrl().GetItemCount()))
		{
			GetPrintLine(-1, line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += "\r\n";
		}

		// Now all the data.
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CStringArray line;
			GetPrintLine((*iter), line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += "\r\n";
		}

		// alloc mem block & copy text in
		HGLOBAL temp = GlobalAlloc(GHND, data.GetLength()+1);
		if (NULL != temp)
		{
			LPTSTR str = (LPTSTR)GlobalLock(temp);
			lstrcpy(str, (LPCTSTR)data);
			GlobalUnlock((void*)temp);
			// send data to clipbard
			SetClipboardData(CF_TEXT, temp);
		}

		CloseClipboard();
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
