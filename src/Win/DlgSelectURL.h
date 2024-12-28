#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgSelectURL class
 * @author David Connet
 *
 * Revision History
 * 2012-02-24 Changed GetName to Name (GetName is wxDialog api)
 * 2009-02-11 Ported to wxWidgets.
 * 2004-03-30 Created.
 */

#include "LibARBWin/Logger.h"


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgSelectURL : public wxDialog
{
public:
	CDlgSelectURL(wxString const& name, wxWindow* pParent = nullptr);
	CDlgSelectURL(wxString const& name, bool bAllowEmpty, wxWindow* pParent = nullptr);

	wxString Name() const;

private:
	void InitDlg(wxWindow* pParent, bool bAllowEmpty);
	void OnFilename(wxCommandEvent& evt);

	wxString m_Name;
	ARBWin::CTextCtrl* m_textCtrl;
	ARBWin::CStackLogger m_trace;
};

} // namespace dconSoft
