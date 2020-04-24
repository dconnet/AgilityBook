/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Autodetect URLs and open a window when clicked.
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2014-11-04 OnUrl was being called on mouseover. Caused nasty multiple-open.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-19 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-30 Created
 */

#include "stdafx.h"
#include "LibARBWin/RichEditCtrl2.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CRichEditCtrl2::CRichEditCtrl2(
	wxWindow* parent,
	wxWindowID id,
	const wxString& value,
	const wxPoint& pos,
	const wxSize& size,
	bool bReadOnly)
	: CTextCtrl(
		parent,
		id,
		value,
		pos,
		size,
		wxTE_AUTO_URL | wxTE_MULTILINE | wxTE_RICH2 | (bReadOnly ? wxTE_READONLY : 0))
{
	Bind(wxEVT_COMMAND_TEXT_URL, &CRichEditCtrl2::OnUrl, this);
	if (bReadOnly)
	{
		SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	}
	else
	{
		EnableSpellChecking();
	}
}


void CRichEditCtrl2::SetEditable(bool editable)
{
	if (editable != IsEditable())
	{
		CTextCtrl::SetEditable(editable);
		if (editable)
			SetBackgroundColour(wxNullColour); // reset to default
		else
			SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	}
}


void CRichEditCtrl2::OnUrl(wxTextUrlEvent& evt)
{
	if (evt.GetMouseEvent().LeftDown())
	{
		wxString url = GetRange(evt.GetURLStart(), evt.GetURLEnd());
		wxLaunchDefaultBrowser(url);
	}
}
