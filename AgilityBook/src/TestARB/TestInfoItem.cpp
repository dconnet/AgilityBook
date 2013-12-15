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
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBInfo.h"
#include "ARB/ARBInfoItem.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


struct TestInfoItemData
{
	ElementNodePtr tree;
	ElementNodePtr data; // clubinfo
	TestInfoItemData();
};


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


SUITE(TestInfoItem)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			CHECK(NULL != info.get());
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			info->SetName(L"Testing");
			ARBInfoItemPtr info2 = info->Clone();
			CHECK(NULL != info2.get());
			CHECK(info.get() != info2.get());
			CHECK(*info == *info2);
			info->SetName(L"Test2");
			CHECK(info->GetName() != info2->GetName());
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Testing");
			ARBInfoItemPtr info2 = ARBInfoItem::New();
			CHECK(*info1 != *info2);
			*info1 = *info2;
			CHECK(*info1 == *info2);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"A");
			ARBInfoItemPtr info2 = ARBInfoItem::New();
			info2->SetName(L"B");
			CHECK(*info1 < *info2);
			CHECK(!(*info1 > *info2));
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"A");
			CHECK(info1->GetGenericName() == L"A");
		}
	}


	TEST_FIXTURE(TestInfoItemData, Load)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info->Load(data, ARBVersion(1, 0), callback, TREE_CLUBINFO));
			std::wstring name = info->GetGenericName();
			CHECK(!name.empty());
		}
	}


	TEST_FIXTURE(TestInfoItemData, Save)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemPtr info = ARBInfoItem::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info->Load(data, ARBVersion(2, 0), callback, TREE_CLUBINFO));
			ElementNodePtr ele = ElementNode::New();
			CHECK(info->Save(ele, TREE_CLUBINFO));
		}
	}
}


SUITE(TestInfoItemList)
{
	TEST_FIXTURE(TestInfoItemData, Load)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(infolist.Load(data, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(L"Name", L"PASA");
			CHECK(!infolist.Load(ele, ARBVersion(2, 0), callback));
			CHECK_EQUAL(1u, infolist.size());
		}
	}


	TEST(Load2)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"InfoItem");
			ele->SetValue(L"These are some notes");
			ARBInfoItemList infolist(TREE_CLUBINFO);
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(!infolist.Load(ele, ARBVersion(2, 0), callback));
		}
	}


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Test2");
			info1->SetComment(L"A note");
			ARBInfoItemPtr info2 = info1->Clone();
			info2->SetName(L"Test1");
			CHECK(infolist.AddItem(info1));
			CHECK(infolist.AddItem(info2));

			ARBInfoItemList infolist2(TREE_CLUBINFO);
			infolist.Clone(infolist2);
			CHECK(infolist == infolist2);
			infolist.sort();
			CHECK(infolist != infolist2);
		}
	}


	TEST_FIXTURE(TestInfoItemData, GetAllItems)
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info.Load(tree, ARBVersion(1, 0), callback));
			std::set<std::wstring> items;
			CHECK_EQUAL(4u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
		}
	}


	TEST_FIXTURE(TestInfoItemData, CondenseContent)
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info.Load(tree, ARBVersion(1, 0), callback));
			std::set<std::wstring> items;
			items.insert(L"Club1");
			items.insert(L"Club3");
			info.GetInfo(ARBInfo::eClubInfo).CondenseContent(items);
			items.clear();
			CHECK_EQUAL(3u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
		}
	}


	TEST_FIXTURE(TestInfoItemData, Find)
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info.Load(tree, ARBVersion(1, 0), callback));
			CHECK(info.GetInfo(ARBInfo::eClubInfo).FindItem(L"Club3"));
			CHECK(!info.GetInfo(ARBInfo::eClubInfo).FindItem(L"Club3xc"));
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			ARBInfoItemList infolist(TREE_CLUBINFO);
			ARBInfoItemPtr info1 = ARBInfoItem::New();
			info1->SetName(L"Test2");
			info1->SetComment(L"A note");
			ARBInfoItemPtr info2 = info1->Clone();
			info2->SetName(L"Test1");
			CHECK(infolist.AddItem(info1));
			CHECK(infolist.AddItem(info2));
			infolist.sort();
			ARBInfoItemPtr info3 = infolist[0]->Clone();
			CHECK(*info2 == *info3);
			info3->SetComment(L"Test comments");
			CHECK(*info2 != *info3);
			CHECK(!infolist.AddItem(info3));
			CHECK(!infolist.AddItem(info1->Clone()));
			info3->SetName(L"Test3");
			CHECK(infolist.AddItem(info3));
			infolist.sort();
			CHECK_EQUAL(3u, infolist.size());
			CHECK(infolist.DeleteItem(info1));
			CHECK_EQUAL(2u, infolist.size());
			CHECK(!infolist.DeleteItem(info1));
			CHECK_EQUAL(2u, infolist.size());
			CHECK(*infolist[0] != *infolist[1]);
			ARBInfoItemList infolist2(TREE_CLUBINFO);
			infolist.Clone(infolist2);
			CHECK(infolist == infolist2);
			CHECK(infolist[0].get() != infolist2[0].get());
			CHECK(*infolist[0] == *infolist2[0]);
		}
	}
}


SUITE(TestInfo)
{
	TEST_FIXTURE(TestInfoItemData, Load)
	{
		if (!g_bMicroTest)
		{
			ARBInfo info;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(info.Load(tree, ARBVersion(1, 0), callback));
			CHECK_EQUAL(4u, info.GetInfo(ARBInfo::eClubInfo).size());
			CHECK_EQUAL(2u, info.GetInfo(ARBInfo::eJudgeInfo).size());
			CHECK_EQUAL(1u, info.GetInfo(ARBInfo::eLocationInfo).size());

			ARBInfo info2(info);
			CHECK(info == info2);
			CHECK(info.GetInfo(ARBInfo::eClubInfo)[0].get() != info2.GetInfo(ARBInfo::eClubInfo)[0].get());
			CHECK(*info.GetInfo(ARBInfo::eClubInfo)[0] == *info2.GetInfo(ARBInfo::eClubInfo)[0]);
		}
	}
}
