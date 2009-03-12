/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Test Element class
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-27 DRC Replace local LoadTree with LoadXMLData
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBDate.h"
#include "ARBStructure.h"
#include "Element.h"

#ifndef _WIN32
#ifdef UNICODE
#define _tunlink	_wunlink
#else
#define _tunlink	_unlink
#endif
#endif


SUITE(TestElement)
{
	TEST(Name)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		CHECK(wxT("name") == ele->GetName());
	}


	TEST(Value)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		tstring str = wxT("This random & text @#$@()<>");
		ele->SetValue(str);
		CHECK(str == ele->GetValue());
	}


	TEST(AddAttrib)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		bool b = true;
		ele->AddAttrib(wxT("name"), b);
		ElementNode::AttribLookup rc = ele->GetAttrib(wxT("name"), b);
		CHECK_EQUAL(ElementNode::eFound, rc);
		CHECK(b);
		tstring s;
		rc = ele->GetAttrib(wxT("name"), s);
		CHECK_EQUAL(ElementNode::eFound, rc);
		CHECK(wxT("y") == s);
	}


	TEST(AddAttribShort)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		short i = 42;
		ele->AddAttrib(wxT("test"), i);
		tstring s;
		ele->GetAttrib(wxT("test"), s);
		CHECK(wxT("42") == s);
		i = 0;
		ele->GetAttrib(wxT("test"), i);
		CHECK_EQUAL(42, i);
	}


	TEST(AddAttribLong)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		long i = 42;
		ele->AddAttrib(wxT("test"), i);
		tstring s;
		ele->GetAttrib(wxT("test"), s);
		CHECK(wxT("42") == s);
		i = 0;
		ele->GetAttrib(wxT("test"), i);
		CHECK_EQUAL(42, i);
	}


	TEST(AddAttribDbl)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		double i = 42.446;
		ele->AddAttrib(wxT("test"), i);
		tstring s;
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


	TEST(AddAttribDate)
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


	TEST(AddAttribBadDate)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddAttrib(wxT("test"), wxT("1999-99-27"));
		ARBDate d;
		ElementNode::AttribLookup rc = ele->GetAttrib(wxT("test"), d);
		CHECK_EQUAL(ElementNode::eInvalidValue, rc);
	}


	TEST(BadAttrib)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddAttrib(wxT("test"), wxT("junk"));
		ele->AddAttrib(wxT("test"), wxT("junk2"));
		CHECK_EQUAL(1, ele->GetAttribCount());
	}


	TEST(ReallyBadAttrib)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		tstring attrib;
		CHECK(!ele->AddAttrib(attrib, wxT("test")));
	}


	TEST(RemoveAttrib)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddAttrib(wxT("test"), wxT("junk"));
		CHECK_EQUAL(1, ele->GetAttribCount());
		CHECK(!ele->RemoveAttrib(wxT("test1")));
		CHECK_EQUAL(1, ele->GetAttribCount());
		CHECK(ele->RemoveAttrib(wxT("test")));
		CHECK_EQUAL(0, ele->GetAttribCount());
	}


	TEST(RemoveAllAttribs)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddAttrib(wxT("test"), wxT("junk"));
		ele->AddAttrib(wxT("test1"), wxT("junk"));
		CHECK_EQUAL(2, ele->GetAttribCount());
		ele->RemoveAllAttribs();
		CHECK_EQUAL(0, ele->GetAttribCount());
	}


	TEST(AddElement)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ElementNodePtr newEle = ele->AddElementNode(wxT("test"));
		CHECK(wxT("test") == newEle->GetName());
		CHECK_EQUAL(1, ele->GetElementCount());
		ElementNodePtr ele2 = ele->GetElementNode(0);
		CHECK_EQUAL(newEle.get(), ele2.get());
	}


	TEST(AddElementAt)
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


	TEST(RemoveElement)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddElementNode(wxT("test1"));
		ele->AddElementNode(wxT("test2"));
		CHECK_EQUAL(2, ele->GetElementCount());
		ele->RemoveElement(0);
		CHECK_EQUAL(1, ele->GetElementCount());
	}


	TEST(FindElement)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddElementNode(wxT("test1"));
		ele->AddElementNode(wxT("test2"));
		CHECK_EQUAL(2, ele->GetElementCount());
		CHECK_EQUAL(1, ele->FindElement(wxT("test2")));
	}


	TEST(FindElementFrom)
	{
		ElementNodePtr ele = ElementNode::New(wxT("name"));
		ele->AddElementNode(wxT("test1"));
		ele->AddElementNode(wxT("test2"));
		ele->AddElementNode(wxT("test1"));
		ele->AddElementNode(wxT("test2"));
		CHECK_EQUAL(4, ele->GetElementCount());
		CHECK_EQUAL(3, ele->FindElement(wxT("test2"), 2));
	}


	TEST(Load)
	{
		ElementNodePtr tree = LoadXMLData(IDR_XML_DEFAULT_CONFIG);

		CHECK(wxT("DefaultConfig") == tree->GetName());
		CHECK_EQUAL(1, tree->GetAttribCount());
		int config = tree->FindElement(TREE_CONFIG);
		CHECK(0 <= config);
	}


	TEST(Save)
	{
		ElementNodePtr tree = LoadXMLData(IDR_XML_DEFAULT_CONFIG);

		wxChar const* const tmpFile = wxT("data.tmp");
		std::ostringstream tmp1;
		CHECK(tree->SaveXML(tmpFile));
		CHECK(tree->SaveXML(tmp1));

		ElementNodePtr tree2(ElementNode::New());
		tstring errs;
		CHECK(tree2->LoadXMLFile(tmpFile, errs));

		_tunlink(tmpFile);

		std::ostringstream tmp2;
		CHECK(tree2->SaveXML(tmp2));

		CHECK(tmp1.str() == tmp2.str());
	}
}
