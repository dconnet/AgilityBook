/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include <wx/cmdline.h>
#include <wx/config.h>


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
	SetAppName(wxT("Agility Record Book"));
	wxConfig::Set(new wxConfig(wxT("Agility Record Book"), wxT("dcon Software")));

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
#if wxCHECK_VERSION(2, 9, 0)
		{wxCMD_LINE_SWITCH, "t", "tmp",  "Temp file, delete when done"},
		{wxCMD_LINE_OPTION, "f", "file", "Downloaded file"},
#else
		{wxCMD_LINE_SWITCH, wxT("t"), wxT("tmp"),  wxT("Temp file, delete when done")},
		{wxCMD_LINE_OPTION, wxT("f"), wxT("file"), wxT("Downloaded file")},
#endif
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
		return false;

//   - [arbupdate] extract from zip
//   - [arbupdate] delete zip
//   - [arbupdate] run "<name>.msi /qb" (or /qr, need to experiment)
//   - [arbupdate] run 'arb.exe'
//   - [arbupdate] exit

	return false;
}
