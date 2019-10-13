/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @author David Connet
 *
 * Revision History
 * 2019-10-13 Separated ARB specific things from TestLib.
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ConfigHandler.h"
#include "TestARB.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/ARBUtils.h"
#include "fmt/printf.h"

#if defined(__WXWINDOWS__)
#include <wx/app.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("ARB")
{
	SECTION("ARBConfigFiles")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			ARBVersion version;
			config.Default(&handler, &version);
			REQUIRE(version.IsSet());
			REQUIRE(version == ARBAgilityRecordBook::GetCurrentDocVersion());
		}
	}
}


ElementNodePtr LoadXMLData(size_t id)
{
	fmt::wmemory_buffer errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
	assert(tree);
#if defined(__WXWINDOWS__)
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = wxString(GetARBResourceDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat").wx_str();
#else
#pragma PRAGMA_TODO(write LoadXMLData)
#ifdef WIN32
	wchar_t fileName[MAX_PATH];
	GetModuleFileNameW(nullptr, fileName, _countof(fileName));
	std::wstring datafile(fileName);
	size_t n = datafile.find_last_of('.');
	datafile = datafile.substr(0, n);
	datafile += L".dat";
#else
	std::wstring datafile = L"./testarb.dat";
#endif
#endif
	assert(id < gc_NumConfigs);
	std::stringstream data;
	bool bOk = CConfigHandler::LoadWxFile(datafile, gc_Configs[id], data);
	assert(bOk);
	if (!bOk || !tree->LoadXML(data, errMsg))
	{
		fmt::print(std::wcout, L"{}\n", fmt::to_string(errMsg));
		tree.reset();
	}
	return tree;
}


bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config)
{
	assert(tree);
	if (!tree)
		return false;
	assert(tree->GetName() == L"DefaultConfig");
	if (tree->GetName() != L"DefaultConfig")
		return false;
	ARBVersion version;
	tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
	int idx = tree->FindElement(TREE_CONFIG);
	assert(0 <= idx);
	fmt::wmemory_buffer errMsg;
	ARBErrorCallback err(errMsg);
	return config.Load(tree->GetElementNode(idx), version, err);
}


ElementNodePtr CreateActionList()
{
	static char const* const configData =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<RootNode>\n\
<Action NewName=\"SSA\" OldName=\"Sweepstakes\" Verb=\"RenameVenue\" />\n\
<Action NewName=\"MXF\" OldName=\"MF\" Venue=\"AKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"ATCH\" OldName=\"V-ATCH\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"ATCH-OP\" OldName=\"V-ATCH-OP\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"ATCH-SP\" OldName=\"V-ATCH-SP\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"Regular\" OldName=\"Standard\" Venue=\"CPE\" Verb=\"RenameDivision\" />\n\
<Action NewName=\"Regular\" OldName=\"Junior\" Venue=\"CPE\" Verb=\"RenameDivision\" />\n\
<Action NewName=\"UAGI\" OldName=\"UAgI\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"UAGII\" OldName=\"UAgII\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"UAGIII\" OldName=\"UAgIII\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"ADCH\" OldName=\"ADCh\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD1\" OldName=\"PDI\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD2\" OldName=\"PDII\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD3\" OldName=\"PDIII\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PJ3\" OldName=\"PJ\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PG3\" OldName=\"PG\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PK3\" OldName=\"PS\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PJ3\" OldName=\"PJ\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Novice Triple Superior Performance Dog\" OldName=\"Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Novice\" OldName=\"Novice Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Open\" OldName=\"Open Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Elite\" OldName=\"Elite Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Triple\" OldName=\"Triple Triple Superior Performance\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"O-Vers-NATCH\" OldName=\"Outstanding Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"O-Vers-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"S-Vers-NATCH\" OldName=\"Superior Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"S-Vers-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Bronze Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Silver Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Gold Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NATCH\" OldName=\"O-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NATCH\" OldName=\"S-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Platinum Speed Star\" OldName=\"Platinum Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Platinum Versatility Speed Star\" OldName=\"Platinum Versatility Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Purple Achievement Cup\" OldName=\"Purple Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Purple Versatility Achievement Cup\" OldName=\"Purple Versatility Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action OldName=\"NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"CHC\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"CHS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Novice Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Open Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Elite Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"Chances\" OldName=\"Wild Card\" Venue=\"NADAC\" Verb=\"RenameEvent\" />\n\
<Action OldName=\"Championship\" Venue=\"NADAC\" Verb=\"DeleteEvent\" />\n\
</RootNode>";

	ElementNodePtr actions = ElementNode::New();
	fmt::wmemory_buffer errmsg;
	bool bParse = actions->LoadXML(configData, static_cast<unsigned int>(strlen(configData)), errmsg);
	if (!bParse)
	{
		fmt::print(std::wcout, L"{}\n", fmt::to_string(errmsg));
	}
	assert(bParse);
	return actions;
}
