/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 */

#include "stdafx.h"
#include <set>
#include "AgilityBook.h"
#include "AgilityBookViewCalendar.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBCalendar.h"
#include "DlgCalendar.h"
#include "DlgListCtrl.h"
#include "MainFrm.h"
#include "TabView.h"

using namespace std;

#define DAY_BORDER		1
#define DAY_TEXT_INSET	2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar

IMPLEMENT_DYNCREATE(CAgilityBookViewCalendar, CScrollView)

BEGIN_MESSAGE_MAP(CAgilityBookViewCalendar, CScrollView)
	//{{AFX_MSG_MAP(CAgilityBookViewCalendar)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_CALENDAR, OnUpdateCalendarEdit)
	ON_COMMAND(ID_AGILITY_EDIT_CALENDAR, OnCalendarEdit)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnCalendarNew)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_CALENDAR, OnUpdateCalendarDelete)
	ON_COMMAND(ID_AGILITY_DELETE_CALENDAR, OnAgilityCalendarDelete)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_CREATEENTRY_CALENDAR, OnUpdateAgilityCreateentryCalendar)
	ON_COMMAND(ID_AGILITY_CREATEENTRY_CALENDAR, OnAgilityCreateentryCalendar)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CAgilityBookViewCalendar construction/destruction

CAgilityBookViewCalendar::CAgilityBookViewCalendar()
	: m_nWeeks(0)
{
}

CAgilityBookViewCalendar::~CAgilityBookViewCalendar()
{
}

BOOL CAgilityBookViewCalendar::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewCalendar::SetCurrentDate(const ARBDate& date, bool bEnsureVisible)
{
	bool bSet = false;
	if (date != m_Current && date >= m_First && date <= m_Last)
	{
		bSet = true;
		if (m_Current.IsValid())
		{
			CRect r = GetDateRect(m_Current, false);
			r.InflateRect(2, 2);
			InvalidateRect(r);
		}
		m_Current = date;
		if (m_Current.IsValid())
		{
			CRect r = GetDateRect(m_Current, false);
			CRect rCurrent = GetDateRect(m_Current, true);
			r.InflateRect(2, 2);
			InvalidateRect(r);
			// Take it back before checking if date is visible.
			r.InflateRect(-2, -2);
			if (bEnsureVisible)
			{
				CRect rClient;
				GetClientRect(rClient);
				CRect r2;
				if (!r2.IntersectRect(rClient, r))
				{
					CPoint pt = GetScrollPosition();
					pt.y = rCurrent.top;
					ScrollToPosition(pt);
				}
			}
		}
	}
	return bSet;
}

bool CAgilityBookViewCalendar::GetMessage(CString& msg) const
{
	msg.FormatMessage(IDS_NUM_EVENTS, m_Calendar.size());
	return true;
}

size_t CAgilityBookViewCalendar::GetEntriesOn(const ARBDate& date, std::vector<ARBCalendar*>& entries) const
{
	entries.clear();
	for (vector<ARBCalendar*>::const_iterator iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		if ((*iter)->InRange(date))
			entries.push_back((*iter));
	}
	return entries.size();
}

void CAgilityBookViewCalendar::LoadData()
{
	// Clear everything.
	m_Calendar.clear();
	m_Last = ARBDate::Today();
	m_First = ARBDate::Today();
	m_nWeeks = 0;
	m_szEntry = CAgilityBookOptions::GetCalendarEntrySize();

	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	bool bViewAll = CAgilityBookOptions::ViewAllCalendarEntries();
	bool bHide = CAgilityBookOptions::HideOverlappingCalendarEntries();

	// Add items.
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
		if (!m_First.IsValid() || pCal->GetStartDate() < m_First)
			m_First = pCal->GetStartDate();
		if (!m_Last.IsValid() || pCal->GetEndDate() > m_Last)
			m_Last = pCal->GetEndDate();
		m_Calendar.push_back(pCal);
	}
	if (!m_Current.IsValid())
		m_Current = ARBDate::Today();
	m_First -= m_First.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek()); // Set the first day to the start of the week.
	m_Last += 6 - m_Last.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	m_nWeeks = ((m_Last - m_First) / 7) + 1;
	SetScrollSizes(MM_LOENGLISH,
		CSize(8 * DAY_BORDER + 7 * m_szEntry.cx,
			(m_nWeeks + 1) * DAY_BORDER + m_nWeeks * m_szEntry.cy));

	CString msg;
	if (GetMessage(msg) && IsWindowVisible())
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg);
	Invalidate();
}

/**
 * Returns the working rect for a date. Borders are handled entirely separately.
 */
CRect CAgilityBookViewCalendar::GetDateRect(const ARBDate& date, bool bLogical) const
{
	CRect r(0, 0, m_szEntry.cx, -m_szEntry.cy);
	int nWeek = (date - m_First) / 7;
	int nday = date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	r.OffsetRect(
		(nday + 1) * DAY_BORDER + nday * m_szEntry.cx,
		-((nWeek + 1) * DAY_BORDER + nWeek * m_szEntry.cy));
	if (!bLogical)
	{
		CClientDC dc(const_cast<CAgilityBookViewCalendar*>(this));
		dc.SetMapMode(MM_LOENGLISH);
		r.OffsetRect(-GetScrollPosition());
		dc.LPtoDP(r);
	}
	return r;
}

void CAgilityBookViewCalendar::GetDateFromPoint(CPoint pt, ARBDate& date)
{
	date.clear();
	CSize sz = GetTotalSize();
	if (pt.x >= sz.cx)
		return;
	if (pt.y >= sz.cy)
		return;
	int x = abs(pt.x / (DAY_BORDER + m_szEntry.cx));
	int y = abs(pt.y / (DAY_BORDER + m_szEntry.cy));
	date = m_First;
	date += x + y * 7;
}

/////////////////////////////////////////////////////////////////////////////

void CAgilityBookViewCalendar::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg);
	if (m_Current.IsValid())
	{
		CRect r = GetDateRect(m_Current, false);
		InvalidateRect(r);
	}
}

void CAgilityBookViewCalendar::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (0 == lHint || (UPDATE_CALENDAR_VIEW & lHint) || (UPDATE_OPTIONS & lHint))
		LoadData();
	else if (UPDATE_CALENDAR_SEL & lHint)
	{
		ARBCalendar* pCal = reinterpret_cast<ARBCalendar*>(pHint);
		if (pCal && pCal->GetStartDate().IsValid())
			SetCurrentDate(pCal->GetStartDate(), true);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar drawing

void CAgilityBookViewCalendar::OnDraw(CDC* pDC) 
{
	if (m_First.IsValid() && m_Last.IsValid())
	{
		if (!pDC->IsPrinting())
		{
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
			pDC->SetBkMode(TRANSPARENT);
		}
		bool bActive = (((CMainFrame*)AfxGetMainWnd())->GetActiveView() == this);
		CBrush brCurrentActive(GetSysColor(COLOR_HIGHLIGHT));
		CBrush brCurrentInActive(GetSysColor(COLOR_MENU));
		CFont fontDate, fontText;
		CFontInfo fontInfo;
		CAgilityBookOptions::GetCalendarDateFontInfo(fontInfo, pDC->IsPrinting());
		fontInfo.CreateFont(fontDate, NULL);
		CAgilityBookOptions::GetCalendarTextFontInfo(fontInfo, pDC->IsPrinting());
		fontInfo.CreateFont(fontText, NULL);
		CPen pen;
		pen.CreatePen(PS_SOLID, DAY_BORDER, GetSysColor(COLOR_WINDOWTEXT));
		CFont* pOldFont = pDC->SelectObject(&fontDate);
		CPen* pOldPen = pDC->SelectObject(&pen);

		// Draw the calendar framework.
		int i;
		ARBDate date = m_First;
		for (i = 0; i < m_nWeeks; ++i)
		{
			int y = -i * (DAY_BORDER + m_szEntry.cy);
			pDC->MoveTo(0, y);
			pDC->LineTo(8 * DAY_BORDER + 7 * m_szEntry.cx, y);
			for (int d = 0; d < 7; ++d, ++date)
			{
				CRect r = GetDateRect(date, true);
				CRect rFull(r);
				r.InflateRect(-DAY_TEXT_INSET, DAY_TEXT_INSET);
				CString str(date.GetString(false, false).c_str());
				if (!pDC->IsPrinting())
				{
					if (m_Current == date)
					{
						// This is where we 'paint' the background.
						CBrush* oldBrush = NULL;
						if (bActive)
						{
							oldBrush = pDC->SelectObject(&brCurrentActive);
							pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
						}
						else
						{
							oldBrush = pDC->SelectObject(&brCurrentInActive);
							pDC->SetTextColor(GetSysColor(COLOR_MENUTEXT));
						}
						pDC->PatBlt(rFull.left, rFull.top, rFull.Width(), rFull.Height(), PATCOPY);
						pDC->SelectObject(oldBrush);
					}
					else
						pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				}
				pDC->DrawText(str, r, DT_NOPREFIX|DT_SINGLELINE|DT_BOTTOM|DT_CENTER);
			}
		}
		// Finish off the last line.
		int y = -i * (m_szEntry.cy + DAY_BORDER);
		pDC->MoveTo(0, y);
		pDC->LineTo(8 * DAY_BORDER + 7 * m_szEntry.cx, y);
		// Now the day dividers.
		for (i = 0; i < 8; ++i)
		{
			int x = i * (DAY_BORDER + m_szEntry.cx);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, y);
		}

		// Add the calendar entries.
		// First, get all the dates with an event.
		set<ARBDate> dates;
		vector<ARBCalendar*>::const_iterator iter;
		for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
		{
			const ARBCalendar* pCal = (*iter);
			for (ARBDate date = pCal->GetStartDate(); date <= pCal->GetEndDate(); ++date)
			{
				dates.insert(date);
			}
		}
		pDC->SelectObject(&fontText);
		for (set<ARBDate>::iterator iterDate = dates.begin(); iterDate != dates.end(); ++iterDate)
		{
			ARBDate date = (*iterDate);
			// Should be...
			vector<ARBCalendar*> entries;
			// Then for each date, get all the events on that date to print.
			if (0 < GetEntriesOn(date, entries))
			{
				CRect r = GetDateRect(date, true);
				r.InflateRect(-DAY_TEXT_INSET, DAY_TEXT_INSET);
				CString str;
				for (iter = entries.begin(); iter != entries.end(); ++iter)
				{
					const ARBCalendar* pCal = (*iter);
					if (!str.IsEmpty())
						str += "\r\n";
					// @todo: Make data in calendar entries user selectable
					const string& venue = pCal->GetVenue();
					const string& loc = pCal->GetLocation();
					if (ARBCalendar::eEntered == pCal->GetEntered())
						str += "*";
					else if (ARBCalendar::ePlanning == pCal->GetEntered())
						str += "@";
					if (0 == venue.length())
						str += "?";
					else
						str += venue.c_str();
					str += " ";
					if (0 == loc.length())
						str += "?";
					else
						str += loc.c_str();
				}
				if (!pDC->IsPrinting())
				{
					if (m_Current == date)
					{
						if (bActive)
							pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
						else
							pDC->SetTextColor(GetSysColor(COLOR_MENUTEXT));
					}
					else
						pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				}
				pDC->DrawText(str, r, DT_NOPREFIX);
			}
		}
		pDC->SelectObject(pOldFont);
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar printing

BOOL CAgilityBookViewCalendar::OnPreparePrinting(CPrintInfo* pInfo)
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

struct CPrintData
{
	int nEntriesPerPage;
	CSize sz;
};

void CAgilityBookViewCalendar::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	pDC->SetMapMode(MM_LOENGLISH);
	CSize szDevice(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	CRect r(CPoint(0,0), szDevice);
	pDC->DPtoLP(r);

	CPrintData* pData = new CPrintData;
	pInfo->m_lpUserData = reinterpret_cast<void*>(pData);
	pData->nEntriesPerPage = abs(r.Height()) / (2 * DAY_BORDER + m_szEntry.cy);
	pData->sz = CSize(r.Width(), abs(r.Height()));

	int nMax = m_nWeeks / pData->nEntriesPerPage;
	if (nMax * pData->nEntriesPerPage != m_nWeeks)
		++nMax;
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(nMax);
}

void CAgilityBookViewCalendar::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	CPrintData* pData = reinterpret_cast<CPrintData*>(pInfo->m_lpUserData);
	delete pData;
}

void CAgilityBookViewCalendar::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CPrintData* pData = reinterpret_cast<CPrintData*>(pInfo->m_lpUserData);
	int nMarginX = (pData->sz.cx - (8 * DAY_BORDER + 7 * m_szEntry.cx)) / 2;
	if (0 > nMarginX)
		nMarginX = 0;
	int nMarginY = (pData->sz.cy - ((pData->nEntriesPerPage + 1) * DAY_BORDER + pData->nEntriesPerPage * m_szEntry.cy)) / 2;
	if (0 > nMarginY)
		nMarginY = 0;
	int top = (pInfo->m_nCurPage - 1) * (pData->nEntriesPerPage * (DAY_BORDER + m_szEntry.cy));
	CRect r(0, -top,
		8 * DAY_BORDER + 7 * m_szEntry.cx,
		-(top + (pData->nEntriesPerPage + 1) * DAY_BORDER + pData->nEntriesPerPage * m_szEntry.cy));
	pDC->SetWindowOrg(-nMarginX + r.left, nMarginY + r.top);
	pDC->IntersectClipRect(r);
	OnDraw(pDC);
	pDC->SelectClipRgn(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewCalendar diagnostics

#ifdef _DEBUG
void CAgilityBookViewCalendar::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAgilityBookViewCalendar::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewCalendar::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

// CAgilityBookViewCalendar message handlers

void CAgilityBookViewCalendar::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewCalendar::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CScrollView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
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

void CAgilityBookViewCalendar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (!m_Current.IsValid())
		return;
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		CRect rect = GetDateRect(m_Current, false);
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

BOOL CAgilityBookViewCalendar::OnEraseBkgnd(CDC* pDC) 
{
	COLORREF crOld = pDC->SetBkColor(GetSysColor(COLOR_APPWORKSPACE));
	CRect r;
	GetClientRect(r);
	int nOldMode = pDC->SetMapMode(MM_LOENGLISH);
	pDC->DPtoLP(r);
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	r = CRect(CPoint(0,0), GetTotalSize());
	r.bottom = -r.bottom;
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
	pDC->SetBkColor(crOld);
	pDC->SetMapMode(nOldMode);
	return TRUE;
}

void CAgilityBookViewCalendar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	dc.SetMapMode(MM_LOENGLISH);
	CPoint pt(point);
	dc.DPtoLP(&pt);
	pt += GetScrollPosition();
	ARBDate date;
	GetDateFromPoint(pt, date);
	SetCurrentDate(date, false);
	CScrollView::OnLButtonDown(nFlags, point);
}

void CAgilityBookViewCalendar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnCalendarEdit();
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CAgilityBookViewCalendar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
			OnCalendarEdit();
			break;
		case VK_LEFT:
			--date;
			break;
		case VK_RIGHT:
			++date;
			break;
		case VK_UP:
			date -= 7;
			break;
		case VK_DOWN:
			date += 7;
			break;

		case VK_PRIOR:
		case VK_NEXT:
			{
				CClientDC dc(this);
				dc.SetMapMode(MM_LOENGLISH);
				CRect rClient;
				GetClientRect(rClient);
				dc.DPtoLP(rClient);
				int nEntriesPerPage = abs(rClient.Height()) / (2 * DAY_BORDER + m_szEntry.cy);
				if (VK_PRIOR == nChar)
					date -= (nEntriesPerPage + 1) * 7;
				else
					date += (nEntriesPerPage + 1) * 7;
			}
			break;

		case VK_HOME:
			bHandled = true;
			if (0 > GetKeyState(VK_CONTROL))
				date = m_First;
			else
				date -= date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
			break;
		case VK_END:
			bHandled = true;
			if (0 > GetKeyState(VK_CONTROL))
				date = m_Last;
			else
				date += 6 - date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
		}
		if (date != m_Current)
			SetCurrentDate(date, true);
	}
	if (!bHandled)
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
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
		vector<ARBCalendar*> entries;
		GetEntriesOn(m_Current, entries);
		CDlgListCtrl dlg(GetDocument(), m_Current, &entries, pView, this);
		if (IDOK == dlg.DoModal())
		{
			LoadData();
			GetDocument()->SetModifiedFlag();
		}
	}
}

void CAgilityBookViewCalendar::OnCalendarNew()
{
	ARBCalendar* cal = new ARBCalendar;
	if (m_Current.IsValid())
	{
		cal->SetStartDate(m_Current);
		cal->SetEndDate(m_Current);
	}
	CDlgCalendar dlg(cal, GetDocument());
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			GetDocument()->GetCalendar().AddCalendar(cal);
			GetDocument()->GetCalendar().sort();
			LoadData();
			GetDocument()->SetModifiedFlag();
		}
	}
	cal->Release();
}

void CAgilityBookViewCalendar::OnUpdateCalendarDelete(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	vector<ARBCalendar*> entries;
	GetEntriesOn(m_Current, entries);
	if (0 < entries.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnAgilityCalendarDelete()
{
	vector<ARBCalendar*> entries;
	GetEntriesOn(m_Current, entries);
	if (0 < entries.size())
		OnCalendarEdit();
}

void CAgilityBookViewCalendar::OnUpdateAgilityCreateentryCalendar(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	vector<ARBCalendar*> entries;
	GetEntriesOn(m_Current, entries);
	if (1 == entries.size())
	{
		// Make sure the venue is recognized.
		ARBCalendar* pCal = *(entries.begin());
		if (GetDocument()->GetConfig().GetVenues().FindVenue(pCal->GetVenue()))
			bEnable = TRUE;
	}
	else if (1 < entries.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewCalendar::OnAgilityCreateentryCalendar() 
{
	vector<ARBCalendar*> entries;
	GetEntriesOn(m_Current, entries);
	if (1 == entries.size())
	{
		ARBCalendar* pCal = *(entries.begin());
		CWnd* pParent = GetParent()->GetParent();
		ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTabView)));
		CTabView* pView = DYNAMIC_DOWNCAST(CTabView, pParent);
		if (GetDocument()->CreateTrialFromCalendar(*pCal, pView))
			GetDocument()->SetModifiedFlag();
	}
	else if (1 < entries.size())
		OnCalendarEdit();
}
