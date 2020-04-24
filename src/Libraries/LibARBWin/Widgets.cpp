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
 * 2018-10-11 Moved to Win LibARBWin
 * 2015-10-22 Fix setting spell checking flags - this allows ink input.
 * 2015-05-19 Add ComputeHeightHint to CTreeCtrl.
 * 2014-11-03 Enable spellchecking on richedit on Win8+.
 * 2009-10-11 Created.
 */

#include "stdafx.h"
#include "LibARBWin/Widgets.h"

#include "LibARBWin/ARBWinUtilities.h"

#ifdef WIN32
#include <richedit.h>
#ifndef IMF_SPELLCHECKING
#define IMF_SPELLCHECKING 0x0800
#endif
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


wxIMPLEMENT_CLASS(CListCtrl, wxListView)
wxIMPLEMENT_CLASS(CTreeCtrl, wxTreeCtrl)
wxIMPLEMENT_CLASS(CTextCtrl, wxTextCtrl)
wxIMPLEMENT_CLASS(CSpellCheckCtrl, CTextCtrl)


static bool ComputeTreeHeight(wxTreeCtrl* tree, wxTreeItemId item, int& height)
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
		HWND hwnd = GetHWND();

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

		LRESULT options = ::SendMessage(hwnd, EM_GETLANGOPTIONS, 0, 0);
		::SendMessage(hwnd, EM_SETLANGOPTIONS, 0, options | IMF_SPELLCHECKING);

		// wParam: flags, lParam: Mask of flags to set
		WPARAM flags = SES_USECTF | SES_CTFALLOWEMBED | SES_CTFALLOWSMARTTAG | SES_CTFALLOWPROOFING;
		::SendMessage(hwnd, EM_SETEDITSTYLE, flags, flags);
	}
#endif
	return bChanged;
}
