#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
