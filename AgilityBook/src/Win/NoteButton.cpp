/*
 * Copyright © 2005-2009 David Connet. All Rights Reserved.
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
 * @brief Special button for invoking notes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-12-12 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "NoteButton.h"

// Extra space around the icon
#define BUTTON_MARGIN	2

// Don't use GetSystemMetrics(SM_CXSMICON)-large font systems have bigger
// small icons - but our image list IS 16x16.
#define SMICON_CX	16
#define SMICON_CY	16

IMPLEMENT_DYNAMIC(CNoteButton, CButton)


CNoteButton::CNoteButton()
	: CButton()
	, m_idxNormal(-1)
{
	Load(IDI_NOTEBTN);
}


CNoteButton::CNoteButton(UINT idIcon)
	: CButton()
	, m_idxNormal(-1)
{
	Load(idIcon);
}


CNoteButton::~CNoteButton()
{
}


void CNoteButton::Load(UINT idIcon)
{
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 1, 0);
	VERIFY(0 <= (m_idxNormal = m_ImageList.Add(theApp.LoadIcon(idIcon))));
}


BEGIN_MESSAGE_MAP(CNoteButton, CButton)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CNoteButton::PreSubclassWindow()
{
	assert(GetStyle() & BS_OWNERDRAW);
	CButton::PreSubclassWindow();
	VERIFY(SetWindowPos(NULL, -1, -1,
		SMICON_CX + 2*BUTTON_MARGIN,
		SMICON_CY + 2*BUTTON_MARGIN,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}


int CNoteButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	assert(GetStyle() & BS_OWNERDRAW);
	VERIFY(SetWindowPos(NULL, -1, -1,
		SMICON_CX + 2*BUTTON_MARGIN,
		SMICON_CY + 2*BUTTON_MARGIN,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
	return 0;
}


void CNoteButton::OnSetFocus(CWnd* pOldWnd)
{
	CButton::OnSetFocus(pOldWnd);
	Invalidate();
	UpdateWindow();
}


void CNoteButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	Invalidate();
	UpdateWindow();
}


BOOL CNoteButton::OnEraseBkgnd(CDC* pDC)
{
	COLORREF crOld = pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	CRect r;
	GetClientRect(r);
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
	pDC->SetBkColor(crOld);
	return TRUE;
}


void CNoteButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	assert(lpDIS != NULL);
	// must have at least the first bitmap loaded before calling DrawItem
	assert(0 <= m_idxNormal);

	// Figure out where things are, etc.
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	UINT state = lpDIS->itemState;
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);

	CSize szIcon(SMICON_CX, SMICON_CY);
	CPoint pt = rect.TopLeft();
	pt.Offset(BUTTON_MARGIN, BUTTON_MARGIN);
	if (state & ODS_SELECTED)
		pt.Offset(1, 1);

	// Make sure the area under the icon is cleared - when we press the
	// button, we offset it, so we could have ghosting.
	COLORREF crOld = pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	CRect r(rect);
	r.DeflateRect(BUTTON_MARGIN, BUTTON_MARGIN);
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
	pDC->SetBkColor(crOld);

	// Draw the button image
	COLORREF crFg = CLR_DEFAULT;
	UINT style = ILD_NORMAL;
	if (state & ODS_DISABLED)
	{
		crFg = CLR_NONE;
		style = ILD_BLEND25;
	}
#if _MSC_VER < 1300
	ImageList_DrawEx(m_ImageList.m_hImageList, m_idxNormal,
		lpDIS->hDC, pt.x, pt.y, szIcon.cx, szIcon.cy,
		CLR_NONE, crFg, style);
#else
	m_ImageList.DrawEx(pDC, m_idxNormal, pt, szIcon, CLR_NONE, crFg, style);
#endif

	// Draw the button frame
	CPen black(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
	CPen gray(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	CPen white(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));

	CPen* old = pDC->SelectObject(&black);
	pDC->MoveTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top - 1);

	if (state & ODS_SELECTED)
	{
		pDC->MoveTo(rect.right - 1, rect.top);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom - 1);
		pDC->SelectObject(&gray);
		pDC->MoveTo(rect.left + 1, rect.bottom - 2);
		pDC->LineTo(rect.right - 2, rect.bottom - 2);
		pDC->LineTo(rect.right - 2, rect.top + 1);
		pDC->LineTo(rect.left + 1, rect.top + 1);
		pDC->LineTo(rect.left + 1, rect.bottom - 2);
	}
	else
	{
		pDC->SelectObject(&white);
		pDC->MoveTo(rect.left, rect.bottom - 2);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.right - 1, rect.top);
		pDC->SelectObject(&gray);
		pDC->MoveTo(rect.left + 1, rect.bottom - 2);
		pDC->LineTo(rect.right - 2, rect.bottom - 2);
		pDC->LineTo(rect.right - 2, rect.top);
	}
	pDC->SelectObject(old);

	// Finally, add the focus rect.
	if (state & ODS_FOCUS)
	{
		rect.DeflateRect(BUTTON_MARGIN+1, BUTTON_MARGIN+1);
		pDC->DrawFocusRect(rect);
	}
}
