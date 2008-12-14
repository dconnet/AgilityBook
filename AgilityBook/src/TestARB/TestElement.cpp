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
 * @brief Test Element class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"
#include "resource.h"

#include "ARBDate.h"
#include "ARBStructure.h"
#include "Element.h"


SUITE(TestElement)
{
	static void LoadTree(ElementNodePtr tree)
	{
#ifdef WXWIDGETS
#pragma message PRAGMA_MESSAGE("TODO")
#else
		HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), _T("XML"));
		VERIFY(hrSrc);
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		VERIFY(hRes);
		DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
		char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
		tstring errs;
		VERIFY(tree->LoadXMLBuffer(pData, size, errs));
		FreeResource(hRes);
#endif
	}

	TEST(Name)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		CHECK(_T("name") == ele->GetName());
	}


	TEST(Value)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		tstring str = _T("This random & text @#$@()<>");
		ele->SetValue(str);
		CHECK(str == ele->GetValue());
	}


	TEST(AddAttrib)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		bool b = true;
		ele->AddAttrib(_T("name"), b);
		ElementNode::AttribLookup rc = ele->GetAttrib(_T("name"), b);
		CHECK_EQUAL(ElementNode::eFound, rc);
		CHECK(b);
		tstring s;
		rc = ele->GetAttrib(_T("name"), s);
		CHECK_EQUAL(ElementNode::eFound, rc);
		CHECK(_T("y") == s);
	}


	TEST(AddAttribShort)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		short i = 42;
		ele->AddAttrib(_T("test"), i);
		tstring s;
		ele->GetAttrib(_T("test"), s);
		CHECK(_T("42") == s);
		i = 0;
		ele->GetAttrib(_T("test"), i);
		CHECK_EQUAL(42, i);
	}


	TEST(AddAttribLong)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		long i = 42;
		ele->AddAttrib(_T("test"), i);
		tstring s;
		ele->GetAttrib(_T("test"), s);
		CHECK(_T("42") == s);
		i = 0;
		ele->GetAttrib(_T("test"), i);
		CHECK_EQUAL(42, i);
	}


	TEST(AddAttribDbl)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		double i = 42.446;
		ele->AddAttrib(_T("test"), i);
		tstring s;
		ele->GetAttrib(_T("test"), s);
		CHECK(_T("42.45") == s);
		i = 0.0;
		ele->GetAttrib(_T("test"), i);
		CHECK_EQUAL(42.45, i);

		i = 42.446;
		ele->AddAttrib(_T("test"), i, 3);
		ele->GetAttrib(_T("test"), s);
		CHECK(_T("42.446") == s);
		i = 0.0;
		ele->GetAttrib(_T("test"), i);
		CHECK_EQUAL(42.446, i);
	}


	TEST(AddAttribDate)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddAttrib(_T("test"), _T("1999-03-27"));
		ARBDate d;
		ElementNode::AttribLookup rc = ele->GetAttrib(_T("test"), d);
		CHECK_EQUAL(ElementNode::eFound, rc);
		CHECK_EQUAL(1999, d.GetYear());
		CHECK_EQUAL(3, d.GetMonth());
		CHECK_EQUAL(27, d.GetDay());
	}


	TEST(AddAttribBadDate)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddAttrib(_T("test"), _T("1999-99-27"));
		ARBDate d;
		ElementNode::AttribLookup rc = ele->GetAttrib(_T("test"), d);
		CHECK_EQUAL(ElementNode::eInvalidValue, rc);
	}


	TEST(BadAttrib)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddAttrib(_T("test"), _T("junk"));
		ele->AddAttrib(_T("test"), _T("junk2"));
		CHECK_EQUAL(1, ele->GetAttribCount());
	}


	TEST(ReallyBadAttrib)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		tstring attrib;
		CHECK(!ele->AddAttrib(attrib, _T("test")));
	}


	TEST(RemoveAttrib)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddAttrib(_T("test"), _T("junk"));
		CHECK_EQUAL(1, ele->GetAttribCount());
		CHECK(!ele->RemoveAttrib(_T("test1")));
		CHECK_EQUAL(1, ele->GetAttribCount());
		CHECK(ele->RemoveAttrib(_T("test")));
		CHECK_EQUAL(0, ele->GetAttribCount());
	}


	TEST(RemoveAllAttribs)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddAttrib(_T("test"), _T("junk"));
		ele->AddAttrib(_T("test1"), _T("junk"));
		CHECK_EQUAL(2, ele->GetAttribCount());
		ele->RemoveAllAttribs();
		CHECK_EQUAL(0, ele->GetAttribCount());
	}


	TEST(AddElement)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ElementNodePtr newEle = ele->AddElementNode(_T("test"));
		CHECK(_T("test") == newEle->GetName());
		CHECK_EQUAL(1, ele->GetElementCount());
		ElementNodePtr ele2 = ele->GetElementNode(0);
		CHECK_EQUAL(newEle.get(), ele2.get());
	}


	TEST(AddElementAt)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddElementNode(_T("test1"));
		ele->AddElementNode(_T("test2"), 0);
		CHECK_EQUAL(2, ele->GetElementCount());
		ElementNodePtr newEle = ele->GetElementNode(0);
		CHECK(_T("test2") == newEle->GetName());
		newEle = ele->GetElementNode(1);
		CHECK(_T("test1") == newEle->GetName());
	}


	TEST(RemoveElement)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddElementNode(_T("test1"));
		ele->AddElementNode(_T("test2"));
		CHECK_EQUAL(2, ele->GetElementCount());
		ele->RemoveElement(0);
		CHECK_EQUAL(1, ele->GetElementCount());
	}


	TEST(FindElement)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddElementNode(_T("test1"));
		ele->AddElementNode(_T("test2"));
		CHECK_EQUAL(2, ele->GetElementCount());
		CHECK_EQUAL(1, ele->FindElement(_T("test2")));
	}


	TEST(FindElementFrom)
	{
		ElementNodePtr ele = ElementNode::New(_T("name"));
		ele->AddElementNode(_T("test1"));
		ele->AddElementNode(_T("test2"));
		ele->AddElementNode(_T("test1"));
		ele->AddElementNode(_T("test2"));
		CHECK_EQUAL(4, ele->GetElementCount());
		CHECK_EQUAL(3, ele->FindElement(_T("test2"), 2));
	}


	TEST(Load)
	{
		ElementNodePtr tree(ElementNode::New());
		LoadTree(tree);

		CHECK(_T("DefaultConfig") == tree->GetName());
		CHECK_EQUAL(1, tree->GetAttribCount());
		int config = tree->FindElement(TREE_CONFIG);
		CHECK(0 <= config);
	}


	TEST(Save)
	{
		ElementNodePtr tree(ElementNode::New());
		LoadTree(tree);

		char const* const tmpFile = "data.tmp";
		std::ostringstream tmp1;
		CHECK(tree->SaveXML(tmpFile));
		CHECK(tree->SaveXML(tmp1));

		ElementNodePtr tree2(ElementNode::New());
		tstring errs;
		CHECK(tree2->LoadXMLFile(_T("data.tmp"), errs));

		_unlink(tmpFile);

		std::ostringstream tmp2;
		CHECK(tree2->SaveXML(tmp2));

		CHECK(tmp1.str() == tmp2.str());
	}
}
