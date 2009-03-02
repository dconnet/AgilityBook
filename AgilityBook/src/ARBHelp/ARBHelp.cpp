/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-02-08 DRC Created
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "DlgPageDecode.h"
#include <wx/cmdline.h>
#include <wx/config.h>


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
	SetAppName(wxT("Agility Record Book"));
	wxConfig::Set(new wxConfig(wxT("Agility Record Book"), wxT("dcon Software")));

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{wxCMD_LINE_SWITCH, wxT("decode"), NULL, wxT("Decode ARBHelp data from clipboard")},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
		return false;

	if (cmdline.Found(wxT("decode")))
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
