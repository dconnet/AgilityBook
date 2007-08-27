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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "ListBox.h"

#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBox2

IMPLEMENT_DYNAMIC(CListBox2, CListBox)


CListBox2::CListBox2(bool bAutoDelete)
	: m_bAutoDelete(bAutoDelete)
	, m_HorzExtent(0)
{
}


CListBox2::~CListBox2()
{
}


CListData* CListBox2::GetData(int index) const
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
		return reinterpret_cast<CListData*>(GetItemDataPtr(index));
	else
		return NULL;
}


void CListBox2::SetData(int index, CListData* inData)
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
	{
		CListData* pData = GetData(index);
		if (pData)
			delete pData;
		SetItemDataPtr(index, inData);
	}
}


BOOL CListBox2::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_HSCROLL;
	return CListBox::PreCreateWindow(cs);
}


BEGIN_MESSAGE_MAP(CListBox2, CListBox)
	//{{AFX_MSG_MAP(CListBox2)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(LB_ADDSTRING, OnAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
	ON_MESSAGE(LB_RESETCONTENT, OnResetContent)
	ON_MESSAGE(LB_DELETESTRING, OnDeleteString)
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

void CListBox2::OnDestroy()
{
	ResetContent();
	CListBox::OnDestroy();
}


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


LRESULT CListBox2::OnResetContent(WPARAM, LPARAM)
{
	if (m_bAutoDelete)
	{
		for (int i = GetCount() - 1; i >= 0; --i)
		{
			CListData* pData = GetData(i);
			SetItemDataPtr(i, NULL);
			delete pData;
		}
	}
	return Default();
}


LRESULT CListBox2::OnDeleteString(WPARAM wParam, LPARAM)
{
	if (m_bAutoDelete)
	{
		int index = static_cast<int>(wParam);
		CListData* pData = GetData(index);
		SetItemDataPtr(index, NULL);
		delete pData;
	}
	return Default();
}

/////////////////////////////////////////////////////////////////////////////
// CCheckListBox2

IMPLEMENT_DYNAMIC(CCheckListBox2, CCheckListBox)


CCheckListBox2::CCheckListBox2(bool bAutoDelete)
	: m_bAutoDelete(bAutoDelete)
	, m_HorzExtent(0)
{
}


CCheckListBox2::~CCheckListBox2()
{
}


CListData* CCheckListBox2::GetData(int index) const
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
		return reinterpret_cast<CListData*>(GetItemDataPtr(index));
	else
		return NULL;
}


void CCheckListBox2::SetData(int index, CListData* inData)
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
	{
		CListData* pData = GetData(index);
		if (pData)
			delete pData;
		SetItemDataPtr(index, inData);
	}
}


BOOL CCheckListBox2::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_HSCROLL;
	return CCheckListBox::PreCreateWindow(cs);
}


BEGIN_MESSAGE_MAP(CCheckListBox2, CCheckListBox)
	//{{AFX_MSG_MAP(CCheckListBox2)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(LB_ADDSTRING, OnAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
	ON_MESSAGE(LB_RESETCONTENT, OnResetContent)
	ON_MESSAGE(LB_DELETESTRING, OnDeleteString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCheckListBox2::ComputeExtent(LPCTSTR lpszItem)
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
// CCheckListBox2 message handlers

void CCheckListBox2::OnDestroy()
{
	ResetContent();
	CCheckListBox::OnDestroy();
}


LRESULT CCheckListBox2::OnSetFont(WPARAM, LPARAM)
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


LRESULT CCheckListBox2::OnAddString(WPARAM, LPARAM lParam)
{
	LPCTSTR str = reinterpret_cast<LPCTSTR>(lParam);
	ComputeExtent(str);
	return Default();
}


LRESULT CCheckListBox2::OnInsertString(WPARAM, LPARAM lParam)
{
	LPCTSTR str = reinterpret_cast<LPCTSTR>(lParam);
	ComputeExtent(str);
	return Default();
}


LRESULT CCheckListBox2::OnResetContent(WPARAM, LPARAM)
{
	if (m_bAutoDelete)
	{
		for (int i = GetCount() - 1; i >= 0; --i)
		{
			CListData* pData = GetData(i);
			SetItemDataPtr(i, NULL);
			delete pData;
		}
	}
	return Default();
}


LRESULT CCheckListBox2::OnDeleteString(WPARAM wParam, LPARAM)
{
	if (m_bAutoDelete)
	{
		int index = static_cast<int>(wParam);
		CListData* pData = GetData(index);
		SetItemDataPtr(index, NULL);
		delete pData;
	}
	return Default();
}
