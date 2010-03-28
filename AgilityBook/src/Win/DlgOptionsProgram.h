#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2004-04-08 DRC Created
 */

class CTextCtrl;


class CDlgOptionsProgram : public wxPanel
{
public:
	CDlgOptionsProgram(wxWindow* pParent);

	void Save();
	bool ResetHtmlView() const	{return m_bResetHtmlView;}

private:
	bool m_bResetHtmlView;
	bool m_bAutoCheck;
	int m_Backups;
	bool m_bAutoShow;
	bool m_bShowHtml;
	bool m_UseProxy;
	wxString m_Proxy;
	CTextCtrl* m_ctrlProxy;

	void OnUseProxy(wxCommandEvent& evt);
};
