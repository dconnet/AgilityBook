/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookViewTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-12-14 DRC Re-sort items after editing an existing one.
 * @li 2003-09-21 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookViewTraining.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBTraining.h"
#include "DlgAssignColumns.h"
#include "DlgFind.h"
#include "DlgTraining.h"
#include "MainFrm.h"
#include "TabView.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewTrainingData

class CAgilityBookViewTrainingData
{
	friend int CALLBACK CompareTraining(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3);
public:
	CAgilityBookViewTrainingData(CAgilityBookViewTraining* pView, ARBTraining* pTraining)
		: m_RefCount(1)
		, m_pView(pView)
		, m_pTraining(pTraining)
	{
		if (m_pTraining)
			m_pTraining->AddRef();
	}
	void AddRef();
	void Release();

	bool CanEdit() const		{return true;}
	bool CanDelete() const		{return true;}

	ARBTraining* GetTraining()	{return m_pTraining;}
	CString OnNeedText(int iCol) const;

private:
	~CAgilityBookViewTrainingData()
	{
		if (m_pTraining)
			m_pTraining->Release();
	}
	UINT m_RefCount;
	CAgilityBookViewTraining* m_pView;
	ARBTraining* m_pTraining;
};

void CAgilityBookViewTrainingData::AddRef()
{
	++m_RefCount;
}

void CAgilityBookViewTrainingData::Release()
{
	--m_RefCount;
	if (0 == m_RefCount)
		delete this;
}

CString CAgilityBookViewTrainingData::OnNeedText(int iCol) const
{
	CString str;
	if (m_pTraining)
	{
		switch (m_pView->m_Columns[iCol])
		{
		case IO_LOG_DATE:
			str = m_pTraining->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eTraining)).c_str();
			break;
		case IO_LOG_NAME:
			str = m_pTraining->GetName().c_str();
			break;
		case IO_LOG_SUBNAME:
			str = m_pTraining->GetSubName().c_str();
			break;
		case IO_LOG_NOTES:
			str = m_pTraining->GetNote().c_str();
			str.Replace("\n", " ");
			break;
		}
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

struct SORT_TRAINING_INFO
{
	CAgilityBookViewTraining *pThis;
	int nCol;
};

int CALLBACK CompareTraining(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	SORT_TRAINING_INFO* sortInfo = reinterpret_cast<SORT_TRAINING_INFO*>(lParam3);
	if (!sortInfo || 0 == sortInfo->nCol)
		return 0;
	CAgilityBookViewTrainingData* pItem1 = reinterpret_cast<CAgilityBookViewTrainingData*>(lParam1);
	CAgilityBookViewTrainingData* pItem2 = reinterpret_cast<CAgilityBookViewTrainingData*>(lParam2);
	int nRet = 0;
	int iCol = abs(sortInfo->nCol);
	switch (sortInfo->pThis->m_Columns[iCol-1])
	{
	case IO_LOG_DATE:
		if (pItem1->m_pTraining->GetDate() < pItem2->m_pTraining->GetDate())
			nRet = -1;
		else if (pItem1->m_pTraining->GetDate() > pItem2->m_pTraining->GetDate())
			nRet = 1;
		break;
	case IO_LOG_NAME:
		if (pItem1->m_pTraining->GetName() < pItem2->m_pTraining->GetName())
			nRet = -1;
		else if (pItem1->m_pTraining->GetName() > pItem2->m_pTraining->GetName())
			nRet = 1;
		break;
	case IO_LOG_SUBNAME:
		if (pItem1->m_pTraining->GetSubName() < pItem2->m_pTraining->GetSubName())
			nRet = -1;
		else if (pItem1->m_pTraining->GetSubName() > pItem2->m_pTraining->GetSubName())
			nRet = 1;
		break;
	case IO_LOG_NOTES:
		if (pItem1->m_pTraining->GetNote() < pItem2->m_pTraining->GetNote())
			nRet = -1;
		else if (pItem1->m_pTraining->GetNote() > pItem2->m_pTraining->GetNote())
			nRet = 1;
		break;
	}
	if (0 > sortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Find

bool CFindTraining::Search() const
{
	bool bFound = false;
	int inc = 1;
	if (!SearchDown())
		inc = -1;
	int index = m_pView->GetSelection();
	if (0 <= index && index < m_pView->GetListCtrl().GetItemCount())
	{
		index += inc;
	}
	else if (0 > index && SearchDown())
		index = 0;
	else if (index >= m_pView->GetListCtrl().GetItemCount() && !SearchDown())
		index = m_pView->GetListCtrl().GetItemCount() - 1;
	CString search = Text();
	if (!MatchCase())
		search.MakeLower();
	for (; !bFound && 0 <= index && index < m_pView->GetListCtrl().GetItemCount(); index += inc)
	{
		std::set<std::string> strings;
		if (SearchAll())
		{
			CAgilityBookViewTrainingData* pData = m_pView->GetItemData(index);
			if (pData)
				pData->GetTraining()->GetSearchStrings(strings);
		}
		else
		{
			int nColumns = m_pView->GetListCtrl().GetHeaderCtrl()->GetItemCount();
			for (int i = 0; i < nColumns; ++i)
			{
				strings.insert((LPCTSTR)m_pView->GetListCtrl().GetItemText(index, i));
			}
		}
		for (std::set<std::string>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			CString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->SetSelection(index, true);
				bFound = true;
			}
		}
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
// CAgilityBookViewTraining

IMPLEMENT_DYNCREATE(CAgilityBookViewTraining, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewTraining, CListView2)
	//{{AFX_MSG_MAP(CAgilityBookViewTraining)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_TRAINING, OnUpdateTrainingEdit)
	ON_COMMAND(ID_AGILITY_EDIT_TRAINING, OnTrainingEdit)
	ON_COMMAND(ID_AGILITY_NEW_TRAINING, OnTrainingNew)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE, OnUpdateEditDuplicate)
	ON_COMMAND(ID_EDIT_DUPLICATE, OnEditDuplicate)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_TRAINING, OnUpdateTrainingDelete)
	ON_COMMAND(ID_AGILITY_DELETE_TRAINING, OnTrainingDelete)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewTraining construction/destruction

#pragma warning (push)
#pragma warning ( disable : 4355 )
CAgilityBookViewTraining::CAgilityBookViewTraining()
	: m_Callback(this)
	, m_SortColumn(0)
{
}
#pragma warning (pop)

CAgilityBookViewTraining::~CAgilityBookViewTraining()
{
}

BOOL CAgilityBookViewTraining::PreCreateWindow(CREATESTRUCT& cs)
{
	// This is actually set in TabView.cpp.
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS;
	return CListView2::PreCreateWindow(cs);
}

int CAgilityBookViewTraining::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	SetupColumns();

	return 0;
}

void CAgilityBookViewTraining::OnDestroy() 
{
	GetListCtrl().DeleteAllItems();
	CListView2::OnDestroy();
}

void CAgilityBookViewTraining::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
}

void CAgilityBookViewTraining::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (0 == lHint || ((UPDATE_TRAINING_VIEW | UPDATE_OPTIONS) & lHint))
		LoadData();
}

#ifdef _DEBUG
// CAgilityBookViewTraining diagnostics
void CAgilityBookViewTraining::AssertValid() const
{
	CListView2::AssertValid();
}

void CAgilityBookViewTraining::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewTraining::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewTraining::GetPrintLine(int nItem, CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

void CAgilityBookViewTraining::SetCurrentDate(ARBDate const& inDate)
{
	int index = -1;
	for (int i = 0; i < GetListCtrl().GetItemCount(); ++i)
	{
		CAgilityBookViewTrainingData* pTraining = GetItemData(i);
		if (pTraining && pTraining->GetTraining()->GetDate() == inDate)
		{
			index = i;
			break;
		}
	}
	SetSelection(index, true);
}

bool CAgilityBookViewTraining::IsFiltered() const
{
	if (CAgilityBookOptions::GetTrainingViewAllDates()
	&& CAgilityBookOptions::GetTrainingViewAllNames())
		return false;
	else
		return true;
}

bool CAgilityBookViewTraining::GetMessage(CString& msg) const
{
	msg.FormatMessage(IDS_NUM_TRAINING, GetListCtrl().GetItemCount());
	return true;
}

CAgilityBookViewTrainingData* CAgilityBookViewTraining::GetItemData(int index) const
{
	CAgilityBookViewTrainingData *pData = NULL;
	if (0 <= index && index < GetListCtrl().GetItemCount())
		pData = reinterpret_cast<CAgilityBookViewTrainingData*>(GetListCtrl().GetItemData(index));
	return pData;
}

void CAgilityBookViewTraining::SetupColumns()
{
	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewLog, IO_TYPE_VIEW_TRAINING_LIST, m_Columns))
	{
		LV_COLUMN col;
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
		for (size_t iCol = 0; iCol < m_Columns.size(); ++iCol)
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[iCol]);
			col.fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[iCol]);
			col.pszText = str.GetBuffer(0);
			col.iSubItem = static_cast<int>(iCol);
			GetListCtrl().InsertColumn(static_cast<int>(iCol), &col);
			str.ReleaseBuffer();
		}
	}
}

void CAgilityBookViewTraining::LoadData()
{
	// Remember what's selected.
	CAgilityBookViewTrainingData* pCurData = GetItemData(GetSelection());
	if (pCurData)
		pCurData->AddRef();

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Add items.
	int i = 0;
	for (ARBTrainingList::iterator iter = GetDocument()->GetTraining().begin();
	iter != GetDocument()->GetTraining().end();
	++iter)
	{
		ARBTraining* pTraining = (*iter);
		if (pTraining->IsFiltered())
			continue;
		CAgilityBookViewTrainingData* pData = new CAgilityBookViewTrainingData(this, pTraining);
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = i;
		item.iSubItem = 0;
		item.iImage = I_IMAGECALLBACK;
		item.lParam = reinterpret_cast<LPARAM>(pData);
		int index = GetListCtrl().InsertItem(&item);
		// We may have modified the entry, so don't do a full equality test.
		// Just check the start/end date, location, club and venue. This allows
		// us to modify the opens/closes dates, notes and entry status.
		// Note: This is only important when editing the entry from the other
		// calendar view! If we edit locally, this is not a problem since we
		// just modified our own entry.
		if (pCurData)
		{
			ARBTraining* pCurTraining = pCurData->GetTraining();
			if (*pCurTraining == *pTraining
			|| pCurTraining->GetDate() == pTraining->GetDate())
			{
				SetSelection(index, true);
			}
		}
		++i;
	}
	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (GetMessage(msg) && IsWindowVisible())
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());

	SORT_TRAINING_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	GetListCtrl().SortItems(CompareTraining, reinterpret_cast<LPARAM>(&info));

	// Cleanup.
	if (pCurData)
		pCurData->Release();
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

// CAgilityBookViewTraining message handlers

void CAgilityBookViewTraining::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewTraining::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CListView2::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
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

void CAgilityBookViewTraining::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int index = GetSelection();
	if (0 > index)
	{
		MessageBeep(0);
		return;
	}
	CAgilityBookViewTrainingData* pData = GetItemData(index);
	if (!pData)
		return;
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		GetListCtrl().GetItemRect(index, &rect, FALSE);
		point.x = rect.left + rect.Width() / 3;
		point.y = rect.top + rect.Height() / 2;
		ClientToScreen(&point);
	}
	UINT idMenu = IDR_TRAINING;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CAgilityBookViewTraining::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nBackwards = 1;
	if (m_SortColumn == pNMListView->iSubItem + 1)
		nBackwards = -1;
	m_SortColumn = (pNMListView->iSubItem + 1) * nBackwards;
	SORT_TRAINING_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	GetListCtrl().SortItems(CompareTraining, reinterpret_cast<LPARAM>(&info));
	*pResult = 0;
}

void CAgilityBookViewTraining::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CAgilityBookViewTrainingData *pData = reinterpret_cast<CAgilityBookViewTrainingData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}

void CAgilityBookViewTraining::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CAgilityBookViewTrainingData *pData = reinterpret_cast<CAgilityBookViewTrainingData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CAgilityBookViewTraining::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTrainingEdit();
	*pResult = 0;
}

void CAgilityBookViewTraining::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		OnTrainingEdit();
		break;
	}
	*pResult = 0;
}

void CAgilityBookViewTraining::OnEditFind()
{
	CDlgFind dlg(m_Callback, this);
	dlg.DoModal();
}

void CAgilityBookViewTraining::OnEditFindNext()
{
	m_Callback.SearchDown(true);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookViewTraining::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookViewTraining::OnUpdateTrainingEdit(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetItemData(GetSelection()))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnTrainingEdit()
{
	CAgilityBookViewTrainingData* pData = GetItemData(GetSelection());
	if (pData)
	{
		CDlgTraining dlg(pData->GetTraining(), GetDocument());
		if (IDOK == dlg.DoModal())
		{
			GetDocument()->GetTraining().sort();
			LoadData();
			GetDocument()->SetModifiedFlag();
			Invalidate();
			int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
			for (int i = 0; i < nColumnCount; ++i)
				GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		}
	}
}

void CAgilityBookViewTraining::OnTrainingNew()
{
	ARBTraining* training = new ARBTraining();
	CDlgTraining dlg(training, GetDocument());
	if (IDOK == dlg.DoModal())
	{
		GetDocument()->GetTraining().AddTraining(training);
		GetDocument()->GetTraining().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		SetCurrentDate(training->GetDate());
		int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < nColumnCount; ++i)
			GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	training->Release();
}

void CAgilityBookViewTraining::OnUpdateEditDuplicate(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetItemData(GetSelection()))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnEditDuplicate()
{
	CAgilityBookViewTrainingData* pData = GetItemData(GetSelection());
	if (pData)
	{
		// Currently, we don't need to worry if this is visible. The only filtering
		// is on name/date. So they can see the item that's being duped, which means
		// the new one is visible too.
		ARBTraining* training = new ARBTraining(*(pData->GetTraining()));
		GetDocument()->GetTraining().AddTraining(training);
		GetDocument()->GetTraining().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		SetCurrentDate(training->GetDate());
		training->Release();
	}
}

void CAgilityBookViewTraining::OnUpdateTrainingDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetItemData(GetSelection()))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnTrainingDelete()
{
	int index = GetSelection();
	if (0 <= index)
	{
		CAgilityBookViewTrainingData* pData = GetItemData(index);
		if (pData)
		{
			GetDocument()->GetTraining().DeleteTraining(pData->GetTraining());
			GetListCtrl().DeleteItem(GetSelection());
			GetDocument()->SetModifiedFlag();
		}
	}
}

void CAgilityBookViewTraining::OnViewCustomize()
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eViewLog);
	if (IDOK == dlg.DoModal())
	{
		SetupColumns();
		LoadData();
	}
}
