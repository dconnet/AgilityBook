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

#include "../Win/SetupApp.h"
#include "../Win/SetupAppARB.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CARBUpdaterApp : public CBaseApp
{
	DECLARE_NO_COPY_CLASS(CARBUpdaterApp)
public:
	CARBUpdaterApp();
	virtual bool InitLocale()	{return true;}
	virtual bool OnInit();
};


IMPLEMENT_APP(CARBUpdaterApp)


CARBUpdaterApp::CARBUpdaterApp()
	: CBaseApp(ARB_CONFIG_ENTRY)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CARBUpdaterApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{wxCMD_LINE_SWITCH, "g", "generate",
			"Ignore all other options and display a dialog allowing MD5 generation"},
		{wxCMD_LINE_OPTION, "f", "file",
			"Downloaded file"},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
		BaseAppCleanup();
		return false;
	}

	wxString file;
	bool bHasFile = cmdline.Found(L"file", &file);
	if (bHasFile)
	{
		if (file.length() > 1 && file[0] == '"')
		{
			file = file.SubString(1, file.length()-2);
		}
		file.Trim(true);
	}

	if (cmdline.Found(L"generate") || !bHasFile)
	{
		CDlgDigest dlg(file);
		dlg.ShowModal();
	}
	else if (bHasFile)
	{
		SHELLEXECUTEINFO info;
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(info);
		info.fMask = SEE_MASK_NOCLOSEPROCESS;
		info.lpVerb = _T("open");
		info.lpFile = file.wx_str();
		info.lpParameters = _T("/qb START_APP=1");
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
			wxMessageBox(wxString::Format(L"\"%s\" failed: %08x", file.wx_str(), x));
		}
		// In theory, there could be timing issue here - if ARB starts really
		// quickly, it could try deleting the file before we exit. ARB will
		// handle that by sleeping for a moment and trying again.
	}

	BaseAppCleanup();
	return false;
}
