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
#include "DlgDigest.h"

#include "../Win/SetupAppARB.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/SetupApp.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
using namespace ARBWin;

class CARBUpdaterApp : public CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CARBUpdaterApp)
public:
	CARBUpdaterApp();
	bool InitLanguage() override
	{
		return true;
	}
	bool OnInit() override;

	bool GetResImageName(wxArtID const& id, wxArtClient const& client, wxString& outName, bool& outSvg) const override;
};


wxIMPLEMENT_APP(CARBUpdaterApp);


CARBUpdaterApp::CARBUpdaterApp()
	: CBaseApp(ARB_CONFIG_ENTRY)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CARBUpdaterApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	constexpr wxCmdLineEntryDesc cmdLineDesc[] = {
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

	wxString file;
	bool bHasFile = cmdline.Found(L"file", &file);
	if (bHasFile)
	{
		if (file.length() > 1 && file[0] == '"')
		{
			file = file.SubString(1, file.length() - 2);
		}
		file.Trim(true);
	}

	if (cmdline.Found(L"generate") || !bHasFile)
	{
		CDlgDigest dlg(file);
		SetTopWindow(&dlg);
		dlg.ShowModal();
		SetTopWindow(nullptr);
	}
	else if (bHasFile)
	{
		SHELLEXECUTEINFO info;
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(info);
		info.fMask = SEE_MASK_NOCLOSEPROCESS;
		info.lpVerb = L"open";
		info.lpFile = file.wx_str();
		info.lpParameters = L"/qb START_APP=1";
		info.nShow = SW_SHOWNORMAL;
		if (ShellExecuteEx(&info))
		{
			WaitForSingleObject(info.hProcess, INFINITE);
			CloseHandle(info.hProcess);
			wxRemoveFile(file);
		}
		else
		{
			DWORD x = GetLastError();
			wxMessageBox(fmt::format(L"\"{}\" failed: {:08x}", file.wx_str(), x));
		}
		// In theory, there could be timing issue here - if ARB starts really
		// quickly, it could try deleting the file before we exit. ARB will
		// handle that by sleeping for a moment and trying again.
	}

	BaseAppCleanup(true);
	return false;
}


bool CARBUpdaterApp::GetResImageName(wxArtID const& id, wxArtClient const& client, wxString& outName, bool& outSvg)
	const
{
	outSvg = false;

	bool found = true;
	if (id == ImageMgrApp)
	{
		// No SVG for the icon yet
		// outName = L"AgilityBook";
		// outSvg = true;
		if (client == wxART_MESSAGE_BOX)
			outName = L"AgilityBook32";
		else
			outName = L"AgilityBook16";
	}

	else
		found = false;
	return found;
}

} // namespace dconSoft
