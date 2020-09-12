/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * 2013-10-30 Fixed a problem where msi was spawned hidden.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-01 Ported to wxWidgets.
 * 2007-02-08 Created
 */

#include "stdafx.h"
#include "CalSites.h"

#include "CalendarSites.h"

#include "../Win/ConfigHandler.h"
#include "../Win/SetupAppARB.h"
#include "ARB/ARBAgilityRecordBook.h"
#include "LibARBWin/SetupApp.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_APP(CCalSitesApp)


CCalSitesApp::CCalSitesApp()
	: CBaseApp(ARB_CONFIG_ENTRY, wxEmptyString, ARBLanguageCatalog::Embedded)
	, m_Localization()
	, m_imageHelper()
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CCalSitesApp::OnCreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size, wxBitmap& outBmp)
{
	return m_imageHelper.DoCreateBitmap(GetTopWindow(), id, client, size, outBmp);
}


bool CCalSitesApp::OnCreateIconBundle(const wxArtID& id, const wxArtClient& client, wxIconBundle& outIcon)
{
	return m_imageHelper.DoCreateIconBundle(GetTopWindow(), id, client, outIcon);
}


bool CCalSitesApp::InitLanguage()
{
	IARBLocalization::Init(&m_Localization);
	return CBaseApp::InitLanguage();
}


bool CCalSitesApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	wxImage::AddHandler(new wxPNGHandler);

	/*
	static const wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_SWITCH, "g", "generate", "Ignore all other options and display a dialog allowing MD5 generation"},
		{wxCMD_LINE_OPTION, "f", "file", "Downloaded file"},
		{wxCMD_LINE_NONE},
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
		BaseAppCleanup(true);
		return false;
	}
	*/

	// TODO: loading and saving of entries
	ARBAgilityRecordBook book;
	CConfigHandler handler;
	book.Default(&handler);

	CCalendarSites calSites;
	calSites.FindEntries(book);

	BaseAppCleanup(true);
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CErrorCallback::CErrorCallback()
	: ARBErrorCallback(m_ErrMsg)
{
}


bool CErrorCallback::OnError(wchar_t const* const pMsg)
{
	return wxYES == wxMessageBox(pMsg, wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_EXCLAMATION);
}
