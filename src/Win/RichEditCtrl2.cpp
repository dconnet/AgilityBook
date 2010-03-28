/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Autodetect URLs and open a window when clicked.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-19 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-30 DRC Created
 */

#include "stdafx.h"
#include "RichEditCtrl2.h"


CRichEditCtrl2::CRichEditCtrl2(
		wxWindow* parent,
		wxWindowID id,
		const wxString& value,
		const wxPoint& pos,
		const wxSize& size,
		bool bReadOnly)
	: CTextCtrl(parent, id, value, pos, size,
		wxTE_AUTO_URL|wxTE_MULTILINE|wxTE_RICH | (bReadOnly ? wxTE_READONLY : 0))
{
	Connect(wxEVT_COMMAND_TEXT_URL, wxTextUrlEventHandler(CRichEditCtrl2::OnUrl));
	if (bReadOnly)
	{
		SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
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
	wxString url = GetRange(evt.GetURLStart(), evt.GetURLEnd());
	wxLaunchDefaultBrowser(url);
}
