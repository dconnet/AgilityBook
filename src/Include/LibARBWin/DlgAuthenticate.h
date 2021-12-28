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
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Created
 */

#include "LibwxARBWin.h"


class ARBWIN_API CDlgAuthenticate : public wxDialog
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
};
