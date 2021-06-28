#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Change certain default wxWidget behaviors.
 * @author David Connet
 *
 * List Controls: Override SetColumnWidth.
 * Tree Controls: Add window support for states.
 * Text Controls: Turn off tabstops on multiline readonly controls.
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2015-05-19 Add ComputeHeightHint to CTreeCtrl.
 * 2014-11-03 Enable spellchecking on richedit on Win8+.
 * 2010-03-28 Moved SetColumnWidth override from CReportListCtrl.
 *            Removed wx2.9 kludges (was causing problems on Mac).
 * 2009-10-11 Created.
 */

#include "LibwxARBWin.h"

#include <wx/listctrl.h>
#include <wx/treectrl.h>


class ARBWIN_API CListCtrl : public wxListView
{
	DECLARE_CLASS(CListCtrl)
public:
	CListCtrl()
	{
	}
	CListCtrl(
		wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxLC_ICON,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxListCtrlNameStr)
		: wxListView(parent, id, pos, size, style, validator, name)
	{
		EnableSystemTheme();
	}
	bool SetColumnWidth(int col, int width) override
	{
#ifndef __WXMSW__
		if (wxLIST_AUTOSIZE_USEHEADER == width)
		{
			// Don't use header on non-windows platforms. According to docs, it
			// is only set to 80 pixels. On Mac with generic list, it appears to
			// size to the header text, but not the contents.
			width = wxLIST_AUTOSIZE;
		}
#endif
		return wxListView::SetColumnWidth(col, width);
	}
};


#ifdef __WXMSW__
// GetState/SetState for state image lists only seems to work for Windows.
// So we'll use the normal image list on other platforms.
#define WX_TREE_HAS_STATE
#endif


class ARBWIN_API CTreeCtrl : public wxTreeCtrl
{
	DECLARE_CLASS(CTreeCtrl)
public:
	CTreeCtrl()
	{
	}
	explicit CTreeCtrl(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxTreeCtrlNameStr)
		: wxTreeCtrl(parent, id, pos, size, style, validator, name)
	{
		EnableSystemTheme();
	}

	// Beware: We're hiding non-virtual base class functions
	int GetItemState(const wxTreeItemId& item)
	{
#ifdef WX_TREE_HAS_STATE
		return wxTreeCtrl::DoGetItemState(item);
#else
		return wxTreeCtrl::GetItemImage(item);
#endif
	}
	void SetItemState(const wxTreeItemId& item, int state)
	{
#ifdef WX_TREE_HAS_STATE
		wxTreeCtrl::DoSetItemState(item, state);
#else
		wxTreeCtrl::SetItemImage(item, state);
#endif
	}

	// Usage: After Layout/Fit in a dialog, call this and then Fit again.
	// Tree controls do not autosize to their content. This will.
	bool ComputeHeightHint();
};


class ARBWIN_API CTextCtrl : public wxTextCtrl
{
	DECLARE_CLASS(CTextCtrl)
public:
	CTextCtrl()
		: wxTextCtrl()
		, m_bAllowMultilineTabstop(false)
	{
	}
	CTextCtrl(
		wxWindow* parent,
		wxWindowID id,
		const wxString& value = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxTextCtrlNameStr)
		: wxTextCtrl(parent, id, value, pos, size, style, validator, name)
		, m_bAllowMultilineTabstop(false)
	{
	}

	bool AllowMultilineTabstop() const
	{
		return m_bAllowMultilineTabstop;
	}
	void AllowMultilineTabstop(bool bAllow)
	{
		m_bAllowMultilineTabstop = bAllow;
	}

	bool AcceptsFocusFromKeyboard() const override
	{
		return (IsEditable() || (IsMultiLine() && m_bAllowMultilineTabstop)) && wxControl::AcceptsFocus();
	}

	bool EnableSpellChecking(bool bForceRtfAsPlainText = true);

private:
	bool m_bAllowMultilineTabstop;
};


class ARBWIN_API CSpellCheckCtrl : public CTextCtrl
{
	DECLARE_CLASS(CSpellCheckCtrl)

public:
	CSpellCheckCtrl()
		: CTextCtrl()
	{
	}
	CSpellCheckCtrl(
		wxWindow* parent,
		wxWindowID id,
		const wxString& value = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxTextCtrlNameStr)
		: CTextCtrl()
	{
		bool bPlainText = false;
		if (!(style & (wxTE_RICH | wxTE_RICH2)))
		{
			bPlainText = true;
			style |= wxTE_RICH2;
		}

		Create(parent, id, value, pos, size, style, validator, name);

		EnableSpellChecking(bPlainText);
	}
};
