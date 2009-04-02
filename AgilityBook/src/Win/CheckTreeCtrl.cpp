/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * [wxWidgets] The above comments are still pertinent.
 *
 * Revision History
 */

#include "stdafx.h"
#include "CheckTreeCtrl.h"

#include "res/CalEmpty.xpm"
#include "res/checked.xpm"
#include "res/unchecked.xpm"

#ifdef WIN32
// GetState/SetState for state image lists only seems to work for Windows.
// So we'll use the normal image list on other platforms.
#define WX_TREE_HAS_STATE
#endif


IMPLEMENT_CLASS(CCheckTreeCtrl, wxTreeCtrl)


DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_CHECK_CHANGED)


CCheckTreeCtrl::CCheckTreeCtrl(
		wxWindow* pParent,
		const wxPoint& pos,
		const wxSize& size)
	: wxTreeCtrl()
	, m_stateList(16,16)
	, m_stateNone(-1)
	, m_stateUnChecked(-1)
	, m_stateChecked(-1)
{
	wxTreeCtrl::Create(pParent, wxID_ANY, pos, size,
		wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_ROW_LINES|wxTR_SINGLE);
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CCheckTreeCtrl::OnClick), NULL, this);
	Connect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(CCheckTreeCtrl::OnKeyDown), NULL, this);

	m_stateNone = m_stateList.Add(wxIcon(CalEmpty_xpm));
	m_stateUnChecked = m_stateList.Add(wxIcon(unchecked_xpm));
	m_stateChecked = m_stateList.Add(wxIcon(checked_xpm));
#ifdef WX_TREE_HAS_STATE
	SetStateImageList(&m_stateList);
#else
	SetImageList(&m_stateList);
#endif
}


void CCheckTreeCtrl::ShowCheckbox(
		wxTreeItemId hItem,
		bool bShow)
{
	if (IsCheckVisible(hItem) && !bShow)
	{
#ifdef WX_TREE_HAS_STATE
		wxTreeCtrl::SetState(hItem, m_stateNone);
#else
		wxTreeCtrl::SetItemImage(hItem, m_stateNone);
#endif
	}
	else if (!IsCheckVisible(hItem) && bShow)
	{
#ifdef WX_TREE_HAS_STATE
		wxTreeCtrl::SetState(hItem, m_stateUnChecked);
#else
		wxTreeCtrl::SetItemImage(hItem, m_stateUnChecked);
#endif
	}
}


bool CCheckTreeCtrl::IsCheckVisible(wxTreeItemId hItem)
{
#ifdef WX_TREE_HAS_STATE
	return wxTreeCtrl::GetState(hItem) != m_stateNone;
#else
	int index = wxTreeCtrl::GetItemImage(hItem);
	return index == m_stateChecked || index == m_stateUnChecked;
#endif
}


bool CCheckTreeCtrl::GetChecked(wxTreeItemId hItem)
{
#ifdef WX_TREE_HAS_STATE
	return wxTreeCtrl::GetState(hItem) == m_stateChecked;
#else
	return wxTreeCtrl::GetItemImage(hItem) == m_stateChecked;
#endif
}


bool CCheckTreeCtrl::SetChecked(
		wxTreeItemId hItem,
		bool bChecked,
		bool bCascade)
{
	if (IsCheckVisible(hItem))
	{
		int icon = m_stateNone;
		if (bChecked)
		{
			icon = m_stateChecked;
			CheckParentCheck(GetItemParent(hItem));
			if (bCascade)
				Cascade(hItem, true);
		}
		else
		{
			icon = m_stateUnChecked;
			if (bCascade)
				Cascade(hItem, false);
		}
#ifdef WX_TREE_HAS_STATE
		wxTreeCtrl::SetState(hItem, icon);
#else
		wxTreeCtrl::SetItemImage(hItem, icon);
#endif
		return true;
	}
	else
		return false;
}


void CCheckTreeCtrl::SendDispInfo(wxTreeItemId hItem)
{
	wxTreeEvent evt(wxEVT_COMMAND_TREE_CHECK_CHANGED, GetId());
	evt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(evt);
}


// When we check a child, make sure the parent chain is checked.
void CCheckTreeCtrl::CheckParentCheck(wxTreeItemId hItem)
{
	if (hItem.IsOk() && hItem != GetRootItem())
	{
		bool bChecked = GetChecked(hItem);
		if (!bChecked)
		{
			if (SetChecked(hItem, true, false))
				SendDispInfo(hItem);
			else
				CheckParentCheck(GetItemParent(hItem));
		}
	}
}


// Cascade to children (don't do hItem)
int CCheckTreeCtrl::Cascade(
		wxTreeItemId hItem,
		bool bChecked)
{
	if (!hItem.IsOk())
		return 0;
	int nChanged = 0;

	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = GetFirstChild(hItem, cookie);
	while (hChildItem)
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
		hChildItem = GetNextChild(hChildItem, cookie);
	}
	return nChanged;
}


// We could connect to EVT_TREE_STATE_IMAGE_CLICK, but that only works on
// Windows. So just handle all clicks ourselves and it will work on platforms.
void CCheckTreeCtrl::OnClick(wxMouseEvent& evt)
{
	int flags = 0;
	wxTreeItemId item = HitTest(evt.GetPosition(), flags);
	if (item.IsOk() && (flags & (wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMSTATEICON)))
	{
		bool bChecked = !GetChecked(item);
		SetChecked(item, bChecked);
		SendDispInfo(item);
	}
	evt.Skip();
}


void CCheckTreeCtrl::OnKeyDown(wxTreeEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		{
			wxTreeItemId hItem = GetSelection();
			if (hItem.IsOk() && IsCheckVisible(hItem))
			{
				bool bChecked = !GetChecked(hItem);
				SetChecked(hItem, bChecked);
				SendDispInfo(hItem);
			}
		}
		break;
	}
}
