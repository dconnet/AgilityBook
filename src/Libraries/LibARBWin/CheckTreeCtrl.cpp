/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
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
 * 2018-10-11 Moved to Win LibARBWin
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 */

#include "stdafx.h"
#include "LibARBWin/CheckTreeCtrl.h"

#include "LibARBWin/DPI.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/ResourceManager.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxIMPLEMENT_CLASS(CCheckTreeCtrl, CTreeCtrl)


DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_CHECK_CHANGED)


CCheckTreeCtrl::CCheckTreeCtrl(wxWindow* pParent, const wxPoint& pos, const wxSize& size)
	: CTreeCtrl()
	, m_stateList()
	, m_stateNone(-1)
	, m_stateUnChecked(-1)
	, m_stateChecked(-1)
{
	CTreeCtrl::Create(
		pParent,
		wxID_ANY,
		pos,
		size,
		wxTR_FULL_ROW_HIGHLIGHT | wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxTR_SINGLE);
	Bind(wxEVT_LEFT_DOWN, &CCheckTreeCtrl::OnClick, this);
	Bind(wxEVT_KEY_DOWN, &CCheckTreeCtrl::OnKeyDown, this);

	m_stateList.Create(DPI::Scale(this, 16), DPI::Scale(this, 16));

	m_stateNone = m_stateList.Add(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_stateUnChecked = m_stateList.Add(CResourceManager::Get()->GetIcon(ImageMgrUnChecked));
	m_stateChecked = m_stateList.Add(CResourceManager::Get()->GetIcon(ImageMgrChecked));
#ifdef WX_TREE_HAS_STATE
	SetStateImageList(&m_stateList);
#else
	SetImageList(&m_stateList);
#endif
}


CCheckTreeCtrl::~CCheckTreeCtrl()
{
	// Note: wx2.8 has no problem. But wx2.9 gets a custom draw message during
	// destruction that then accesses the image list and asserts. Clearing them
	// here fixes that. Should note, it only appears to happen with state image
	// lists, but I'll clear both for safety.
#ifdef WX_TREE_HAS_STATE
	SetStateImageList(nullptr);
#else
	SetImageList(nullptr);
#endif
}


void CCheckTreeCtrl::ShowCheckbox(wxTreeItemId hItem, bool bShow)
{
	if (IsCheckVisible(hItem) && !bShow)
	{
		SetItemState(hItem, m_stateNone);
	}
	else if (!IsCheckVisible(hItem) && bShow)
	{
		SetItemState(hItem, m_stateUnChecked);
	}
}


bool CCheckTreeCtrl::IsCheckVisible(wxTreeItemId hItem)
{
	int index = GetItemState(hItem);
	return index == m_stateChecked || index == m_stateUnChecked;
}


bool CCheckTreeCtrl::GetChecked(wxTreeItemId hItem)
{
	return GetItemState(hItem) == m_stateChecked;
}


bool CCheckTreeCtrl::SetChecked(wxTreeItemId hItem, bool bChecked, bool bCascade)
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
		SetItemState(hItem, icon);
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
int CCheckTreeCtrl::Cascade(wxTreeItemId hItem, bool bChecked)
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
		hChildItem = GetNextChild(hItem, cookie);
	}
	return nChanged;
}


// We could connect to EVT_TREE_STATE_IMAGE_CLICK, but that only works on
// Windows. So just handle all clicks ourselves and it will work on platforms.
void CCheckTreeCtrl::OnClick(wxMouseEvent& evt)
{
	int flags = 0;
	wxTreeItemId item = HitTest(evt.GetPosition(), flags);
	if (item.IsOk() && (flags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMSTATEICON)))
	{
		bool bChecked = !GetChecked(item);
		SetChecked(item, bChecked);
		SendDispInfo(item);
	}
	evt.Skip();
}


void CCheckTreeCtrl::OnKeyDown(wxKeyEvent& evt)
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
	evt.Skip();
}
