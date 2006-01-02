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
 * @brief A tree control with checkboxes.
 * @author David Connet
 *
 * This tree control supports items without a checkbox, with a checkbox and
 * "disabled" checkboxes. If the user presses 'space', the checkbox state
 * is toggled only if a checkbox is visible and enabled. (The TVS_CHECKBOX
 * style causes an item without a checkbox to suddenly display a checkbox
 * when 'space' is pressed - that's why we have this special class.)
 *
 * CTreeCtrl compatibility:
 * This class will not work with a control that has the TVS_CHECKBOX style set.
 * Get/SetCheck calls must be replaced with Get/SetChecked calls. Do not call
 * Get/SetCheck via the base class - the checkboxes will get messed up.
 *
 * Revision History
 */

#include "stdafx.h"
#include "resource.h"
#include "CheckTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckTreeCtrl

CCheckTreeCtrl::CCheckTreeCtrl()
	: CTreeCtrl()
{
	m_stateList.Create(16, 16, ILC_MASK, 3, 0);
	m_stateList.Add(AfxGetApp()->LoadIcon(IDI_UNCHECKED)); // Index 0 is ignored.
	m_stateUnChecked = m_stateList.Add(AfxGetApp()->LoadIcon(IDI_UNCHECKED));
	m_stateChecked = m_stateList.Add(AfxGetApp()->LoadIcon(IDI_CHECKED));
}

CCheckTreeCtrl::~CCheckTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CCheckTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCheckTreeCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT_EX(TVN_KEYDOWN, OnKeyDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCheckTreeCtrl::ShowCheckbox(
		HTREEITEM hItem,
		bool bShow)
{
	if (IsCheckVisible(hItem))
	{
		if (!bShow)
			CTreeCtrl::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
	}
	else
	{
		if (bShow)
			CTreeCtrl::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(m_stateUnChecked), TVIS_STATEIMAGEMASK);
	}
}

bool CCheckTreeCtrl::IsCheckVisible(HTREEITEM hItem) const
{
	UINT state = GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12;
	return 0 != state;
}

bool CCheckTreeCtrl::GetChecked(HTREEITEM hItem) const
{
	UINT state = GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12;
	return m_stateChecked == state;
}

bool CCheckTreeCtrl::SetChecked(
		HTREEITEM hItem,
		bool bChecked,
		bool bCascade)
{
	if (IsCheckVisible(hItem))
	{
		int icon = 0;
		if (bChecked)
		{
			icon = m_stateChecked;
			CheckParentCheck(GetParentItem(hItem));
			if (bCascade)
				Cascade(hItem, true);
		}
		else
		{
			icon = m_stateUnChecked;
			if (bCascade)
				Cascade(hItem, false);
		}
		CTreeCtrl::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(icon), TVIS_STATEIMAGEMASK);
		return true;
	}
	else
		return false;
}

// Private

// When we check a child, make sure the parent chain is checked.
void CCheckTreeCtrl::CheckParentCheck(HTREEITEM hItem)
{
	if (NULL != hItem)
	{
		bool bChecked = GetChecked(hItem);
		if (!bChecked)
		{
			if (SetChecked(hItem, true, false))
				SendDispInfo(hItem);
			else
				CheckParentCheck(GetParentItem(hItem));
		}
	}
}

// Cascade to children (don't do hItem)
int CCheckTreeCtrl::Cascade(
		HTREEITEM hItem,
		bool bChecked)
{
	if (NULL == hItem)
		return 0;
	int nChanged = 0;
	HTREEITEM hChildItem = GetNextItem(hItem, TVGN_CHILD);
	while (NULL != hChildItem)
	{
		if (GetChecked(hChildItem) != bChecked)
		{
			if (SetChecked(hChildItem, bChecked))
			{
				SendDispInfo(hChildItem);
				++nChanged;
			}
		}
		nChanged += Cascade(hChildItem, bChecked);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	return nChanged;
}

void CCheckTreeCtrl::SendDispInfo(HTREEITEM hItem)
{
	if (hItem)
	{
		NMTVDISPINFO tvdi;
		tvdi.hdr.hwndFrom = GetSafeHwnd();
		tvdi.hdr.idFrom = 0;
		tvdi.hdr.code = TVN_SETDISPINFO;
		tvdi.item.mask = TVIF_STATE;
		tvdi.item.hItem = hItem;
		tvdi.item.state = (GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12);
		tvdi.item.stateMask = TVIS_STATEIMAGEMASK;
		tvdi.item.pszText = NULL;
		tvdi.item.cchTextMax = 0;
		tvdi.item.iImage = 0;
		tvdi.item.iSelectedImage = 0;
		tvdi.item.cChildren = 0;
		tvdi.item.lParam = 0;
		GetParent()->SendMessage(WM_NOTIFY, NULL, reinterpret_cast<LPARAM>(&tvdi));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCheckTreeCtrl message handlers

void CCheckTreeCtrl::PreSubclassWindow() 
{
	CTreeCtrl::PreSubclassWindow();
	ASSERT(!(GetStyle() & TVS_CHECKBOXES));
	SetImageList(&m_stateList, TVSIL_STATE);
}

int CCheckTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	ASSERT(!(GetStyle() & TVS_CHECKBOXES));
	SetImageList(&m_stateList, TVSIL_STATE);
	return 0;
}

void CCheckTreeCtrl::OnLButtonDown(
		UINT nFlags,
		CPoint point) 
{
	UINT flags;
	HTREEITEM hItem = HitTest(point, &flags);
	if (hItem && (TVHT_ONITEMSTATEICON & flags) && IsCheckVisible(hItem))
	{
		bool bChecked = !GetChecked(hItem);
		SetChecked(hItem, bChecked);
		SendDispInfo(hItem);
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CCheckTreeCtrl::OnKeyDown(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = reinterpret_cast<TV_KEYDOWN*>(pNMHDR);
	LRESULT res = 0;
	if (VK_SPACE == pTVKeyDown->wVKey)
	{
		HTREEITEM hItem = GetSelectedItem();
		if (hItem && IsCheckVisible(hItem))
		{
			bool bChecked = !GetChecked(hItem);
			SetChecked(hItem, bChecked);
			SendDispInfo(hItem);
		}
		res = 1;
	}
	*pResult = res;
	return static_cast<BOOL>(res);
}
