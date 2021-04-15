#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgAbout class
 * @author David Connet
 *
 * Revision History
 * 2008-12-14 Ported to wxWidgets.
 * 2006-12-10 Simplified download links.
 * 2005-02-08 Added another link.
 * 2004-07-20 Added a note for updating information.
 * 2003-09-17 Added a 'check for updates' control.
 */

#include "ARBCommon/MailTo.h"
#include <wx/hyperlink.h>
class CAgilityBookDoc;
class CMainFrame;


class CDlgAbout : public wxDialog
{
public:
	CDlgAbout(CAgilityBookDoc* pDoc, CMainFrame* parent);

private:
	void OnHelpEmail(wxHyperlinkEvent& evt);
	void OnCheckForUpdates(wxCommandEvent& evt);

	CAgilityBookDoc* m_pDoc;
	CMainFrame* m_pFrame;
	CMailTo m_mailto;
};
