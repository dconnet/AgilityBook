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


BEGIN_TEST(Element_Name)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	WIN_ASSERT_STRING_EQUAL(_T("name"), ele->GetName().c_str());
}
END_TEST


BEGIN_TEST(Element_Value)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	tstring str = _T("This random & text @#$@()<>");
	ele->SetValue(str);
	WIN_ASSERT_STRING_EQUAL(str.c_str(), ele->GetValue().c_str());
}
END_TEST


BEGIN_TEST(Element_AddAttrib)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	bool b = true;
	ele->AddAttrib(_T("name"), b);
	ElementNode::AttribLookup rc = ele->GetAttrib(_T("name"), b);
	WIN_ASSERT_EQUAL(ElementNode::eFound, rc);
	WIN_ASSERT_TRUE(b);
	tstring s;
	rc = ele->GetAttrib(_T("name"), s);
	WIN_ASSERT_EQUAL(ElementNode::eFound, rc);
	WIN_ASSERT_EQUAL(_T("y"), s);
}
END_TEST


BEGIN_TEST(Element_AddAttribShort)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	short i = 42;
	ele->AddAttrib(_T("test"), i);
	tstring s;
	ele->GetAttrib(_T("test"), s);
	WIN_ASSERT_STRING_EQUAL(_T("42"), s.c_str());
	i = 0;
	ele->GetAttrib(_T("test"), i);
	WIN_ASSERT_EQUAL(42, i);
}
END_TEST


BEGIN_TEST(Element_AddAttribLong)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	long i = 42;
	ele->AddAttrib(_T("test"), i);
	tstring s;
	ele->GetAttrib(_T("test"), s);
	WIN_ASSERT_STRING_EQUAL(_T("42"), s.c_str());
	i = 0;
	ele->GetAttrib(_T("test"), i);
	WIN_ASSERT_EQUAL(42, i);
}
END_TEST


BEGIN_TEST(Element_AddAttribDbl)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	double i = 42.446;
	ele->AddAttrib(_T("test"), i);
	tstring s;
	ele->GetAttrib(_T("test"), s);
	WIN_ASSERT_STRING_EQUAL(_T("42.45"), s.c_str());
	i = 0.0;
	ele->GetAttrib(_T("test"), i);
	WIN_ASSERT_EQUAL(42.45, i);

	i = 42.446;
	ele->AddAttrib(_T("test"), i, 3);
	ele->GetAttrib(_T("test"), s);
	WIN_ASSERT_STRING_EQUAL(_T("42.446"), s.c_str());
	i = 0.0;
	ele->GetAttrib(_T("test"), i);
	WIN_ASSERT_EQUAL(42.446, i);
}
END_TEST


BEGIN_TEST(Element_AddAttribDate)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddAttrib(_T("test"), _T("1999-03-27"));
	ARBDate d;
	ElementNode::AttribLookup rc = ele->GetAttrib(_T("test"), d);
	WIN_ASSERT_EQUAL(ElementNode::eFound, rc);
	WIN_ASSERT_EQUAL(1999, d.GetYear());
	WIN_ASSERT_EQUAL(3, d.GetMonth());
	WIN_ASSERT_EQUAL(27, d.GetDay());
}
END_TEST


BEGIN_TEST(Element_AddAttribBadDate)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddAttrib(_T("test"), _T("1999-99-27"));
	ARBDate d;
	ElementNode::AttribLookup rc = ele->GetAttrib(_T("test"), d);
	WIN_ASSERT_EQUAL(ElementNode::eInvalidValue, rc);
}
END_TEST


BEGIN_TEST(Element_BadAttrib)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddAttrib(_T("test"), _T("junk"));
	ele->AddAttrib(_T("test"), _T("junk2"));
	WIN_ASSERT_EQUAL(1, ele->GetAttribCount());
}
END_TEST


BEGIN_TEST(Element_ReallyBadAttrib)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	tstring attrib;
	WIN_ASSERT_FALSE(ele->AddAttrib(attrib, _T("test")));
}
END_TEST


BEGIN_TEST(Element_RemoveAttrib)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddAttrib(_T("test"), _T("junk"));
	WIN_ASSERT_EQUAL(1, ele->GetAttribCount());
	WIN_ASSERT_FALSE(ele->RemoveAttrib(_T("test1")));
	WIN_ASSERT_EQUAL(1, ele->GetAttribCount());
	WIN_ASSERT_TRUE(ele->RemoveAttrib(_T("test")));
	WIN_ASSERT_EQUAL(0, ele->GetAttribCount());
}
END_TEST


BEGIN_TEST(Element_RemoveAllAttribs)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddAttrib(_T("test"), _T("junk"));
	ele->AddAttrib(_T("test1"), _T("junk"));
	WIN_ASSERT_EQUAL(2, ele->GetAttribCount());
	ele->RemoveAllAttribs();
	WIN_ASSERT_EQUAL(0, ele->GetAttribCount());
}
END_TEST


BEGIN_TEST(Element_AddElement)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ElementNodePtr newEle = ele->AddElementNode(_T("test"));
	WIN_ASSERT_STRING_EQUAL(_T("test"), newEle->GetName().c_str());
	WIN_ASSERT_EQUAL(1, ele->GetElementCount());
	ElementNodePtr ele2 = ele->GetElementNode(0);
	WIN_ASSERT_EQUAL(newEle.get(), ele2.get());
}
END_TEST


BEGIN_TEST(Element_AddElementAt)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddElementNode(_T("test1"));
	ele->AddElementNode(_T("test2"), 0);
	WIN_ASSERT_EQUAL(2, ele->GetElementCount());
	ElementNodePtr newEle = ele->GetElementNode(0);
	WIN_ASSERT_STRING_EQUAL(_T("test2"), newEle->GetName().c_str());
	newEle = ele->GetElementNode(1);
	WIN_ASSERT_STRING_EQUAL(_T("test1"), newEle->GetName().c_str());
}
END_TEST


BEGIN_TEST(Element_RemoveElement)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddElementNode(_T("test1"));
	ele->AddElementNode(_T("test2"));
	WIN_ASSERT_EQUAL(2, ele->GetElementCount());
	ele->RemoveElement(0);
	WIN_ASSERT_EQUAL(1, ele->GetElementCount());
}
END_TEST


BEGIN_TEST(Element_FindElement)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddElementNode(_T("test1"));
	ele->AddElementNode(_T("test2"));
	WIN_ASSERT_EQUAL(2, ele->GetElementCount());
	WIN_ASSERT_EQUAL(1, ele->FindElement(_T("test2")));
}
END_TEST


BEGIN_TEST(Element_FindElementFrom)
{
	ElementNodePtr ele = ElementNode::New(_T("name"));
	ele->AddElementNode(_T("test1"));
	ele->AddElementNode(_T("test2"));
	ele->AddElementNode(_T("test1"));
	ele->AddElementNode(_T("test2"));
	WIN_ASSERT_EQUAL(4, ele->GetElementCount());
	WIN_ASSERT_EQUAL(3, ele->FindElement(_T("test2"), 2));
}
END_TEST


static void LoadTree(ElementNodePtr tree)
{
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), _T("XML"));
	WIN_ASSERT_TRUE(hrSrc);
	HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
	WIN_ASSERT_TRUE(hRes);
	DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
	char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
	tstring errs;
	WIN_ASSERT_TRUE(tree->LoadXMLBuffer(pData, size, errs));
	FreeResource(hRes);
}


BEGIN_TEST(Element_Load)
{
	tstring errs;
	WIN_ASSERT_TRUE(Element::Initialize(errs));
	ElementNodePtr tree(ElementNode::New());
	LoadTree(tree);

	WIN_ASSERT_STRING_EQUAL(_T("DefaultConfig"), tree->GetName().c_str());
	WIN_ASSERT_EQUAL(1, tree->GetAttribCount());
	int config = tree->FindElement(TREE_CONFIG);
	WIN_ASSERT_TRUE(0 <= config);

	Element::Terminate();
}
END_TEST


BEGIN_TEST(Element_Save)
{
	tstring errs;
	WIN_ASSERT_TRUE(Element::Initialize(errs));
	ElementNodePtr tree(ElementNode::New());
	LoadTree(tree);

	CStringA tmpfile("data.tmp");
	std::ostringstream tmp1;
	WIN_ASSERT_TRUE(tree->SaveXML(tmpfile));
	WIN_ASSERT_TRUE(tree->SaveXML(tmp1));

	ElementNodePtr tree2(ElementNode::New());
	WIN_ASSERT_TRUE(tree2->LoadXMLFile(tmpfile, errs));

	DeleteFile(CString(tmpfile));

	std::ostringstream tmp2;
	WIN_ASSERT_TRUE(tree2->SaveXML(tmp2));

	WIN_ASSERT_STRING_EQUAL(tmp1.str().c_str(), tmp2.str().c_str());

	Element::Terminate();
}
END_TEST
