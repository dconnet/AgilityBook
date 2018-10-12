/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgMessage class
 * @author David Connet
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2009-02-09 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgMessage.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgMessage::CDlgMessage(
		std::wstring const& msg,
		wxWindow* pParent,
		std::wstring caption)
	: wxDialog()
{
	if (caption.empty())
		caption = StringUtil::stringW(_("IDD_MESSAGE"));
	Create(pParent, wxID_ANY, caption.c_str(), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	// Controls (these are done first to control tab order)

	CTextCtrl* textCtrl = new CTextCtrl(this, wxID_ANY, msg.c_str(),
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(260, 160)),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDC_MESSAGE_CLOSE"));
	btnClose->SetDefault();

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(btnClose, 0, wxALIGN_RIGHT | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
}
