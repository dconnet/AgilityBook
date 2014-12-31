/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Web authentication
 * @author David Connet
 *
 * Revision History
 * 2012-02-16 Fix initial focus.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Created
 */

#include "stdafx.h"
#include "DlgAuthenticate.h"

#include "Widgets.h"

#include "ARBCommon/StringUtil.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/valgen.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CDlgAuthenticate::CDlgAuthenticate(
		std::wstring const& userName,
		wxWindow* parent,
		std::wstring caption,
		std::wstring message)
	: wxDialog()
	, m_Name(userName.c_str())
{
	if (caption.empty())
		caption = StringUtil::stringW(_("Authentication"));
	Create(parent, wxID_ANY, caption.c_str(), wxDefaultPosition, wxDefaultSize);
	// Controls (these are done first to control tab order)

	wxStaticText* textMsg = nullptr;
	if (!message.empty())
	{
		textMsg = new wxStaticText(this, wxID_ANY, message.c_str(), wxDefaultPosition, wxDefaultSize, 0);
		textMsg->Wrap(-1);
	}

	wxStaticText* textUserName = new wxStaticText(this, wxID_ANY, _("User Name"),
		wxDefaultPosition, wxDefaultSize, 0);
	textUserName->Wrap(-1);

	CTextCtrl* ctrlUsername = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 100), -1), 0,
		wxGenericValidator(&m_Name));

	wxStaticText* textPassword = new wxStaticText(this, wxID_ANY, _("Password"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPassword->Wrap(-1);

	CTextCtrl* ctrlPassword = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 100), -1), wxTE_PASSWORD,
		wxGenericValidator(&m_Password));

	// Sizers

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	if (textMsg)
		sizer->Add(textMsg, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerRows = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerUserName = new wxBoxSizer(wxHORIZONTAL);
	sizerUserName->Add(textUserName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerUserName->Add(ctrlUsername, 1, wxALIGN_CENTER_VERTICAL, 0);

	sizerRows->Add(sizerUserName, 1, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerPassword = new wxBoxSizer(wxHORIZONTAL);
	sizerPassword->Add(textPassword, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPassword->Add(ctrlPassword, 1, wxALIGN_CENTER_VERTICAL, 0);

	sizerRows->Add(sizerPassword, 1, wxEXPAND, 0);

	sizer->Add(sizerRows, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	sizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(sizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	if (userName.empty())
		IMPLEMENT_ON_INIT(CDlgAuthenticate, ctrlUsername)
	else
		IMPLEMENT_ON_INIT(CDlgAuthenticate, ctrlPassword)
}


DEFINE_ON_INIT(CDlgAuthenticate)


std::wstring CDlgAuthenticate::GetUserName() const
{
	return StringUtil::stringW(m_Name);
}


std::wstring CDlgAuthenticate::GetPassword() const
{
	return StringUtil::stringW(m_Password);
}
