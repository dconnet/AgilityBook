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

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBDebug.h"
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// Recurse directory
static void SearchFor(CDlgARBHelp* pParent, wxString const& inFullPath)
{
	wxArrayString files;
	if (wxDir::Exists(inFullPath))
	{
		wxDir::GetAllFiles(inFullPath, &files, L"*.arb*", wxDIR_DIRS | wxDIR_FILES);
		for (size_t n = 0; n < files.GetCount(); ++n)
			pParent->SetARBFileStatus(StringUtil::stringW(files[n]));
	}
}

/////////////////////////////////////////////////////////////////////////////

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
		wxSize(-1, wxDLG_UNIT_Y(this, 120)),
		0);
	text2->Wrap(wxDLG_UNIT_X(this, 345));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxEXPAND, wxDLG_UNIT_X(this, 5));
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
	// exe
	directories.insert(wxStandardPaths::Get().GetDataDir());
	// C:\Documents and Settings\username\Documents
	directories.insert(wxStandardPaths::Get().GetDocumentsDir());
	// C:\Documents and Settings\All Users\Application Data
	directories.insert(wxStandardPaths::Get().GetConfigDir());
	// C:\Documents and Settings\username\Application Data
	directories.insert(wxStandardPaths::Get().GetUserConfigDir());
	// C:\Documents and Settings\username\Local Settings\Application Data\appname
	directories.insert(wxStandardPaths::Get().GetUserLocalDataDir());
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
		SearchFor(m_Parent, *i);
	}

	return true;
}
