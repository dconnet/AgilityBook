/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgMessage class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgMessage.h"

#include "Widgets.h"


CDlgMessage::CDlgMessage(
		wxString const& msg,
		wxWindow* pParent,
		wxString const& caption)
	: wxDialog(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	// Controls (these are done first to control tab order)

	CTextCtrl* textCtrl = new CTextCtrl(this, wxID_ANY, msg,
		wxDefaultPosition, wxSize(450, 300),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDD_MESSAGE_CLOSE"));
	btnClose->SetDefault();

	// Sizers (sizer creation is in same order as wxFormBuilder)

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
