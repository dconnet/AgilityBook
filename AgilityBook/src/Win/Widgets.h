#pragma once

/*
 * Copyright (c) 2009 David Connet. All Rights Reserved.
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
 * @brief Change certain default wxWidget behaviors.
 * @author David Connet
 *
 * List Controls: Override DoGetBestSize.
 * Tree Controls: Override DoGetBestSize, add window support for states.
 * Text Controls: Turn off tabstops on multiline readonly controls.
 *
 * Revision History
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
	}
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
