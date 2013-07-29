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
 * @li 2013-01-30 DRC Moved zip code into LibArchive.
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-04 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibArchive/LibArchive.h"

#if defined(__WXWINDOWS__)
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CConfigHandler::LoadWxFile(
		std::wstring const& zipFile,
		std::wstring const& archiveFile,
		std::ostream& outData)
{
#if defined(__WXWINDOWS__)
	return ExtractFile(zipFile, StringUtil::stringWX(archiveFile), outData);
#else
	return ExtractFile(StringUtil::stringA(zipFile), StringUtil::stringA(archiveFile), outData);
#endif
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


std::string CConfigHandler::LoadDTD() const
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

	return data.str();
}
