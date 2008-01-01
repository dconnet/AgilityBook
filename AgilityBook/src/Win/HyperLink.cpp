/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief HyperLink static control.
 * @author David Connet
 *
 * Comments from original Hyperlink code:
 *  HyperLink static control. Will open the default browser with the given URL
 *  when the user clicks on the link.
 *  Copyright (C) 1997 - 1999 Chris Maunder
 *  All rights reserved. May not be sold for profit.
 *
 *  Thanks to Pal K. Tonder for auto-size and window caption changes.
 *
 *  "GotoURL" function by Stuart Patterson
 *  As seen in the August, 1997 Windows Developer's Journal.
 *  Copyright 1997 by Miller Freeman, Inc. All rights reserved.
 *  Modified by Chris Maunder to use TCHARs instead of chars.
 *
 *  "Default hand cursor" from Paul DiLascia's Jan 1998 MSJ article.
 *
 *  2/29/00 -- P. Shaffer standard font mod.
 * The code has been significantly changed (style) and some problems with
 * the tooltip fixed.
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-04-01 DRC Made hyperlinks keyboard-capable.
 * @li 2004-03-05 DRC Made goto-url functionality visible.
 * @li 2003-08-24 DRC Removed 4311 warning in VC7 with HINSTANCE.
 * @li 2003-08-16 DRC Modified code from Chris Maunder's original
 */

#include "stdafx.h"
#include "HyperLink.h"
#include "resource.h"

#include "atlconv.h"	// for Unicode conversion - requires #include <afxdisp.h> // MFC OLE automation classes
#include "ARBTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1

/////////////////////////////////////////////////////////////////////////////

static LONG GetRegKey(
		HKEY key,
		LPCTSTR subkey,
		LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		lstrcpy(retdata,data);
		RegCloseKey(hkey);
	}
	return retval;
}


bool CHyperLink::GotoURL(CString const& url)
{
	CWaitCursor wait;

	// First try ShellExecute()
	// Use INT_PTR instead of 'int' for compatibility with VC7.
	// Removes the '4311' warning message.
	INT_PTR result = reinterpret_cast<INT_PTR>(ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW));

	// If it failed, get the .htm regkey and lookup the program
	if (result <= HINSTANCE_ERROR)
	{
		TCHAR key[MAX_PATH + MAX_PATH];
		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
		{
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
			{
				TCHAR* pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {	// No quotes found
					pos = _tcsstr(key, _T("%1"));	// Check for %1, without quotes
					if (pos == NULL)				// No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';				// Remove the parameter
				}
				else
					*pos = '\0';					// Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);

				result = WinExec(CStringA(key), SW_SHOW);
			}
		}
	}

	if (result <= HINSTANCE_ERROR)
	{
		CString str;
		str.LoadString(IDS_UNABLE_TO_OPEN);
		otstringstream msg;
			msg << (LPCTSTR)str
				<< (LPCTSTR)url
				<< _T("\n");
		switch (result)
		{
		case 0:
			str.LoadString(IDS_SYSERR_MEMORY);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_PNF:
			str.LoadString(IDS_SYSERR_SE_ERR_PNF);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_FNF:
			str.LoadString(IDS_SYSERR_SE_ERR_FNF);
			msg << (LPCTSTR)str;
			break;
		case ERROR_BAD_FORMAT:
			str.LoadString(IDS_SYSERR_ERROR_BAD_FORMAT);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_ACCESSDENIED:
			str.LoadString(IDS_SYSERR_SE_ERR_ACCESSDENIED);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_ASSOCINCOMPLETE:
			str.LoadString(IDS_SYSERR_SE_ERR_ASSOCINCOMPLETE);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_DDEBUSY:
			str.LoadString(IDS_SYSERR_SE_ERR_DDEBUSY);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_DDEFAIL:
			str.LoadString(IDS_SYSERR_SE_ERR_DDEFAIL);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_DDETIMEOUT:
			str.LoadString(IDS_SYSERR_SE_ERR_DDETIMEOUT);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_DLLNOTFOUND:
			str.LoadString(IDS_SYSERR_SE_ERR_DLLNOTFOUND);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_NOASSOC:
			str.LoadString(IDS_SYSERR_SE_ERR_NOASSOC);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_OOM:
			str.LoadString(IDS_SYSERR_SE_ERR_OOM);
			msg << (LPCTSTR)str;
			break;
		case SE_ERR_SHARE:
			str.LoadString(IDS_SYSERR_SE_ERR_SHARE);
			msg << (LPCTSTR)str;
			break;
		default:
			str.FormatMessage(IDS_SYSERR_UNKNOWN, (long)result);
			msg << (LPCTSTR)str;
			break;
		}
		AfxMessageBox(msg.str().c_str(), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
	: m_crLinkColor(RGB(0,0,238))		// Blue
	, m_crVisitedColor(RGB(85,26,139))	// Purple
	, m_crHoverColor(RGB(255,0,0))		// Red
	, m_bOverControl(false)				// Cursor not yet over control
	, m_bVisited(false)					// Hasn't been visited yet.
	, m_nUnderline(ulHover)				// Underline the link?
	, m_bAdjustToFit(true)				// Resize the window to fit the text?
	, m_strURL(_T(""))					// hyperlink URL
	, m_UnderlineFont()					// Font for underline display
	, m_StdFont()						// Standard font
	, m_hLinkCursor(NULL)				// Cursor for hyperlink
	, m_ToolTip()						// The tooltip
	, m_nTimerID(100)
{
}


CHyperLink::~CHyperLink()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink overrides

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}


BOOL CHyperLink::DestroyWindow()
{
	KillTimer(m_nTimerID);
	return CStatic::DestroyWindow();
}


void CHyperLink::PreSubclassWindow()
{
	// We want to get mouse clicks via STN_CLICKED
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	// Set the URL as the window text
	if (m_strURL.IsEmpty())
		GetWindowText(m_strURL);

	// Check that the window text isn't empty. If it is, set it as the URL.
	CString strWndText;
	GetWindowText(strWndText);
	if (strWndText.IsEmpty())
	{
		//ASSERT(!m_strURL.IsEmpty());	// Window and URL both NULL. DUH!
		// This should be ok - I may be setting the text in the initdialog.
		// This means the control will run the presubclass code before I
		// have a chance to do my thing...
		if (!m_strURL.IsEmpty())
			SetWindowText(m_strURL);
	}

	CFont* pFont = GetFont();
	if (!pFont)
	{
		HFONT hFont = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		if (hFont == NULL)
			hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
		if (hFont)
			pFont = CFont::FromHandle(hFont);
	}
	ASSERT(pFont);
	if (pFont)
	{
		ASSERT(pFont->GetSafeHandle());

		// Create the underline font
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_StdFont.CreateFontIndirect(&lf);
		lf.lfUnderline = static_cast<BYTE>(TRUE);
		m_UnderlineFont.CreateFontIndirect(&lf);
	}

	// Create the tooltip - must do this before calling PositionWindow!
	CRect rect;
	GetClientRect(rect);
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(this, m_strURL, rect, TOOLTIP_ID);

	PositionWindow();		// Adjust size of window to fit URL if necessary
	SetDefaultCursor();		// Try and load up a "hand" cursor
	SetUnderline();

	CStatic::PreSubclassWindow();
}


BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	//{{AFX_MSG_MAP(CHyperLink)
	ON_WM_GETDLGCODE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

UINT CHyperLink::OnGetDlgCode()
{
	return DLGC_WANTCHARS;
}


HBRUSH CHyperLink::CtlColor(
		CDC* pDC,
		UINT nCtlColor)
{
	UNUSED(nCtlColor);
	ASSERT(nCtlColor == CTLCOLOR_STATIC);
	if (m_bOverControl)
		pDC->SetTextColor(m_crHoverColor);
	else if (m_bVisited)
		pDC->SetTextColor(m_crVisitedColor);
	else
		pDC->SetTextColor(m_crLinkColor);
	// transparent text.
	pDC->SetBkMode(TRANSPARENT);
	return reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}


BOOL CHyperLink::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->SetBkColor(::GetSysColor(COLOR_3DFACE));
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	//pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));
	return TRUE;
}


void CHyperLink::OnSetFocus(CWnd* /*pOldWnd*/)
{
	DrawFocusRect();
}


void CHyperLink::OnKillFocus(CWnd* /*pNewWnd*/)
{
	DrawFocusRect();
}


void CHyperLink::OnMouseMove(
		UINT nFlags,
		CPoint point)
{
	if (!m_bOverControl)	// Cursor has just moved over control
	{
		m_bOverControl = true;
		if (m_nUnderline == ulHover)
			SetFont(&m_UnderlineFont);
		Invalidate();
		SetTimer(m_nTimerID, 100, NULL);
	}
	CStatic::OnMouseMove(nFlags, point);
}


void CHyperLink::OnTimer(UINT_PTR nIDEvent)
{
	CPoint p(GetMessagePos());
	ScreenToClient(&p);
	CRect rect;
	GetClientRect(rect);
	if (!rect.PtInRect(p))
	{
		m_bOverControl = false;
		KillTimer(m_nTimerID);
		if (m_nUnderline != ulAlways)
			SetFont(&m_StdFont);
		rect.bottom += 10;
		InvalidateRect(rect);
	}
	CStatic::OnTimer(nIDEvent);
}


BOOL CHyperLink::OnSetCursor(
		CWnd* /*pWnd*/,
		UINT /*nHitTest*/,
		UINT /*message*/)
{
	if (m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}
	return FALSE;
}


void CHyperLink::OnChar(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags)
{
	if (VK_SPACE == nChar)
	{
		if (GotoURL(m_strURL))
			SetVisited();	// Repaint to show visited colour
	}
	else
		CStatic::OnChar(nChar, nRepCnt, nFlags);
}


void CHyperLink::OnClicked()
{
	m_bOverControl = false;
	if (GotoURL(m_strURL))
		SetVisited();	// Repaint to show visited colour
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink operations

void CHyperLink::SetURL(CString const& strURL)
{
	m_strURL = strURL;
	if (::IsWindow(GetSafeHwnd()))
	{
		PositionWindow();
		m_ToolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
	}
}


void CHyperLink::SetColors(
		COLORREF crLinkColor,
		COLORREF crVisitedColor,
		COLORREF crHoverColor)
{
	m_crLinkColor = crLinkColor;
	m_crVisitedColor = crVisitedColor;
	if (crHoverColor == -1)
		m_crHoverColor = ::GetSysColor(COLOR_HIGHLIGHT);
	else
		m_crHoverColor = crHoverColor;
	if (::IsWindow(m_hWnd))
		Invalidate();
}


void CHyperLink::SetVisited(bool bVisited)
{
	m_bVisited = bVisited;
	if (::IsWindow(GetSafeHwnd()))
		Invalidate();
}


void CHyperLink::SetLinkCursor(HCURSOR hCursor)
{
	m_hLinkCursor = hCursor;
	if (m_hLinkCursor == NULL)
		SetDefaultCursor();
}


void CHyperLink::SetUnderline(UnderLineOptions nUnderline)
{
	if (m_nUnderline == nUnderline)
		return;
	if (::IsWindow(GetSafeHwnd()))
	{
		if (nUnderline == ulAlways)
			SetFont(&m_UnderlineFont);
		else
			SetFont(&m_StdFont);
		Invalidate();
	}

	m_nUnderline = nUnderline;
}


void CHyperLink::SetAutoSize(bool bAutoSize)
{
	m_bAdjustToFit = bAutoSize;
	if (::IsWindow(GetSafeHwnd()))
		PositionWindow();
}


void CHyperLink::DrawFocusRect()
{
	CWnd* pParent = GetParent();
	ASSERT(pParent);
	CRect r;
	GetWindowRect(&r);
	r.InflateRect(1, 1);
	pParent->ScreenToClient(&r);
	CClientDC dc(pParent);
	dc.DrawFocusRect(&r);
}


// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
// Suggested by Pål K. Tønder

void CHyperLink::PositionWindow()
{
	if (!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit)
		return;

	// Get the current window position
	CRect WndRect, ClientRect;
	GetWindowRect(WndRect);
	GetClientRect(ClientRect);

	ClientToScreen(ClientRect);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->ScreenToClient(WndRect);
		pParent->ScreenToClient(ClientRect);
	}

	// Get the size of the window text
	CString strWndText;
	GetWindowText(strWndText);

	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_UnderlineFont);
	CSize Extent = pDC->GetTextExtent(strWndText);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	// Adjust for window borders
	Extent.cx += WndRect.Width() - ClientRect.Width();
	Extent.cy += WndRect.Height() - ClientRect.Height();

	// Get the text justification via the window style
	DWORD dwStyle = GetStyle();

	// Recalc the window size and position based on the text justification
	if (dwStyle & SS_CENTERIMAGE)
		WndRect.DeflateRect(0, (WndRect.Height() - Extent.cy)/2);
	else
		WndRect.bottom = WndRect.top + Extent.cy;

	if (dwStyle & SS_CENTER)
		WndRect.DeflateRect((WndRect.Width() - Extent.cx)/2, 0);
	else if (dwStyle & SS_RIGHT)
		WndRect.left = WndRect.right - Extent.cx;
	else // SS_LEFT = 0, so we can't test for it explicitly
		WndRect.right = WndRect.left + Extent.cx;

	// Move the window
	SetWindowPos(NULL, WndRect.left, WndRect.top, WndRect.Width(), WndRect.Height(), SWP_NOZORDER);

	// Bug fix: Update the rectangle for the tooltip when we change the size!
	GetClientRect(ClientRect);
	m_ToolTip.SetToolRect(this, TOOLTIP_ID, ClientRect);
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink implementation

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CHyperLink::SetDefaultCursor()
{
	if (m_hLinkCursor == NULL)		// No cursor handle - load our own
	{
		// Note: If we are targetting win98/me,win2000/xp and the SDK is
		// installed, we could simply use IDC_HAND. But I'm currently
		// supporting win95 - oh well... so we use the original stuff.

		// Get the windows directory
		CString strWndDir;
		GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
		strWndDir.ReleaseBuffer();

		strWndDir += _T("\\winhlp32.exe");
		// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
		HMODULE hModule = LoadLibrary(strWndDir);
		if (hModule)
		{
			HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
			if (hHandCursor)
				m_hLinkCursor = CopyCursor(hHandCursor);
		}
		FreeLibrary(hModule);
	}
}
