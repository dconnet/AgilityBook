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

	wxWindow* GetResourceWindow() override;
	bool GetResImageName(
		wxArtID const& id,
		wxArtClient const& client,
		std::wstring& outName,
		bool& outSvg,
		bool& outCall) const override;
	wxBitmap GetResImage(wxArtID const& id, wxArtClient const& client) const override;
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

	wxImage::AddHandler(new wxPNGHandler);
	InitFSHandlers();

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


wxWindow* CARBHelpApp::GetResourceWindow()
{
	return wxGetApp().GetTopWindow();
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
		if (client == wxART_MESSAGE_BOX)
			outName = L"AgilityBook32";
		else
			outName = L"AgilityBook16";
	}
	else if (id == ImageMgrApp48)
		outName = L"AgilityBook48";
	else if (id == ImageMgrApp256)
		outName = L"AgilityBook256";
	else
		found = false;

#if defined(_DEBUG) || defined(__WXDEBUG__)
	assert(!outName.empty());
#endif
	return found;
}


wxBitmap CARBHelpApp::GetResImage(wxArtID const& id, wxArtClient const& client) const
{
	return wxBitmap();
}
