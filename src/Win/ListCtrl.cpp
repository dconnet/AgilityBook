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
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include "stdafx.h"
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

/////////////////////////////////////////////////////////////////////////////
// CListCtrl2

BEGIN_MESSAGE_MAP(CListCtrl2, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrl2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CListCtrl2::CListCtrl2()
{
}

CListCtrl2::~CListCtrl2()
{
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
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

CListView2::CListView2()
{
}

CListView2::~CListView2()
{
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
	line.RemoveAll();
	int nColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumns; ++i)
	{
		if (0 > nItem)
		{
			char buffer[1000];
			HDITEM hdr;
			hdr.mask = HDI_TEXT;
			hdr.pszText = buffer;
			hdr.cchTextMax = sizeof(buffer) / sizeof(char);
			GetListCtrl().GetHeaderCtrl()->GetItem(i, &hdr);
			line.Add(buffer);
		}
		else
			line.Add(GetListCtrl().GetItemText(nItem, i));
	}
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
	CListPrintData *pData = new CListPrintData;
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

		// Take care of the header.
		GetPrintLine(-1, line);
		for (int i = 0; i < line.GetSize(); ++i)
		{
			if (0 < i)
				data += '\t';
			data += line[i];
		}
		data += "\r\n";

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
