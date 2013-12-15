#pragma once

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
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Created
 */

class CDlgAuthenticate : public wxDialog
{
public:
	CDlgAuthenticate(
			std::wstring const& userName,
			wxWindow* parent,
			std::wstring caption = std::wstring(),
			std::wstring message = std::wstring());

	std::wstring GetUserName() const;
	std::wstring GetPassword() const;

private:
	wxString m_Name;
	wxString m_Password;

	DECLARE_ON_INIT()
};
