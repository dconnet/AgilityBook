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
