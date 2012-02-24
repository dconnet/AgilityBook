#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgName class
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-24 DRC Changed GetName to Name (GetName is wxDialog api)
 * @li 2009-02-11 DRC Ported to wxWidgets.
 */


class CDlgName : public wxDialog
{
public:
	CDlgName(
			wxString const& name,
			wxWindow* pParent = NULL)
		: m_Name(name)
	{
		Create(_("IDD_NAME"), pParent);
	}
	CDlgName(
			wxString const& name,
			wxString const& caption,
			wxWindow* pParent = NULL)
		: m_Name(name)
	{
		Create(caption, pParent);
	}

	bool Create(
			wxString const& caption,
			wxWindow* pParent = NULL);

	wxString Name() const	{return m_Name;}

private:
	wxString m_Name;

	DECLARE_ON_INIT()
};
