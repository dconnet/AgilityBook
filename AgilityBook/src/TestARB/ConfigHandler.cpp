/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-04 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "ARB/Element.h"
#include "ARB/StringUtil.h"
#include <wx/filesys.h>
#include <wx/mstream.h>
#if defined(__WXWINDOWS__)
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CConfigHandler::LoadWxFile(
		std::wstring const& zipFile,
		std::wstring const& archiveFile,
		std::iostream& ioData)
{
	wxString zipfile = wxFileSystem::FileNameToURL(wxString(zipFile.c_str()));
	zipfile += L"#zip:" + StringUtil::stringWX(archiveFile);
	wxFileSystem filesys;
	wxFSFile* file = filesys.OpenFile(zipfile);
	if (file)
	{
		size_t size = 0;
		wxInputStream* input = file->GetStream();
		while (input->CanRead())
		{
			char buffer[1024];
			size_t num = 1024;
			input->Read(buffer, num);
			ioData.write(buffer, input->LastRead());
			size += input->LastRead();
		}
		delete file;
		return true;
	}
	else
	{
		wxLogMessage(zipfile);
		assert(file);
	}
	return false;
}


CConfigHandler::CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	bool bOk = false;
	std::wostringstream errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());

#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	std::wstring datafile = L"./testarb.dat";
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = StringUtil::stringW(wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat");
#endif
#else
	std::wstring datafile = L"./testarb.dat";
#endif
	std::stringstream data;
	if (LoadWxFile(datafile, L"DefaultConfig.xml", data))
		bOk = tree->LoadXML(data, errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD(bool bNormalizeCRNL) const
{
#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	std::wstring datafile = L"./testarb.dat";
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = StringUtil::stringW(wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat");
#endif
#else
	std::wstring datafile = L"./testarb.dat";
#endif

	std::stringstream data;
	LoadWxFile(datafile, L"AgilityRecordBook.dtd", data);
	std::string dtd(data.str());

	if (bNormalizeCRNL)
		dtd = StringUtil::Replace(dtd, "\r\n", "\n");
	return dtd;
}
