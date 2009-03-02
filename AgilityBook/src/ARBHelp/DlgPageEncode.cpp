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
 * @brief implementation of the CDlgPageEncode class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2008-01-12 DRC Dump reg data in .reg format
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageEncode.h"

#include "..\Win\VersionNum.h"
#include "ARBHelp.h"
#include "ARBTypes.h"
#include "DlgARBHelp.h"
#include <set>
#include <sstream>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
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
		otstringstream* data,
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
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" unknown") << std::endl;
				break;
			case wxConfigBase::Type_String:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" string") << std::endl;
					*data << wxConfig::Get()->Read(str, wxEmptyString) << std::endl;
				}
				else if (items)
					items->push_back(wxConfig::Get()->Read(str, wxEmptyString));
				break;
			case wxConfigBase::Type_Boolean:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" bool") << std::endl;
					bool b;
					wxConfig::Get()->Read(str, &b);
					*data << b << std::endl;
				}
				break;
			case wxConfigBase::Type_Integer:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" int") << std::endl;
					*data << wxConfig::Get()->Read(str, 0L) << std::endl;
				}
				break;
			case wxConfigBase::Type_Float:
				if (data)
				{
					*data << wxConfig::Get()->GetPath() << '/' << str << wxT(" float") << std::endl;
					double d;
					wxConfig::Get()->Read(str, &d);
					*data << d << std::endl;
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

	wxString msg, tmp;

	// Get system information
	{
		otstringstream info;
#ifdef WIN32
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(os);
		GetVersionEx(&os);
		switch (os.dwPlatformId)
		{
		default: // Win32s
			info << _T("Windows32s ")
				<< os.dwMajorVersion
				<< '.'
				<< os.dwMinorVersion
				<< ' '
				<< os.szCSDVersion
				<< _T("\r\n");
			break;
		case VER_PLATFORM_WIN32_WINDOWS: // Win95/98
			if (0 == os.dwMinorVersion)
				info << _T("Windows 95 ")
					<< os.dwMajorVersion
					<< '.'
					<< os.dwMinorVersion
					<< '.'
					<< int(LOWORD(os.dwBuildNumber))
					<< ' '
					<< os.szCSDVersion
					<< _T("\r\n");
			else
				info << _T("Windows 98 ")
					<< os.dwMajorVersion
					<< '.'
					<< os.dwMinorVersion
					<< '.'
					<< int(LOWORD(os.dwBuildNumber))
					<< ' '
					<< os.szCSDVersion
					<< _T("\r\n");
			break;
		case VER_PLATFORM_WIN32_NT: // NT/Win2000
			info << _T("Windows ")
				<< os.dwMajorVersion
				<< '.'
				<< os.dwMinorVersion
				<< '.'
				<< os.dwBuildNumber
				<< ' '
				<< os.szCSDVersion
				<< _T("\r\n");
			break;
		}
#else
#error Please implement platform dependent identity code here
#endif

		// Me.
		{
			CVersionNum ver;
			info << wxStandardPaths::Get().GetExecutablePath().c_str() << wxT(": ");
			if (ver.Valid())
				info << ver.GetVersionString().c_str();
			else
			{
				wxString badVer = _("IDS_BAD_VERSION");
				info << badVer.c_str();
			}
			info << std::endl;
		}

		// wxWidgets
		info << wxVERSION_STRING << std::endl;

		m_Parent->AddSysInfo(info.str());
	}

	otstringstream data;
	DumpGroup(&data, wxEmptyString, NULL);
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
	std::vector<wxString> items;
	DumpGroup(NULL, wxT("Recent File List"), &items);
	for (std::vector<wxString>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		wxString path = *iter;
		if (path.empty())
			continue;
		wxFileName name(path);
#ifdef WIN32
		bool bFound = false;
		for (std::set<wxString>::iterator iDir = directories.begin(); iDir != directories.end(); ++iDir)
		{
			if (0 == name.GetPath().CmpNoCase(*iDir))
				bFound = true;
		}
		if (!bFound)
			directories.insert(name.GetPath());
#else
		directories.insert(name.GetPath());
#endif
	}

	for (std::set<wxString>::iterator i = directories.begin(); i != directories.end(); ++i)
	{
		if ((*i).empty())
			continue;
		SearchFor(m_Parent, *i);
	}

	return true;
}
