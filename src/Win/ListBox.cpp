/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "ListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBox2

IMPLEMENT_DYNAMIC(CListBox2, CListBox)

CListBox2::CListBox2()
	: m_HorzExtent(0)
{
}

CListBox2::~CListBox2()
{
}

BOOL CListBox2::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_HSCROLL;
	return CListBox::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CListBox2, CListBox)
	//{{AFX_MSG_MAP(CListBox2)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(LB_ADDSTRING, OnAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CListBox2::ComputeExtent(LPCTSTR lpszItem)
{
	if (!lpszItem)
		return;
	DWORD style = GetStyle();
	if (style & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE))
	{
		if (!(style & LBS_HASSTRINGS))
			return;
	}
	CWindowDC dc(this);
	CFont* pOld = dc.SelectObject(GetFont());
	CSize sz = dc.GetTextExtent(lpszItem);
	dc.SelectObject(pOld);
	CRect rWin, rClient;
	GetWindowRect(&rWin);
	GetClientRect(&rClient);
	sz.cx += rWin.Width() - rClient.Width();
	if (sz.cx > m_HorzExtent)
	{
		m_HorzExtent = sz.cx;
		SetHorizontalExtent(m_HorzExtent);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CListBox2 message handlers

LRESULT CListBox2::OnSetFont(WPARAM, LPARAM)
{
	Default();
	SetRedraw(FALSE);
	m_HorzExtent = 0;
	SetHorizontalExtent(m_HorzExtent);
	int count = GetCount();
	for (int i = 0; i < count; ++i)
	{
		CString text;
		GetText(i, text);
		ComputeExtent(text);
	}
	SetRedraw(TRUE);
	return 0;
}

LRESULT CListBox2::OnAddString(WPARAM, LPARAM lParam)
{
	LPCTSTR str = reinterpret_cast<LPCTSTR>(lParam);
	ComputeExtent(str);
	return Default();
}

LRESULT CListBox2::OnInsertString(WPARAM, LPARAM lParam)
{
	LPCTSTR str = reinterpret_cast<LPCTSTR>(lParam);
	ComputeExtent(str);
	return Default();
}
