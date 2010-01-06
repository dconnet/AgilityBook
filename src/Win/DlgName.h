#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgName class
 * @author David Connet
 *
 * Revision History
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

	wxString GetName() const	{return m_Name;}

private:
	wxString m_Name;
};
