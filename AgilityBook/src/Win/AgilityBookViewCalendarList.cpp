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
 * @brief implementation of the CAgilityBookViewCalendarList class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-11-22 DRC Update the view when creating or editing an entry.
 * @li 2003-11-21 DRC Enabled consistent copy and select all (old copy moved).
 * @li 2003-08-30 DRC Fixed bug when copying list to clipboard.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-07-24 DRC Calendar view didn't update when item deleted from list.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookViewCalendarList.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookViewCalendar.h"
#include "ARBCalendar.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "DlgFind.h"
#include "DlgSelectDog.h"
#include "DlgTrial.h"
#include "MainFrm.h"
#include "TabView.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendarData

class CAgilityBookViewCalendarData
{
public:
	CAgilityBookViewCalendarData(CAgilityBookViewCalendarList* pView, ARBCalendar* pCal)
		: m_RefCount(1)
		, m_pView(pView)
		, m_pCal(pCal)
	{
		if (m_pCal)
			m_pCal->AddRef();
	}
	~CAgilityBookViewCalendarData()
	{
		if (m_pCal)
			m_pCal->Release();
	}
	void AddRef();
	void Release();

	bool CanEdit() const		{return true;}
	bool CanDelete() const		{return true;}

	ARBCalendar* GetCalendar()	{return m_pCal;}
	CString OnNeedText(int iCol) const;

private:
	UINT m_RefCount;
	CAgilityBookViewCalendarList* m_pView;
	ARBCalendar* m_pCal;
};

void CAgilityBookViewCalendarData::AddRef()
{
	++m_RefCount;
}

void CAgilityBookViewCalendarData::Release()
{
	--m_RefCount;
	if (0 == m_RefCount)
		delete this;
}

CString CAgilityBookViewCalendarData::OnNeedText(int iCol) const
{
	CString str;
	if (m_pCal)
	{
		switch (m_pView->m_Columns[iCol])
		{
		case IO_CAL_START_DATE:
			str = m_pCal->GetStartDate().GetString(false, false).c_str();
			break;
		case IO_CAL_END_DATE:
			str = m_pCal->GetEndDate().GetString(false, false).c_str();
			break;
		case IO_CAL_LOCATION:
			str = m_pCal->GetLocation().c_str();
			break;
		case IO_CAL_CLUB:
			str = m_pCal->GetClub().c_str();
			break;
		case IO_CAL_VENUE:
			str = m_pCal->GetVenue().c_str();
			break;
		case IO_CAL_OPENS:
			if (m_pCal->GetOpeningDate().IsValid())
				str = m_pCal->GetOpeningDate().GetString(false, false).c_str();
			break;
		case IO_CAL_CLOSES:
			if (m_pCal->GetClosingDate().IsValid())
				str = m_pCal->GetClosingDate().GetString(false, false).c_str();
			break;
		case IO_CAL_NOTES:
			str = m_pCal->GetNote().c_str();
			str.Replace("\n", " ");
			break;
		}
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// Find

bool CFindCalendar::Search() const
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
			CAgilityBookViewCalendarData* pData = m_pView->GetItemData(index);
			if (pData)
				pData->GetCalendar()->GetSearchStrings(strings);
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
// CAgilityBookViewCalendarList

IMPLEMENT_DYNCREATE(CAgilityBookViewCalendarList, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewCalendarList, CListView2)
	//{{AFX_MSG_MAP(CAgilityBookViewCalendarList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_CREATEENTRY_CALENDAR, OnUpdateCalendarCreateEntry)
	ON_COMMAND(ID_AGILITY_CREATEENTRY_CALENDAR, OnCalendarCreateEntry)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_CALENDAR, OnUpdateCalendarEdit)
	ON_COMMAND(ID_AGILITY_EDIT_CALENDAR, OnCalendarEdit)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnCalendarNew)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_CALENDAR, OnUpdateCalendarDelete)
	ON_COMMAND(ID_AGILITY_DELETE_CALENDAR, OnCalendarDelete)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewCalendarList construction/destruction

#pragma warning (push)
#pragma warning ( disable : 4355 )
CAgilityBookViewCalendarList::CAgilityBookViewCalendarList()
	: m_bSuppressSelect(false)
	, m_Callback(this)
{
	m_ImageList.Create(16, 16, ILC_MASK, 3, 0);
	CWinApp* app = AfxGetApp();
	m_imgEmpty = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_EMPTY));
	m_imgTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_TENTATIVE));
	m_imgPlan = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN));
	m_imgPlanTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN_TENTATIVE));
	m_imgEntered = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED));
	m_imgEnteredTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED_TENTATIVE));
}
#pragma warning (pop)

CAgilityBookViewCalendarList::~CAgilityBookViewCalendarList()
{
}

BOOL CAgilityBookViewCalendarList::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER;
	return CListView2::PreCreateWindow(cs);
}

int CAgilityBookViewCalendarList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	GetListCtrl().SetImageList(&m_ImageList, LVSIL_SMALL);

	SetupColumns();

	return 0;
}

void CAgilityBookViewCalendarList::OnDestroy() 
{
	GetListCtrl().DeleteAllItems();
	CListView2::OnDestroy();
}

void CAgilityBookViewCalendarList::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
}

void CAgilityBookViewCalendarList::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (0 == lHint || (UPDATE_CALENDAR_VIEW & lHint) || (UPDATE_OPTIONS & lHint))
		LoadData();
}

#ifdef _DEBUG
// CAgilityBookViewCalendarList diagnostics
void CAgilityBookViewCalendarList::AssertValid() const
{
	CListView2::AssertValid();
}

void CAgilityBookViewCalendarList::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewCalendarList::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewCalendarList::GetPrintLine(int nItem, CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewCalendarList::IsFiltered() const
{
	return false;
}

bool CAgilityBookViewCalendarList::GetMessage(CString& msg) const
{
	msg.FormatMessage(IDS_NUM_EVENTS, GetListCtrl().GetItemCount());
	return true;
}

CAgilityBookViewCalendarData* CAgilityBookViewCalendarList::GetItemData(int index) const
{
	CAgilityBookViewCalendarData *pData = NULL;
	if (0 <= index && index < GetListCtrl().GetItemCount())
		pData = reinterpret_cast<CAgilityBookViewCalendarData*>(GetListCtrl().GetItemData(index));
	return pData;
}

void CAgilityBookViewCalendarList::SetupColumns()
{
	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewCal, IO_TYPE_VIEW_CALENDAR_LIST, m_Columns))
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

void CAgilityBookViewCalendarList::LoadData()
{
	m_bSuppressSelect = true;

	// Remember what's selected.
	CAgilityBookViewCalendarData* pCurData = GetItemData(GetSelection());
	if (pCurData)
		pCurData->AddRef();

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	bool bViewAll = CAgilityBookOptions::ViewAllCalendarEntries();
	bool bHide = CAgilityBookOptions::HideOverlappingCalendarEntries();

	// Add items.
	int i = 0;
	vector<const ARBCalendar*> entered;
	if (bHide)
		GetDocument()->GetCalendar().GetAllEntered(entered);
	for (ARBCalendarList::iterator iter = GetDocument()->GetCalendar().begin();
	iter != GetDocument()->GetCalendar().end();
	++iter)
	{
		ARBCalendar* pCal = (*iter);
		if (!bViewAll)
		{
			if (pCal->IsBefore(today))
				continue;
		}
		if (bHide)
		{
			bool bSuppress = false;
			for (vector<const ARBCalendar*>::const_iterator iterE = entered.begin();
			!bSuppress && iterE != entered.end();
			++iterE)
			{
				const ARBCalendar* pEntered = (*iterE);
				if (pCal != pEntered
				&& pCal->IsRangeOverlapped(pEntered->GetStartDate(), pEntered->GetEndDate()))
				{
					bSuppress = true;
				}
			}
			if (bSuppress)
				continue;
		}
		CAgilityBookViewCalendarData* pData = new CAgilityBookViewCalendarData(this, pCal);
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
			ARBCalendar* pCurCal = pCurData->GetCalendar();
			if (*pCurCal == *pCal
			|| (pCurCal->GetStartDate() == pCal->GetStartDate()
			&& pCurCal->GetEndDate() == pCal->GetEndDate()
			&& pCurCal->GetLocation() == pCal->GetLocation()
			&& pCurCal->GetClub() == pCal->GetClub()
			&& pCurCal->GetVenue() == pCal->GetVenue()))
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

	// Cleanup.
	if (pCurData)
		pCurData->Release();
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();

	m_bSuppressSelect = false;
}

// CAgilityBookViewCalendarList message handlers

void CAgilityBookViewCalendarList::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewCalendarList::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
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

void CAgilityBookViewCalendarList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int index = GetSelection();
	if (0 > index)
	{
		MessageBeep(0);
		return;
	}
	CAgilityBookViewCalendarData* pData = GetItemData(index);
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
	UINT idMenu = IDR_CALENDAR;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CAgilityBookViewCalendarList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CAgilityBookViewCalendarData *pData = reinterpret_cast<CAgilityBookViewCalendarData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
	}
	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		CAgilityBookViewCalendarData *pData = reinterpret_cast<CAgilityBookViewCalendarData*>(pDispInfo->item.lParam);
		if (pData)
		{
			switch (pData->GetCalendar()->GetEntered())
			{
			default:
				if (pData->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgTentative;
				else
					pDispInfo->item.iImage = m_imgEmpty;
				break;
			case ARBCalendar::ePlanning:
				if (pData->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgPlanTentative;
				else
					pDispInfo->item.iImage = m_imgPlan;
				break;
			case ARBCalendar::eEntered:
				if (pData->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgEnteredTentative;
				else
					pDispInfo->item.iImage = m_imgEntered;
				break;
			}
		}
	}
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;
	// I only care about the item being selected.
	if (0 <= pNMListView->iItem
	&& (LVIF_STATE & pNMListView->uChanged)
	&& !(LVIS_SELECTED & pNMListView->uOldState)
	&& (LVIS_SELECTED & pNMListView->uNewState))
	{
		if (!m_bSuppressSelect)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
			if (pData && pData->GetCalendar() && pData->GetCalendar()->GetStartDate().IsValid())
			{
				CAgilityBookViewCalendar* pCalView = GetDocument()->GetCalendarView();
				pCalView->SuppressSelect(true);
				pCalView->SetCurrentDate(pData->GetCalendar()->GetStartDate(), true);
				pCalView->SuppressSelect(false);
			}
		}
	}
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CAgilityBookViewCalendarData *pData = reinterpret_cast<CAgilityBookViewCalendarData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnCalendarEdit();
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		OnCalendarEdit();
		break;
	}
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnEditFind()
{
	CDlgFind dlg(m_Callback, this);
	dlg.DoModal();
}

void CAgilityBookViewCalendarList::OnEditFindNext()
{
	m_Callback.SearchDown(true);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookViewCalendarList::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search();
}

void CAgilityBookViewCalendarList::OnUpdateCalendarCreateEntry(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarCreateEntry()
{
	if (0 == GetDocument()->GetDogs().size())
		return;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData)
	{
		ARBCalendar* pCal = pData->GetCalendar();
		CWnd* pParent = GetParent()->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTabView)));
		CTabView* pView = DYNAMIC_DOWNCAST(CTabView, pParent);
		GetDocument()->CreateTrialFromCalendar(*pCal, pView);
	}
}

void CAgilityBookViewCalendarList::OnUpdateCalendarEdit(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData && pData->CanEdit())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarEdit()
{
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData && pData->CanEdit())
	{
		if (pData->CanEdit())
		{
			CDlgCalendar dlg(pData->GetCalendar(), GetDocument());
			if (IDOK == dlg.DoModal())
			{
				if (CAgilityBookOptions::AutoDeleteCalendarEntries() && pData->GetCalendar()->GetEndDate() < ARBDate::Today())
				{
					ARBDate today(ARBDate::Today());
					today -= CAgilityBookOptions::DaysTillEntryIsPast();
					GetDocument()->GetCalendar().TrimEntries(today);
				}
				GetDocument()->GetCalendar().sort();
				LoadData();
				GetDocument()->SetModifiedFlag();
				GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
			}
		}
	}
}

void CAgilityBookViewCalendarList::OnCalendarNew()
{
	ARBCalendar* cal = new ARBCalendar;
	CDlgCalendar dlg(cal, GetDocument());
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			GetDocument()->GetCalendar().AddCalendar(cal);
			GetDocument()->GetCalendar().sort();
			LoadData();
			GetDocument()->SetModifiedFlag();
			GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
		}
	}
	cal->Release();
}

void CAgilityBookViewCalendarList::OnUpdateCalendarDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData && pData->CanDelete())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarDelete()
{
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection());
	if (pData && pData->CanDelete())
	{
		if (pData->CanDelete())
		{
			ARBCalendar* cal = new ARBCalendar(*pData->GetCalendar()); // Make a copy
			for (int index = 0; index < GetListCtrl().GetItemCount(); ++index)
			{
				CAgilityBookViewCalendarData* pFind = GetItemData(index);
				// It's okay to just compare pointers here. This is being called
				// from the item being deleted, so we know the items are the same.
				if (pFind->GetCalendar()
				&& (pFind->GetCalendar() == pData->GetCalendar()))
				{
					// This will cause pData to be deleted.
					GetListCtrl().DeleteItem(index);
					pData = NULL;
					break;
				}
			}
			GetDocument()->GetCalendar().DeleteCalendar(cal);
			GetDocument()->SetModifiedFlag();
			GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
			cal->Release();
		}
	}
}

void CAgilityBookViewCalendarList::OnViewCustomize()
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eViewCal);
	if (IDOK == dlg.DoModal())
	{
		SetupColumns();
		LoadData();
	}
}
