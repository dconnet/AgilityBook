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
			std::wstring const& name,
			wxWindow* pParent = NULL);
	CDlgName(
			std::wstring const& name,
			std::wstring const& caption,
			wxWindow* pParent = NULL);

	bool Create(
			std::wstring const& caption,
			wxWindow* pParent = NULL);

	std::wstring Name() const;

private:
	wxString m_Name;

	DECLARE_ON_INIT()
};
