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
 * @brief implementation of the CAgilityBookViewPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2006-07-16 DRC Let user specify how to sort events within a venue.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-10-18 DRC Remember last selected item when reloading data.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2005-09-15 DRC Added code to filter multi-Qs by date (forgot it - oops!)
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-05-04 DRC Added subtotaling by division to lifetime points.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-11 DRC Mark the document dirty when creating a title.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-10-20 DRC Do not accumulate NA titling runs.
 * @li 2004-08-25 DRC Existing other pts were included in all other points.
 * @li 2004-08-12 DRC Allow creating a new title.
 * @li 2004-06-23 DRC Fixed a problem when getting the Q/NQ ratio when a filter
 *                    was in place. Trials that were hidden also removed any
 *                    runs that might have been applicable to the ratio.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 *                    Fix filtering on co-sanctioned trials.
 * @li 2004-05-27 DRC Lifetime point accumulation did not display the points
 *                    for existing runs.
 * @li 2004-05-20 DRC Add Dogs name and current date to report. Oops, just
 *                    realized the only thing that shouldn't be filtered on
 *                    runs is the Q-status (resets the last 2 changes).
 * @li 2004-05-16 DRC Do filter levels.
 * @li 2004-05-03 DRC Do not filter runs, only venues and titles.
 *                    Added percent qualifying.
 * @li 2004-02-02 DRC Added ExistingPoints accumulation.
 * @li 2004-01-05 DRC Header didn't generate properly if there were no titles.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-12-09 DRC Fixed problem tallying QQs when a 3rd run is present.
 * @li 2003-11-22 DRC Added more dog information into the report.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-28 DRC Added '*' to Title if it has been received.
 * @li 2003-10-14 DRC Fixed problem inserting other point lines.
 * @li 2003-10-13 DRC Don't tally runs that have no titling points.
 * @li 2003-08-28 DRC Completed Other Points tallying
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 * @li 2003-06-11 DRC Accumulate points based on level, not sublevel, name.
 */

#include "stdafx.h"
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "AgilityBook.h"
#include "AgilityBookViewPoints.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBDogClub.h"
#include "ARBDogTrial.h"
#include "ARBTypes.h"
#include "DlgPointsViewSort.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "PointsData.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Columns:
// 1: Venue
// 2: Reg-Number
// 1: <blank>
// 2: Date
// 3: Title
// 1: <blank>
// 2: Division
// 3: Level
// 4: Event
// 5: n Runs, n Judges, n Partners
// 6: n Qs (x %), (n clean), n Judges, n Partners
// 7: Points
// 8: SuperQs/SpeedPts(per run), total MQs/speedpts
// 9: SpeedPts (if SuperQs too) [possible, but an event like this doesn't exist]
//
// 1: Other Points
// 2: Name
// 3: Points total
#define MAX_COLUMNS		9

/////////////////////////////////////////////////////////////////////////////

class PointsData : public CListData
{
public:
	PointsData(CPointsDataBasePtr inData)
		: m_Data(inData)
	{
	}
	virtual ARBString OnNeedText(size_t index) const
	{
		return m_Data->OnNeedText(index);
	}
	virtual bool HasDetails() const
	{
		return m_Data->HasDetails();
	}
	virtual void Details() const
	{
		m_Data->Details();
	}
	virtual bool IsEqual(CPointsDataBasePtr inData)
	{
		return m_Data->IsEqual(inData);
	}

	CPointsDataBasePtr m_Data;
};

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewPoints

IMPLEMENT_DYNCREATE(CAgilityBookViewPoints, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewPoints, CListView2)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewPoints)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_UPDATE_COMMAND_UI(ID_DETAILS, OnUpdateDetails)
	ON_COMMAND(ID_DETAILS, OnDetails)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TITLE, OnUpdateAgilityNewTitle)
	ON_COMMAND(ID_AGILITY_NEW_TITLE, OnAgilityNewTitle)
	ON_COMMAND(ID_VIEW_POINTS_VIEW_SORT, OnViewPointsViewSort)
	ON_COMMAND(ID_VIEW_HIDDEN, OnViewHiddenTitles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CAgilityBookViewPoints::CAgilityBookViewPoints()
{
	SetAutoDelete(true);
}


CAgilityBookViewPoints::~CAgilityBookViewPoints()
{
}


BOOL CAgilityBookViewPoints::PreCreateWindow(CREATESTRUCT& cs)
{
	// This is actually set in TabView.cpp.
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER;
	return CListView2::PreCreateWindow(cs);
}


LRESULT CAgilityBookViewPoints::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_POINTS, HH_HELP_CONTEXT);
	return 1;
}


int CAgilityBookViewPoints::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < MAX_COLUMNS; ++i)
	{
		CString title("");
		col.fmt = LVCFMT_LEFT;
		if (1 == i)
			title.LoadString(IDS_TITLING_POINTS);
		col.pszText = title.GetBuffer(0);
		col.iSubItem = i;
		InsertColumn(i, &col);
		title.ReleaseBuffer();
	}

	return 0;
}


void CAgilityBookViewPoints::OnRclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, reinterpret_cast<WPARAM>(m_hWnd), GetMessagePos());
	*pResult = 1;
}


void CAgilityBookViewPoints::OnContextMenu(
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
	UINT idMenu = IDR_POINTS;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}


void CAgilityBookViewPoints::OnNMDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	PointsData* pData = GetItemData(GetSelection(true));
	if (pData)
		pData->Details();
	else
		MessageBeep(0);
	*pResult = 0;
}


void CAgilityBookViewPoints::OnKeydown(
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
		{
			PointsData* pData = GetItemData(GetSelection(true));
			if (pData)
				pData->Details();
			else
				MessageBeep(0);
		}
		break;
	}
	*pResult = 0;
}


void CAgilityBookViewPoints::OnLvnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
		PointsData* pData = dynamic_cast<PointsData*>(pRawData);
		if (pData)
		{
			ARBString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str.c_str(), pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
		else
			pDispInfo->item.pszText[0] = '\0';
	}
	*pResult = 0;
}


void CAgilityBookViewPoints::OnActivateView(
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


void CAgilityBookViewPoints::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
{
	if (0 == lHint
	|| (UPDATE_POINTS_VIEW & lHint)
	|| (UPDATE_OPTIONS & lHint)
	|| (UPDATE_CONFIG & lHint))
		LoadData();
}


#ifdef _DEBUG
// CAgilityBookViewPoints diagnostics
void CAgilityBookViewPoints::AssertValid() const
{
	CListView2::AssertValid();
}


void CAgilityBookViewPoints::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}


CAgilityBookDoc* CAgilityBookViewPoints::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewPoints::GetPrintLine(
		int nItem,
		CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewPoints::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookViewPoints::GetMessage(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}


bool CAgilityBookViewPoints::GetMessage2(CString& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName().c_str();
		return true;
	}
	else
	{
		msg.Empty();
		return false;
	}
}


PointsData* CAgilityBookViewPoints::GetItemData(int index) const
{
	return dynamic_cast<PointsData*>(GetData(index));
}


void CAgilityBookViewPoints::LoadData()
{
	CWaitCursor wait;

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Get the current item.
	CPointsDataBasePtr curData;
	PointsData* pCurData = GetItemData(GetSelection(true));
	if (pCurData)
		curData = pCurData->m_Data;

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	CPointsDataItems items;
	items.LoadData(this, GetDocument(), GetDocument()->GetCurrentDog());

	size_t nItems = items.NumLines();
	for (size_t nItem = 0; nItem < nItems; ++nItem)
	{
		CPointsDataBasePtr data = items.GetLine(nItem);
		if (data->IsVisible())
		{
			LVITEM item;
			item.iItem = GetListCtrl().GetItemCount();
			item.iSubItem = 0;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(new PointsData(data));
			GetListCtrl().InsertItem(&item);
		}
	}

	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (IsWindowVisible())
	{
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}

	if (curData)
	{
		int n = GetListCtrl().GetItemCount();
		for (int i = 0; i < n; ++i)
		{
			PointsData* pBase = GetItemData(i);
			if (pBase)
			{
				if (pBase->IsEqual(curData))
				{
					SetSelection(i);
					GetListCtrl().EnsureVisible(i, FALSE);
					break;
				}
			}
		}
		curData.reset();
	}

	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewPoints message handlers

void CAgilityBookViewPoints::OnUpdateDetails(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	PointsData* pData = GetItemData(GetSelection(true));
	if (pData && pData->HasDetails())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}


void CAgilityBookViewPoints::OnDetails()
{
	PointsData* pData = GetItemData(GetSelection(true));
	if (pData)
		pData->Details();
}


void CAgilityBookViewPoints::OnUpdateAgilityNewTitle(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	ARBDogPtr pDog = GetDocument()->GetCurrentDog();
	if (pDog)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}


void CAgilityBookViewPoints::OnAgilityNewTitle()
{
	ARBDogPtr pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		// Convenience! No duplicated code!
		CAgilityBookTreeDataDog data(GetDocument()->GetTreeView(), pDog);
		if (data.OnCmd(ID_AGILITY_NEW_TITLE, NULL))
			GetDocument()->SetModifiedFlag();
	}
}


void CAgilityBookViewPoints::OnViewPointsViewSort()
{
	CDlgPointsViewSort dlg(this);
	if (IDOK == dlg.DoModal())
		LoadData();
}


void CAgilityBookViewPoints::OnViewHiddenTitles()
{
	CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	for (ARBDogList::iterator iterDogs = GetDocument()->GetDogs().begin();
			iterDogs != GetDocument()->GetDogs().end();
			++iterDogs)
	{
		for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin();
				iterTitle != (*iterDogs)->GetTitles().end();
				++iterTitle)
		{
			GetDocument()->ResetVisibility(venues, *iterTitle);
		}
	}
	LoadData();
}
