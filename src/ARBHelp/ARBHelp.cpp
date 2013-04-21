/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-02-08 DRC Created
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "DlgPageDecode.h"

#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CARBHelpApp : public wxApp
{
	DECLARE_NO_COPY_CLASS(CARBHelpApp)
public:
	CARBHelpApp();
	virtual bool OnInit();
};


IMPLEMENT_APP(CARBHelpApp)


CARBHelpApp::CARBHelpApp()
{
}


bool CARBHelpApp::OnInit()
{
	SetAppName(L"Agility Record Book");
	wxConfig::Set(new wxConfig(L"Agility Record Book", L"dcon Software"));

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
#if wxCHECK_VERSION(2, 9, 5)
		{wxCMD_LINE_SWITCH, "decode", NULL, "Decode ARBHelp data from clipboard"},
#else
		{wxCMD_LINE_SWITCH, L"decode", NULL, L"Decode ARBHelp data from clipboard"},
#endif
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
		return false;

	if (cmdline.Found(L"decode"))
	{
		CDlgPageDecode* dlg = new CDlgPageDecode();
		dlg->Show();
	}
	else
	{
		new CDlgARBHelp();
	}

	return true;
}
