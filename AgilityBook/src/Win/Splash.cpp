/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the Splash Screen component
 * @author David Connet
 *
 * Revision History
 * @li 2004-04-15 DRC Created (used vc6 splash screen component)
 */

#include "stdafx.h"
#include "resource.h"
#include "Splash.h"

#include "VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Border (one side)
#define BORDER	7

/////////////////////////////////////////////////////////////////////////////
// static stuff

BOOL CSplashWnd::c_bShowSplashWnd = TRUE;

CSplashWnd* CSplashWnd::c_pSplashWnd = NULL;

void CSplashWnd::EnableSplashScreen(BOOL bEnable)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd, bool bTimed)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd(bTimed);
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

void CSplashWnd::HideSplashScreen()
{
	if (c_pSplashWnd)
		c_pSplashWnd->HideSplashScreenImpl();
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreenImpl();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

CSplashWnd::CSplashWnd(bool bTimed)
	: m_bTimed(bTimed)
	, m_szBitmap(0,0)
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);
	m_szBitmap = CSize(bm.bmWidth, bm.bmHeight);

	CVersionNum ver;
	if (ver.Valid())
		m_Version.FormatMessage(IDS_ABOUT_VERSION, (LPCTSTR)ver.GetVersionString());

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE,
		0, 0,
		m_szBitmap.cx + 2 * BORDER, m_szBitmap.cy + 2 * BORDER,
		pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreenImpl()
{
	DestroyWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	if (m_bTimed)
		SetTimer(1, 2000, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	CRect r;
	GetClientRect(&r);

	// Fill with white...
	COLORREF cr = dc.SetBkColor(RGB(255,255,255));
	dc.ExtTextOut(0, 0, ETO_OPAQUE, &r, NULL, 0, NULL);
	dc.SetBkColor(cr);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(BORDER, BORDER, m_szBitmap.cx, m_szBitmap.cy, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);

	// Draw the frame: Outer frame, inner fill, inside frame
	// (remember, lineto goes _to_, not thru!)
	int i;
	// Gray lines first.
	CPen pen(PS_SOLID, 1, RGB(128, 128, 128));
	CPen* pOldPen = dc.SelectObject(&pen);
	// left, top outer frame sides
	dc.MoveTo(0, r.bottom - 1);
	dc.LineTo(0, 0);
	dc.LineTo(r.right, 0);
	// right, bottom fill
	// no need for "left, top fill", handled by ExtTextOut
	for (i = 1; i < BORDER - 1; ++i)
	{
		dc.MoveTo(i, r.bottom - i - 1);
		dc.LineTo(r.right - i - 1, r.bottom - i - 1);
		dc.LineTo(r.right - i - 1, i);
	}
	dc.SelectObject(pOldPen);
	dc.SelectStockObject(BLACK_PEN);
	// right, bottom outer frame
	dc.MoveTo(1, r.bottom - 1);
	dc.LineTo(r.right - 1, r.bottom - 1);
	dc.LineTo(r.right - 1, 0);
	// inside frame
	dc.MoveTo(BORDER - 1, BORDER - 1);
	dc.LineTo(BORDER - 1, r.bottom - BORDER);
	dc.LineTo(r.right - BORDER, r.bottom - BORDER);
	dc.LineTo(r.right - BORDER, BORDER - 1);
	dc.LineTo(BORDER - 1, BORDER - 1);

	// Now toss in the version number.
	if (!m_Version.IsEmpty())
	{
		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		lf.lfHeight = 100;
		lf.lfWeight = FW_BOLD;
		lstrcpy(lf.lfFaceName, "Comic Sans MS");
		font.CreatePointFontIndirect(&lf, &dc);
		CFont* pOldFont = dc.SelectObject(&font);
		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm);
		r.right -= (BORDER + tm.tmDescent); // This makes the text look square in the corner
		r.bottom -= BORDER;
		dc.SetBkMode(TRANSPARENT);
		// Create a shadow/smear the real text will sit on.
		dc.SetTextColor(RGB(192,192,192));
		dc.DrawText(m_Version, r, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
		r.OffsetRect(-2,0);
		dc.DrawText(m_Version, r, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
		r.OffsetRect(0,-2);
		dc.DrawText(m_Version, r, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
		r.OffsetRect(2,0);
		dc.DrawText(m_Version, r, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
		// The real text.
		r.OffsetRect(-1,1);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(m_Version, r, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
		dc.SelectObject(pOldFont);
	}
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreenImpl();
}
