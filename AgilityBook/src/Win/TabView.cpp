/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-24 DRC Removed sending initialupdate messages (redundant)
 * @li 2003-12-07 DRC Fixed a crash when opening a bad ARB file version.
 * @li 2003-09-21 DRC Added training log.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "TabView.h"

#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewHtml.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"
#include "AgilityBookViewTraining.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Default splitter widths
#define DEFAULT_RUN_WIDTH	200
#define DEFAULT_CAL_WIDTH	200

/////////////////////////////////////////////////////////////////////////////
// CTabView

IMPLEMENT_DYNCREATE(CTabView, CCtrlView)

BEGIN_MESSAGE_MAP(CTabView, CCtrlView)
	//{{AFX_MSG_MAP(CTabView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelChanging)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabView::CTabView()
	: CCtrlView(_T("SysTabControl32"), AFX_WS_DEFAULT_VIEW)
	, m_pLastFocusRuns(NULL)
	, m_pLastFocusCal(NULL)
{
}

CTabView::~CTabView()
{
}

void CTabView::OnDestroy()
{
	m_Panes.clear();

	// If doc fails to open, no window was ever created.
	if (IsWindow(GetSafeHwnd()) && IsWindow(m_splitterRuns.GetSafeHwnd()) && IsWindow(m_splitterCal.GetSafeHwnd()))
	{
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("View"), GetTabCtrl().GetCurSel());
		int cxCur, cxMin;
		m_splitterRuns.GetColumnInfo(0, cxCur, cxMin);
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("splitCX"), cxCur);
		m_splitterCal.GetColumnInfo(0, cxCur, cxMin);
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("splitCX2"), cxCur);
	}

	CCtrlView::OnDestroy();
}

BOOL CTabView::PreCreateWindow(CREATESTRUCT& cs)
{
	// FYI: FOCUSNEVER doesn't seem to work properly...
	cs.style |= TCS_TABS | TCS_FOCUSNEVER;
	return CCtrlView::PreCreateWindow(cs);
}

void CTabView::OnInitialUpdate()
{
	CCtrlView::OnInitialUpdate();

	if (0 != GetTabCtrl().GetItemCount())
		return;

	reinterpret_cast<CMainFrame*>(GetParentFrame())->m_pView = this;

	CString str;
	str.LoadString(IDS_RUNS);
	GetTabCtrl().InsertItem(0, str);
	str.LoadString(IDS_POINTS);
	GetTabCtrl().InsertItem(1, str);
	str.LoadString(IDS_CALENDAR);
	GetTabCtrl().InsertItem(2, str);
	str.LoadString(IDS_TRAINING);
	GetTabCtrl().InsertItem(3, str);

	CCreateContext context;
	context.m_pCurrentDoc = GetDocument();
	context.m_pNewDocTemplate = NULL;
	context.m_pLastView = NULL;
	context.m_pCurrentFrame = NULL;

	if (!m_splitterRuns.CreateStatic(this, 1, 2))
		return;
	m_splitterRuns.ShowWindow(SW_HIDE);
	m_splitterRuns.EnableWindow(FALSE);
	int cx = AfxGetApp()->GetProfileInt(_T("Settings"), _T("splitCX"), DEFAULT_RUN_WIDTH);
	context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookTree);
	if (!m_splitterRuns.CreateView(0, 0, RUNTIME_CLASS(CAgilityBookTree), CSize(cx, 100), &context))
		return;
	context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewRuns);
	if (!m_splitterRuns.CreateView(0, 1, RUNTIME_CLASS(CAgilityBookViewRuns), CSize(200, 100), &context))
		return;
	m_Panes.push_back(&m_splitterRuns);

	bool bCreateList = !CAgilityBookOptions::ShowHtmlPoints();
	if (!bCreateList)
	{
		CAgilityBookViewHtml* html = reinterpret_cast<CAgilityBookViewHtml*>(RUNTIME_CLASS(CAgilityBookViewHtml)->CreateObject());
		if (!html)
			bCreateList = true;
		else
		{
			m_Panes.push_back(html);
			context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewHtml);
			DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER & ~WS_VISIBLE;
			html->Create(NULL, NULL,
				dwStyle,
				CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST+1, &context);
		}
	}
	if (bCreateList)
	{
		CAgilityBookViewPoints* points = reinterpret_cast<CAgilityBookViewPoints*>(RUNTIME_CLASS(CAgilityBookViewPoints)->CreateObject());
		m_Panes.push_back(points);
		context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewPoints);
		DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER & ~WS_VISIBLE;
		points->CreateEx(WS_EX_CLIENTEDGE, NULL, NULL,
			dwStyle | LVS_REPORT | LVS_SHOWSELALWAYS,
			CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST+1, &context);
	}

	if (!m_splitterCal.CreateStatic(this, 1, 2))
		return;
	m_splitterCal.ShowWindow(SW_HIDE);
	m_splitterCal.EnableWindow(FALSE);
	cx = AfxGetApp()->GetProfileInt(_T("Settings"), _T("splitCX2"), DEFAULT_CAL_WIDTH);
	context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewCalendarList);
	if (!m_splitterCal.CreateView(0, 0, RUNTIME_CLASS(CAgilityBookViewCalendarList), CSize(cx, 100), &context))
		return;
	context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewCalendar);
	if (!m_splitterCal.CreateView(0, 1, RUNTIME_CLASS(CAgilityBookViewCalendar), CSize(200, 100), &context))
		return;
	m_Panes.push_back(&m_splitterCal);

	CAgilityBookViewTraining* training = reinterpret_cast<CAgilityBookViewTraining*>(RUNTIME_CLASS(CAgilityBookViewTraining)->CreateObject());
	m_Panes.push_back(training);
	context.m_pNewViewClass = RUNTIME_CLASS(CAgilityBookViewTraining);
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER & ~WS_VISIBLE;
	training->CreateEx(WS_EX_CLIENTEDGE, NULL, NULL,
		dwStyle | LVS_REPORT | LVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST+1, &context);

	int nSel = AfxGetApp()->GetProfileInt(_T("Settings"), _T("View"), 0);
	// The usage of 'View' has changed. It was cal=1,pts=2,runs=3. (v0.2.0.3)
	// It is now the index of the tab.
	if (0 > nSel || nSel > 3)
		nSel = 0;
	SetCurSel(nSel);
}

// CTabView drawing

void CTabView::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
}

// CTabView diagnostics
#ifdef _DEBUG
void CTabView::AssertValid() const
{
	CCtrlView::AssertValid();
}

void CTabView::Dump(CDumpContext& dc) const
{
	CCtrlView::Dump(dc);
}

CAgilityBookDoc* CTabView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

void CTabView::SetCurSel(int index)
{
	NMHDR hdr;
	hdr.code = TCN_SELCHANGING;
	hdr.hwndFrom = GetTabCtrl().GetSafeHwnd();
	hdr.idFrom = GetTabCtrl().GetDlgCtrlID();
	SendMessage(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&hdr));
	GetTabCtrl().SetCurSel(index);
	hdr.code = TCN_SELCHANGE;
	SendMessage(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&hdr));
}

void CTabView::SetActiveView()
{
	CView* pView = NULL;
	ICommonView* pCommon = NULL;
	int nIndex = GetTabCtrl().GetCurSel();
	switch (nIndex)
	{
	case 0:
		// We may need to kick start the view the 1st time.
		if (!m_pLastFocusRuns)
			m_pLastFocusRuns = reinterpret_cast<CView*>(m_splitterRuns.GetPane(0,0));
		pView = m_pLastFocusRuns;
		pCommon = dynamic_cast<ICommonView*>(reinterpret_cast<CView*>(m_splitterRuns.GetPane(0,1)));
		break;
	case 1:
		pView = reinterpret_cast<CView*>(m_Panes[nIndex]);
		pCommon = dynamic_cast<ICommonView*>(pView);
		break;
	case 2:
		if (!m_pLastFocusCal)
			m_pLastFocusCal = reinterpret_cast<CView*>(m_splitterCal.GetPane(0,0));
		pView = m_pLastFocusCal;
		pCommon = dynamic_cast<ICommonView*>(reinterpret_cast<CView*>(m_splitterCal.GetPane(0,0)));
		break;
	case 3:
		pView = reinterpret_cast<CView*>(m_Panes[nIndex]);
		pCommon = dynamic_cast<ICommonView*>(pView);
		break;
	}
	if (pView)
	{
		pView->SetFocus();
		GetParentFrame()->SetActiveView(pView);
	}
	if (pCommon)
	{
		CString msg;
		if (pCommon->GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, pCommon->IsFiltered());
		if (pCommon->GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabView message handlers

void CTabView::OnSize(
		UINT nType,
		int cx,
		int cy)
{
	CCtrlView::OnSize(nType, cx, cy);
	if (!m_Panes.empty())
	{
		CRect r;
		GetTabCtrl().GetClientRect(r);
		GetTabCtrl().AdjustRect(FALSE, r);
		for (std::vector<CWnd*>::iterator iter = m_Panes.begin(); iter != m_Panes.end(); ++iter)
		{
			(*iter)->MoveWindow(r.left, r.top, r.Width(), r.Height());
		}
	}
}

void CTabView::OnSetFocus(CWnd* pOldWnd)
{
	int nIndex = GetTabCtrl().GetCurSel();
	if (pOldWnd && DYNAMIC_DOWNCAST(CView, pOldWnd))
	{
		switch (nIndex)
		{
		case 0:
			m_pLastFocusRuns = reinterpret_cast<CView*>(pOldWnd);
			break;
		case 2:
			m_pLastFocusCal = reinterpret_cast<CView*>(pOldWnd);
			break;
		}
	}
	SetActiveView();
	//CCtrlView::OnSetFocus(pOldWnd);
}

void CTabView::OnSelChanging(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	int nIndex = GetTabCtrl().GetCurSel();
	if (0 > nIndex || nIndex >= GetTabCtrl().GetItemCount())
		return;
	if (0 == nIndex)
	{
		int r, c;
		m_splitterRuns.GetActivePane(&r, &c);
		if (0 <= r && 0 <= c)
			m_pLastFocusRuns = reinterpret_cast<CView*>(m_splitterRuns.GetPane(r, c));
		// No else. If there is no active, we want to inherit what was set
		// in the OnSetFocus handler.
	}
	else if (2 == nIndex)
	{
		int r, c;
		m_splitterCal.GetActivePane(&r, &c);
		if (0 <= r && 0 <= c)
			m_pLastFocusCal = reinterpret_cast<CView*>(m_splitterCal.GetPane(r, c));
	}
	m_Panes[nIndex]->ShowWindow(SW_HIDE);
	m_Panes[nIndex]->EnableWindow(FALSE);
	*pResult = 0;
}

void CTabView::OnSelChange(
		NMHDR* /*pNMHDR*/,
		LRESULT* pResult)
{
	int nIndex = GetTabCtrl().GetCurSel();
	if (0 > nIndex || nIndex >= GetTabCtrl().GetItemCount())
		return;
	m_Panes[nIndex]->EnableWindow(TRUE);
	m_Panes[nIndex]->ShowWindow(SW_SHOW);
	SetActiveView();
	*pResult = 0;
}
