#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Change certain default wxWidget behaviors.
 * @author David Connet
 *
 * List Controls: Override DoGetBestSize.
 * Tree Controls: Override DoGetBestSize, add window support for states.
 * Text Controls: Turn off tabstops on multiline readonly controls.
 *
 * Revision History
 * @li 2010-03-28 DRC Fixed a problem with list sizing on Mac.
 * @li 2009-10-11 DRC Created.
 */

#include <wx/listctrl.h>
#include <wx/treectrl.h>


// When first starting up, the sizers just aren't sizing correctly on
// wx 2.9. On Windows, the tree control causes issues. On Mac, the list
// controls are. The behavior of each is slightly different. On Windows,
// the controls think they have much more real estate then they really do,
// so they size themselves huge. On Mac, it starts correctly, but as the
// window is sized larger than smaller, the control never shrinks.
#define WXWINDOW_FIX_INITIAL_SIZER \
	protected: virtual wxSize DoGetBestSize() const {return wxSize(1,1);}


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
#if !wxCHECK_VERSION(2, 9, 1) && defined(__WXMAC__)
		// On Mac, when I set a minimal size for lists, that list will often
		// be sized to 1 pixel. So fix it. This has been fixed in 2.9
		if (wxDefaultSize != size)
			SetMinSize(size);
#endif
	}
#if !wxCHECK_VERSION(2, 9, 1) && defined(__WXMAC__)
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
	WXWINDOW_FIX_INITIAL_SIZER
};


#ifdef WIN32
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
	#if wxCHECK_VERSION(2, 9, 0)
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
	#if wxCHECK_VERSION(2, 9, 0)
		wxTreeCtrl::DoSetItemState(item, state);
	#else
		wxTreeCtrl::SetState(item, state);
	#endif
#else
		wxTreeCtrl::SetItemImage(item, state);
#endif
	}
	WXWINDOW_FIX_INITIAL_SIZER
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
