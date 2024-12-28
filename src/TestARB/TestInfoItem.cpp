/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBInfoItem class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBInfo.h"
#include "ARB/ARBInfoItem.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

namespace
{
struct TestInfoItemData
{
	ElementNodePtr tree;
	ElementNodePtr data; // clubinfo
	TestInfoItemData();
};
} // namespace


TestInfoItemData::TestInfoItemData()
{
	tree = ElementNode::New(TREE_INFO);
	data = tree->AddElementNode(TREE_CLUBINFO);
	data->AddAttrib(ATTRIB_INFO_NAME, L"Club1");
	data->SetValue(L"Club 1 info");
	ElementNodePtr club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, L"Club2");
	club->SetValue(L"Club 2 info");
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, L"Club3");
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, L"Club4");
	ElementNodePtr judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, L"Judge One");
	judge->SetValue(L"Judge 1 info");
	judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, L"Judge Two");
	judge->SetValue(L"Judge 2 info");
	ElementNodePtr loc = tree->AddElementNode(TREE_LOCATIONINFO);
	loc->AddAttrib(ATTRIB_INFO_NAME, L"Sunnyvale");
}


TEST_CASE("InfoItem")
{
	TestInfoItemData data;

	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			REQUIRE(!!info.get());
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			info->SetName(L"Testing");
			ARBInfoItemPtr info2 = info->Clone();
			REQUIRE(!!info2.get());
			REQUIRE(info.get() != info2.get());
			REQUIRE(*info == *info2);
			info->SetName(L"Test2");
			REQUIRE(info->GetName() != info2->GetName());
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Testing");
			ARBInfoItemPtr info2 = ARBInfoItem::New();
			REQUIRE(*info1 != *info2);
			*info1 = *info2;
			REQUIRE(*info1 == *info2);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"A");
			ARBInfoItemPtr info2 = ARBInfoItem::New();
			info2->SetName(L"B");
			REQUIRE(*info1 < *info2);
			REQUIRE(!(*info1 > *info2));
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"A");
			REQUIRE(info1->GetGenericName() == L"A");
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info->Load(data.data, ARBVersion(1, 0), callback, TREE_CLUBINFO));
			wxString name = info->GetGenericName();
			REQUIRE(!name.empty());
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info->Load(data.data, ARBVersion(2, 0), callback, TREE_CLUBINFO));
			ElementNodePtr ele = ElementNode::New();
			REQUIRE(info->Save(ele, TREE_CLUBINFO));
		}
	}
}


TEST_CASE("InfoItemList")
{
	TestInfoItemData data;

	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(infolist.Load(data.data, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(L"Name", L"PASA");
			REQUIRE(!infolist.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(1u == infolist.size());
		}
	}


	SECTION("Load2")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"InfoItem");
			ele->SetValue(L"These are some notes");
			ARBInfoItemList infolist(TREE_CLUBINFO);
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(!infolist.Load(ele, ARBVersion(2, 0), callback));
		}
	}


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Test2");
			info1->SetComment(L"A note");
			ARBInfoItemPtr info2 = info1->Clone();
			info2->SetName(L"Test1");
			REQUIRE(infolist.AddItem(info1));
			REQUIRE(infolist.AddItem(info2));

			ARBInfoItemList infolist2(TREE_CLUBINFO);
			infolist.Clone(infolist2);
			REQUIRE(infolist == infolist2);
			infolist.sort();
			REQUIRE(infolist != infolist2);
		}
	}


	SECTION("GetAllItems")
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info.Load(data.tree, ARBVersion(1, 0), callback));
			std::set<wxString> items;
			REQUIRE(4u == info.GetInfo(ARBInfoType::Club).GetAllItems(items, false));
		}
	}


	SECTION("CondenseContent")
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info.Load(data.tree, ARBVersion(1, 0), callback));
			std::set<wxString> items;
			items.insert(L"Club1");
			items.insert(L"Club3");
			info.GetInfo(ARBInfoType::Club).CondenseContent(items);
			items.clear();
			REQUIRE(3u == info.GetInfo(ARBInfoType::Club).GetAllItems(items, false));
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info.Load(data.tree, ARBVersion(1, 0), callback));
			REQUIRE(info.GetInfo(ARBInfoType::Club).FindItem(L"Club3"));
			REQUIRE(!info.GetInfo(ARBInfoType::Club).FindItem(L"Club3xc"));
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Test2");
			info1->SetComment(L"A note");
			ARBInfoItemPtr info2 = info1->Clone();
			info2->SetName(L"Test1");
			REQUIRE(infolist.AddItem(info1));
			REQUIRE(infolist.AddItem(info2));
			infolist.sort();
			ARBInfoItemPtr info3 = infolist[0]->Clone();
			REQUIRE(*info2 == *info3);
			info3->SetComment(L"Test comments");
			REQUIRE(*info2 != *info3);
			REQUIRE(!infolist.AddItem(info3));
			REQUIRE(!infolist.AddItem(info1->Clone()));
			info3->SetName(L"Test3");
			REQUIRE(infolist.AddItem(info3));
			infolist.sort();
			REQUIRE(3u == infolist.size());
			REQUIRE(infolist.DeleteItem(info1));
			REQUIRE(2u == infolist.size());
			REQUIRE(!infolist.DeleteItem(info1));
			REQUIRE(2u == infolist.size());
			REQUIRE(*infolist[0] != *infolist[1]);
			ARBInfoItemList infolist2(TREE_CLUBINFO);
			infolist.Clone(infolist2);
			REQUIRE(infolist == infolist2);
			REQUIRE(infolist[0].get() != infolist2[0].get());
			REQUIRE(*infolist[0] == *infolist2[0]);
		}
	}
}


TEST_CASE("Info")
{
	TestInfoItemData data;

	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			wxString errs;
			ARBErrorCallback callback(errs);
			REQUIRE(info.Load(data.tree, ARBVersion(1, 0), callback));
			REQUIRE(4u == info.GetInfo(ARBInfoType::Club).size());
			REQUIRE(2u == info.GetInfo(ARBInfoType::Judge).size());
			REQUIRE(1u == info.GetInfo(ARBInfoType::Location).size());

			ARBInfo info2(info);
			REQUIRE(info == info2);
			REQUIRE(info.GetInfo(ARBInfoType::Club)[0].get() != info2.GetInfo(ARBInfoType::Club)[0].get());
			REQUIRE(*info.GetInfo(ARBInfoType::Club)[0] == *info2.GetInfo(ARBInfoType::Club)[0]);
		}
	}
}

} // namespace dconSoft
