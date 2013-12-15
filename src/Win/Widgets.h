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
 * 2010-03-28 Moved SetColumnWidth override from CReportListCtrl.
 *            Removed wx2.9 kludges (was causing problems on Mac).
 * 2009-10-11 Created.
 */

#include <wx/listctrl.h>


class CListCtrl : public wxListView
{
	DECLARE_CLASS(CListCtrl)
public:
	CListCtrl() {}
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
#if !wxCHECK_VERSION(3, 0, 0) && defined(__WXMAC__)
		// On Mac, when I set a minimal size for lists, that list will often
		// be sized wrong. So fix it. This has been fixed in 2.9.1.
		if (wxDefaultSize != size)
			SetMinSize(size);
#endif
	}
#if !wxCHECK_VERSION(3, 0, 0) && defined(__WXMAC__)
	bool Create(
			wxWindow* parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxLC_ICON,
			const wxValidator& validator = wxDefaultValidator,
			const wxString& name = wxListCtrlNameStr)
	{
		bool rc = wxListView::Create(parent, id, pos, size, style, validator, name);
		if (wxDefaultSize != size)
			SetMinSize(size);
		return rc;
	}
#endif
	bool SetColumnWidth(int col, int width)
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


class CTreeCtrl : public wxTreeCtrl
{
	DECLARE_CLASS(CTreeCtrl)
public:
	CTreeCtrl() {}
	CTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
			const wxValidator& validator = wxDefaultValidator,
			const wxString& name = wxTreeCtrlNameStr)
		: wxTreeCtrl(parent, id, pos, size, style, validator, name)
	{
	}
	int GetItemState(const wxTreeItemId& item)
	{
#ifdef WX_TREE_HAS_STATE
	#if wxCHECK_VERSION(3, 0, 0)
		return wxTreeCtrl::DoGetItemState(item);
	#else
		return wxTreeCtrl::GetState(item);
	#endif
#else
		return wxTreeCtrl::GetItemImage(item);
#endif
	}
	void SetItemState(const wxTreeItemId& item, int state)
	{
#ifdef WX_TREE_HAS_STATE
	#if wxCHECK_VERSION(3, 0, 0)
		wxTreeCtrl::DoSetItemState(item, state);
	#else
		wxTreeCtrl::SetState(item, state);
	#endif
#else
		wxTreeCtrl::SetItemImage(item, state);
#endif
	}
};


class CTextCtrl : public wxTextCtrl
{
	DECLARE_CLASS(CTextCtrl)
public:
	CTextCtrl()
		: wxTextCtrl()
		, m_bAllowMultilineTabstop(false)
	{
	}
	CTextCtrl(
			wxWindow *parent,
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

	bool AllowMultilineTabstop() const		{return m_bAllowMultilineTabstop;}
	void AllowMultilineTabstop(bool bAllow)	{m_bAllowMultilineTabstop = bAllow;}

	virtual bool AcceptsFocusFromKeyboard() const
	{
		return (IsEditable() || (IsMultiLine() && m_bAllowMultilineTabstop))
			&& wxControl::AcceptsFocus();
	}

private:
	bool m_bAllowMultilineTabstop;
};
