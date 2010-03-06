/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test Element class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-12-27 DRC Replace local LoadTree with LoadXMLData
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBDate.h"
#include "ARBStructure.h"
#include "Element.h"
#include <wx/filefn.h>
#include <wx/mstream.h>


SUITE(TestElement)
{
	TEST(Name)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			CHECK(wxT("name") == ele->GetName());
		}
	}


	TEST(Value)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			wxString str = wxT("This random & text @#$@()<>");
			ele->SetValue(str);
			CHECK(str == ele->GetValue());
		}
	}


	TEST(AddAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			bool b = true;
			ele->AddAttrib(wxT("name"), b);
			ElementNode::AttribLookup rc = ele->GetAttrib(wxT("name"), b);
			CHECK_EQUAL(ElementNode::eFound, rc);
			CHECK(b);
			wxString s;
			rc = ele->GetAttrib(wxT("name"), s);
			CHECK_EQUAL(ElementNode::eFound, rc);
			CHECK(wxT("y") == s);
		}
	}


	TEST(AddAttribShort)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			short i = 42;
			ele->AddAttrib(wxT("test"), i);
			wxString s;
			ele->GetAttrib(wxT("test"), s);
			CHECK(wxT("42") == s);
			i = 0;
			ele->GetAttrib(wxT("test"), i);
			CHECK_EQUAL(42, i);
		}
	}


	TEST(AddAttribLong)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			long i = 42;
			ele->AddAttrib(wxT("test"), i);
			wxString s;
			ele->GetAttrib(wxT("test"), s);
			CHECK(wxT("42") == s);
			i = 0;
			ele->GetAttrib(wxT("test"), i);
			CHECK_EQUAL(42, i);
		}
	}


	TEST(AddAttribDbl)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			double i = 42.446;
			ele->AddAttrib(wxT("test"), i);
			wxString s;
			ele->GetAttrib(wxT("test"), s);
			CHECK(wxT("42.45") == s);
			i = 0.0;
			ele->GetAttrib(wxT("test"), i);
			CHECK_EQUAL(42.45, i);

			i = 42.446;
			ele->AddAttrib(wxT("test"), i, 3);
			ele->GetAttrib(wxT("test"), s);
			CHECK(wxT("42.446") == s);
			i = 0.0;
			ele->GetAttrib(wxT("test"), i);
			CHECK_EQUAL(42.446, i);
		}
	}


	TEST(AddAttribDate)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddAttrib(wxT("test"), wxT("1999-03-27"));
			ARBDate d;
			ElementNode::AttribLookup rc = ele->GetAttrib(wxT("test"), d);
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
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddAttrib(wxT("test"), wxT("1999-99-27"));
			ARBDate d;
			ElementNode::AttribLookup rc = ele->GetAttrib(wxT("test"), d);
			CHECK_EQUAL(ElementNode::eInvalidValue, rc);
		}
	}


	TEST(BadAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddAttrib(wxT("test"), wxT("junk"));
			ele->AddAttrib(wxT("test"), wxT("junk2"));
			CHECK_EQUAL(1, ele->GetAttribCount());
		}
	}


	TEST(ReallyBadAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			wxString attrib;
			CHECK(!ele->AddAttrib(attrib, wxT("test")));
		}
	}


	TEST(RemoveAttrib)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddAttrib(wxT("test"), wxT("junk"));
			CHECK_EQUAL(1, ele->GetAttribCount());
			CHECK(!ele->RemoveAttrib(wxT("test1")));
			CHECK_EQUAL(1, ele->GetAttribCount());
			CHECK(ele->RemoveAttrib(wxT("test")));
			CHECK_EQUAL(0, ele->GetAttribCount());
		}
	}


	TEST(RemoveAllAttribs)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddAttrib(wxT("test"), wxT("junk"));
			ele->AddAttrib(wxT("test1"), wxT("junk"));
			CHECK_EQUAL(2, ele->GetAttribCount());
			ele->RemoveAllAttribs();
			CHECK_EQUAL(0, ele->GetAttribCount());
		}
	}


	TEST(AddElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ElementNodePtr newEle = ele->AddElementNode(wxT("test"));
			CHECK(wxT("test") == newEle->GetName());
			CHECK_EQUAL(1, ele->GetElementCount());
			ElementNodePtr ele2 = ele->GetElementNode(0);
			CHECK_EQUAL(newEle.get(), ele2.get());
		}
	}


	TEST(AddElementAt)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddElementNode(wxT("test1"));
			ele->AddElementNode(wxT("test2"), 0);
			CHECK_EQUAL(2, ele->GetElementCount());
			ElementNodePtr newEle = ele->GetElementNode(0);
			CHECK(wxT("test2") == newEle->GetName());
			newEle = ele->GetElementNode(1);
			CHECK(wxT("test1") == newEle->GetName());
		}
	}


	TEST(RemoveElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddElementNode(wxT("test1"));
			ele->AddElementNode(wxT("test2"));
			CHECK_EQUAL(2, ele->GetElementCount());
			ele->RemoveElement(0);
			CHECK_EQUAL(1, ele->GetElementCount());
		}
	}


	TEST(FindElement)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddElementNode(wxT("test1"));
			ele->AddElementNode(wxT("test2"));
			CHECK_EQUAL(2, ele->GetElementCount());
			CHECK_EQUAL(1, ele->FindElement(wxT("test2")));
		}
	}


	TEST(FindElementFrom)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(wxT("name"));
			ele->AddElementNode(wxT("test1"));
			ele->AddElementNode(wxT("test2"));
			ele->AddElementNode(wxT("test1"));
			ele->AddElementNode(wxT("test2"));
			CHECK_EQUAL(4, ele->GetElementCount());
			CHECK_EQUAL(3, ele->FindElement(wxT("test2"), 2));
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData(IDR_XML_DEFAULT_CONFIG);

			CHECK(wxT("DefaultConfig") == tree->GetName());
			CHECK_EQUAL(1, tree->GetAttribCount());
			int config = tree->FindElement(TREE_CONFIG);
			CHECK(0 <= config);
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr tree = LoadXMLData(IDR_XML_DEFAULT_CONFIG);

			wxString tmpFile(wxT("data.tmp"));
			wxMemoryOutputStream tmp1;
			CHECK(tree->SaveXML(tmpFile));
			CHECK(tree->SaveXML(tmp1));

			ElementNodePtr tree2(ElementNode::New());
			wxString errs;
			CHECK(tree2->LoadXMLFile(tmpFile, errs));

			wxRemoveFile(tmpFile);

			wxMemoryOutputStream tmp2;
			CHECK(tree2->SaveXML(tmp2));

			std::string tmp1data = tstringUtil::tstringA(tmp1);
			std::string tmp2data = tstringUtil::tstringA(tmp2);
			CHECK(tmp1data == tmp2data);
		}
	}
}
