#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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

class CDlgAuthenticate : public wxDialog
{
public:
	CDlgAuthenticate(
			wxString const& userName,
			wxWindow* parent,
			wxString const& caption = _("IDD_AUTHENTICATE"),
			wxString const& message = wxEmptyString);

	wxString GetUserName() const	{return m_Name;}
	wxString GetPassword() const	{return m_Password;}

private:
	wxString m_Name;
	wxString m_Password;
};
