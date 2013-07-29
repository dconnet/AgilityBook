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
 * @li 2009-01-03 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibArchive/LibArchive.h"
#include <wx/stdpaths.h>

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

	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat";
	std::stringstream data;
	if (LoadWxFile(StringUtil::stringW(datafile), L"DefaultConfig.xml", data))
		bOk = tree->LoadXML(data, errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD() const
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat";

	std::stringstream data;
	LoadWxFile(StringUtil::stringW(datafile), L"AgilityRecordBook.dtd", data);

	return data.str();
}
