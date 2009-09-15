/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Test ARBInfoItem class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBInfo.h"
#include "ARBInfoItem.h"
#include "ARBStructure.h"
#include "Element.h"


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
	data->AddAttrib(ATTRIB_INFO_NAME, wxT("Club1"));
	data->SetValue(wxT("Club 1 info"));
	ElementNodePtr club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, wxT("Club2"));
	club->SetValue(wxT("Club 2 info"));
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, wxT("Club3"));
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, wxT("Club4"));
	ElementNodePtr judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, wxT("Judge One"));
	judge->SetValue(wxT("Judge 1 info"));
	judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, wxT("Judge Two"));
	judge->SetValue(wxT("Judge 2 info"));
	ElementNodePtr loc = tree->AddElementNode(TREE_LOCATIONINFO);
	loc->AddAttrib(ATTRIB_INFO_NAME, wxT("Sunnyvale"));
}


SUITE(TestInfoItem)
{
	TEST(New)
	{
		ARBInfoItemPtr info = ARBInfoItem::New();
		CHECK(NULL != info.get());
	}


	TEST(Clone)
	{
		ARBInfoItemPtr info = ARBInfoItem::New();
		info->SetName(wxT("Testing"));
		ARBInfoItemPtr info2 = info->Clone();
		CHECK(NULL != info2.get());
		CHECK(info.get() != info2.get());
		CHECK(*info == *info2);
		info->SetName(wxT("Test2"));
		CHECK(info->GetName() != info2->GetName());
	}


	TEST(OpEqual)
	{
		ARBInfoItemPtr info1 = ARBInfoItem::New();
		info1->SetName(wxT("Testing"));
		ARBInfoItemPtr info2 = ARBInfoItem::New();
		CHECK(*info1 != *info2);
		*info1 = *info2;
		CHECK(*info1 == *info2);
	}


	TEST(Compare)
	{
		ARBInfoItemPtr info1 = ARBInfoItem::New();
		info1->SetName(wxT("A"));
		ARBInfoItemPtr info2 = ARBInfoItem::New();
		info2->SetName(wxT("B"));
		CHECK(*info1 < *info2);
		CHECK(!(*info1 > *info2));
	}


	TEST(GenName)
	{
		ARBInfoItemPtr info1 = ARBInfoItem::New();
		info1->SetName(wxT("A"));
		CHECK(info1->GetGenericName() == wxT("A"));
	}


	TEST_FIXTURE(TestInfoItemData, Load)
	{
		ARBInfoItemPtr info = ARBInfoItem::New();
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(info->Load(data, ARBVersion(1, 0), callback, TREE_CLUBINFO));
		wxString name = info->GetGenericName();
		CHECK(!name.empty());
	}


	TEST_FIXTURE(TestInfoItemData, Save)
	{
		ARBInfoItemPtr info = ARBInfoItem::New();
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(info->Load(data, ARBVersion(2, 0), callback, TREE_CLUBINFO));
		ElementNodePtr ele = ElementNode::New();
		CHECK(info->Save(ele, TREE_CLUBINFO));
	}
}


SUITE(TestInfoItemList)
{
	TEST_FIXTURE(TestInfoItemData, Load)
	{
		ARBInfoItemList infolist(TREE_CLUBINFO);
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(infolist.Load(data, ARBVersion(2, 0), callback));
		ElementNodePtr ele = ElementNode::New(wxT("Doesnt matter"));
		ele->SetValue(wxT("These are some notes"));
		ele->AddAttrib(wxT("Name"), wxT("PASA"));
		CHECK(!infolist.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(1u, infolist.size());
	}


	TEST(Load2)
	{
		ElementNodePtr ele = ElementNode::New(wxT("InfoItem"));
		ele->SetValue(wxT("These are some notes"));
		ARBInfoItemList infolist(TREE_CLUBINFO);
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(!infolist.Load(ele, ARBVersion(2, 0), callback));
	}


	TEST(sort)
	{
		ARBInfoItemList infolist(TREE_CLUBINFO);
		ARBInfoItemPtr info1 = ARBInfoItem::New();
		info1->SetName(wxT("Test2"));
		info1->SetComment(wxT("A note"));
		ARBInfoItemPtr info2 = info1->Clone();
		info2->SetName(wxT("Test1"));
		CHECK(infolist.AddItem(info1));
		CHECK(infolist.AddItem(info2));

		ARBInfoItemList infolist2(TREE_CLUBINFO);
		infolist.Clone(infolist2);
		CHECK(infolist == infolist2);
		infolist.sort();
		CHECK(infolist != infolist2);
	}


	TEST_FIXTURE(TestInfoItemData, GetAllItems)
	{
		ARBInfo info;
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(info.Load(tree, ARBVersion(1, 0), callback));
		std::set<wxString> items;
		CHECK_EQUAL(4u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
	}


	TEST_FIXTURE(TestInfoItemData, CondenseContent)
	{
		ARBInfo info;
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(info.Load(tree, ARBVersion(1, 0), callback));
		std::set<wxString> items;
		items.insert(wxT("Club1"));
		items.insert(wxT("Club3"));
		info.GetInfo(ARBInfo::eClubInfo).CondenseContent(items);
		items.clear();
		CHECK_EQUAL(3u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
	}


	TEST_FIXTURE(TestInfoItemData, Find)
	{
		ARBInfo info;
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(info.Load(tree, ARBVersion(1, 0), callback));
		CHECK(info.GetInfo(ARBInfo::eClubInfo).FindItem(wxT("Club3")));
		CHECK(!info.GetInfo(ARBInfo::eClubInfo).FindItem(wxT("Club3xc")));
	}


	TEST(AddDelete)
	{
		ARBInfoItemList infolist(TREE_CLUBINFO);
		ARBInfoItemPtr info1 = ARBInfoItem::New();
		info1->SetName(wxT("Test2"));
		info1->SetComment(wxT("A note"));
		ARBInfoItemPtr info2 = info1->Clone();
		info2->SetName(wxT("Test1"));
		CHECK(infolist.AddItem(info1));
		CHECK(infolist.AddItem(info2));
		infolist.sort();
		ARBInfoItemPtr info3 = infolist[0]->Clone();
		CHECK(*info2 == *info3);
		info3->SetComment(wxT("Test comments"));
		CHECK(*info2 != *info3);
		CHECK(!infolist.AddItem(info3));
		CHECK(!infolist.AddItem(info1->Clone()));
		info3->SetName(wxT("Test3"));
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


SUITE(TestInfo)
{
	TEST_FIXTURE(TestInfoItemData, Load)
	{
		ARBInfo info;
		wxString errs;
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
