/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgPageEncode class
 * @author David Connet
 *
 * Revision History
 * @li 2009-08-26 DRC Fixed streaming wxString to otstringstream.
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2008-01-12 DRC Dump reg data in .reg format
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageEncode.h"

#include "../ARB/VersionNum.h"
#include "ARBHelp.h"
#include "ARBTypes.h"
#include "DlgARBHelp.h"
#include <set>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>


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
		wxDir::GetAllFiles(inFullPath, &files, wxT("*.arb*"), wxDIR_DIRS|wxDIR_FILES);
		for (size_t n = 0; n < files.GetCount(); ++n)
			pParent->SetARBFileStatus(files[n]);
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgPageEncode::CDlgPageEncode(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
{
	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		wxT("When Agility Record Book has a problem, this program helps by gathering information that may be useful in determining what when wrong.\n"),
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(600);
	bSizer->Add(text1, 0, wxALL|wxEXPAND, 5);

	wxStaticText* text2 = new wxStaticText(this, wxID_ANY,
		wxT("In addition to any ARB files you select in the following dialog, basic system information from the registry will be included."),
		wxDefaultPosition, wxDefaultSize, 0);
	text2->Wrap(600);
	bSizer->Add(text2, 0, wxALL, 5);

	SetSizer(bSizer);
	Layout();
}


void CDlgPageEncode::DumpGroup(
		wxString* data,
		wxString const& group,
		std::vector<wxString>* items)
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
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" unknown\n");
				break;
			case wxConfigBase::Type_String:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" string\n");
					*data << wxConfig::Get()->Read(str, wxEmptyString) << wxT("\n");
				}
				else if (items)
					items->push_back(wxConfig::Get()->Read(str, wxEmptyString));
				break;
			case wxConfigBase::Type_Boolean:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" bool\n");
					bool b;
					wxConfig::Get()->Read(str, &b);
					*data << b << wxT("\n");
				}
				break;
			case wxConfigBase::Type_Integer:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" int\n");
					*data << wxConfig::Get()->Read(str, 0L) << wxT("\n");
				}
				break;
			case wxConfigBase::Type_Float:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" float\n");
					double d;
					wxConfig::Get()->Read(str, &d);
					*data << d << wxT("\n");
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
		wxConfig::Get()->SetPath(wxT(".."));
}


bool CDlgPageEncode::TransferDataFromWindow()
{
	wxBusyCursor wait;

	// Get system information
	{
		wxString str;

		// OS version
		wxPlatformInfo info;
		str << wxT("OS: ")
			<< info.GetOperatingSystemIdName()
			<< ' '
			<< info.GetOSMajorVersion()
			<< '.'
			<< info.GetOSMinorVersion()
			<< wxT("\n");
		if (wxPORT_BASE != info.GetPortId())
		{
			str << wxT("Port: ")
				<< info.GetPortIdName()
				<< ' '
				<< info.GetToolkitMajorVersion()
				<< '.'
				<< info.GetToolkitMinorVersion()
				<< wxT("\n");
		}
		str << wxT("Architecture: ")
			<< info.GetArchName()
			<< wxT(", ")
			<< info.GetEndiannessName()
			<< wxT("\n");

		// Me.
		{
			CVersionNum ver(true);
			str << wxStandardPaths::Get().GetExecutablePath() << wxT(": ");
			if (ver.Valid())
				str << ver.GetVersionString();
			else
				str << _("IDS_BAD_VERSION");
			str << wxT("\n");
		}

		// wxWidgets
		str << wxVERSION_STRING << wxT("\n");

		m_Parent->AddSysInfo(str);
	}

	wxString data;
	DumpGroup(&data, wxEmptyString, NULL);
	m_Parent->AddRegistryInfo(data);

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
	std::vector<wxString> items;
	DumpGroup(NULL, wxT("Recent File List"), &items);
	for (std::vector<wxString>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		wxString path = *iter;
		if (path.empty())
			continue;
		wxFileName name(path);
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