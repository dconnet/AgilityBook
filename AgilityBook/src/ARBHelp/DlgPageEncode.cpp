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

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include <set>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


/////////////////////////////////////////////////////////////////////////////
// Dump registry information

// .reg format (http://support.microsoft.com/kb/310516)
//
// RegistryEditorVersion
// Blank line
// [RegistryPath1]
// "DataItemName1"=DataType1:DataValue1
// DataItemName2=DataType2:DataValue2
// Blank line
// [RegistryPath2]
// "DataItemName3"="DataType3:DataValue3"
// "DataItemName4"="DataValue4"
// Blank line
//
// Difference between "REGEDIT4" and "Windows Registry Editor Version 5.00"
// - v5 supports unicode
//
// DataType (REG_SZ assumed unless DataType specified):
//  REG_BINARY		hex
//  REG_DWORD		dword
//  REG_EXPAND_SZ	hex(2)
//  REG_MULTI_SZ	hex(7)
//
// examples:
// "SetupType"=dword:00000000
// "CmdLine"="setup -newsetup"
// "SystemPrefix"=hex:c5,0b,00,00,00,40,36,02
// ; "String1\0String2" [in v5 (unicode) form, don't know how they look in v4]
// "Test"=hex(7):53,00,74,00,72,00,69,00,6e,00,67,00,20,00,31,00,00,00,53,00,74,\
//   00,72,00,69,00,6e,00,67,00,20,00,32,00,00,00,00,00
// ; %TMP%
// "Expand"=hex(2):25,00,54,00,4d,00,50,00,25,00,00,00
//
// To delete an item (hyphen in front of key):
// [-RegistryPath2]
// To delete a value (hyphen after '=' ):
// "DataItemName4"=-


// Recurse directory
static void SearchFor(
		CDlgARBHelp* pParent,
		wxString const& inFullPath)
{
	wxArrayString files;
	if (wxDir::Exists(inFullPath))
	{
		wxDir::GetAllFiles(inFullPath, &files, L"*.arb*", wxDIR_DIRS|wxDIR_FILES);
		for (size_t n = 0; n < files.GetCount(); ++n)
			pParent->SetARBFileStatus(StringUtil::stringW(files[n]));
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgPageEncode::CDlgPageEncode(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
{
	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		L"When Agility Record Book has a problem, this program helps by gathering information that may be useful in determining what when wrong.",
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(600);

	wxStaticText* text2 = new wxStaticText(this, wxID_ANY,
		L"In addition to any ARB files you select in the following dialog, basic system information from the registry will be included.",
		wxDefaultPosition, wxDefaultSize, 0);
	text2->Wrap(600);

	// Sizers
#pragma PRAGMA_TODO(convert to dialog units)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(text2, 0, wxALL, 5);

	SetSizer(bSizer);
	Layout();
}


void CDlgPageEncode::DumpGroup(
		std::wostringstream* data,
		wxString const& group,
		std::vector<std::wstring>* items)
{
	if (!group.empty())
		wxConfig::Get()->SetPath(group);

	wxString str;
	long dummy;
	if (wxConfig::Get()->GetFirstEntry(str, dummy))
	{
		do
		{
			switch (wxConfig::Get()->GetEntryType(str))
			{
			default:
				if (data)
					*data << wxConfig::Get()->GetPath().wx_str() << L'/' << str.wx_str() << L" unknown\n";
				break;
			case wxConfigBase::Type_String:
				if (data)
				{
					*data << wxConfig::Get()->GetPath().wx_str() << L'/' << str.wx_str() << L" string\n";
					*data << wxConfig::Get()->Read(str, wxEmptyString).wx_str() << L"\n";
				}
				else if (items)
					items->push_back(StringUtil::stringW(wxConfig::Get()->Read(str, wxEmptyString)));
				break;
			case wxConfigBase::Type_Boolean:
				if (data)
				{
					*data << wxConfig::Get()->GetPath().wx_str() << L'/' << str.wx_str() << L" bool\n";
					bool b;
					wxConfig::Get()->Read(str, &b);
					*data << b << L"\n";
				}
				break;
			case wxConfigBase::Type_Integer:
				if (data)
				{
					*data << wxConfig::Get()->GetPath().wx_str() << L'/' << str.wx_str() << L" int\n";
					*data << wxConfig::Get()->Read(str, 0L) << L"\n";
				}
				break;
			case wxConfigBase::Type_Float:
				if (data)
				{
					*data << wxConfig::Get()->GetPath().wx_str() << L'/' << str.wx_str() << L" float\n";
					double d;
					wxConfig::Get()->Read(str, &d);
					*data << d << L"\n";
				}
				break;
			}
		} while (wxConfig::Get()->GetNextEntry(str, dummy));
	}

	if (wxConfig::Get()->GetFirstGroup(str, dummy))
	{
		do
		{
			DumpGroup(data, str, items);
		} while (wxConfig::Get()->GetNextGroup(str, dummy));
	}

	if (!group.empty())
		wxConfig::Get()->SetPath(L"..");
}


bool CDlgPageEncode::TransferDataFromWindow()
{
	wxBusyCursor wait;

	// Get system information
	{
		wxString str;

		// OS version
		str << GetOSInfo().c_str();

		// Me.
		{
			CVersionNum ver(true);
			str << wxStandardPaths::Get().GetExecutablePath()
#ifdef _WIN64
				<< L" (64-bit): ";
#else
				<< L" (32-bit): ";
#endif
			if (ver.Valid())
				str << ver.GetVersionString().c_str();
			else
				str << _("IDS_BAD_VERSION");
			str << L"\n";
		}

		// wxWidgets
		str << wxVERSION_STRING << L"\n";

		m_Parent->AddSysInfo(StringUtil::stringW(str));
	}

	std::wostringstream data;
	DumpGroup(&data, wxEmptyString, nullptr);
	m_Parent->AddRegistryInfo(data.str().c_str());

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
	DumpGroup(nullptr, L"Recent File List", &items);
	for (std::vector<std::wstring>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		std::wstring path = *iter;
		if (path.empty())
			continue;
		wxFileName name(path.c_str());
		bool bFound = false;
		if (!wxFileName::IsCaseSensitive())
		{
			for (std::set<wxString>::iterator iDir = directories.begin();
					!bFound && iDir != directories.end();
					++iDir)
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
