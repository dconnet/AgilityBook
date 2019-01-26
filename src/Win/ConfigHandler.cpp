/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2013-01-30 Moved zip code into LibArchive.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-03 Ported to wxWidgets.
 * 2008-10-31 Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibArchive/LibArchive.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CConfigHandler::LoadWxFile(
		std::wstring const& zipFile,
		std::wstring const& archiveFile,
		std::ostream& outData)
{
	CLibArchive archive(zipFile);
	return archive.ExtractFile(archiveFile, outData);
}


CConfigHandler::CConfigHandler()
{
}


CConfigHandler::~CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	bool bOk = false;
	fmt::wmemory_buffer errMsg;
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
