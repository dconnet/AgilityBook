/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Web authentication
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2007-08-03 DRC Created
 */

#include "stdafx.h"
#include "DlgAuthenticate.h"

#include "Widgets.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/valgen.h>


CDlgAuthenticate::CDlgAuthenticate(
		wxString const& userName,
		wxWindow* parent,
		wxString const& caption,
		wxString const& message)
	: wxDialog(parent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize)
{
	// Controls (these are done first to control tab order)

	wxStaticText* textMsg = NULL;
	if (!message.IsEmpty())
	{
		textMsg = new wxStaticText(this, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, 0);
		textMsg->Wrap(-1);
	}

	wxStaticText* textUserName = new wxStaticText(this, wxID_ANY, _("User Name"),
		wxDefaultPosition, wxDefaultSize, 0);
	textUserName->Wrap(-1);

	CTextCtrl* ctrlUsername = new CTextCtrl(this, wxID_ANY, userName,
		wxDefaultPosition, wxSize(180, -1), 0,
		wxGenericValidator(&m_Name));

	wxStaticText* textPassword = new wxStaticText(this, wxID_ANY, _("Password"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPassword->Wrap(-1);

	CTextCtrl* ctrlPassword = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(180, -1), wxTE_PASSWORD,
		wxGenericValidator(&m_Password));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	if (textMsg)
	{
		sizer->Add(textMsg, 0, wxALL|wxEXPAND, 5);
		sizer->Add(0, 10, 0, 0, 0);
	}

	wxBoxSizer* sizerRows = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerUserName = new wxBoxSizer(wxHORIZONTAL);
	sizerUserName->Add(textUserName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerUserName->Add(ctrlUsername, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerRows->Add(sizerUserName, 1, wxEXPAND, 5);

	wxBoxSizer* sizerPassword = new wxBoxSizer(wxHORIZONTAL);
	sizerPassword->Add(textPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPassword->Add(ctrlPassword, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerRows->Add(sizerPassword, 1, wxEXPAND, 5);

	sizer->Add(sizerRows, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	sizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(sizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	if (userName.empty())
		ctrlUsername->SetFocus();
	else
		ctrlPassword->SetFocus();
}
