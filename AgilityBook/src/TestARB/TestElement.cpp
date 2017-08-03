/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test Element class
 * @author David Connet
 *
 * Revision History
 * 2017-08-03 Added basic read verification
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-12-27 Replace local LoadTree with LoadXMLData
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#if defined(__WXWINDOWS__)
#include <wx/filefn.h>
#include <wx/mstream.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestElement)
{
	TEST(Name)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			CHECK(L"name" == ele->GetName());
		}
	}


	TEST(Value)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			std::wstring str = L"This random & text @#$@()<>";
			ele->SetValue(str);
			CHECK(str == ele->GetValue());
		}
	}


	TEST(AddAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			bool b = true;
			ele->AddAttrib(L"name", b);
			ElementNode::AttribLookup rc = ele->GetAttrib(L"name", b);
			CHECK_EQUAL(ElementNode::eFound, rc);
			CHECK(b);
			std::wstring s;
			rc = ele->GetAttrib(L"name", s);
			CHECK_EQUAL(ElementNode::eFound, rc);
			CHECK(L"y" == s);
		}
	}


	TEST(AddAttribShort)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			short i = 42;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			CHECK(L"42" == s);
			i = 0;
			ele->GetAttrib(L"test", i);
			CHECK_EQUAL(42, i);
		}
	}


	TEST(AddAttribLong)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			long i = 42;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			CHECK(L"42" == s);
			i = 0;
			ele->GetAttrib(L"test", i);
			CHECK_EQUAL(42, i);
		}
	}


	TEST(AddAttribDbl)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			double i = 42.446;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			CHECK(L"42.45" == s);
			i = 0.0;
			ele->GetAttrib(L"test", i);
			CHECK_EQUAL(42.45, i);

			i = 42.446;
			ele->AddAttrib(L"test", i, 3);
			ele->GetAttrib(L"test", s);
			CHECK(L"42.446" == s);
			i = 0.0;
			ele->GetAttrib(L"test", i);
			CHECK_EQUAL(42.446, i);
		}
	}


	TEST(AddAttribDate)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"1999-03-27");
			ARBDate d;
			ElementNode::AttribLookup rc = ele->GetAttrib(L"test", d);
			CHECK_EQUAL(ElementNode::eFound, rc);
			CHECK_EQUAL(1999, d.GetYear());
			CHECK_EQUAL(3, d.GetMonth());
			CHECK_EQUAL(27, d.GetDay());
		}
	}


	TEST(AddAttribBadDate)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"1999-99-27");
			ARBDate d;
			ElementNode::AttribLookup rc = ele->GetAttrib(L"test", d);
			CHECK_EQUAL(ElementNode::eInvalidValue, rc);
		}
	}


	TEST(BadAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			ele->AddAttrib(L"test", L"junk2");
			CHECK_EQUAL(1, ele->GetAttribCount());
		}
	}


	TEST(ReallyBadAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			std::wstring attrib;
			CHECK(!ele->AddAttrib(attrib, L"test"));
		}
	}


	TEST(RemoveAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			CHECK_EQUAL(1, ele->GetAttribCount());
			CHECK(!ele->RemoveAttrib(L"test1"));
			CHECK_EQUAL(1, ele->GetAttribCount());
			CHECK(ele->RemoveAttrib(L"test"));
			CHECK_EQUAL(0, ele->GetAttribCount());
		}
	}


	TEST(RemoveAllAttribs)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			ele->AddAttrib(L"test1", L"junk");
			CHECK_EQUAL(2, ele->GetAttribCount());
			ele->RemoveAllAttribs();
			CHECK_EQUAL(0, ele->GetAttribCount());
		}
	}


	TEST(AddElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ElementNodePtr newEle = ele->AddElementNode(L"test");
			CHECK(L"test" == newEle->GetName());
			CHECK_EQUAL(1, ele->GetElementCount());
			ElementNodePtr ele2 = ele->GetElementNode(0);
			CHECK_EQUAL(newEle.get(), ele2.get());
		}
	}


	TEST(AddElementAt)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2", 0);
			CHECK_EQUAL(2, ele->GetElementCount());
			ElementNodePtr newEle = ele->GetElementNode(0);
			CHECK(L"test2" == newEle->GetName());
			newEle = ele->GetElementNode(1);
			CHECK(L"test1" == newEle->GetName());
		}
	}


	TEST(RemoveElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			CHECK_EQUAL(2, ele->GetElementCount());
			ele->RemoveElement(0);
			CHECK_EQUAL(1, ele->GetElementCount());
		}
	}


	TEST(FindElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			CHECK_EQUAL(2, ele->GetElementCount());
			CHECK_EQUAL(1, ele->FindElement(L"test2"));
		}
	}


	TEST(FindElementFrom)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			CHECK_EQUAL(4, ele->GetElementCount());
			CHECK_EQUAL(3, ele->FindElement(L"test2", 2));
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData();

			CHECK(L"DefaultConfig" == tree->GetName());
			CHECK_EQUAL(1, tree->GetAttribCount());
			int config = tree->FindElement(TREE_CONFIG);
			CHECK(0 <= config);
		}
	}


	TEST(LoadXML)
	{
		if (!g_bMicroTest)
		{
			std::stringstream data;
			data << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				<< "<Test attrib='a'>\n"
					<< "<ele>Content</ele>\n"
					<< "<ele ele='2'>More content</ele>"
				<< "</Test>";

			std::wostringstream errMsg;
			ElementNodePtr tree(ElementNode::New());
			CHECK(tree->LoadXML(data, errMsg));

			std::wstring str;
			CHECK(tree->GetName() == L"Test");
			CHECK(tree->GetAttribCount() == 1);
			CHECK(tree->GetAttrib(L"attrib", str) == ElementNode::eFound);
			CHECK(str == L"a");
			CHECK(tree->GetElementCount() == 2);
			CHECK(tree->GetElementNode(0)->GetAttribCount() == 0);
			CHECK(tree->GetElementNode(1)->GetAttribCount() == 1);
			CHECK(tree->GetElementNode(1)->GetAttrib(L"ele", str) == ElementNode::eFound);
			CHECK(str == L"2");
			CHECK(tree->GetElementNode(0)->GetValue() == L"Content");
			CHECK(tree->GetElementNode(1)->GetValue() == L"More content");
		}
	}

	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData();

			std::wstring tmpFile(L"data.tmp");
			std::stringstream tmp1;
			CHECK(tree->SaveXML(tmpFile));
			CHECK(tree->SaveXML(tmp1));

			ElementNodePtr tree2(ElementNode::New());
			std::wostringstream errs;
			CHECK(tree2->LoadXML(tmpFile.c_str(), errs));

#if defined(__WXWINDOWS__)
			wxRemoveFile(tmpFile.c_str());
#else
#pragma PRAGMA_TODO(remove file)
#endif

			std::stringstream tmp2;
			CHECK(tree2->SaveXML(tmp2));

			std::string tmp1data = tmp1.str();
			std::string tmp2data = tmp2.str();
			CHECK(tmp1data == tmp2data);
		}
	}
}
