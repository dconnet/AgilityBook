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
 * @brief implementation of the CAgilityBookViewCalendarList class
 * @author David Connet
 *
 * Revision History
 * @li 2006-05-29 DRC Sync cal view when item date changes.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-27 DRC Add color coding to calendar for entries that need attention.
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-24 DRC Added a sort header image.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-06 DRC Added cut/copy/paste support.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
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
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookViewCalendar.h"
#include "ARBCalendar.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "DlgFind.h"
#include "DlgSelectDog.h"
#include "DlgTrial.h"
#include "Element.h"
#include "FilterOptions.h"
#include "ListData.h"
#include "MainFrm.h"
#include "TabView.h"
#include "Wizard.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Offsets for first and other columns.
const int OFFSET_FIRST = 2;
const int OFFSET_OTHER = 6;

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendarData

class CAgilityBookViewCalendarData : public CListData
{
	friend int CALLBACK CompareCalendar(LPARAM, LPARAM, LPARAM);
public:
	CAgilityBookViewCalendarData(
			CAgilityBookViewCalendarList* pView,
			ARBCalendarPtr pCal)
		: m_pView(pView)
		, m_pCal(pCal)
	{
	}
	~CAgilityBookViewCalendarData()
	{
	}

	bool CanEdit() const		{return true;}
	bool CanDelete() const		{return true;}

	ARBCalendarPtr GetCalendar()	{return m_pCal;}
	CString OnNeedText(int iCol) const;
	int GetIcon() const;
	int GetStateIcon() const;

	COLORREF GetTextColor(
			int iCol,
			bool bSelected,
			bool bFocused) const;
	COLORREF GetBackgroundColor(
			int iCol,
			bool bSelected,
			bool bFocused) const;

private:
	bool HighlightOpeningNear(int iCol) const;
	bool HighlightClosingNear(int iCol) const;
	CAgilityBookViewCalendarList* m_pView;
	ARBCalendarPtr m_pCal;
};

CString CAgilityBookViewCalendarData::OnNeedText(int iCol) const
{
	CString str;
	if (m_pCal)
	{
		ARBDate::DateFormat dFmt = CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eCalList);
		switch (m_pView->m_Columns[iCol])
		{
		case IO_CAL_START_DATE:
			str = m_pCal->GetStartDate().GetString(dFmt).c_str();
			break;
		case IO_CAL_END_DATE:
			str = m_pCal->GetEndDate().GetString(dFmt).c_str();
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
				str = m_pCal->GetOpeningDate().GetString(dFmt).c_str();
			break;
		case IO_CAL_DRAWS:
			if (m_pCal->GetDrawDate().IsValid())
				str = m_pCal->GetDrawDate().GetString(dFmt).c_str();
			break;
		case IO_CAL_CLOSES:
			if (m_pCal->GetClosingDate().IsValid())
				str = m_pCal->GetClosingDate().GetString(dFmt).c_str();
			break;
		case IO_CAL_NOTES:
			str = m_pCal->GetNote().c_str();
			str.Replace(_T("\n"), _T(" "));
			break;
		}
	}
	return str;
}

int CAgilityBookViewCalendarData::GetIcon() const
{
	int idxImage = -1;
	switch (m_pCal->GetEntered())
	{
	default:
		if (m_pCal->IsTentative())
			idxImage = m_pView->m_imgTentative;
		else
			idxImage = m_pView->m_imgEmpty;
		break;
	case ARBCalendar::ePlanning:
		if (m_pCal->IsTentative())
			idxImage = m_pView->m_imgPlanTentative;
		else
			idxImage = m_pView->m_imgPlan;
		break;
	case ARBCalendar::eEntered:
		if (m_pCal->IsTentative())
			idxImage = m_pView->m_imgEnteredTentative;
		else
			idxImage = m_pView->m_imgEntered;
		break;
	}
	return idxImage;
}

int CAgilityBookViewCalendarData::GetStateIcon() const
{
	int idxImage = -1;
	if (ARBCalendar::eNot == m_pCal->GetEntered())
		idxImage = m_pView->m_imgStateEmpty;
	else switch (m_pCal->GetAccommodation())
	{
	default:
	case ARBCalendar::eAccomNone:
		idxImage = m_pView->m_imgStateNone;
		break;
	case ARBCalendar::eAccomTodo:
		idxImage = m_pView->m_imgStateTodo;
		break;
	case ARBCalendar::eAccomConfirmed:
		idxImage = m_pView->m_imgStateConfirm;
		break;
	}
	return idxImage;
}

COLORREF CAgilityBookViewCalendarData::GetTextColor(
		int iCol,
		bool bSelected,
		bool bFocused) const
{
	if (HighlightClosingNear(iCol))
		return CAgilityBookOptions::CalendarClosingNearColor();
	else if (HighlightOpeningNear(iCol))
		return CAgilityBookOptions::CalendarOpeningNearColor();
	if (bSelected)
	{
		if (bFocused)
			return ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		else
			return ::GetSysColor(COLOR_MENUTEXT);
	}
	else
		return ::GetSysColor(COLOR_WINDOWTEXT);
}

COLORREF CAgilityBookViewCalendarData::GetBackgroundColor(
		int iCol,
		bool bSelected,
		bool bFocused) const
{
	if (HighlightOpeningNear(iCol)
	|| HighlightClosingNear(iCol))
		return ::GetSysColor(COLOR_WINDOW);
	if (bSelected)
	{
		if (bFocused)
			return ::GetSysColor(COLOR_HIGHLIGHT);
		else
			return ::GetSysColor(COLOR_MENU);
	}
	else
		return ::GetSysColor(COLOR_WINDOW);
}

bool CAgilityBookViewCalendarData::HighlightOpeningNear(int iCol) const
{
	bool bHighlight = false;
	int nearDays = CAgilityBookOptions::CalendarOpeningNear();
	if (0 <= iCol && 0 <= nearDays
	&& ARBCalendar::ePlanning == m_pCal->GetEntered()
	&& m_pCal->GetOpeningDate().IsValid())
	{
		ARBDate today = ARBDate::Today();
		// If 'interval' is less than 0, then the date has passed.
		long interval = m_pCal->GetOpeningDate() - today;
		if (interval <= nearDays)
		{
			bHighlight = true;
			if (0 > interval
			&& m_pCal->GetClosingDate().IsValid()
			&& m_pCal->GetClosingDate() < today)
				bHighlight = false;
		}
	}
	return bHighlight;
}

bool CAgilityBookViewCalendarData::HighlightClosingNear(int iCol) const
{
	bool bHighlight = false;
	int nearDays = CAgilityBookOptions::CalendarClosingNear();
	if (0 <= iCol && 0 <= nearDays
	&& ARBCalendar::ePlanning == m_pCal->GetEntered()
	&& m_pCal->GetClosingDate().IsValid())
	{
		// If 'interval' is less than 0, then the date has passed.
		long interval = m_pCal->GetClosingDate() - ARBDate::Today();
		if (interval >= 0 && interval <= nearDays)
		{
			bHighlight = true;
		}
	}
	return bHighlight;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookViewCalendarList::CSortColumn::CSortColumn(std::vector<int>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}

void CAgilityBookViewCalendarList::CSortColumn::Initialize()
{
	int realCol = IO_CAL_START_DATE;
	realCol = AfxGetApp()->GetProfileInt(_T("Sorting"), _T("Calendar"), realCol);
	int neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	int col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_CAL_START_DATE);
	m_iCol = col * neg;
}

void CAgilityBookViewCalendarList::CSortColumn::SetColumn(int iCol)
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
	AfxGetApp()->WriteProfileInt(_T("Sorting"), _T("Calendar"), realCol);
}

int CAgilityBookViewCalendarList::CSortColumn::LookupColumn(int iCol) const
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

struct SORT_CAL_INFO
{
	CAgilityBookViewCalendarList* pThis;
	int nCol;
};

int CALLBACK CompareCalendar(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	SORT_CAL_INFO* sortInfo = reinterpret_cast<SORT_CAL_INFO*>(lParam3);
	if (!sortInfo || 0 == sortInfo->nCol)
		return 0;
	CListData* pRawItem1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawItem2 = reinterpret_cast<CListData*>(lParam2);
	CAgilityBookViewCalendarData* pItem1 = dynamic_cast<CAgilityBookViewCalendarData*>(pRawItem1);
	CAgilityBookViewCalendarData* pItem2 = dynamic_cast<CAgilityBookViewCalendarData*>(pRawItem2);
	int nRet = 0;
	int iCol = abs(sortInfo->nCol);
	switch (sortInfo->pThis->m_Columns[iCol-1])
	{
	case IO_CAL_START_DATE:
		if (pItem1->m_pCal->GetStartDate() < pItem2->m_pCal->GetStartDate())
			nRet = -1;
		else if (pItem1->m_pCal->GetStartDate() > pItem2->m_pCal->GetStartDate())
			nRet = 1;
		break;
	case IO_CAL_END_DATE:
		if (pItem1->m_pCal->GetEndDate() < pItem2->m_pCal->GetEndDate())
			nRet = -1;
		else if (pItem1->m_pCal->GetEndDate() > pItem2->m_pCal->GetEndDate())
			nRet = 1;
		break;
	case IO_CAL_LOCATION:
		if (pItem1->m_pCal->GetLocation() < pItem2->m_pCal->GetLocation())
			nRet = -1;
		else if (pItem1->m_pCal->GetLocation() > pItem2->m_pCal->GetLocation())
			nRet = 1;
		break;
	case IO_CAL_CLUB:
		if (pItem1->m_pCal->GetClub() < pItem2->m_pCal->GetClub())
			nRet = -1;
		else if (pItem1->m_pCal->GetClub() > pItem2->m_pCal->GetClub())
			nRet = 1;
		break;
	case IO_CAL_VENUE:
		if (pItem1->m_pCal->GetVenue() < pItem2->m_pCal->GetVenue())
			nRet = -1;
		else if (pItem1->m_pCal->GetVenue() > pItem2->m_pCal->GetVenue())
			nRet = 1;
		break;
	case IO_CAL_OPENS:
		{
			bool bOk1 = pItem1->m_pCal->GetOpeningDate().IsValid();
			bool bOk2 = pItem2->m_pCal->GetOpeningDate().IsValid();
			if (bOk1 && bOk2)
			{
				if (pItem1->m_pCal->GetOpeningDate() < pItem2->m_pCal->GetOpeningDate())
					nRet = -1;
				else if (pItem1->m_pCal->GetOpeningDate() > pItem2->m_pCal->GetOpeningDate())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_CAL_CLOSES:
		{
			bool bOk1 = pItem1->m_pCal->GetClosingDate().IsValid();
			bool bOk2 = pItem2->m_pCal->GetClosingDate().IsValid();
			if (bOk1 && bOk2)
			{
				if (pItem1->m_pCal->GetClosingDate() < pItem2->m_pCal->GetClosingDate())
					nRet = -1;
				else if (pItem1->m_pCal->GetClosingDate() > pItem2->m_pCal->GetClosingDate())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_CAL_NOTES:
		if (pItem1->m_pCal->GetNote() < pItem2->m_pCal->GetNote())
			nRet = -1;
		else if (pItem1->m_pCal->GetNote() > pItem2->m_pCal->GetNote())
			nRet = 1;
		break;
	}
	if (0 > sortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Find

bool CFindCalendar::Search(CDlgFind* pDlg) const
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
			CAgilityBookViewCalendarData* pData = m_pView->GetItemData(index);
			if (pData)
				pData->GetCalendar()->GetSearchStrings(strings);
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
				m_pView->GetListCtrl().EnsureVisible(index, FALSE);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		CString msg;
		msg.FormatMessage(IDS_CANNOT_FIND, (LPCTSTR)m_strSearch);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendarList

IMPLEMENT_DYNCREATE(CAgilityBookViewCalendarList, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewCalendarList, CListView2)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewCalendarList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_CREATEENTRY_CALENDAR, OnUpdateCalendarCreateEntry)
	ON_COMMAND(ID_AGILITY_CREATEENTRY_CALENDAR, OnCalendarCreateEntry)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EXPORT_CALENDAR, OnUpdateCalendarExport)
	ON_COMMAND(ID_AGILITY_EXPORT_CALENDAR, OnCalendarExport)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_CALENDAR, OnUpdateCalendarEdit)
	ON_COMMAND(ID_AGILITY_EDIT_CALENDAR, OnCalendarEdit)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnCalendarNew)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE, OnUpdateEditDuplicate)
	ON_COMMAND(ID_EDIT_DUPLICATE, OnEditDuplicate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
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
	, m_SortColumn(m_Columns)
{
	SetAutoDelete(true);
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 6, 0);
	CWinApp* app = AfxGetApp();
	m_imgEmpty = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_EMPTY));
	m_imgTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_TENTATIVE));
	m_imgPlan = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN));
	m_imgPlanTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN_TENTATIVE));
	m_imgEntered = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED));
	m_imgEnteredTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED_TENTATIVE));
	m_ImageStateList.Create(16, 16, ILC_MASK | ILC_COLOR32, 4, 0);
	m_imgStateEmpty = m_ImageStateList.Add(app->LoadIcon(IDI_CALENDAR_EMPTY));
	m_imgStateNone = m_ImageStateList.Add(app->LoadIcon(IDI_ACCOMMODATION_NONE));
	m_imgStateTodo = m_ImageStateList.Add(app->LoadIcon(IDI_ACCOMMODATION_TODO));
	m_imgStateConfirm = m_ImageStateList.Add(app->LoadIcon(IDI_ACCOMMODATION_CONFIRMED));
}
#pragma warning (pop)

CAgilityBookViewCalendarList::~CAgilityBookViewCalendarList()
{
}

BOOL CAgilityBookViewCalendarList::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED;
	return CListView2::PreCreateWindow(cs);
}

LRESULT CAgilityBookViewCalendarList::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_CALENDAR, HH_HELP_CONTEXT);
	return 1;
}

int CAgilityBookViewCalendarList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	GetListCtrl().SetImageList(&m_ImageStateList, LVSIL_STATE);
	GetListCtrl().SetImageList(&m_ImageList, LVSIL_SMALL);
	return 0;
}

void CAgilityBookViewCalendarList::OnDestroy() 
{
	GetListCtrl().DeleteAllItems();
	CListView2::OnDestroy();
}

DWORD CAgilityBookViewCalendarList::GetFormatFlags(int iCol) const
{
	DWORD dwFlags = DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
	DWORD dwFormat = DT_LEFT;
	CHeaderCtrl* pHdr = GetListCtrl().GetHeaderCtrl();
	if (pHdr)
	{
		HDITEM info;
		info.mask = HDI_FORMAT;
		if (pHdr->GetItem(iCol, &info))
		{
			switch (info.fmt & HDF_JUSTIFYMASK)
			{
			default:
				break;
			case HDF_CENTER:
				dwFormat = DT_CENTER;
				break;
			case HDF_RIGHT:
				dwFormat = DT_RIGHT;
				break;
			}
		}
	}
	return dwFlags | dwFormat;
}

void CAgilityBookViewCalendarList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (!GetListCtrl().GetHeaderCtrl() || lpDrawItemStruct->itemID < 0)
	{
		// No header control or no items in list.
		return;
	}
	int nColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	if (0 >= nColumns)
	{
		// No columns...
		return;
	}

	UINT state = GetListCtrl().GetItemState(lpDrawItemStruct->itemID, static_cast<UINT>(-1));
	bool bFocused = (GetFocus() == this);
	bool bSelected = ((LVIS_SELECTED & state) && (bFocused || GetListCtrl().GetStyle() & LVS_SHOWSELALWAYS));
	bSelected = bSelected || (state & LVIS_DROPHILITED);

	CListData* pRawData = reinterpret_cast<CListData*>(lpDrawItemStruct->itemData);
	CAgilityBookViewCalendarData* pData = dynamic_cast<CAgilityBookViewCalendarData*>(pRawData);

	CRect rLineFull; // Entire line (of data)
	GetListCtrl().GetItemRect(lpDrawItemStruct->itemID, &rLineFull, LVIR_BOUNDS);
	CRect rLineIcon; // Icon area - only normal icon, NOT state area also
	GetListCtrl().GetItemRect(lpDrawItemStruct->itemID, &rLineIcon, LVIR_ICON);
	CRect rLineLabel; // Area of first column
	GetListCtrl().GetItemRect(lpDrawItemStruct->itemID, &rLineLabel, LVIR_LABEL);
	CRect rLine(rLineFull);
	rLine.left = rLineLabel.left;

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	int saveOrigDc = dc.SaveDC();

	dc.SelectObject(GetFont());
	/*int oldMode =*/ dc.SetBkMode(OPAQUE /*TRANSPARENT*/);

	// Draw the background of the row.
	dc.SetBkColor(pData->GetBackgroundColor(-1, bSelected, bFocused));
	if (GetListCtrl().GetExtendedStyle() & LVS_EX_FULLROWSELECT)
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rLine, NULL, 0, NULL);
	else
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rLineLabel, NULL, 0, NULL);

	// Now the icons.
	CSize szIcon(16, 16);
	CRect rIconPos(rLineIcon);
	rIconPos.OffsetRect(0, (rLineIcon.Height() - szIcon.cy) / 2);
	CImageList* pImageStateList = GetListCtrl().GetImageList(LVSIL_STATE);
	if (pImageStateList)
	{
		CRect rIconPos2(rIconPos);
		rIconPos2.OffsetRect(-16, 0); // The state icon is beside the normal
		int idxImage = (LVIS_STATEIMAGEMASK & state) >> 12;
		if (0 < idxImage)
		{
#if _MSC_VER < 1300
			ImageList_DrawEx(pImageStateList->m_hImageList, idxImage,
				dc.GetSafeHdc(), rIconPos2.left, rIconPos2.top, szIcon.cx, szIcon.cy,
				CLR_NONE, CLR_DEFAULT, ILD_NORMAL);
#else
			pImageStateList->DrawEx(&dc, idxImage, rIconPos2.TopLeft(), szIcon, CLR_NONE, CLR_DEFAULT, ILD_NORMAL);
#endif
		}
	}
	CImageList* pImageList = GetListCtrl().GetImageList(LVSIL_SMALL);
	if (pData && pImageList)
	{
		int idxImage = pData->GetIcon();
		if (0 <= idxImage)
		{
#if _MSC_VER < 1300
			ImageList_DrawEx(pImageList->m_hImageList, idxImage,
				dc.GetSafeHdc(), rIconPos.left, rIconPos.top, szIcon.cx, szIcon.cy,
				CLR_NONE, CLR_DEFAULT, ILD_NORMAL);
#else
			pImageList->DrawEx(&dc, idxImage, rIconPos.TopLeft(), szIcon, CLR_NONE, CLR_DEFAULT, ILD_NORMAL);
#endif
			idxImage = (LVIS_OVERLAYMASK & state) >> 8;
			if (0 < idxImage)
				pImageList->Draw(&dc, 0, rIconPos.TopLeft(), (LVIS_OVERLAYMASK & state));
		}
	}

	// Finally the text.
	rLineLabel.InflateRect(-OFFSET_FIRST, 0);
	dc.SetTextColor(pData->GetTextColor(0, bSelected, bFocused));
	dc.SetBkColor(pData->GetBackgroundColor(0, bSelected, bFocused));
	dc.DrawText(pData->OnNeedText(0), rLineLabel, GetFormatFlags(0));
	for (int i = 1; i < nColumns; ++i)
	{
		CRect r;
		GetListCtrl().GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_BOUNDS, r);
		r.InflateRect(-OFFSET_OTHER, 0);
		dc.SetTextColor(pData->GetTextColor(i, bSelected, bFocused));
		dc.SetBkColor(pData->GetBackgroundColor(i, bSelected, bFocused));
		dc.DrawText(pData->OnNeedText(i), r, GetFormatFlags(i));
	}

	if ((LVIS_FOCUSED & state) && bFocused)
		dc.DrawFocusRect(rLine);

	if (0 != saveOrigDc)
		dc.RestoreDC(saveOrigDc);
	dc.Detach();
}

void CAgilityBookViewCalendarList::OnInitialUpdate()
{
	SetupColumns();
	CListView2::OnInitialUpdate();
}

void CAgilityBookViewCalendarList::OnActivateView(
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

void CAgilityBookViewCalendarList::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
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
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewCalendarList::GetPrintLine(
		int nItem,
		CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewCalendarList::IsFiltered() const
{
	if (!CFilterOptions::Options().GetViewAllDates())
		return true;
	if (!CFilterOptions::Options().GetViewAllVenues())
		return true;
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	return filter.IsFiltered();
}

bool CAgilityBookViewCalendarList::GetMessage(CString& msg) const
{
	msg.FormatMessage(IDS_NUM_EVENTS, GetListCtrl().GetItemCount());
	return true;
}

bool CAgilityBookViewCalendarList::GetMessage2(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}

CAgilityBookViewCalendarData* CAgilityBookViewCalendarList::GetItemData(int index) const
{
	return dynamic_cast<CAgilityBookViewCalendarData*>(GetData(index));
}

void CAgilityBookViewCalendarList::SetupColumns()
{
	int nColumnCount = HeaderItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewCalList, IO_TYPE_VIEW_CALENDAR_LIST, m_Columns))
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

void CAgilityBookViewCalendarList::LoadData()
{
	m_bSuppressSelect = true;

	// Remember what's selected.
	ARBCalendarPtr pCurCal;
	CAgilityBookViewCalendarData* pCurData = GetItemData(GetSelection());
	if (pCurData)
		pCurCal = pCurData->GetCalendar();

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
	std::vector<ARBCalendarPtr> entered;
	if (bHide)
		GetDocument()->GetCalendar().GetAllEntered(entered);
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	for (ARBCalendarList::iterator iter = GetDocument()->GetCalendar().begin();
	iter != GetDocument()->GetCalendar().end();
	++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (pCal->IsFiltered())
			continue;
		// Additional filtering
		if (!((ARBCalendar::eNot == pCal->GetEntered() && filter.ViewNotEntered())
		|| (ARBCalendar::ePlanning == pCal->GetEntered() && filter.ViewPlanning())
		|| (ARBCalendar::eEntered == pCal->GetEntered() && filter.ViewEntered())))
			continue;
		if (!bViewAll)
		{
			if (pCal->IsBefore(today))
				continue;
		}
		if (bHide && ARBCalendar::eEntered != pCal->GetEntered())
		{
			bool bSuppress = false;
			for (std::vector<ARBCalendarPtr>::const_iterator iterE = entered.begin();
			!bSuppress && iterE != entered.end();
			++iterE)
			{
				ARBCalendarPtr pEntered = (*iterE);
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
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = i;
		item.iSubItem = 0;
		item.iImage = I_IMAGECALLBACK;
		item.state = INDEXTOSTATEIMAGEMASK(pData->GetStateIcon());
		item.lParam = reinterpret_cast<LPARAM>(static_cast<CListData*>(pData));
		int index = GetListCtrl().InsertItem(&item);
		// We may have modified the entry, so don't do a full equality test.
		// Just check the start/end date, location, club and venue. This allows
		// us to modify the opens/closes dates, notes and entry status.
		// Note: This is only important when editing the entry from the other
		// calendar view! If we edit locally, this is not a problem since we
		// just modified our own entry.
		if (pCurCal)
		{
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

	SORT_CAL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn.GetColumn();
	GetListCtrl().SortItems(CompareCalendar, reinterpret_cast<LPARAM>(&info));
	HeaderSort(abs(m_SortColumn.GetColumn())-1,
		info.nCol > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	// Now make sure the selected item is visible.
	GetListCtrl().EnsureVisible(GetSelection(), FALSE);

	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();

	m_bSuppressSelect = false;
}

// CAgilityBookViewCalendarList message handlers

void CAgilityBookViewCalendarList::OnRclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, reinterpret_cast<WPARAM>(m_hWnd), GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewCalendarList::OnContextMenu(
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
	UINT idMenu = IDR_CALENDAR;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void CAgilityBookViewCalendarList::OnColumnclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	HeaderSort(abs(m_SortColumn.GetColumn())-1, CHeaderCtrl2::eNoSort);
	int nBackwards = 1;
	if (m_SortColumn.GetColumn() == pNMListView->iSubItem + 1)
		nBackwards = -1;
	m_SortColumn.SetColumn((pNMListView->iSubItem + 1) * nBackwards);
	SORT_CAL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn.GetColumn();
	GetListCtrl().SortItems(CompareCalendar, reinterpret_cast<LPARAM>(&info));
	HeaderSort(abs(m_SortColumn.GetColumn())-1,
		nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
	CAgilityBookViewCalendarData* pData = dynamic_cast<CAgilityBookViewCalendarData*>(pRawData);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		if (pData)
			pDispInfo->item.iImage = pData->GetIcon();
	}
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnItemChanged(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NMLISTVIEW* pNMListView = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
	// I only care about the item being selected.
	if (0 <= pNMListView->iItem
	&& (LVIF_STATE & pNMListView->uChanged)
	&& !(LVIS_SELECTED & pNMListView->uOldState)
	&& (LVIS_SELECTED & pNMListView->uNewState))
	{
		if (!m_bSuppressSelect)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(GetSelection(true));
			if (pData && pData->GetCalendar() && pData->GetCalendar()->GetStartDate().IsValid())
			{
				CAgilityBookViewCalendar* pCalView = GetDocument()->GetCalendarView();
				pCalView->SetCurrentDate(pData->GetCalendar()->GetStartDate(), true);
			}
		}
	}
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnCalendarEdit();
	*pResult = 0;
}

void CAgilityBookViewCalendarList::OnKeydown(
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
		m_Callback.Search(NULL);
}

void CAgilityBookViewCalendarList::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search(NULL);
}

void CAgilityBookViewCalendarList::OnUpdateCalendarCreateEntry(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarCreateEntry()
{
	if (0 == GetDocument()->GetDogs().size())
		return;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection(true));
	if (pData)
	{
		ARBCalendarPtr pCal = pData->GetCalendar();
		CWnd* pParent = GetParent()->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTabView)));
		CTabView* pView = DYNAMIC_DOWNCAST(CTabView, pParent);
		GetDocument()->CreateTrialFromCalendar(*pCal, pView);
	}
}

void CAgilityBookViewCalendarList::OnUpdateCalendarExport(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarExport()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		std::vector<ARBCalendarPtr> items;
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(*iter);
			if (pData)
			{
				ARBCalendarPtr pCal = pData->GetCalendar();
				items.push_back(pCal);
			}
		}
		std::vector<ARBCalendarPtr>* exportItems = NULL;
		if (0 < items.size())
			exportItems = &items;
		CWizard wiz(GetDocument(), exportItems);
		wiz.DoModal();
	}
}

void CAgilityBookViewCalendarList::OnUpdateCalendarEdit(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection(true));
	if (pData && pData->CanEdit())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarEdit()
{
	CAgilityBookViewCalendarData* pData = GetItemData(GetSelection(true));
	if (pData && pData->CanEdit())
	{
		if (pData->CanEdit())
		{
			ARBDate oldDate = pData->GetCalendar()->GetStartDate();
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
				if (oldDate != pData->GetCalendar()->GetStartDate())
				{
					CAgilityBookViewCalendar* pCalView = GetDocument()->GetCalendarView();
					pCalView->SetCurrentDate(pData->GetCalendar()->GetStartDate(), true);
				}
				LoadData();
				GetDocument()->SetModifiedFlag();
				GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
			}
		}
	}
}

void CAgilityBookViewCalendarList::OnCalendarNew()
{
	ARBCalendarPtr cal(ARBCalendar::New());
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
}

void CAgilityBookViewCalendarList::OnUpdateEditDuplicate(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnEditDuplicate()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		std::vector<CAgilityBookViewCalendarData*> items;
		for (std::vector<int>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(*iterData);
			if (pData)
				items.push_back(pData);
		}
		int nNewIsNotVisible = 0;
		for (std::vector<CAgilityBookViewCalendarData*>::iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			// We need to warn the user if the duplicated entry is not visible.
			// This will happen if the source is marked as entered and they have
			// selected the option to hide dates.
			ARBCalendarPtr cal = (*iter)->GetCalendar()->Clone();
			if ((*iter)->GetCalendar()->GetEntered() == ARBCalendar::eEntered
			&& CAgilityBookOptions::HideOverlappingCalendarEntries())
			{
				++nNewIsNotVisible;
			}
			cal->SetEntered(ARBCalendar::eNot);
			GetDocument()->GetCalendar().AddCalendar(cal);
		}
		GetDocument()->GetCalendar().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
		if (0 < nNewIsNotVisible)
		{
			CString msg;
			msg.FormatMessage(IDS_NOT_VISIBLE, nNewIsNotVisible);
			AfxMessageBox(msg, MB_ICONWARNING);
		}
	}
}

void CAgilityBookViewCalendarList::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	OnUpdateEditCopy(pCmdUI);
}

void CAgilityBookViewCalendarList::OnEditCut()
{
	OnEditCopy();
	OnCalendarDelete();
}

void CAgilityBookViewCalendarList::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnEditCopy()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		CClipboardDataWriter clpData;
		if (!clpData.isOkay())
			return;

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

		ARBostringstream iCal;
		Element tree(CLIPDATA);

		// Now all the data.
		int nWarning = CAgilityBookOptions::CalendarOpeningNear();
		ICalendar* iCalendar = ICalendar::iCalendarBegin(iCal, 2);
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(*iter);
			if (pData)
			{
				pData->GetCalendar()->Save(tree);
				pData->GetCalendar()->iCalendar(iCalendar, nWarning);
			}
			CStringArray line2;
			GetPrintLine((*iter), line2);
			for (int i = 0; i < line2.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line2[i];
			}
			data += _T("\r\n");
		}
		iCalendar->Release();

		clpData.SetData(eFormatCalendar, tree);
		clpData.SetData(data);
		clpData.SetData(eFormatiCalendar, iCal.str());
	}
}

void CAgilityBookViewCalendarList::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (CClipboardDataReader::IsFormatAvailable(eFormatCalendar))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnEditPaste()
{
	bool bLoaded = false;
	Element tree;
	CClipboardDataReader clpData;
	if (clpData.GetData(eFormatCalendar, tree))
	{
		if (CLIPDATA == tree.GetName())
		{
			for (int i = 0; i < tree.GetElementCount(); ++i)
			{
				Element const& element = tree.GetElement(i);
				if (element.GetName() == TREE_CALENDAR)
				{
					ARBCalendarPtr pCal(ARBCalendar::New());
					CErrorCallback err;
					if (pCal->Load(element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
					{
						bLoaded = true;
						GetDocument()->GetCalendar().AddCalendar(pCal);
					}
				}
			}
		}
	}
	clpData.Close();
	if (bLoaded)
	{
		GetDocument()->GetCalendar().sort();
		LoadData();
		GetDocument()->SetModifiedFlag();
		GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
	}
}

void CAgilityBookViewCalendarList::OnUpdateCalendarDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(*iter);
			if (pData && pData->CanDelete())
			{
				bEnable = TRUE;
				break;
			}
		}
	}
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendarList::OnCalendarDelete()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		std::vector<CAgilityBookViewCalendarData*> items;
		for (std::vector<int>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
		{
			CAgilityBookViewCalendarData* pData = GetItemData(*iterData);
			if (pData && pData->CanDelete())
				items.push_back(pData);
		}
		for (std::vector<CAgilityBookViewCalendarData*>::iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			GetDocument()->GetCalendar().DeleteCalendar((*iter)->GetCalendar());
		}
		if (0 < items.size())
		{
			LoadData();
			int index = indices[0];
			if (index >= GetListCtrl().GetItemCount())
				index = GetListCtrl().GetItemCount() - 1;
			SetSelection(index);
			GetListCtrl().EnsureVisible(index, FALSE);
			GetDocument()->SetModifiedFlag();
			GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
		}
	}
}

void CAgilityBookViewCalendarList::OnViewCustomize()
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eViewCalList);
	if (IDOK == dlg.DoModal())
	{
		SetupColumns();
		LoadData();
	}
}
