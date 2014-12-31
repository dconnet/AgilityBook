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
 * 2009-02-09 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgMessage.h"

#include "Widgets.h"

#include "ARBCommon/StringUtil.h"

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
		wxDefaultPosition, wxSize(450, 300),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDC_MESSAGE_CLOSE"));
	btnClose->SetDefault();

	// Sizers
#pragma PRAGMA_TODO(convert to dialog units)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 1, wxALL|wxEXPAND, 5);
	bSizer->Add(btnClose, 0, wxALIGN_RIGHT|wxALL, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
}
