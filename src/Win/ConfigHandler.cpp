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
 * 2013-01-30 Moved zip code into LibArchive.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-03 Ported to wxWidgets.
 * 2008-10-31 Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ResourceManager.h"
#include <sstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
using namespace ARB;
using namespace ARBWin;

CConfigHandler::CConfigHandler()
{
}


CConfigHandler::~CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	bool bOk = false;
	wxString errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());

	auto resMgr = CResourceManager::Get();
	assert(resMgr);
	std::stringstream data;
	if (resMgr->LoadFile(L"DefaultConfig.xml", data))
		bOk = tree->LoadXML(data, errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD() const
{
	auto resMgr = CResourceManager::Get();
	assert(resMgr);
	std::stringstream data;
	resMgr->LoadFile(L"AgilityRecordBook.dtd", data);
	return data.str();
}

} // namespace dconSoft
