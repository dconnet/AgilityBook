/*
 * Copyright © 2005-2009 David Connet. All Rights Reserved.
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
		bool bReadOnly,
		const wxValidator& validator)
	: wxTextCtrl(parent, id, value, pos, size,
	wxTE_AUTO_URL|wxTE_MULTILINE|wxTE_RICH | (bReadOnly ? wxTE_READONLY : 0),
		validator)
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
		wxTextCtrl::SetEditable(editable);
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
