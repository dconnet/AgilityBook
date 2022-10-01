/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgPageEncode class
 * @author David Connet
 *
 * Revision History
 * 2022-10-01 Add log files to gathered files.
 * 2018-01-28 Move sysinfo/registry dump to ARBDebug.
 * 2012-07-10 Fix serialization. Broken in 4/15 wxString checkin.
 * 2011-10-19 Wrap '/' with wxT() so it's not output as int.
 * 2009-08-26 Fixed streaming wxString to otstringstream.
 * 2009-03-01 Ported to wxWidgets.
 * 2008-01-12 Dump reg data in .reg format
 * 2007-01-02 Created
 */

#include "stdafx.h"
#include "DlgPageEncode.h"

#include "ARBHelp.h"
#include "DlgARBHelp.h"
#include "VersionNumber.h"

#include "../Win/RegItems.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBDebug.h"
#include "LibARBWin/Logger.h"
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
// Recurse directory
void SearchFor(CDlgARBHelp* pParent, wxString const& inFullPath, wxString const& pattern)
{
	if (inFullPath.empty() || !wxDir::Exists(inFullPath))
		return;
	wxArrayString files;
	wxDir::GetAllFiles(inFullPath, &files, pattern, wxDIR_DIRS | wxDIR_FILES);
	for (size_t n = 0; n < files.GetCount(); ++n)
		pParent->SetARBFileStatus(StringUtil::stringW(files[n]));
}
} // namespace


CDlgPageEncode::CDlgPageEncode(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
	, m_DiskChoices(nullptr)
{
	wxStaticText* text1 = new wxStaticText(
		this,
		wxID_ANY,
		L"When Agility Record Book has a problem, this program helps by gathering information that may be useful in determining what when wrong.",
		wxDefaultPosition,
		wxDefaultSize,
		0);
	text1->Wrap(wxDLG_UNIT_X(this, 345));

	wxStaticText* text2 = new wxStaticText(
		this,
		wxID_ANY,
		L"In addition to any ARB files you select in the following dialog, basic system information from the registry will be included.",
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 120)), // To set initial page size sufficiently.
		0);
	text2->Wrap(wxDLG_UNIT_X(this, 345));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxEXPAND);
	bSizer->Add(text2, 0, wxTOP, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
}


bool CDlgPageEncode::TransferDataFromWindow()
{
	wxBusyCursor wait;

	// Get system information
	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	m_Parent->AddSysInfo(ARBDebug::GetSystemInfo(this, ver));

	// Add registry info
	m_Parent->AddRegistryInfo(ARBDebug::GetRegistryInfo().c_str());

	std::set<wxString> directories;

	// (comments are where files are on Windows 10)

	// exe
	directories.insert(wxStandardPaths::Get().GetDataDir());

	// C:\Users\<username>\Documents
	directories.insert(wxStandardPaths::Get().GetDocumentsDir());

	// C:\\ProgramData\\dcon Software\\Agility Record Book
	directories.insert(wxStandardPaths::Get().GetConfigDir());

	// C:\Users\<username>\AppData\Roaming
	directories.insert(wxStandardPaths::Get().GetUserConfigDir());

	// C:\Users\<username>\AppData\Local\dcon Software\Agility Record Book
	directories.insert(wxStandardPaths::Get().GetUserLocalDataDir());

	// CAgilityBookOptions::GetBackupDirectory());
	directories.insert(wxConfig::Get()->Read(CFG_SETTINGS_BACKUPDIR, wxString()));

	std::vector<std::wstring> items;
	ARBDebug::DumpRegistryGroup(L"Recent File List", nullptr, &items);
	for (std::vector<std::wstring>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		std::wstring path = *iter;
		if (path.empty())
			continue;
		wxFileName name(path.c_str());
		bool bFound = false;
		if (!wxFileName::IsCaseSensitive())
		{
			for (std::set<wxString>::iterator iDir = directories.begin(); !bFound && iDir != directories.end(); ++iDir)
			{
				if (0 == name.GetPath().CmpNoCase(*iDir))
					bFound = true;
			}
		}
		if (!bFound)
			directories.insert(name.GetPath());
	}

	for (std::set<wxString>::iterator i = directories.begin(); i != directories.end(); ++i)
	{
		if ((*i).empty())
			continue;
		SearchFor(m_Parent, *i, L"*.arb*");
	}

	// Gather logging files
	CLogger logger;
	logger.Initialize(wxGetApp().GetUpdateInfoKey().c_str());
	// Note: This is GetUserLocalDataDir(). 
	SearchFor(m_Parent, logger.GetCurrentLogDir(), L"*.log");

	return true;
}

} // namespace dconSoft
