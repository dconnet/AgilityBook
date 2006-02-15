/*
 * Copyright © 2003-2006 David Connet. All Rights Reserved.
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
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-24 DRC Added a sort header image.
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
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBTraining.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgFind.h"
#include "DlgTraining.h"
#include "Element.h"
#include "ListData.h"
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

class CAgilityBookViewTrainingData : public CListData
{
	friend int CALLBACK CompareTraining(LPARAM, LPARAM, LPARAM);
public:
	CAgilityBookViewTrainingData(
			CAgilityBookViewTraining* pView,
			ARBTrainingPtr pTraining)
		: m_RefCount(1)
		, m_pView(pView)
		, m_pTraining(pTraining)
	{
	}
	void AddRef();
	void Release();

	bool CanEdit() const			{return true;}
	bool CanDelete() const			{return true;}

	ARBTrainingPtr GetTraining()	{return m_pTraining;}
	CString OnNeedText(int iCol) const;

private:
	~CAgilityBookViewTrainingData()
	{
	}
	UINT m_RefCount;
	CAgilityBookViewTraining* m_pView;
	ARBTrainingPtr m_pTraining;
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
			str.Replace(_T("\n"), _T(" "));
			break;
		}
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookViewTraining::CSortColumn::CSortColumn(std::vector<int>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}

void CAgilityBookViewTraining::CSortColumn::Initialize()
{
	int realCol = IO_LOG_DATE;
	realCol = AfxGetApp()->GetProfileInt(_T("Sorting"), _T("Training"), realCol);
	int neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	int col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_LOG_DATE);
	m_iCol = col * neg;
}

void CAgilityBookViewTraining::CSortColumn::SetColumn(int iCol)
{
	m_iCol = iCol;
	if (0 == iCol)
		return;
	int neg = 1;
	int col = iCol;
	if (0 > iCol)
	{
		neg = -1;
		col = iCol * -1;
	}
	int realCol = m_Columns[col-1] * neg;
	AfxGetApp()->WriteProfileInt(_T("Sorting"), _T("Training"), realCol);
}

int CAgilityBookViewTraining::CSortColumn::LookupColumn(int iCol) const
{
	size_t n = m_Columns.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (m_Columns[i] == iCol)
		{
			return static_cast<int>(i+1);
		}
	}
	return -1;
}

struct SORT_TRAINING_INFO
{
	CAgilityBookViewTraining* pThis;
	int nCol;
};

int CALLBACK CompareTraining(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	SORT_TRAINING_INFO* sortInfo = reinterpret_cast<SORT_TRAINING_INFO*>(lParam3);
	if (!sortInfo || 0 == sortInfo->nCol)
		return 0;
	CListData* pRawItem1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawItem2 = reinterpret_cast<CListData*>(lParam2);
	CAgilityBookViewTrainingData* pItem1 = dynamic_cast<CAgilityBookViewTrainingData*>(pRawItem1);
	CAgilityBookViewTrainingData* pItem2 = dynamic_cast<CAgilityBookViewTrainingData*>(pRawItem2);
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

bool CFindTraining::Search(CDlgFind* pDlg) const
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
		std::set<ARBString> strings;
		if (SearchAll())
		{
			CAgilityBookViewTrainingData* pData = m_pView->GetItemData(index);
			if (pData)
				pData->GetTraining()->GetSearchStrings(strings);
		}
		else
		{
			int nColumns = m_pView->HeaderItemCount();
			for (int i = 0; i < nColumns; ++i)
			{
				strings.insert((LPCTSTR)m_pView->GetListCtrl().GetItemText(index, i));
			}
		}
		for (std::set<ARBString>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
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
		ARBostringstream msg;
		msg << _T("Cannot find \"")
			<< (LPCTSTR)m_strSearch
			<< _T("\"");
		AfxMessageBox(msg.str().c_str(), MB_ICONINFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewTraining

IMPLEMENT_DYNCREATE(CAgilityBookViewTraining, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewTraining, CListView2)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewTraining)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
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
	, m_SortColumn(m_Columns)
{
	SetAutoDelete(true);
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

LRESULT CAgilityBookViewTraining::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_TRAINING, HH_HELP_CONTEXT);
	return 1;
}

int CAgilityBookViewTraining::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	return 0;
}

void CAgilityBookViewTraining::OnInitialUpdate()
{
	SetupColumns();
	CListView2::OnInitialUpdate();
}

void CAgilityBookViewTraining::OnActivateView(
		BOOL bActivate,
		CView* pActivateView,
		CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (pActivateView)
	{
		CString msg;
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}
}

void CAgilityBookViewTraining::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
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
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewTraining::GetPrintLine(
		int nItem,
		CStringArray& line)
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
	if (CAgilityBookOptions::GetViewAllDates()
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

bool CAgilityBookViewTraining::GetMessage2(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}

CAgilityBookViewTrainingData* CAgilityBookViewTraining::GetItemData(int index) const
{
	return dynamic_cast<CAgilityBookViewTrainingData*>(GetData(index));
}

void CAgilityBookViewTraining::SetupColumns()
{
	int nColumnCount = HeaderItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		DeleteColumn(0);
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
			InsertColumn(static_cast<int>(iCol), &col);
			str.ReleaseBuffer();
		}
		m_SortColumn.Initialize();
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
		ARBTrainingPtr pTraining = (*iter);
		if (pTraining->IsFiltered())
			continue;
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = i;
		item.iSubItem = 0;
		item.iImage = I_IMAGECALLBACK;
		item.lParam = reinterpret_cast<LPARAM>(
			static_cast<CListData*>(
				new CAgilityBookViewTrainingData(this, pTraining)));
		int index = GetListCtrl().InsertItem(&item);
		// We may have modified the entry, so don't do a full equality test.
		// Just check the start/end date, location, club and venue. This allows
		// us to modify the opens/closes dates, notes and entry status.
		// Note: This is only important when editing the entry from the other
		// calendar view! If we edit locally, this is not a problem since we
		// just modified our own entry.
		if (pCurData)
		{
			ARBTrainingPtr pCurTraining = pCurData->GetTraining();
			if (*pCurTraining == *pTraining
			|| pCurTraining->GetDate() == pTraining->GetDate())
			{
				SetSelection(index, true);
			}
		}
		++i;
	}
	int nColumnCount = HeaderItemCount();
	for (i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (IsWindowVisible())
	{
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}

	SORT_TRAINING_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn.GetColumn();
	GetListCtrl().SortItems(CompareTraining, reinterpret_cast<LPARAM>(&info));
	HeaderSort(abs(m_SortColumn.GetColumn())-1,
		info.nCol > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);

	// Cleanup.
	if (pCurData)
		pCurData->Release();
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

// CAgilityBookViewTraining message handlers

void CAgilityBookViewTraining::OnRclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, reinterpret_cast<WPARAM>(m_hWnd), GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewTraining::OnContextMenu(
		CWnd* pWnd,
		CPoint point)
{
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		int index = GetSelection();
		if (0 <= index)
			GetListCtrl().GetItemRect(index, &rect, FALSE);
		else
			GetListCtrl().GetClientRect(&rect);
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
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void CAgilityBookViewTraining::OnColumnclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	HeaderSort(abs(m_SortColumn.GetColumn())-1, CHeaderCtrl2::eNoSort);
	int nBackwards = 1;
	if (m_SortColumn.GetColumn() == pNMListView->iSubItem + 1)
		nBackwards = -1;
	m_SortColumn.SetColumn((pNMListView->iSubItem + 1) * nBackwards);
	SORT_TRAINING_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn.GetColumn();
	GetListCtrl().SortItems(CompareTraining, reinterpret_cast<LPARAM>(&info));
	HeaderSort(abs(m_SortColumn.GetColumn())-1,
		nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	*pResult = 0;
}

void CAgilityBookViewTraining::OnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
		CAgilityBookViewTrainingData* pData = dynamic_cast<CAgilityBookViewTrainingData*>(pRawData);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}

void CAgilityBookViewTraining::OnDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnTrainingEdit();
	*pResult = 0;
}

void CAgilityBookViewTraining::OnKeydown(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
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
		m_Callback.Search(NULL);
}

void CAgilityBookViewTraining::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search(NULL);
}

void CAgilityBookViewTraining::OnUpdateTrainingEdit(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetItemData(GetSelection(true)))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnTrainingEdit()
{
	CAgilityBookViewTrainingData* pData = GetItemData(GetSelection(true));
	if (pData)
	{
		CDlgTraining dlg(pData->GetTraining(), GetDocument());
		if (IDOK == dlg.DoModal())
		{
			GetDocument()->GetTraining().sort();
			LoadData();
			GetDocument()->SetModifiedFlag();
			Invalidate();
			int nColumnCount = HeaderItemCount();
			for (int i = 0; i < nColumnCount; ++i)
				GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		}
	}
}

void CAgilityBookViewTraining::OnTrainingNew()
{
	ARBTrainingPtr training(new ARBTraining());
	CDlgTraining dlg(training, GetDocument());
	if (IDOK == dlg.DoModal())
	{
		GetDocument()->GetTraining().AddTraining(training);
		GetDocument()->GetTraining().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		SetCurrentDate(training->GetDate());
		int nColumnCount = HeaderItemCount();
		for (int i = 0; i < nColumnCount; ++i)
			GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
}

void CAgilityBookViewTraining::OnUpdateEditDuplicate(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnEditDuplicate()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		std::vector<CAgilityBookViewTrainingData*> items;
		for (std::vector<int>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
		{
			CAgilityBookViewTrainingData* pData = GetItemData(*iterData);
			if (pData)
				items.push_back(pData);
		}
		ARBDate date;
		for (std::vector<CAgilityBookViewTrainingData*>::iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			// Currently, we don't need to worry if this is visible. The only filtering
			// is on name/date. So they can see the item that's being duped, which means
			// the new one is visible too.
			ARBTrainingPtr training(new ARBTraining(*((*iter)->GetTraining())));
			GetDocument()->GetTraining().AddTraining(training);
			GetDocument()->GetTraining().sort();
			date = training->GetDate();
		}
		if (0 < items.size())
		{
			LoadData();
			GetDocument()->SetModifiedFlag();
			SetCurrentDate(date);
		}
	}
}

void CAgilityBookViewTraining::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	OnUpdateEditCopy(pCmdUI);
}

void CAgilityBookViewTraining::OnEditCut()
{
	OnEditCopy();
	OnTrainingDelete();
}

void CAgilityBookViewTraining::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnEditCopy()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
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
			data += _T("\r\n");
		}

		Element tree;
		tree.SetName(CLIPDATA);

		// Now all the data.
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewTrainingData* pData = GetItemData(*iter);
			if (pData)
				pData->GetTraining()->Save(tree);
			CStringArray line;
			GetPrintLine((*iter), line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += _T("\r\n");
		}

		CopyDataToClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatLog), tree, data);
	}
}

void CAgilityBookViewTraining::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (IsClipboardFormatAvailable(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatLog)))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnEditPaste()
{
	bool bLoaded = false;
	Element tree;
	if (GetDataFromClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatLog), tree))
	{
		if (CLIPDATA == tree.GetName())
		{
			for (int i = 0; i < tree.GetElementCount(); ++i)
			{
				Element const& element = tree.GetElement(i);
				if (element.GetName() == TREE_TRAINING)
				{
					ARBTrainingPtr pLog(new ARBTraining());
					CErrorCallback err;
					if (pLog->Load(element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
					{
						bLoaded = true;
						GetDocument()->GetTraining().AddTraining(pLog);
					}
				}
			}
		}
	}
	if (bLoaded)
	{
		GetDocument()->GetCalendar().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
	}
}

void CAgilityBookViewTraining::OnUpdateTrainingDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewTraining::OnTrainingDelete()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		std::vector<CAgilityBookViewTrainingData*> items;
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewTrainingData* pData = GetItemData(*iter);
			if (pData)
				GetDocument()->GetTraining().DeleteTraining(pData->GetTraining());
		}
		GetDocument()->SetModifiedFlag();
		LoadData();
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
