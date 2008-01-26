/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
 * @brief Main access (in non-mfc dll, we'd have the DllMain here)
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"
#include "Local.h"

#include "ARBConfig.h"
#include "ARBStructure.h"
#include "Element.h"

#pragma comment(lib, "WinUnitLib.lib")


// In theory, this stuff should only be done once - but the framework doesn't
// support that, so just provide global functions so it can be implemented in
// one place. In time, when this is added to WinUnit, we can just make these
// empty functions - unless there really is something we want to do across all
// fixture tests.

bool CommonSetup()
{
	static CLocalization localization;
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		IARBLocalization::Init(&localization);
	}
	tstring errs;
	return Element::Initialize(errs);
}


bool CommonTeardown()
{
	Element::Terminate();
	return true;
}


ElementNodePtr LoadXMLData(UINT id)
{
	tstring errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(id), _T("XML"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			if (!tree->LoadXMLBuffer(pData, size, errMsg))
			{
				WIN_TRACE(errMsg.c_str());
				tree.reset();
			}
			FreeResource(hRes);
		}
	}
	WIN_ASSERT_TRUE(tree);
	return tree;
}


bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config)
{
	WIN_ASSERT_TRUE(tree);
	WIN_ASSERT_STRING_EQUAL(_T("DefaultConfig"), tree->GetName().c_str());
	ARBVersion version;
	tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
	int idx = tree->FindElement(TREE_CONFIG);
	WIN_ASSERT_TRUE(0 <= idx);
	tstring errMsg;
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
	tstring errmsg;
	bool bParse = actions->LoadXMLBuffer(configData, strlen(configData), errmsg);
	if (!bParse)
	{
		WIN_TRACE(errmsg.c_str());
	}
	WIN_ASSERT_TRUE(bParse);
	return actions;
}
