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
 * @brief Test ARBInfoItem class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBInfo.h"
#include "ARBInfoItem.h"
#include "ARBStructure.h"
#include "Element.h"


FIXTURE(InfoItem)


namespace
{
	ElementNodePtr tree;
	ElementNodePtr data; // clubinfo
}


SETUP(InfoItem)
{
	WIN_ASSERT_TRUE(CommonSetup());

	tree = ElementNode::New(TREE_INFO);
	data = tree->AddElementNode(TREE_CLUBINFO);
	data->AddAttrib(ATTRIB_INFO_NAME, _T("Club1"));
	data->SetValue(_T("Club 1 info"));
	ElementNodePtr club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, _T("Club2"));
	club->SetValue(_T("Club 2 info"));
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, _T("Club3"));
	club = tree->AddElementNode(TREE_CLUBINFO);
	club->AddAttrib(ATTRIB_INFO_NAME, _T("Club4"));
	ElementNodePtr judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, _T("Judge One"));
	judge->SetValue(_T("Judge 1 info"));
	judge = tree->AddElementNode(TREE_JUDGEINFO);
	judge->AddAttrib(ATTRIB_INFO_NAME, _T("Judge Two"));
	judge->SetValue(_T("Judge 2 info"));
	ElementNodePtr loc = tree->AddElementNode(TREE_LOCATIONINFO);
	loc->AddAttrib(ATTRIB_INFO_NAME, _T("Sunnyvale"));
}


TEARDOWN(InfoItem)
{
	tree.reset();
	WIN_ASSERT_TRUE(CommonTeardown());
}


BEGIN_TEST(InfoItem_New)
{
	ARBInfoItemPtr info = ARBInfoItem::New();
	WIN_ASSERT_NOT_NULL(info.get());
}
END_TEST


BEGIN_TEST(InfoItem_Clone)
{
	ARBInfoItemPtr info = ARBInfoItem::New();
	info->SetName(_T("Testing"));
	ARBInfoItemPtr info2 = info->Clone();
	WIN_ASSERT_NOT_NULL(info2.get());
	WIN_ASSERT_NOT_EQUAL(info.get(), info2.get());
	WIN_ASSERT_EQUAL(*info, *info2);
	info->SetName(_T("Test2"));
	WIN_ASSERT_NOT_EQUAL(info->GetName(), info2->GetName());
}
END_TEST


BEGIN_TEST(InfoItem_OpEqual)
{
	ARBInfoItemPtr info1 = ARBInfoItem::New();
	info1->SetName(_T("Testing"));
	ARBInfoItemPtr info2 = ARBInfoItem::New();
	WIN_ASSERT_NOT_EQUAL(*info1, *info2);
	*info1 = *info2;
	WIN_ASSERT_EQUAL(*info1, *info2);
}
END_TEST


BEGIN_TEST(InfoItem_Compare)
{
	ARBInfoItemPtr info1 = ARBInfoItem::New();
	info1->SetName(_T("A"));
	ARBInfoItemPtr info2 = ARBInfoItem::New();
	info2->SetName(_T("B"));
	WIN_ASSERT_TRUE(*info1 < *info2);
	WIN_ASSERT_FALSE(*info1 > *info2);
}
END_TEST


BEGIN_TEST(InfoItem_GenName)
{
	ARBInfoItemPtr info1 = ARBInfoItem::New();
	info1->SetName(_T("A"));
	WIN_ASSERT_STRING_EQUAL(_T("A"), info1->GetGenericName().c_str());
}
END_TEST


BEGIN_TESTF(InfoItem_Load, InfoItem)
{
	ARBInfoItemPtr info = ARBInfoItem::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info->Load(data, ARBVersion(1, 0), callback, TREE_CLUBINFO));
	tstring name = info->GetGenericName();
	WIN_ASSERT_FALSE(name.empty());
}
END_TESTF


BEGIN_TESTF(InfoItem_Save, InfoItem)
{
	ARBInfoItemPtr info = ARBInfoItem::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info->Load(data, ARBVersion(2, 0), callback, TREE_CLUBINFO));
	ElementNodePtr ele = ElementNode::New();
	WIN_ASSERT_TRUE(info->Save(ele, TREE_CLUBINFO));
}
END_TESTF


BEGIN_TESTF(InfoItemList_Load, InfoItem)
{
	ARBInfoItemList infolist(TREE_CLUBINFO);
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(infolist.Load(data, ARBVersion(2, 0), callback));
	ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
	ele->SetValue(_T("These are some notes"));
	ele->AddAttrib(_T("Name"), _T("PASA"));
	WIN_ASSERT_FALSE(infolist.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(1u, infolist.size());
}
END_TESTF


BEGIN_TESTF(InfoItemList_Load2, InfoItem)
{
	ElementNodePtr ele = ElementNode::New(_T("InfoItem"));
	ele->SetValue(_T("These are some notes"));
	ARBInfoItemList infolist(TREE_CLUBINFO);
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_FALSE(infolist.Load(ele, ARBVersion(2, 0), callback));
}
END_TESTF


BEGIN_TEST(InfoItemList_SortAddDelete)
{
	ARBInfoItemList infolist(TREE_CLUBINFO);
	ARBInfoItemPtr info1 = ARBInfoItem::New();
	info1->SetName(_T("Test2"));
	info1->SetComment(_T("A note"));
	ARBInfoItemPtr info2 = info1->Clone();
	info2->SetName(_T("Test1"));
	WIN_ASSERT_TRUE(infolist.AddItem(info1));
	WIN_ASSERT_TRUE(infolist.AddItem(info2));
	infolist.sort();
	ARBInfoItemPtr info3 = infolist[0]->Clone();
	WIN_ASSERT_EQUAL(*info2, *info3);
	info3->SetComment(_T("Test comments"));
	WIN_ASSERT_NOT_EQUAL(*info2, *info3);
	WIN_ASSERT_FALSE(infolist.AddItem(info3));
	WIN_ASSERT_FALSE(infolist.AddItem(info1->Clone()));
	info3->SetName(_T("Test3"));
	WIN_ASSERT_TRUE(infolist.AddItem(info3));
	infolist.sort();
	WIN_ASSERT_EQUAL(3u, infolist.size());
	WIN_ASSERT_TRUE(infolist.DeleteItem(info1));
	WIN_ASSERT_EQUAL(2u, infolist.size());
	WIN_ASSERT_FALSE(infolist.DeleteItem(info1));
	WIN_ASSERT_EQUAL(2u, infolist.size());
	WIN_ASSERT_NOT_EQUAL(*infolist[0], *infolist[1]);
	ARBInfoItemList infolist2(TREE_CLUBINFO);
	infolist.Clone(infolist2);
	WIN_ASSERT_EQUAL(infolist, infolist2);
	WIN_ASSERT_NOT_EQUAL(infolist[0].get(), infolist2[0].get());
	WIN_ASSERT_EQUAL(*infolist[0], *infolist2[0]);
}
END_TEST


BEGIN_TESTF(InfoItemList_GetAllItems, InfoItem)
{
	ARBInfo info;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info.Load(tree, ARBVersion(1, 0), callback));
	std::set<tstring> items;
	WIN_ASSERT_EQUAL(4u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
}
END_TESTF


BEGIN_TESTF(InfoItemList_CondenseContent, InfoItem)
{
	ARBInfo info;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info.Load(tree, ARBVersion(1, 0), callback));
	std::set<tstring> items;
	items.insert(_T("Club1"));
	items.insert(_T("Club3"));
	info.GetInfo(ARBInfo::eClubInfo).CondenseContent(items);
	items.clear();
	WIN_ASSERT_EQUAL(3u, info.GetInfo(ARBInfo::eClubInfo).GetAllItems(items, false));
}
END_TESTF


BEGIN_TESTF(InfoItemList_Find, InfoItem)
{
	ARBInfo info;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info.Load(tree, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(info.GetInfo(ARBInfo::eClubInfo).FindItem(_T("Club3")));
	WIN_ASSERT_FALSE(info.GetInfo(ARBInfo::eClubInfo).FindItem(_T("Club3xc")));
}
END_TESTF


BEGIN_TESTF(Info_Load, InfoItem)
{
	ARBInfo info;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(info.Load(tree, ARBVersion(1, 0), callback));
	WIN_ASSERT_EQUAL(4u, info.GetInfo(ARBInfo::eClubInfo).size());
	WIN_ASSERT_EQUAL(2u, info.GetInfo(ARBInfo::eJudgeInfo).size());
	WIN_ASSERT_EQUAL(1u, info.GetInfo(ARBInfo::eLocationInfo).size());

	ARBInfo info2(info);
	WIN_ASSERT_EQUAL(info, info2);
	WIN_ASSERT_NOT_EQUAL(info.GetInfo(ARBInfo::eClubInfo)[0].get(), info2.GetInfo(ARBInfo::eClubInfo)[0].get());
	WIN_ASSERT_EQUAL(*info.GetInfo(ARBInfo::eClubInfo)[0], *info2.GetInfo(ARBInfo::eClubInfo)[0]);
}
END_TESTF
