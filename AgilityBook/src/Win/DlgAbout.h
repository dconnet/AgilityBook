#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgAbout class
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Ported to wxWidgets.
 * @li 2006-12-10 DRC Simplified download links.
 * @li 2005-02-08 DRC Added another link.
 * @li 2004-07-20 DRC Added a note for updating information.
 * @li 2003-09-17 DRC Added a 'check for updates' control.
 */

#include <wx/hyperlink.h>
class CAgilityBookDoc;


class CDlgAbout : public wxDialog
{
public:
	CDlgAbout(CAgilityBookDoc* pDoc, wxWindow* parent);

private:
	void OnHelpEmail(wxHyperlinkEvent& evt);
	void OnCheckForUpdates(wxCommandEvent& evt);

	CAgilityBookDoc* m_pDoc;
};
