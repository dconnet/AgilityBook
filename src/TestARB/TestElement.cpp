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
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2017-08-03 Added basic read verification
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-12-27 Replace local LoadTree with LoadXMLData
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestLib.h"

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


TEST_CASE("Element")
{
	SECTION("Name")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			REQUIRE(L"name" == ele->GetName());
		}
	}


	SECTION("Value")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			std::wstring str = L"This random & text @#$@()<>";
			ele->SetValue(str);
			REQUIRE(str == ele->GetValue());
		}
	}


	SECTION("AddAttrib")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			bool b = true;
			ele->AddAttrib(L"name", b);
			ARBAttribLookup rc = ele->GetAttrib(L"name", b);
			REQUIRE(ARBAttribLookup::Found == rc);
			REQUIRE(b);
			std::wstring s;
			rc = ele->GetAttrib(L"name", s);
			REQUIRE(ARBAttribLookup::Found == rc);
			REQUIRE(L"y" == s);
		}
	}


	SECTION("AddAttribShort")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			short i = 42;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			REQUIRE(L"42" == s);
			i = 0;
			ele->GetAttrib(L"test", i);
			REQUIRE(42 == i);
		}
	}


	SECTION("AddAttribLong")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			long i = 42;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			REQUIRE(L"42" == s);
			i = 0;
			ele->GetAttrib(L"test", i);
			REQUIRE(42 == i);
		}
	}


	SECTION("AddAttribDbl")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			double i = 42.446;
			ele->AddAttrib(L"test", i);
			std::wstring s;
			ele->GetAttrib(L"test", s);
			REQUIRE(L"42.45" == s);
			i = 0.0;
			ele->GetAttrib(L"test", i);
			REQUIRE(42.45 == i);

			i = 42.446;
			ele->AddAttrib(L"test", i, 3);
			ele->GetAttrib(L"test", s);
			REQUIRE(L"42.446" == s);
			i = 0.0;
			ele->GetAttrib(L"test", i);
			REQUIRE(42.446 == i);
		}
	}


	SECTION("AddAttribDate")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"1999-03-27");
			ARBDate d;
			ARBAttribLookup rc = ele->GetAttrib(L"test", d);
			REQUIRE(ARBAttribLookup::Found == rc);
			REQUIRE(1999 == d.GetYear());
			REQUIRE(3 == d.GetMonth());
			REQUIRE(27 == d.GetDay());
		}
	}


	SECTION("AddAttribBadDate")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"1999-99-27");
			ARBDate d;
			ARBAttribLookup rc = ele->GetAttrib(L"test", d);
			REQUIRE(ARBAttribLookup::Invalid == rc);
		}
	}


	SECTION("BadAttrib")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			ele->AddAttrib(L"test", L"junk2");
			REQUIRE(1 == ele->GetAttribCount());
		}
	}


	SECTION("ReallyBadAttrib")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			std::wstring attrib;
			REQUIRE(!ele->AddAttrib(attrib, L"test"));
		}
	}


	SECTION("RemoveAttrib")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			REQUIRE(1 == ele->GetAttribCount());
			REQUIRE(!ele->RemoveAttrib(L"test1"));
			REQUIRE(1 == ele->GetAttribCount());
			REQUIRE(ele->RemoveAttrib(L"test"));
			REQUIRE(0 == ele->GetAttribCount());
		}
	}


	SECTION("RemoveAllAttribs")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddAttrib(L"test", L"junk");
			ele->AddAttrib(L"test1", L"junk");
			REQUIRE(2 == ele->GetAttribCount());
			ele->RemoveAllAttribs();
			REQUIRE(0 == ele->GetAttribCount());
		}
	}


	SECTION("AddElement")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ElementNodePtr newEle = ele->AddElementNode(L"test");
			REQUIRE(L"test" == newEle->GetName());
			REQUIRE(1 == ele->GetElementCount());
			ElementNodePtr ele2 = ele->GetElementNode(0);
			REQUIRE(newEle.get() == ele2.get());
		}
	}


	SECTION("AddElementAt")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2", 0);
			REQUIRE(2 == ele->GetElementCount());
			ElementNodePtr newEle = ele->GetElementNode(0);
			REQUIRE(L"test2" == newEle->GetName());
			newEle = ele->GetElementNode(1);
			REQUIRE(L"test1" == newEle->GetName());
		}
	}


	SECTION("RemoveElement")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			REQUIRE(2 == ele->GetElementCount());
			ele->RemoveElement(0);
			REQUIRE(1 == ele->GetElementCount());
		}
	}


	SECTION("FindElement")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			REQUIRE(2 == ele->GetElementCount());
			REQUIRE(1 == ele->FindElement(L"test2"));
		}
	}


	SECTION("FindElementFrom")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(L"name");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			ele->AddElementNode(L"test1");
			ele->AddElementNode(L"test2");
			REQUIRE(4 == ele->GetElementCount());
			REQUIRE(3 == ele->FindElement(L"test2", 2));
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData();

			REQUIRE(L"DefaultConfig" == tree->GetName());
			REQUIRE(1 == tree->GetAttribCount());
			int config = tree->FindElement(TREE_CONFIG);
			REQUIRE(0 <= config);
		}
	}


	SECTION("LoadXML")
	{
		if (!g_bMicroTest)
		{
			std::stringstream data;
			data << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				<< "<Test attrib='a'>\n"
					<< "<ele>Content</ele>\n"
					<< "<ele ele='2'>More content</ele>"
				<< "</Test>";

			fmt::wmemory_buffer errMsg;
			ElementNodePtr tree(ElementNode::New());
			REQUIRE(tree->LoadXML(data, errMsg));

			std::wstring str;
			REQUIRE(tree->GetName() == L"Test");
			REQUIRE(tree->GetAttribCount() == 1);
			REQUIRE(tree->GetAttrib(L"attrib", str) == ARBAttribLookup::Found);
			REQUIRE(str == L"a");
			REQUIRE(tree->GetElementCount() == 2);
			REQUIRE(tree->GetElementNode(0)->GetAttribCount() == 0);
			REQUIRE(tree->GetElementNode(1)->GetAttribCount() == 1);
			REQUIRE(tree->GetElementNode(1)->GetAttrib(L"ele", str) == ARBAttribLookup::Found);
			REQUIRE(str == L"2");
			REQUIRE(tree->GetElementNode(0)->GetValue() == L"Content");
			REQUIRE(tree->GetElementNode(1)->GetValue() == L"More content");
		}
	}

	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData();

			std::wstring tmpFile(L"data.tmp");
			std::stringstream tmp1;
			REQUIRE(tree->SaveXML(tmpFile));
			REQUIRE(tree->SaveXML(tmp1));

			ElementNodePtr tree2(ElementNode::New());
			fmt::wmemory_buffer errs;
			REQUIRE(tree2->LoadXML(tmpFile.c_str(), errs));

#if defined(__WXWINDOWS__)
			wxRemoveFile(tmpFile.c_str());
#else
#pragma PRAGMA_TODO(remove file)
#endif

			std::stringstream tmp2;
			REQUIRE(tree2->SaveXML(tmp2));

			std::string tmp1data = tmp1.str();
			std::string tmp2data = tmp2.str();
			REQUIRE(tmp1data == tmp2data);
		}
	}
}
