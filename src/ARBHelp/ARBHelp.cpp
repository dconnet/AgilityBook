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
#include "DlgARBHelp.h"

#include "DlgPageDecode.h"

#include "../Win/SetupAppARB.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/SetupApp.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CARBHelpApp : public CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CARBHelpApp)
public:
	CARBHelpApp();
	bool InitLanguage() override
	{
		return true;
	}
	bool OnInit() override;

	bool GetResImageName(
		wxArtID const& id,
		wxArtClient const& client,
		std::wstring& outName,
		bool& outSvg,
		bool& outCall) const override;
};


wxIMPLEMENT_APP(CARBHelpApp);


CARBHelpApp::CARBHelpApp()
	: CBaseApp(ARB_CONFIG_ENTRY)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CARBHelpApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	static const wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_SWITCH, "decode", nullptr, "Decode ARBHelp data from clipboard"},
		{wxCMD_LINE_NONE},
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
		BaseAppCleanup(true);
		return false;
	}

	wxDialog* dlg = nullptr;
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


bool CARBHelpApp::GetResImageName(
	wxArtID const& id,
	wxArtClient const& client,
	std::wstring& outName,
	bool& outSvg,
	bool& outCall) const
{
	outSvg = false;
	outCall = false;

	bool found = true;
	if (id == ImageMgrApp)
	{
		// No SVG for the icon yet
		//#if wxCHECK_VERSION(3, 1, 6)
		// outName = L"AgilityBook";
		// outSvg = true;
		//#else
		if (client == wxART_MESSAGE_BOX)
			outName = L"AgilityBook32";
		else
			outName = L"AgilityBook16";
		//#endif
	}

	else
		found = false;
	return found;
}
