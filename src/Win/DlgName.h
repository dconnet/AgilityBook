#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgName class
 * @author David Connet
 *
 * Revision History
 * 2012-02-24 Changed GetName to Name (GetName is wxDialog api)
 * 2009-02-11 Ported to wxWidgets.
 */

#include "LibARBWin/Logger.h"


namespace dconSoft
{


class CDlgName : public wxDialog
{
public:
	CDlgName(std::wstring const& name, wxWindow* pParent = nullptr);
	CDlgName(std::wstring const& name, wxString const& caption, wxWindow* pParent = nullptr);

	std::wstring Name() const;

private:
	bool Create(wxString const& caption, wxWindow* pParent = nullptr);

	wxString m_Name;
	ARBWin::CStackLogger m_trace;
};

} // namespace dconSoft
