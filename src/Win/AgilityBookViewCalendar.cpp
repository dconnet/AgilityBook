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
 * @brief implementation of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-13 DRC Fixed a problem with scroll bar not appearing.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-10-04 DRC Added div-by-0 tests.
 * @li 2004-09-28 DRC Set the page scrolling size to 4 weeks.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-11-22 DRC Update the list when creating an entry.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-31 DRC Cleaned up pagedown scrolling.
 * @li 2003-10-29 DRC Support the mouse wheel for scrolling.
 * @li 2003-10-22 DRC Right click sets the current date.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-09 DRC When dbl-clicking on a date, make sure all entries are
 *                    visible - even if 'hide on entered' option is on.
 */

#include "stdafx.h"
#include <set>
#include "AgilityBook.h"
#include "AgilityBookViewCalendar.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBCalendar.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "DlgListCtrl.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "TabView.h"

using namespace std;

#define WEEKS_PER_PAGE		6
#define DAY_TEXT_INSET		2
#define INC_FULL_RECT		1 // Used to include right/bottom of rect when drawing

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar

IMPLEMENT_DYNCREATE(CAgilityBookViewCalendar, CView)

BEGIN_MESSAGE_MAP(CAgilityBookViewCalendar, CView)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewCalendar)
	ON_WM_INITMENUPOPUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_CALENDAR, OnUpdateCalendarEdit)
	ON_COMMAND(ID_AGILITY_EDIT_CALENDAR, OnCalendarEdit)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnCalendarNew)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_CALENDAR, OnUpdateCalendarDelete)
	ON_COMMAND(ID_AGILITY_DELETE_CALENDAR, OnAgilityCalendarDelete)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_CREATEENTRY_CALENDAR, OnUpdateAgilityCreateentryCalendar)
	ON_COMMAND(ID_AGILITY_CREATEENTRY_CALENDAR, OnAgilityCreateentryCalendar)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE, OnUpdateNotHandled)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateNotHandled)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateNotHandled)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CAgilityBookViewCalendar construction/destruction

CAgilityBookViewCalendar::CAgilityBookViewCalendar()
	: m_First()
	, m_Last()
	, m_nMonths(0)
	, m_nCurOffset(-1)
	, m_Current()
{
}

CAgilityBookViewCalendar::~CAgilityBookViewCalendar()
{
}

BOOL CAgilityBookViewCalendar::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

LRESULT CAgilityBookViewCalendar::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_CALENDAR, HH_HELP_CONTEXT);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewCalendar::SetCurrentDate(
		ARBDate const& date,
		bool bEnsureVisible)
{
	bool bSet = false;
	ARBDate dateFirstVisible = FirstDayOfWeek(m_First);
	ARBDate dateLastVisible(m_Last.GetYear(), m_Last.GetMonth(), 1);
	dateLastVisible = FirstDayOfWeek(dateLastVisible);
	dateLastVisible += WEEKS_PER_PAGE * 7 - 1;
	if (date != m_Current
	&& dateFirstVisible <= date && date <= dateLastVisible)
	{
		bSet = true;
		// Invalidate the current date.
		if (m_Current.IsValid())
		{
			CRect r = GetDateRect(m_Current);
			InvalidateRect(r);
		}
		// Now handle the new date.
		m_Current = date;
		if (m_Current.IsValid())
		{
			bool bInvalidate = true;
			if (bEnsureVisible)
			{
				ARBDate dateTop = FirstDayOfMonth(GetScrollPos(SB_VERT));
				ARBDate dateBottom = dateTop + WEEKS_PER_PAGE * 7 - 1;
				if (!m_Current.isBetween(dateTop, dateBottom))
				{
					int offset = (m_Current.GetYear() - m_First.GetYear()) * 12 + m_Current.GetMonth() - m_First.GetMonth();
					SetScrollPos(SB_VERT, offset);
					Invalidate();
					bInvalidate = false;
				}
			}
			if (bInvalidate)
			{
				CRect r = GetDateRect(m_Current);
				InvalidateRect(r);
			}
		}
	}
	return bSet;
}

bool CAgilityBookViewCalendar::IsFiltered() const
{
	if (!CFilterOptions::Options().GetViewAllDates())
		return true;
	if (!CFilterOptions::Options().GetViewAllVenues())
		return true;
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	return filter.IsFiltered();
}

bool CAgilityBookViewCalendar::GetMessage(CString& msg) const
{
	msg.FormatMessage(IDS_NUM_EVENTS, m_Calendar.size());
	return true;
}

bool CAgilityBookViewCalendar::GetMessage2(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}

size_t CAgilityBookViewCalendar::GetEntriesOn(
		ARBDate const& date,
		std::vector<ARBCalendarPtr>& entries,
		bool bGetHidden) const
{
	entries.clear();
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	for (vector<ARBCalendarPtr>::const_iterator iter = m_Calendar.begin();
		iter != m_Calendar.end();
		++iter)
	{
		ARBCalendarPtr pCal = *iter;
		if (pCal->InRange(date)
		&& ((ARBCalendar::eNot == pCal->GetEntered() && filter.ViewNotEntered())
		|| (ARBCalendar::ePlanning == pCal->GetEntered() && filter.ViewPlanning())
		|| (ARBCalendar::eEntered == pCal->GetEntered() && filter.ViewEntered())))
		{
			entries.push_back((*iter));
		}
		// Only show opening/closing dates if we're planning on entering
		else if (ARBCalendar::ePlanning == pCal->GetEntered()
		&& filter.ViewPlanning())
		{
			if ((CAgilityBookOptions::ViewAllCalendarOpening()
			&& pCal->GetOpeningDate() == date)
			|| (CAgilityBookOptions::ViewAllCalendarClosing()
			&& pCal->GetClosingDate() == date))
				entries.push_back((*iter));
		}
	}
	if (bGetHidden)
	{
		for (vector<ARBCalendarPtr>::const_iterator iter = m_CalendarHidden.begin();
			iter != m_CalendarHidden.end();
			++iter)
		{
			if ((*iter)->InRange(date))
				entries.push_back((*iter));
		}
	}
	return entries.size();
}

void CAgilityBookViewCalendar::LoadData()
{
	SetRedraw(FALSE);

	// Clear everything.
	m_Calendar.clear();
	m_CalendarHidden.clear();
	m_Last = ARBDate::Today();
	m_First = ARBDate::Today();

	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewCal, IO_TYPE_VIEW_CALENDAR, m_Columns);

	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	bool bViewAll = CAgilityBookOptions::ViewAllCalendarEntries();
	bool bHide = CAgilityBookOptions::HideOverlappingCalendarEntries();
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();

	// Add items.
	std::vector<ARBCalendarPtr> entered;
	if (bHide)
		GetDocument()->GetCalendar().GetAllEntered(entered);
	for (ARBCalendarList::iterator iter = GetDocument()->GetCalendar().begin();
		iter != GetDocument()->GetCalendar().end();
		++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (pCal->IsFiltered())
			continue;
		bool bSuppress = false;
		if (!bViewAll)
		{
			if (pCal->IsBefore(today))
				bSuppress = true;
		}
		if (!bSuppress && bHide)
		{
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
		}
		if (bSuppress)
			m_CalendarHidden.push_back(pCal);
		else
		{
			bool bAdd = false;
			ARBDate f, l;
			if (filter.ViewNotEntered()
			|| filter.ViewPlanning()
			|| filter.ViewEntered())
			{
				bAdd = true;
				f = pCal->GetStartDate();
				l = pCal->GetEndDate();
			}
			if (ARBCalendar::ePlanning == pCal->GetEntered()
			&& filter.ViewPlanning())
			{
				if (CAgilityBookOptions::ViewAllCalendarOpening()
				&& pCal->GetOpeningDate().IsValid())
				{
					bAdd = true;
					if (!f.IsValid() || pCal->GetOpeningDate() < f)
						f = pCal->GetOpeningDate();
					if (!l.IsValid() || pCal->GetOpeningDate() > l)
						l = pCal->GetOpeningDate();
				}
				if (CAgilityBookOptions::ViewAllCalendarClosing()
				&& pCal->GetClosingDate().IsValid())
				{
					bAdd = true;
					if (!f.IsValid() || pCal->GetClosingDate() < f)
						f = pCal->GetClosingDate();
					if (!l.IsValid() || pCal->GetClosingDate() > l)
						l = pCal->GetClosingDate();
				}
			}
			if (f.IsValid() && (!m_First.IsValid() || f < m_First))
				m_First = f;
			if (l.IsValid() && (!m_Last.IsValid() || l > m_Last))
				m_Last = l;
			if (bAdd)
				m_Calendar.push_back(pCal);
		}
	}
	if (!m_Current.IsValid()
	|| m_Current < m_First)
		m_Current = ARBDate::Today();
	// Set to first day of month
	m_First.SetDate(m_First.GetYear(), m_First.GetMonth(), 1);
	// Set of last day of month
	if (12 == m_Last.GetMonth())
		m_Last.SetDate(m_Last.GetYear() + 1, 1, 1);
	else
		m_Last.SetDate(m_Last.GetYear(), m_Last.GetMonth() + 1, 1);
	--m_Last;
	m_nMonths = (m_Last.GetYear() - m_First.GetYear()) * 12 + m_Last.GetMonth() - m_First.GetMonth() + 1;
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	info.nMin = 0;
	info.nMax = m_nMonths - 1;
	info.nPos = 0;
	info.nPage = 1;
	if (info.nMin == info.nMax)
	{
		// There seems to be a bug where the first time the scroll bar is set,
		// if the range is 0, then the scroll bar is not presented even though
		// we asked for it (SIF_DISABLENOSCROLL).
		info.nMax = info.nMin + 1;
		SetScrollInfo(SB_VERT, &info, FALSE);
		info.nMax = info.nMin;
	}
	SetScrollInfo(SB_VERT, &info, FALSE);

	// Now initialize all our drawing stuff.
	if (m_fontMonthPrint.GetSafeHandle())
		m_fontMonthPrint.DeleteObject();
	if (m_fontTextPrint.GetSafeHandle())
		m_fontTextPrint.DeleteObject();
	if (m_fontMonth.GetSafeHandle())
		m_fontMonth.DeleteObject();
	if (m_fontText.GetSafeHandle())
		m_fontText.DeleteObject();
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	fontInfo.CreateFont(m_fontTextPrint);
	fontInfo.size *= 3;
	fontInfo.CreateFont(m_fontMonthPrint);
	CAgilityBookOptions::GetCalendarFontInfo(fontInfo);
	fontInfo.CreateFont(m_fontText);
	fontInfo.size *= 3;
	fontInfo.CreateFont(m_fontMonth);

	CString msg;
	if (IsWindowVisible())
	{
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}

	// Make sure the current date is visible.
	ARBDate date(m_Current);
	m_Current.clear();
	SetCurrentDate(date, true);

	SetRedraw(TRUE);
	Invalidate();
}

/**
 * Returns the working rect for a date. Borders are handled entirely separately.
 * Note, this is used ONLY on the UI side, not while printing.
 */
CRect CAgilityBookViewCalendar::GetDateRect(ARBDate const& date)
{
	ARBDate visible1 = FirstDayOfMonth(GetScrollPos(SB_VERT));
	ARBDate visible2(visible1);
	visible2 += WEEKS_PER_PAGE * 7 - 1;
	
	if (!date.isBetween(visible1, visible2))
		return CRect(0,0,0,0);

	int span = date - visible1;
	int x = span % 7;
	int y = span / 7;

	CClientDC dc(this);

	CRect rClient;
	GetClientRect(rClient);

	CFont* pOldFont = dc.SelectObject(&m_fontMonth);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	rClient.top += tm.tmHeight;
	dc.SelectObject(&m_fontText);
	dc.GetTextMetrics(&tm);
	rClient.top += tm.tmHeight + 1;
	dc.SelectObject(pOldFont);

	int width = (rClient.Width() - 1) / 7;
	rClient.right = rClient.left + width * 7 + 1;
	int height = (rClient.Height() - 1) / WEEKS_PER_PAGE;
	rClient.bottom = rClient.top + height * WEEKS_PER_PAGE + 1;

	CRect r(rClient.TopLeft(), CSize(width + INC_FULL_RECT, height + INC_FULL_RECT));
	r.InflateRect(-1, -1); // Internal working area
	r.OffsetRect(x * width, y * height);
	return r;
}

ARBDate CAgilityBookViewCalendar::FirstDayOfWeek(ARBDate const& inDate) const
{
	ARBDate date(inDate);
	date -= date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	return date;
}

ARBDate CAgilityBookViewCalendar::LastDayOfWeek(ARBDate const& inDate) const
{
	ARBDate date(inDate);
	date += 6 - date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	return date;
}

// Get the first day on the visible month.
ARBDate CAgilityBookViewCalendar::FirstDayOfMonth(int inOffsetMonth) const
{
	ARBDate date = FirstDayOfWeek(m_First);
	if (0 < inOffsetMonth)
	{
		date = LastDayOfWeek(date); // Go to end-of-week (make sure month is correct)
		// Push forward offset months
		int y = date.GetYear();
		int m = date.GetMonth();
		y += inOffsetMonth / 12;
		m += inOffsetMonth  % 12;
		if (m > 12)
		{
			++y;
			m -= 12;
		}
		// Set to first day of month
		date.SetDate(y, m, 1);
		// Then fix 1st day of week.
		date = FirstDayOfWeek(date);
	}
	return date;
}

void CAgilityBookViewCalendar::GetDateFromPoint(
		CPoint pt,
		ARBDate& date)
{
	date.clear();

	CClientDC dc(this);

	CRect rClient;
	GetClientRect(rClient);

	CFont* pOldFont = dc.SelectObject(&m_fontMonth);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	rClient.top += tm.tmHeight;
	dc.SelectObject(&m_fontText);
	dc.GetTextMetrics(&tm);
	rClient.top += tm.tmHeight + 1;
	dc.SelectObject(pOldFont);

	int width = (rClient.Width() - 1) / 7;
	rClient.right = rClient.left + width * 7 + 1;
	int height = (rClient.Height() - 1) / WEEKS_PER_PAGE;
	rClient.bottom = rClient.top + height * WEEKS_PER_PAGE + 1;

	if (rClient.PtInRect(pt))
	{
		int x = (pt.x - rClient.left) / width;
		int y = (pt.y - rClient.top) / height;

		date = FirstDayOfMonth(GetScrollPos(SB_VERT));
		date += x + y * 7;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CAgilityBookViewCalendar::OnActivateView(
		BOOL bActivate,
		CView* pActivateView,
		CView* pDeactiveView) 
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (pActivateView)
	{
		CString msg;
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}
	if (m_Current.IsValid())
	{
		CRect r = GetDateRect(m_Current);
		InvalidateRect(r);
	}
}

void CAgilityBookViewCalendar::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
{
	if (0 == lHint || (UPDATE_CALENDAR_VIEW & lHint) || (UPDATE_OPTIONS & lHint))
		LoadData();
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar drawing

void CAgilityBookViewCalendar::OnDraw(CDC* pDC) 
{
	if (m_First.IsValid() && m_Last.IsValid())
	{
		int yInc = 1;
		if (pDC->IsPrinting())
		{
			yInc = -1;
			pDC->SetMapMode(MM_LOENGLISH);
		}

		CPen penFrame, penHilite, penShadow;
		penFrame.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
		penShadow.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
		penHilite.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHILIGHT));
		CPen* pOldPen = pDC->SelectObject(&penFrame); // Init for later cleanup

		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetBkMode(TRANSPARENT);

		// Colors for current selected date
		bool bActive = (reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->GetActiveView() == this);

		CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
		COLORREF clrNotEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorNotEntered);
		COLORREF clrPlanning = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPlanning);
		COLORREF clrOpening = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorOpening);
		COLORREF clrClosing = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorClosing);
		COLORREF clrEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorEntered);

		// Figure out which month we're on.
		int pos = m_nCurOffset;
		if (!pDC->IsPrinting())
			pos = GetScrollPos(SB_VERT);
		ARBDate curMonth = FirstDayOfMonth(pos);
		curMonth = LastDayOfWeek(curMonth);
		CTime t(curMonth.GetDate());
		CString str = t.Format(_T("%B %Y"));

		// Total working space.
		CRect rClient(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		if (!pDC->IsPrinting())
			GetClientRect(rClient);
		pDC->DPtoLP(rClient);
		if (pDC->IsPrinting())
		{
			CRect margins;
			CAgilityBookOptions::GetPrinterMargins(margins);
			pDC->DPtoLP(margins);
			rClient.left += margins.left;
			rClient.top += margins.top;
			rClient.right -= margins.right;
			rClient.bottom -= margins.bottom;
		}

		// Print month text
		CFont* pOldFont = pDC->SelectObject(&m_fontMonth);
		if (pDC->IsPrinting())
			pDC->SelectObject(&m_fontMonthPrint);
		pDC->DrawText(str, rClient, DT_NOPREFIX|DT_SINGLELINE|DT_TOP|DT_CENTER);

		// Figure out working areas (weekdays, days).
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		rClient.top += yInc * tm.tmHeight; // Offset for Month text
		if (pDC->IsPrinting())
			pDC->SelectObject(&m_fontTextPrint);
		else
			pDC->SelectObject(&m_fontText);
		pDC->GetTextMetrics(&tm); // We'll use 'tm' below also.
		CRect rWeekDays(rClient); // Working rect for printing day-of-week
		rWeekDays.bottom = rWeekDays.top + yInc * (tm.tmHeight + 2);
		rClient.top += yInc * (tm.tmHeight + 1); // Offset for where days start (note: +1, not +2 so we overlap)

		// Compute how big a day is.
		int width = (rClient.Width() - 1) / 7;
		rWeekDays.right = rClient.right = rClient.left + width * 7 + 1;
		int height = (rClient.Height() - 1) / WEEKS_PER_PAGE;
		rClient.bottom = rClient.top + height * WEEKS_PER_PAGE + 1;

		// Fill the weekday area.
		pDC->FillSolidRect(rWeekDays, GetSysColor(COLOR_BTNFACE));

		// Draw the days-of-week boxes and text (but not frame - we delay
		// drawing frame till end because when printing, the off-month fill
		// can bleed onto lines)
		rWeekDays.right = rWeekDays.left + width + INC_FULL_RECT;
		CRect rect(rWeekDays);
		rect.InflateRect(-1, -yInc); // Internal working area
		ARBDate weekStart = FirstDayOfWeek(m_First);
		for (int iDay = 0; iDay < 7; ++iDay)
		{
			t = CTime((weekStart + iDay).GetDate());
			str = t.Format(_T("%A"));
			pDC->DrawText(str, rect, DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER|DT_CENTER);

			pDC->SelectObject(&penShadow);
			pDC->MoveTo(rect.left, rect.bottom - yInc);
			pDC->LineTo(rect.right-1, rect.bottom - yInc);
			pDC->LineTo(rect.right-1, rect.top);
			pDC->SelectObject(&penHilite);
			pDC->MoveTo(rect.right-1, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			// Offset to next day.
			rect.OffsetRect(width, 0);
		}
		rect.OffsetRect(-width, 0); // Undo last shift
		rect.InflateRect(1, yInc); // Restore from internal area
		rect.left = rClient.left; // Full weekday area
		// Now draw the weekday frame, vertical bars will be done last
		pDC->SelectObject(&penFrame);
		pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(rect);

		// Draw the days
		ARBDate day = FirstDayOfWeek(curMonth);
		int iWeek;
		for (iWeek = 0; iWeek < WEEKS_PER_PAGE; ++iWeek)
		{
			rect.left = rClient.left;
			rect.right = rect.left + width + INC_FULL_RECT;
			rect.top = rClient.top;
			rect.bottom = rect.top + height + yInc * INC_FULL_RECT;
			rect.OffsetRect(0, iWeek * height);
			rect.InflateRect(-1, -yInc); // Internal working area
			rect.InflateRect(-DAY_TEXT_INSET, -yInc * DAY_TEXT_INSET);
			for (int iDay = 0; iDay < 7; ++iDay)
			{
				// Change background of non-current month days
				if (!pDC->IsPrinting() && m_Current == day)
				{
					CRect r(rect);
					r.InflateRect(DAY_TEXT_INSET, yInc * DAY_TEXT_INSET);
					if (bActive)
						pDC->FillSolidRect(r, GetSysColor(COLOR_HIGHLIGHT));
					else
						pDC->FillSolidRect(r, GetSysColor(COLOR_MENU));
				}
				else if (day.GetMonth() != curMonth.GetMonth())
				{
					CRect r(rect);
					r.InflateRect(DAY_TEXT_INSET, yInc * DAY_TEXT_INSET);
					pDC->FillSolidRect(r, GetSysColor(COLOR_APPWORKSPACE));
				}

				pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				if (!pDC->IsPrinting())
				{
					if (m_Current == day)
					{
						if (bActive)
							pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
						else
							pDC->SetTextColor(GetSysColor(COLOR_MENUTEXT));
					}
				}

				// Display date (only day now, not full date)
				//CString str(day.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eCalendar)).c_str());
				str.Format(_T("%d"), day.GetDay());
				pDC->DrawText(str, rect, DT_NOPREFIX|DT_SINGLELINE|DT_TOP|DT_RIGHT);

				// Display entries
				vector<ARBCalendarPtr> entries;
				if (0 < GetEntriesOn(day, entries, false))
				{
					CRect r(rect);
					for (vector<ARBCalendarPtr>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
					{
						r.top += yInc * tm.tmAscent;

						ARBCalendarPtr pCal = (*iter);
						str.Empty();
						for (size_t iCol = 0; iCol < m_Columns.size(); ++iCol)
						{
							if (0 < iCol)
								str += ' ';
							switch (m_Columns[iCol])
							{
							case IO_CAL_LOCATION:
								if (pCal->GetLocation().empty())
									str += '?';
								else
									str += pCal->GetLocation().c_str();
								break;
							case IO_CAL_CLUB:
								if (pCal->GetClub().empty())
									str += '?';
								else
									str += pCal->GetClub().c_str();
								break;
							case IO_CAL_VENUE:
								if (pCal->GetVenue().empty())
									str += '?';
								else
									str += pCal->GetVenue().c_str();
								break;
							case IO_CAL_NOTES:
								if (pCal->GetNote().empty())
									str += '?';
								else
									str += pCal->GetNote().c_str();
								break;
							}
						}
						bool bReset = false;
						COLORREF oldText = 0;
						// Don't change the color on the selected day.
						// (That whole foreground/background thing)
						if (m_Current != day)
						{
							if (CAgilityBookOptions::ViewAllCalendarOpening()
							&& pCal->GetOpeningDate() == day)
							{
								bReset = true;
								oldText = pDC->SetTextColor(clrOpening);
							}
							else if (CAgilityBookOptions::ViewAllCalendarClosing()
							&& pCal->GetClosingDate() == day)
							{
								bReset = true;
								oldText = pDC->SetTextColor(clrClosing);
							}
							else switch (pCal->GetEntered())
							{
							case ARBCalendar::eNot:
								oldText = pDC->SetTextColor(clrNotEntered);
								break;
							case ARBCalendar::ePlanning:
								oldText = pDC->SetTextColor(clrPlanning);
								break;
							case ARBCalendar::eEntered:
								oldText = pDC->SetTextColor(clrEntered);
								break;
							}
						}
						pDC->DrawText(str, r, DT_NOPREFIX|DT_SINGLELINE|DT_LEFT);
						if (bReset)
							pDC->SetTextColor(oldText);
					}
				}

				// Get ready for next round
				rect.OffsetRect(width, 0);
				++day;
			}
		}

		// Now draw all frames (do this last so the print preview looks ok)
		pDC->SelectObject(&penFrame);
		pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(rClient);
		// Vertical lines
		for (int iDay = 1; iDay < 7; ++iDay)
		{
			pDC->MoveTo(rClient.left + iDay * width, rWeekDays.top);
			pDC->LineTo(rClient.left + iDay * width, rClient.bottom);
		}
		// Horizontal lines
		for (iWeek = 1; iWeek < WEEKS_PER_PAGE; ++iWeek)
		{
			pDC->MoveTo(rClient.left, rClient.top + iWeek * height);
			pDC->LineTo(rClient.right, rClient.top + iWeek * height);
		}

		pDC->SelectObject(pOldFont);
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar printing

BOOL CAgilityBookViewCalendar::OnPreparePrinting(CPrintInfo* pInfo)
{
	// In order to enable selection, we must explicitly do it "&= ~..."
	// (default is NOSELECTION). But it doesn't seem to work - it always
	// prints all the pages. So, we'll just turn that off for now.
	pInfo->m_pPD->m_pd.Flags |= PD_NOSELECTION;
	pInfo->SetMaxPage(m_nMonths);
	return DoPreparePrinting(pInfo);
}

void CAgilityBookViewCalendar::OnBeginPrinting(
		CDC* pDC,
		CPrintInfo* pInfo)
{
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(m_nMonths);
}

void CAgilityBookViewCalendar::OnEndPrinting(
		CDC* /*pDC*/,
		CPrintInfo* /*pInfo*/)
{
	m_nCurOffset = -1;
}

void CAgilityBookViewCalendar::OnPrint(
		CDC* pDC,
		CPrintInfo* pInfo)
{
	m_nCurOffset = pInfo->m_nCurPage - 1;
	OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar diagnostics

#ifdef _DEBUG
void CAgilityBookViewCalendar::AssertValid() const
{
	CView::AssertValid();
}

void CAgilityBookViewCalendar::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewCalendar::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

// CAgilityBookViewCalendar message handlers

void CAgilityBookViewCalendar::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	InitMenuPopup(this, pPopupMenu, nIndex, bSysMenu);
}

void CAgilityBookViewCalendar::OnRButtonDown(
		UINT nFlags,
		CPoint point) 
{
	ARBDate date;
	GetDateFromPoint(point, date);
	SetCurrentDate(date, false);
	CView::OnRButtonDown(nFlags, point);
}

void CAgilityBookViewCalendar::OnContextMenu(
		CWnd* pWnd,
		CPoint point)
{
	if (!m_Current.IsValid())
		return;
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		CRect rect = GetDateRect(m_Current);
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

BOOL CAgilityBookViewCalendar::OnEraseBkgnd(CDC* pDC) 
{
	COLORREF crOld = pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	CRect r;
	GetClientRect(r);
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
	pDC->SetBkColor(crOld);
	return TRUE;
}

void CAgilityBookViewCalendar::OnLButtonDown(
		UINT nFlags,
		CPoint point) 
{
	ARBDate date;
	GetDateFromPoint(point, date);
	SetCurrentDate(date, false);
	CView::OnLButtonDown(nFlags, point);
}

void CAgilityBookViewCalendar::OnLButtonDblClk(
		UINT nFlags,
		CPoint point) 
{
	OnCalendarEdit();
	CView::OnLButtonDblClk(nFlags, point);
}

BOOL CAgilityBookViewCalendar::OnMouseWheel(
		UINT nFlags,
		short zDelta,
		CPoint pt) 
{
	int nPos = GetScrollPos(SB_VERT);
	if (0 > zDelta)
		--nPos;
	else
		++nPos;
	SetScrollPos(SB_VERT, nPos);
	Invalidate();
	return TRUE;
}

void CAgilityBookViewCalendar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_BOTTOM:
		SetScrollPos(SB_VERT, m_nMonths - 1);
		Invalidate();
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINEDOWN:
	case SB_PAGEDOWN:
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + 1);
		Invalidate();
		break;
	case SB_LINEUP:			// Scroll one line up.
	case SB_PAGEUP:			// Scroll one page up.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - 1);
		Invalidate();
		break;
	case SB_THUMBPOSITION:	// Scroll to the absolute position. The current position is provided in nPos.
	case SB_THUMBTRACK:		// Drag scroll box to specified position. The current position is provided in nPos.
		SetScrollPos(SB_VERT, nPos);
		Invalidate();
		break;
	case SB_TOP:			// Scroll to top.
		SetScrollPos(SB_VERT, 0);
		Invalidate();
		break;
	}
}

void CAgilityBookViewCalendar::OnKeyDown(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags) 
{
	bool bHandled = false;
	if (m_Current.IsValid())
	{
		ARBDate date(m_Current);
		switch (nChar)
		{
		default:
			break;
		case VK_SPACE:
		case VK_RETURN:
			bHandled = true;
			OnCalendarEdit();
			break;
		case VK_LEFT:
			bHandled = true;
			--date;
			break;
		case VK_RIGHT:
			bHandled = true;
			++date;
			break;
		case VK_UP:
			bHandled = true;
			date -= 7;
			break;
		case VK_DOWN:
			bHandled = true;
			date += 7;
			break;

		case VK_PRIOR:
			{
				bHandled = true;
				bool bScroll = false;
				int pos = GetScrollPos(SB_VERT);
				ARBDate curMonth = FirstDayOfMonth(pos);
				curMonth = LastDayOfWeek(curMonth);
				// There are 3 possible months: before, current, after:
				//  before: no date change, scroll
				//  current: date change, scroll
				//  after: date change, no scroll
				if (date.GetMonth() >= curMonth.GetMonth())
				{
					bool bPossibleScroll = (date.GetMonth() == curMonth.GetMonth());
					if (1 == date.GetMonth())
					{
						date.SetDate(date.GetYear() - 1, 12, date.GetDay());
					}
					else
					{
						// If we failed, it's probably because of end-of-month differences (28/30/31)
						// (Jan & Dec = 31, so no need to deal with above)
						if (!date.SetDate(date.GetYear(), date.GetMonth() - 1, date.GetDay(), false))
						{
							date.SetDate(date.GetYear(), date.GetMonth(), 1);
							--date;
						}
					}
					if (SetCurrentDate(date, true))
						bScroll = bPossibleScroll;
				}
				else
					bScroll = true;
				if (bScroll && pos == GetScrollPos(SB_VERT))
				{
					SetScrollPos(SB_VERT, pos - 1);
					Invalidate();
					date = m_Current; // To prevent multiple setting
				}
			}
			break;
		case VK_NEXT:
			{
				bHandled = true;
				bool bScroll = false;
				int pos = GetScrollPos(SB_VERT);
				ARBDate curMonth = FirstDayOfMonth(pos);
				curMonth = LastDayOfWeek(curMonth);
				// There are 3 possible months: before, current, after:
				//  before: date change, no scroll
				//  current: date change, scroll
				//  after: no date change, scroll
				if (date.GetMonth() <= curMonth.GetMonth())
				{
					bool bPossibleScroll = (date.GetMonth() == curMonth.GetMonth());
					if (12 == date.GetMonth())
					{
						date.SetDate(date.GetYear() + 1, 1, date.GetDay());
					}
					else
					{
						// If we failed, it's probably because of end-of-month differences (28/30/31)
						// (Jan & Dec = 31, so no need to deal with above)
						if (!date.SetDate(date.GetYear(), date.GetMonth() + 1, date.GetDay(), false))
						{
							date.SetDate(date.GetYear(), date.GetMonth() + 2, 1);
							--date;
						}
					}
					if (SetCurrentDate(date, true))
						bScroll = bPossibleScroll;
				}
				else
					bScroll = true;
				if (bScroll && pos == GetScrollPos(SB_VERT))
				{
					SetScrollPos(SB_VERT, pos + 1);
					Invalidate();
					date = m_Current; // To prevent multiple setting
				}
			}
			break;

		case VK_HOME:
			bHandled = true;
			if (0 > GetKeyState(VK_CONTROL))
				date = m_First;
			else
				date = FirstDayOfWeek(date);
			break;
		case VK_END:
			bHandled = true;
			if (0 > GetKeyState(VK_CONTROL))
				date = m_Last;
			else
				date = LastDayOfWeek(date);
		}
		if (date != m_Current)
		{
			if (!SetCurrentDate(date, true))
				MessageBeep(MB_ICONEXCLAMATION);
		}
	}
	if (!bHandled)
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAgilityBookViewCalendar::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < m_Calendar.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnEditCopy()
{
	CClipboardDataWriter clpData;
	if (!clpData.isOkay())
		return;

	static UINT const scColumns[] =
	{
		IDS_COL_START_DATE,
		IDS_COL_END_DATE,
		IDS_COL_VENUE,
		IDS_COL_LOCATION,
		IDS_COL_CLUB,
		IDS_COL_OPENS,
		IDS_COL_CLOSES,
		IDS_COL_NOTES,
	};
	static int const scNumColumns = sizeof(scColumns) / sizeof(scColumns[0]);
#define COL_START_DATE	0
#define COL_END_DATE	1
#define COL_VENUE		2
#define COL_LOCATION	3
#define COL_CLUB		4
#define COL_OPENS		5
#define COL_CLOSES		6
#define COL_NOTES		7

	int index;
	size_t maxLen[scNumColumns];
	CString columns[scNumColumns];
	for (index = 0; index < scNumColumns; ++index)
	{
		CString str;
		str.LoadString(scColumns[index]);
		maxLen[index] = str.GetLength();
		columns[index] = str;
	}
	vector<ARBCalendarPtr>::const_iterator iter;
	ARBDate::DateFormat dFmt = CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eCalList);
	for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		ARBCalendarPtr cal = *iter;
		size_t len = cal->GetStartDate().GetString(dFmt).length();
		if (len > maxLen[COL_START_DATE])
			maxLen[COL_START_DATE] = len;
		len = cal->GetEndDate().GetString(dFmt).length();
		if (len > maxLen[COL_END_DATE])
			maxLen[COL_END_DATE] = len;
		len = cal->GetLocation().length();
		if (len > maxLen[COL_LOCATION])
			maxLen[COL_LOCATION] = len;
		len = cal->GetClub().length();
		if (len > maxLen[COL_CLUB])
			maxLen[COL_CLUB] = len;
		len = cal->GetVenue().length();
		if (len > maxLen[COL_VENUE])
			maxLen[COL_VENUE] = len;
		len = cal->GetOpeningDate().GetString(dFmt).length();
		if (len > maxLen[COL_OPENS])
			maxLen[COL_OPENS] = len;
		len = cal->GetClosingDate().GetString(dFmt).length();
		if (len > maxLen[COL_CLOSES])
			maxLen[COL_CLOSES] = len;
		len = cal->GetNote().length();
		if (len > maxLen[COL_NOTES])
			maxLen[COL_NOTES] = len;
	}
	// The header
	CString data;
	data.Format(_T(" %*s - %-*s %-*s %-*s %-*s %*s - %-*s %-*s"),
		maxLen[COL_START_DATE], (LPCTSTR)columns[COL_START_DATE],
		maxLen[COL_END_DATE], (LPCTSTR)columns[COL_END_DATE],
		maxLen[COL_VENUE], (LPCTSTR)columns[COL_VENUE],
		maxLen[COL_LOCATION], (LPCTSTR)columns[COL_LOCATION],
		maxLen[COL_CLUB], (LPCTSTR)columns[COL_CLUB],
		maxLen[COL_OPENS], (LPCTSTR)columns[COL_OPENS],
		maxLen[COL_CLOSES], (LPCTSTR)columns[COL_CLOSES],
		maxLen[COL_NOTES], (LPCTSTR)columns[COL_NOTES]);
	data.TrimRight();
	data += _T("\r\n");

	// The data
	for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		ARBCalendarPtr cal = *iter;
		ARBString items[scNumColumns];
		items[COL_START_DATE] = cal->GetStartDate().GetString(dFmt);
		items[COL_END_DATE] = cal->GetEndDate().GetString(dFmt);
		items[COL_LOCATION] = cal->GetLocation();
		items[COL_CLUB] = cal->GetClub();
		items[COL_VENUE] = cal->GetVenue();
		items[COL_OPENS] = cal->GetOpeningDate().GetString(dFmt);
		items[COL_CLOSES] = cal->GetClosingDate().GetString(dFmt);
		CString tmp = cal->GetNote().c_str();
		tmp.Replace(_T("\n"), _T(" "));
		items[COL_NOTES] = (LPCTSTR)tmp;
		CString tentative(_T(" "));
		if (cal->IsTentative())
			tentative = _T("?");
		CString str;
		str.Format(_T("%s%*s - %-*s %-*s %-*s %-*s %*s%s%-*s %-*s"),
			(LPCTSTR)tentative,
			maxLen[COL_START_DATE], items[COL_START_DATE].c_str(),
			maxLen[COL_END_DATE], items[COL_END_DATE].c_str(),
			maxLen[COL_VENUE], items[COL_VENUE].c_str(),
			maxLen[COL_LOCATION], items[COL_LOCATION].c_str(),
			maxLen[COL_CLUB], items[COL_CLUB].c_str(),
			maxLen[COL_OPENS], items[COL_OPENS].c_str(),
			(0 < items[COL_OPENS].length() || 0 < items[COL_CLOSES].length()) ? _T(" - ") : _T("   "),
			maxLen[COL_CLOSES], items[COL_CLOSES].c_str(),
			maxLen[COL_NOTES], items[COL_NOTES].c_str());
		str.TrimRight();
		data += str + _T("\r\n");
	}
	clpData.SetData(data);
}

void CAgilityBookViewCalendar::OnUpdateCalendarEdit(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (m_Current.IsValid())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnCalendarEdit()
{
	if (m_Current.IsValid())
	{
		CWnd* pParent = GetParent()->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTabView)));
		CTabView* pView = DYNAMIC_DOWNCAST(CTabView, pParent);
		vector<ARBCalendarPtr> entries;
		GetEntriesOn(m_Current, entries, true);
		CDlgListCtrl dlg(GetDocument(), m_Current, &entries, pView, this);
		if (IDOK == dlg.DoModal())
		{
			LoadData();
			GetDocument()->SetModifiedFlag();
			// DlgListCtrl calls UpdateAllViews.
		}
	}
}

void CAgilityBookViewCalendar::OnCalendarNew()
{
	ARBCalendarPtr cal(ARBCalendar::New());
	if (m_Current.IsValid())
	{
		cal->SetStartDate(m_Current);
		cal->SetEndDate(m_Current + 1);
	}
	CDlgCalendar dlg(cal, GetDocument());
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			SetCurrentDate(cal->GetStartDate(), false);
			GetDocument()->GetCalendar().AddCalendar(cal);
			GetDocument()->GetCalendar().sort();
			LoadData();
			GetDocument()->SetModifiedFlag();
			GetDocument()->UpdateAllViews(this, UPDATE_CALENDAR_VIEW);
		}
	}
}

void CAgilityBookViewCalendar::OnUpdateCalendarDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, true);
	if (0 < entries.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnAgilityCalendarDelete()
{
	vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, true);
	if (0 < entries.size())
		OnCalendarEdit();
}

void CAgilityBookViewCalendar::OnUpdateAgilityCreateentryCalendar(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, false);
	if (1 == entries.size())
	{
		// Make sure the venue is recognized.
		ARBCalendarPtr pCal = *(entries.begin());
		if (GetDocument()->GetConfig().GetVenues().FindVenue(pCal->GetVenue()))
			bEnable = TRUE;
	}
	else if (1 < entries.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnAgilityCreateentryCalendar() 
{
	vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, false);
	if (1 == entries.size())
	{
		ARBCalendarPtr pCal = *(entries.begin());
		CWnd* pParent = GetParent()->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTabView)));
		CTabView* pView = DYNAMIC_DOWNCAST(CTabView, pParent);
		if (GetDocument()->CreateTrialFromCalendar(*pCal, pView))
			GetDocument()->SetModifiedFlag();
	}
	else if (1 < entries.size())
		OnCalendarEdit();
}

void CAgilityBookViewCalendar::OnUpdateNotHandled(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CAgilityBookViewCalendar::OnViewCustomize()
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eViewCal);
	if (IDOK == dlg.DoModal())
	{
		CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewCal, IO_TYPE_VIEW_CALENDAR, m_Columns);
		Invalidate();
	}
}
