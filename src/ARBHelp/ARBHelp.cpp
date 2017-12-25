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
 * 2013-12-24 Fix modeless wizard. Now works correctly on Mac.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-01 Ported to wxWidgets.
 * 2007-02-08 Created
 */

#include "stdafx.h"
#include "Platform/SetupMessages.h"
#include "DlgARBHelp.h"

#include "DlgPageDecode.h"

#include "../Win/SetupApp.h"
#include "../Win/SetupAppARB.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CARBHelpApp : public CBaseApp
{
	DECLARE_NO_COPY_CLASS(CARBHelpApp)
public:
	CARBHelpApp();
	virtual bool InitLocale()	{return true;}
	virtual bool OnInit();
};


IMPLEMENT_APP(CARBHelpApp)


CARBHelpApp::CARBHelpApp()
	: CBaseApp(ARB_CONFIG_ENTRY)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CARBHelpApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	wxImage::AddHandler(new wxPNGHandler);

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{wxCMD_LINE_SWITCH, "decode", nullptr, "Decode ARBHelp data from clipboard"},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
		BaseAppCleanup();
		return false;
	}

	wxDialog* dlg;
	if (cmdline.Found(L"decode"))
	{
		dlg = new CDlgPageDecode();
	}
	else
	{
		CDlgARBHelp* wiz = new CDlgARBHelp();
		dlg = wiz;
		wiz->ShowPage(wiz->GetFirstPage());
	}
	dlg->Show();
	SetTopWindow(dlg);

	return true;
}
