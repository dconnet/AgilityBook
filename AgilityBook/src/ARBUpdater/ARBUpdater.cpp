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
#include "DlgDigest.h"

#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CARBUpdaterApp : public wxApp
{
	DECLARE_NO_COPY_CLASS(CARBUpdaterApp)
public:
	CARBUpdaterApp();
	virtual bool OnInit();
};


IMPLEMENT_APP(CARBUpdaterApp)


CARBUpdaterApp::CARBUpdaterApp()
{
}


bool CARBUpdaterApp::OnInit()
{
	SetAppName(L"Agility Record Book");
	wxConfig::Set(new wxConfig(L"Agility Record Book", L"dcon Software"));

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
#if wxCHECK_VERSION(2, 9, 5)
		{wxCMD_LINE_SWITCH, "g", "generate",
			"Ignore all other options and display a dialog allowing MD5 generation"},
		{wxCMD_LINE_OPTION, "f", "file",
			"Downloaded file"},
#else
		{wxCMD_LINE_SWITCH, L"g", L"generate",
			L"Ignore all other options and display a dialog allowing MD5 generation"},
		{wxCMD_LINE_OPTION, L"f", L"file",
			L"Downloaded file"},
#endif
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
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
	return false;
}
