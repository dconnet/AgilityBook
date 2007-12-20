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
 * @brief implementation of the CMainFrame class
 * @author David Connet
 *
 * Revision History
 * @li 2006-09-01 DRC Added multi-monitor support.
 * @li 2003-05-18 DRC Obsoleted registry settings "Calendar"/"List" (bool)
 *                    "Settings"/"View" (int) changed (see TabView.cpp)
 */

#include "stdafx.h"
#include <afxpriv.h>
#include "AgilityBook.h"
#include "MainFrm.h"

#include "ARBConfigVenue.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"
#include "CommonView.h"
#include "FilterOptions.h"
#include "Splash.h"
#include "TabView.h"

#if WINVER < 0x0500
#if _MSC_VER < 1400
#define COMPILE_MULTIMON_STUBS
#endif
#include <MultiMon.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_STATUS, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FILTERED, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DOG, OnUpdatePane)
	ON_COMMAND(ID_NEXT_TAB, OnNextTab)
	ON_COMMAND(ID_PREV_TAB, OnPrevTab)
	ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_CHOOSE, OnUpdateFileLanguageChoose)
	ON_COMMAND(ID_FILE_LANGUAGE_CHOOSE, OnFileLanguageChoose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_DOG,
	ID_INDICATOR_STATUS,
	ID_INDICATOR_FILTERED,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
};


CMainFrame::CMainFrame()
	: m_pTabView(NULL)
	, m_pLangMgr(NULL)
	, m_pNewMenu(NULL)
{
}


CMainFrame::~CMainFrame()
{
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	int x = theApp.GetProfileInt(_T("Settings"), _T("lastXpos"), cs.x);
	int y = theApp.GetProfileInt(_T("Settings"), _T("lastYpos"), cs.y);
	int cx = theApp.GetProfileInt(_T("Settings"), _T("lastCX"), cs.cx);
	int cy = theApp.GetProfileInt(_T("Settings"), _T("lastCY"), cs.cy);

	bool bCompute = false;
	CPoint curPt;
	GetCursorPos(&curPt);
	if (CW_USEDEFAULT != x)
	{
		bCompute = true;
		curPt.x = x;
	}
	if (CW_USEDEFAULT != y)
	{
		bCompute = true;
		curPt.y = y;
	}
	CSize curSize(0, 0);
	if (CW_USEDEFAULT != cx)
	{
		bCompute = true;
		curSize.cx = cx;
	}
	if (CW_USEDEFAULT != cy)
	{
		bCompute = true;
		curSize.cy = cy;
	}

	if (bCompute)
	{
		HMONITOR hMon = MonitorFromPoint(curPt, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMon, &mi);
		CRect rWorkSpace(mi.rcWork);
		CRect rect(curPt, curSize);
		// Make sure window is not bigger.
		if (rect.Width() > rWorkSpace.Width())
			rect.right = rect.left + rWorkSpace.Width();
		if (rect.Height() > rWorkSpace.Height())
			rect.bottom = rect.top + rWorkSpace.Height();
		// Make sure the window is fully visible in the screen.
		if (!rWorkSpace.PtInRect(rect.TopLeft()))
		{
			if (rect.left < rWorkSpace.left)
				rect.OffsetRect(rWorkSpace.left - rect.left, 0);
			if (rect.top < rWorkSpace.top)
				rect.OffsetRect(0, rWorkSpace.top - rect.top);
		}
		// Only check the bottom-right if the rect has size.
		if (!rect.IsRectEmpty() && !rWorkSpace.PtInRect(rect.BottomRight()))
		{
			if (rect.right > rWorkSpace.right)
				rect.OffsetRect(rWorkSpace.right - rect.right, 0);
			if (rect.bottom > rWorkSpace.bottom)
				rect.OffsetRect(0, rWorkSpace.bottom - rect.bottom);
		}
		if (CW_USEDEFAULT != x)
			cs.x = rect.left;
		if (CW_USEDEFAULT != y)
			cs.y = rect.top;
		if (CW_USEDEFAULT != cx)
			cs.cx = rect.Width();
		if (CW_USEDEFAULT != cy)
			cs.cy = rect.Height();
	}

	return CFrameWnd::PreCreateWindow(cs);
}


#ifdef _DEBUG
// CMainFrame diagnostics
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}


void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


void CMainFrame::InitLangMgr(CLanguageManager* pLangMgr)
{
	m_pLangMgr = pLangMgr;
}


void CMainFrame::SetStatusText(
		CString const& msg,
		bool bFiltered)
{
	if (!::IsWindow(m_wndStatusBar.GetSafeHwnd()))
		return;
	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_STATUS);
	if (0 > index)
		return;
	UINT nId, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(index, nId, nStyle, cxWidth);
	CWindowDC dc(&m_wndStatusBar);
	CFont* pOldFont = dc.SelectObject(m_wndStatusBar.GetFont());
	CSize sz = dc.GetTextExtent(msg);
	dc.SelectObject(pOldFont);
	m_wndStatusBar.SetPaneInfo(index, nId, nStyle, sz.cx);
	// Note, a cmdui handler is required to get text to display.
	m_wndStatusBar.SetPaneText(index, msg);

	CString filtered;
	if (bFiltered)
		filtered.LoadString(ID_INDICATOR_FILTERED);
	index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_FILTERED);
	m_wndStatusBar.SetPaneText(index, filtered);
}


void CMainFrame::SetStatusText2(CString const& msg)
{
	if (!::IsWindow(m_wndStatusBar.GetSafeHwnd()))
		return;
	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_DOG);
	if (0 > index)
		return;
	UINT nId, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(index, nId, nStyle, cxWidth);
	CWindowDC dc(&m_wndStatusBar);
	CFont* pOldFont = dc.SelectObject(m_wndStatusBar.GetFont());
	CSize sz = dc.GetTextExtent(msg);
	dc.SelectObject(pOldFont);
	m_wndStatusBar.SetPaneInfo(index, nId, nStyle, sz.cx);
	m_wndStatusBar.SetPaneText(index, msg);
}


void CMainFrame::SetCurTab(int tab)
{
	m_pTabView->SetCurSel(tab);
}


int CMainFrame::GetCurTab() const
{
	return m_pTabView->GetCurSel();
}


bool CMainFrame::ShowPointsAs(bool bHtml)
{
	return m_pTabView->ShowPointsAs(bHtml);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	int index = m_wndStatusBar.CommandToIndex(ID_INDICATOR_STATUS);
	m_wndStatusBar.SetPaneText(index, _T(""));

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	CSplashWnd::ShowSplashScreen(this);
	return 0;
}


void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
	delete m_pNewMenu;
	m_pNewMenu = NULL;
}


void CMainFrame::OnClose() 
{
	WINDOWPLACEMENT win;
	GetWindowPlacement(&win);
	int state;
	switch (win.showCmd)
	{
	default:
		state = 0;
		{
			CRect r;
			GetWindowRect(&r);
			theApp.WriteProfileInt(_T("Settings"), _T("lastXpos"), r.left);
			theApp.WriteProfileInt(_T("Settings"), _T("lastYpos"), r.top);
			theApp.WriteProfileInt(_T("Settings"), _T("lastCX"), r.Width());
			theApp.WriteProfileInt(_T("Settings"), _T("lastCY"), r.Height());
		}
		break;
	case SW_SHOWMINIMIZED:
		state = -1;
		break;
	case SW_SHOWMAXIMIZED:
		state = 1;
		break;
	}
	theApp.WriteProfileInt(_T("Settings"), _T("lastState"), state);

	CFrameWnd::OnClose();
}


void CMainFrame::OnUpdatePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}


void CMainFrame::OnNextTab()
{
	if (m_pTabView)
	{
		int idx = m_pTabView->GetCurSel();
		++idx;
		if (idx == m_pTabView->GetItemCount())
			idx = 0;
		m_pTabView->SetCurSel(idx);
	}
}


void CMainFrame::OnPrevTab()
{
	if (m_pTabView)
	{
		int idx = m_pTabView->GetCurSel();
		--idx;
		if (idx == -1)
			idx = m_pTabView->GetItemCount() - 1;
		m_pTabView->SetCurSel(idx);
	}
}


void CMainFrame::OnUpdateFileLanguageChoose(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (m_pLangMgr && 0 < m_pLangMgr->NumLanguages())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnFileLanguageChoose()
{
	if (m_pLangMgr && m_pLangMgr->SelectLanguage())
	{
		// Reset frame menu...
		CMenu* pCurrentMenu = GetMenu();
		if (pCurrentMenu->m_hMenu != m_hMenuDefault)
		{
			pCurrentMenu->DestroyMenu();
			delete pCurrentMenu;
		}
		m_pNewMenu = new CMenu;
		m_pNewMenu->LoadMenu(IDR_MAINFRAME);
		SetMenu(m_pNewMenu);
		// ...accelerators...
		m_hAccelTable = NULL;
		LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME));
		// ...and status bar...
		// ...my stuff
		ICommonView* pView = dynamic_cast<ICommonView*>(GetActiveView());
		CString msg;
		bool bFiltered = false;
		if (pView && pView->GetMessage(msg))
			bFiltered = pView->IsFiltered();
		else
			msg.Empty();
		SetStatusText(msg, false);
		if (!pView || !pView->GetMessage2(msg))
			msg.Empty();
		SetStatusText2(msg);
		// ...'Ready' message
		SendMessage(WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
		// Tell all the views (we're SDI, so only need active doc)
		CDocument* pDoc = GetActiveDocument();
		if (pDoc)
			pDoc->UpdateAllViews(NULL, UPDATE_LANG_CHANGE);
		// And finally, fix the tabview
		if (m_pTabView)
			m_pTabView->UpdateLanguage();
	}
}
