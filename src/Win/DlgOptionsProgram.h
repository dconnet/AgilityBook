#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2004-08-31 Added option to disable splash screen.
 * 2004-07-23 Auto-check the config version on document open.
 * 2004-04-08 Created
 */

namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgOptionsProgram : public wxPanel
{
public:
	CDlgOptionsProgram(wxWindow* pParent);

	void Save();

private:
	bool m_bAutoCheck;
	int m_Backups;
	bool m_bAutoShow;
	bool m_bShowCosanction;
	bool m_UseProxy;
	wxString m_Proxy;
	ARBWin::CTextCtrl* m_ctrlProxy;
	bool m_enableDarkMode;

	void OnUseProxy(wxCommandEvent& evt);
};

} // namespace dconSoft
