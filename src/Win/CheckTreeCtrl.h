#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CCheckTreeCtrl class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "Widgets.h"
#include <wx/imaglist.h>


class CCheckTreeCtrl : public CTreeCtrl
{
	DECLARE_CLASS(CCheckTreeCtrl)
public:
	CCheckTreeCtrl(
			wxWindow* pParent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize);

	void ShowCheckbox(
			wxTreeItemId hItem,
			bool bShow = true);
	bool IsCheckVisible(wxTreeItemId hItem);
	bool GetChecked(wxTreeItemId hItem);
	bool SetChecked(
			wxTreeItemId hItem,
			bool bChecked,
			bool bCascade = true);

private:
	void SendDispInfo(wxTreeItemId hItem);
	void CheckParentCheck(wxTreeItemId hItem);
	int Cascade(
			wxTreeItemId hItem,
			bool bChecked);
	// Hide the standard versions to make sure any usage to this tree
	// shows/hides the checkboxes properly.
	// (It's not quite a straight-drop in for wxTreeCtrl)
	// These are not virtual, so access via wxTreeCtrl is possible.
	// Don't do that!
	// [note, this are specifically not implemented]
	bool IsItemChecked(const wxTreeItemId& item) const;
	void SetItemCheck(const wxTreeItemId& item, bool check = true);

	wxImageList m_stateList;
	int m_stateNone;
	int m_stateUnChecked;
	int m_stateChecked;

	void OnClick(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
};

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_TREE_CHECK_CHANGED, 621)
END_DECLARE_EVENT_TYPES()

#define EVT_TREE_CHECK_CHANGED(id, fn) wx__DECLARE_TREEEVT(CHECK_CHANGED, id, fn)
