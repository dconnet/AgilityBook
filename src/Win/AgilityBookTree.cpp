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
 * @brief implementation of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Implemented Find/FindNext.
 * @li 2003-08-30 DRC Added GetPrintLine to allow future differences between
 *                    printing and viewing (already in the listctrl)
 * @li 2003-08-28 DRC Added printing.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookTree.h"

#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "DlgDog.h"
#include "DlgFind.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Find

void CFindTree::FillTree(HTREEITEM hItem) const
{
	if (NULL == hItem)
		return;

	if (TVI_ROOT != hItem)
		m_Items.push_back(hItem);
	HTREEITEM hChildItem = m_pView->GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
	while (hChildItem)
	{
		FillTree(hChildItem);
		hChildItem = m_pView->GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
	}
}

HTREEITEM CFindTree::GetNextItem() const
{
	HTREEITEM hItem = NULL;
	if (SearchDown())
	{
		++m_Iter;
		if (m_Iter == m_Items.end())
			hItem = NULL;
		else
			hItem = *m_Iter;
	}
	else
	{
		if (m_Iter == m_Items.begin())
			hItem = NULL;
		else
		{
			--m_Iter;
			hItem = *m_Iter;
		}
	}
	return hItem;
}

bool CFindTree::Search() const
{
	bool bFound = false;
	m_Items.clear();
	FillTree(TVI_ROOT);
	if (0 == m_Items.size())
		return bFound;

	HTREEITEM hItem = m_pView->GetTreeCtrl().GetSelectedItem();
	if (NULL == hItem)
	{
		if (SearchDown())
			m_Iter = m_Items.begin();
		else
		{
			m_Iter = m_Items.end();
			--m_Iter;
		}
	}
	else
	{
		for (m_Iter = m_Items.begin(); m_Iter != m_Items.end(); ++m_Iter)
		{
			if (*m_Iter == hItem)
				break;
		}
		hItem = GetNextItem();
	}
	CString search = Text();
	if (!MatchCase())
		search.MakeLower();
	while (!bFound && NULL != hItem)
	{
		std::set<std::string> strings;
		if (SearchAll())
		{
			CAgilityBookTreeData* pData = m_pView->GetItemData(hItem);
			if (pData)
				pData->GetARBBase()->GetSearchStrings(strings);
		}
		else
		{
			strings.insert((LPCTSTR)m_pView->GetTreeCtrl().GetItemText(hItem));
		}
		for (std::set<std::string>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			CString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->GetTreeCtrl().Select(hItem, TVGN_CARET);
				m_pView->GetTreeCtrl().EnsureVisible(hItem);
				bFound = true;
			}
		}
		hItem = GetNextItem();
	}
	if (!bFound)
	{
		CString msg;
		msg.Format("Cannot find \"%s\"", (LPCTSTR)m_strSearch);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookTree

IMPLEMENT_DYNCREATE(CAgilityBookTree, CTreeView)

BEGIN_MESSAGE_MAP(CAgilityBookTree, CTreeView)
	//{{AFX_MSG_MAP(CAgilityBookTree)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_EDIT_UNDO, OnDogCmd)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
	ON_UPDATE_COMMAND_UI(ID_EXPAND, OnUpdateExpand)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAll)
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAll)
	ON_UPDATE_COMMAND_UI(ID_COLLAPSE, OnUpdateCollapse)
	ON_COMMAND(ID_COLLAPSE, OnCollapse)
	ON_UPDATE_COMMAND_UI(ID_COLLAPSE_ALL, OnUpdateCollapseAll)
	ON_COMMAND(ID_COLLAPSE_ALL, OnCollapseAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_EDIT_CUT, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_EDIT_COPY, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_EDIT_PASTE, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_DOG, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_NEW_DOG, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TITLE, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_NEW_TITLE, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TRIAL, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_NEW_TRIAL, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_RUN, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_NEW_RUN, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_DOG, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_EDIT_DOG, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_TITLE, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_EDIT_TITLE, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_TRIAL, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_EDIT_TRIAL, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_RUN, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_EDIT_RUN, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_REORDER, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_REORDER, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_DOG, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_DELETE_DOG, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_TITLE, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_DELETE_TITLE, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_TRIAL, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_DELETE_TRIAL, OnDogCmd)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_RUN, OnUpdateDogCmd)
	ON_COMMAND_EX(ID_AGILITY_DELETE_RUN, OnDogCmd)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CAgilityBookTree construction/destruction

#pragma warning (push)
#pragma warning ( disable : 4355 )
CAgilityBookTree::CAgilityBookTree()
	: m_bReset(false)
	, m_bSuppressSelect(false)
	, m_Callback(this)
	, m_pDog(NULL)
{
}
#pragma warning (pop)

CAgilityBookTree::~CAgilityBookTree()
{
}

void CAgilityBookTree::OnDestroy()
{
	if (m_pDog)
	{
		m_pDog->Release();
		m_pDog = NULL;
	}
	CTreeView::OnDestroy();
}

BOOL CAgilityBookTree::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_FULLROWSELECT | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}

void CAgilityBookTree::OnInitialUpdate()
{
	m_bSuppressSelect = true;
	CTreeView::OnInitialUpdate();
	m_bSuppressSelect = false;
}

void CAgilityBookTree::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CTreeView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
}

void CAgilityBookTree::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (0 == lHint || ((UPDATE_TREE_VIEW|UPDATE_OPTIONS|UPDATE_CONFIG) & lHint))
		LoadData();
	else if (UPDATE_NEW_TRIAL & lHint)
	{
		LoadData();
		ARBDogTrial *pTrial = reinterpret_cast<ARBDogTrial*>(pHint);
		ASSERT(pTrial);
		CAgilityBookTreeData* pData = FindData(TVI_ROOT, pTrial);
		ASSERT(pData);
		GetTreeCtrl().Select(pData->GetHTreeItem(), TVGN_CARET);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookTree printing

CString CAgilityBookTree::GetPrintLine(HTREEITEM hItem) const
{
	return GetTreeCtrl().GetItemText(hItem);
}

struct CTreePrintData
{
	CStringArray lines;
	CRect r;
	int nMaxWidth;
	int nHeight;
	int nLinesPerPage;
	int nPages;
};

BOOL CAgilityBookTree::OnPreparePrinting(CPrintInfo* pInfo)
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

void CAgilityBookTree::PrintLine(CDC* pDC, CTreePrintData *pData, HTREEITEM hItem, int indent) const
{
	if (TVI_ROOT != hItem)
	{
		CString str;
		for (int i = 0; i < indent; ++i)
			str += "   ";
		str += GetPrintLine(hItem);
		pData->lines.Add(str);
		CRect r(0,0,0,0);
		pDC->DrawText(str, &r, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
		if (r.Width() > pData->nMaxWidth)
			pData->nMaxWidth = r.Width();
	}
	HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
	while (hChildItem)
	{
		PrintLine(pDC, pData, hChildItem, indent+1);
		hChildItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
	}
}


void CAgilityBookTree::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CTreePrintData *pData = new CTreePrintData;
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

	PrintLine(pDC, pData, TVI_ROOT, 0);

	CRect rTest(0,0,0,0);
	CString strTextForHeight("Testing for height");
	pDC->DrawText(strTextForHeight, &rTest, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
	pData->nHeight = 4 * rTest.Height() / 3;
	pData->nLinesPerPage = pData->r.Height() / pData->nHeight;
	pData->nPages = (static_cast<int>(pData->lines.GetSize()) + 1) / pData->nLinesPerPage + 1;
	//TRACE("Lines per page: %d\nLines: %d\nPages: %d\n",
	//	pData->nLinesPerPage,
	//	GetListCtrl().GetItemCount(),
	//	pData->nPages);
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(pData->nPages);
}

void CAgilityBookTree::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	CTreePrintData* pData = reinterpret_cast<CTreePrintData*>(pInfo->m_lpUserData);
	delete pData;
}

void CAgilityBookTree::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	CFont font;
	fontInfo.CreateFont(font, pDC);
	pDC->SelectObject(&font);

	CTreePrintData* pData = reinterpret_cast<CTreePrintData*>(pInfo->m_lpUserData);

	int nStartItem = pData->nLinesPerPage * (pInfo->m_nCurPage - 1);
	int nMaxItem = static_cast<int>(pData->lines.GetSize());
	for (int nItem = nStartItem; nItem < nMaxItem && nItem - nStartItem < pData->nLinesPerPage; ++nItem)
	{
		CRect r = pData->r;
		r.top += (nItem - nStartItem) * pData->nHeight;
		r.right = r.left + pData->nMaxWidth;
		pDC->DrawText(pData->lines[nItem], r, DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_TOP);
	}
}

#ifdef _DEBUG
// CAgilityBookTree diagnostics
void CAgilityBookTree::AssertValid() const
{
	CTreeView::AssertValid();
}

void CAgilityBookTree::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CAgilityBookDoc* CAgilityBookTree::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

CAgilityBookTreeData* CAgilityBookTree::GetCurrentTreeItem() const
{
	return GetItemData(GetTreeCtrl().GetSelectedItem());
}

CAgilityBookTreeData* CAgilityBookTree::FindData(HTREEITEM hItem, const ARBBase* pBase) const
{
	if (!pBase)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (TVI_ROOT != hItem)
	{
		CAgilityBookTreeData* pCheck = GetItemData(hItem);
		if (pCheck && pCheck->GetARBBase() == pBase)
			pData = pCheck;
	}
	if (!pData)
	{
		HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
		while (hChildItem && !pData)
		{
			pData = FindData(hChildItem, pBase);
			hChildItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return pData;
}

CAgilityBookTreeData* CAgilityBookTree::FindData(HTREEITEM hItem, const ARBDog* pDog) const
{
	if (!pDog)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (TVI_ROOT != hItem)
	{
		CAgilityBookTreeDataDog* pCheck = dynamic_cast<CAgilityBookTreeDataDog*>(GetItemData(hItem));
		// Don't use virtual 'GetDog' on pCheck as that can give back a parent
		// node which isn't what we want here. We want the real dog node.
		if (pCheck && pCheck->GetDog() == pDog)
			pData = pCheck;
	}
	if (!pData)
	{
		HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
		while (hChildItem && !pData)
		{
			pData = FindData(hChildItem, pDog);
			hChildItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return pData;
}

CAgilityBookTreeData* CAgilityBookTree::FindData(HTREEITEM hItem, const ARBDogTrial* pTrial) const
{
	if (!pTrial)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (TVI_ROOT != hItem)
	{
		CAgilityBookTreeDataTrial* pCheck = dynamic_cast<CAgilityBookTreeDataTrial*>(GetItemData(hItem));
		if (pCheck && pCheck->GetTrial() == pTrial)
			pData = pCheck;
	}
	if (!pData)
	{
		HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
		while (hChildItem && !pData)
		{
			pData = FindData(hChildItem, pTrial);
			hChildItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return pData;
}

CAgilityBookTreeData* CAgilityBookTree::FindData(HTREEITEM hItem, const ARBDogRun* pRun) const
{
	if (!pRun)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (TVI_ROOT != hItem)
	{
		CAgilityBookTreeDataRun* pCheck = dynamic_cast<CAgilityBookTreeDataRun*>(GetItemData(hItem));
		if (pCheck && pCheck->GetRun() == pRun)
			pData = pCheck;
	}
	if (!pData)
	{
		HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hItem, TVGN_CHILD);
		while (hChildItem && !pData)
		{
			pData = FindData(hChildItem, pRun);
			hChildItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return pData;
}

HTREEITEM CAgilityBookTree::InsertDog(ARBDog* pDog, bool bSelect)
{
	if (!pDog)
		return NULL;

	CAgilityBookTreeDataDog* pDataDog = new CAgilityBookTreeDataDog(this, pDog);
	HTREEITEM hItem = GetTreeCtrl().InsertItem(TVIF_TEXT | TVIF_PARAM,
		LPSTR_TEXTCALLBACK,
		0, 0, //image, selectedimage
		0, 0, //state, statemask
		reinterpret_cast<LPARAM>(pDataDog),
		TVI_ROOT,
		TVI_LAST);
	pDataDog->SetHTreeItem(hItem);
	for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
		iterTrial != pDog->GetTrials().end();
		++iterTrial)
	{
		InsertTrial((*iterTrial), hItem);
	}
	if (bSelect)
		GetTreeCtrl().Select(hItem, TVGN_CARET);
	return hItem;
}

HTREEITEM CAgilityBookTree::InsertTrial(ARBDogTrial* pTrial, HTREEITEM hParent)
{
	if (!pTrial || pTrial->IsFiltered())
		return NULL;

	CAgilityBookTreeDataTrial* pDataTrial = new CAgilityBookTreeDataTrial(this, pTrial);
	HTREEITEM hTrial = GetTreeCtrl().InsertItem(TVIF_TEXT | TVIF_PARAM,
		LPSTR_TEXTCALLBACK,
		0, 0, //image, selectedimage
		0, 0, //state, statemask
		reinterpret_cast<LPARAM>(pDataTrial),
		hParent,
		TVI_LAST);
	pDataTrial->SetHTreeItem(hTrial);
	for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
		iterRun != pTrial->GetRuns().end();
		++iterRun)
	{
		InsertRun(pTrial, (*iterRun), hTrial);
	}
	return hTrial;
}

HTREEITEM CAgilityBookTree::InsertRun(ARBDogTrial* pTrial, ARBDogRun* pRun, HTREEITEM hParent)
{
	if (!pRun || pRun->IsFiltered())
		return NULL;

	CAgilityBookTreeDataRun* pDataRun = new CAgilityBookTreeDataRun(this, pRun);
	HTREEITEM hRun = GetTreeCtrl().InsertItem(TVIF_TEXT | TVIF_PARAM,
		LPSTR_TEXTCALLBACK,
		0, 0, //image, selectedimage
		0, 0, //state, statemask
		reinterpret_cast<LPARAM>(pDataRun),
		hParent,
		TVI_LAST);
	pDataRun->SetHTreeItem(hRun);
	return hRun;
}

bool CAgilityBookTree::IsFiltered() const
{
	return CAgilityBookOptions::IsFilterEnabled();
}

bool CAgilityBookTree::GetMessage(CString& msg) const
{
	return false;
}

void CAgilityBookTree::LoadData()
{
	m_bSuppressSelect = true;

	CWaitCursor wait;
	// Remember the currently selected item.
	const CAgilityBookTreeData* pData = GetCurrentTreeItem();
	std::vector<const ARBBase*> baseItems;
	while (pData)
	{
		baseItems.push_back(pData->GetARBBase());
		pData = pData->GetParent();
	}
	// Load the data
	CTreeCtrl& tree = GetTreeCtrl();
	tree.DeleteAllItems();
	for (ARBDogList::const_iterator iterDog = GetDocument()->GetDogs().begin();
		iterDog != GetDocument()->GetDogs().end();
		++iterDog)
	{
		InsertDog((*iterDog));
	}
	HTREEITEM hItem = NULL;
	for (std::vector<const ARBBase*>::iterator iter = baseItems.begin();
		NULL == hItem && iter != baseItems.end();
		++iter)
	{
		pData = FindData(TVI_ROOT, (*iter));
		if (pData)
			hItem = pData->GetHTreeItem();
	}
	if (NULL == hItem)
		hItem = tree.GetRootItem();
	tree.SelectItem(hItem);
	tree.Expand(hItem, TVE_EXPAND);

	m_bSuppressSelect = false;
}

CAgilityBookTreeData* CAgilityBookTree::GetItemData(HTREEITEM hItem) const
{
	CAgilityBookTreeData* pData = NULL;
	if (NULL != hItem)
		pData = reinterpret_cast<CAgilityBookTreeData*>(GetTreeCtrl().GetItemData(hItem));
	return pData;
}

// CAgilityBookTree message handlers

void CAgilityBookTree::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookTree::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CTreeView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	CCmdUI cmdUI;
	// (This may have changed for VC7+, but as of MFC4.2 it was required)
	// Hack to make this code work!!!!
	cmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (UINT n = 0; n < cmdUI.m_nIndexMax; ++n)
	{
		cmdUI.m_nIndex = n;
		cmdUI.m_nID = pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
		cmdUI.m_pMenu = pPopupMenu;
		CCmdTarget* pTarget = this;
		// Undocumented MFC cmd calls the ON_UPDATE_COMMAND_UI funcs.
		cmdUI.DoUpdate(pTarget, FALSE);
	}
}

void CAgilityBookTree::OnContextMenu(CWnd* pWnd, CPoint point)
{
	HTREEITEM hCurItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hItem = GetTreeCtrl().GetDropHilightItem();
	m_bReset = false;
	if (hItem)
		m_bReset = true;
	else
		hItem = hCurItem;
	CAgilityBookTreeData* pData = GetItemData(hItem);
	if (!pData)
		return;
	if (m_bReset)
		GetTreeCtrl().SelectItem(hItem);
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		GetTreeCtrl().GetItemRect(hItem, &rect, FALSE);
		point.x = rect.left + rect.Width() / 3;
		point.y = rect.top + rect.Height() / 2;
		ClientToScreen(&point);
	}
	UINT idMenu = pData->GetMenuID();
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		UINT id = pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
		// By processing the menu id ourselves, we can control the selection
		// item. Otherwise, the menu id gets posted and arrives back to the
		// dialog after we reset the select (in case the right click was done
		// on an item other than the current item).
		bool bTreeSelectionSet = false;
		if (pData->OnCmd(id, &bTreeSelectionSet))
			GetDocument()->SetModifiedFlag();
		if (bTreeSelectionSet)
			m_bReset = false;
	}
	if (m_bReset)
		GetTreeCtrl().SelectItem(hCurItem);
}

void CAgilityBookTree::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(pNMTreeView->itemOld.lParam);
	if (pData)
	{
		delete pData;
		pNMTreeView->itemOld.lParam = 0;
	}
	*pResult = 0;
}

void CAgilityBookTree::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pDispInfo = (TV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & TVIF_TEXT)
	{
		CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText();
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = NULL;
	}
	*pResult = 0;
}

void CAgilityBookTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!m_bSuppressSelect)
	{
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		CAgilityBookTreeData* pData = NULL;
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		if (NULL != hItem)
		{
			pData = reinterpret_cast<CAgilityBookTreeData*>(GetTreeCtrl().GetItemData(hItem));
		}
		LPARAM lHint = UPDATE_RUNS_VIEW;
		ARBDog* pDog = pData->GetDog();
		if (!m_pDog || !pDog || m_pDog != pDog)
			lHint |= UPDATE_POINTS_VIEW;
		if (m_pDog)
		{
			m_pDog->Release();
			m_pDog = NULL;
		}
		m_pDog = pDog;
		if (m_pDog)
			m_pDog->AddRef();
		GetDocument()->UpdateAllViews(this, lHint);
	}
	*pResult = 0;
}

void CAgilityBookTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
		pData->Properties();
	*pResult = 1;
}

void CAgilityBookTree::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	switch (pTVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		{
			CAgilityBookTreeData* pData = GetCurrentTreeItem();
			if (pData)
				pData->Properties();
		}
		break;
	}
	*pResult = 0;
}

void CAgilityBookTree::OnUpdateDogCmd(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData && pData->OnUpdateCmd(pCmdUI->m_nID))
		bEnable = TRUE;
	else if (!pData && ID_AGILITY_NEW_DOG == pCmdUI->m_nID)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

BOOL CAgilityBookTree::OnDogCmd(UINT id)
{
	BOOL bHandled = FALSE;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
	{
		if (pData->OnCmd(id, NULL))
			GetDocument()->SetModifiedFlag();
		bHandled = TRUE;
	}
	else
	{
		if (ID_AGILITY_NEW_DOG == id)
		{
			ARBDog* dog = new ARBDog;
			CDlgDog dlg(GetDocument()->GetConfig(), dog);
			if (IDOK == dlg.DoModal())
			{
				GetDocument()->SetModifiedFlag();
				ARBDog* pNewDog = GetDocument()->GetDogs().AddDog(dog);
				InsertDog(pNewDog, true);
			}
			dog->Release();
			bHandled = TRUE;
		}
	}
	return bHandled;
}

void CAgilityBookTree::OnEditFind()
{
	CDlgFind dlg(m_Callback, this);
	dlg.DoModal();
}

void CAgilityBookTree::OnEditFindNext()
{
	m_Callback.SearchDown(true);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookTree::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookTree::OnUpdateExpand(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem && GetTreeCtrl().ItemHasChildren(hItem))
	{
		if (!(TVIS_EXPANDED & GetTreeCtrl().GetItemState(hItem, TVIS_EXPANDED)))
			bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}

void CAgilityBookTree::OnExpand()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	GetTreeCtrl().Expand(hItem, TVE_EXPAND);
	GetTreeCtrl().EnsureVisible(hItem);
}

void CAgilityBookTree::OnUpdateExpandAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem && GetTreeCtrl().ItemHasChildren(hItem))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookTree::OnExpandAll()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	ExpandAll(GetTreeCtrl(), hItem, TVE_EXPAND);
	GetTreeCtrl().EnsureVisible(hItem);
}

void CAgilityBookTree::OnUpdateCollapse(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem && GetTreeCtrl().ItemHasChildren(hItem))
	{
		if ((TVIS_EXPANDED & GetTreeCtrl().GetItemState(hItem, TVIS_EXPANDED)))
			bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}

void CAgilityBookTree::OnCollapse()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	GetTreeCtrl().Expand(hItem, TVE_COLLAPSE);
	GetTreeCtrl().EnsureVisible(hItem);
}

void CAgilityBookTree::OnUpdateCollapseAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem && GetTreeCtrl().ItemHasChildren(hItem))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookTree::OnCollapseAll()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	ExpandAll(GetTreeCtrl(), hItem, TVE_COLLAPSE);
	GetTreeCtrl().EnsureVisible(hItem);
}
