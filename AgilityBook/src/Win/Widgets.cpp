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
 * Revision History
 * 2015-05-19 Add ComputeHeightHint to CTreeCtrl.
 * 2014-11-03 Enable spellchecking on richedit on Win8+.
 * 2009-10-11 Created.
 */

#include "stdafx.h"
#include "Widgets.h"

#include "Globals.h"

#ifdef WIN32
#include <richedit.h>
#ifndef IMF_SPELLCHECKING
#define IMF_SPELLCHECKING 0x0800
#endif
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CListCtrl, wxListView)
IMPLEMENT_CLASS(CTreeCtrl, wxTreeCtrl)
IMPLEMENT_CLASS(CTextCtrl, wxTextCtrl)
IMPLEMENT_CLASS(CSpellCheckCtrl, CTextCtrl)


static bool ComputeTreeHeight(
		wxTreeCtrl* tree,
		wxTreeItemId item,
		int& height)
{
	if (!item.IsOk())
		return false;

	bool bChanged = false;
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = tree->GetFirstChild(item, cookie);
	while (hChildItem)
	{
		wxRect r;
		if (tree->GetBoundingRect(hChildItem, r))
		{
			bChanged = true;
			height += r.height;
			ComputeTreeHeight(tree, hChildItem, height);
		}
		hChildItem = tree->GetNextChild(item, cookie);
	}
	return bChanged;
}


bool CTreeCtrl::ComputeHeightHint()
{
	int height = 0;
	if (ComputeTreeHeight(this, GetRootItem(), height))
	{
		wxRect rClient = GetClientRect();
		if (height > rClient.height)
		{
			SetSizeHints(-1, height);
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CTextCtrl::EnableSpellChecking(bool bForceRtfAsPlainText)
{
	bool bChanged = false;
#ifdef WIN32
	//#define wxTE_SPELLCHECK     (0x0004 | wxTE_RICH2)
	// 4 & 8 look like they're available
	if (!HasFlag(wxTE_READONLY) && IsRich())
	{
		bChanged = true;
		// Note: This only works on Win8+. Thankfully, it has no effect
		// on lower versions, so we can safely just call it.
#if wxCHECK_VERSION(3, 0, 0)
		HWND hwnd = GetHWND();
#else
		HWND hwnd = (HWND)GetHWND();
#endif
		if (bForceRtfAsPlainText)
		{
			// The control must be empty to change the mode.
			wxString val = GetValue();
			if (!val.empty())
				SetValue(wxString());
			::SendMessage(hwnd, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
			if (!val.empty())
				SetValue(val);
		}
		::SendMessage(hwnd, EM_SETLANGOPTIONS, 0, IMF_SPELLCHECKING);
		LRESULT style = ::SendMessage(hwnd, EM_GETEDITSTYLE, 0, 0);
		::SendMessage(hwnd, EM_SETEDITSTYLE, 0, style | SES_USECTF | SES_CTFALLOWEMBED | SES_CTFALLOWSMARTTAG | SES_CTFALLOWPROOFING);
	}
#endif
	return bChanged;
}
