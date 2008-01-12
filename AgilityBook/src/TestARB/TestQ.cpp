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
 * @brief Test ARB_Q class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBTypes.h"
#include "Element.h"


BEGIN_TEST(ARBQ_ctor)
{
	ARB_Q q;
	WIN_ASSERT_STRING_EQUAL(_T("NA"), q.str().c_str());

	ARB_Q q1(ARB_Q::eQ_Q);
	WIN_ASSERT_STRING_EQUAL(_T("Q"), q1.str().c_str());
}
END_TEST


BEGIN_TEST(ARBQ_Types)
{
	WIN_ASSERT_EQUAL(5, ARB_Q::GetNumValidTypes());
	WIN_ASSERT_STRING_EQUAL(_T("NA, Q, NQ, E, SQ"), ARB_Q::GetValidTypes().c_str());
	WIN_ASSERT_EQUAL(ARB_Q(ARB_Q::eQ_NA), ARB_Q::GetValidType(0));
	WIN_ASSERT_EQUAL(ARB_Q(ARB_Q::eQ_Q), ARB_Q::GetValidType(1));
	WIN_ASSERT_EQUAL(ARB_Q(ARB_Q::eQ_NQ), ARB_Q::GetValidType(2));
	WIN_ASSERT_EQUAL(ARB_Q(ARB_Q::eQ_E), ARB_Q::GetValidType(3));
	WIN_ASSERT_EQUAL(ARB_Q(ARB_Q::eQ_SuperQ), ARB_Q::GetValidType(4));

	std::vector<tstring> types;
	ARB_Q::GetValidTypes(types);
	WIN_ASSERT_EQUAL(5u, types.size());
}
END_TEST


BEGIN_TEST(ARBQ_Equality)
{
	ARB_Q q1(ARB_Q::eQ_Q);
	ARB_Q q2(ARB_Q::eQ_NQ);
	WIN_ASSERT_NOT_EQUAL(q1, q2);
	q2 = ARB_Q::eQ_Q;
	WIN_ASSERT_EQUAL(q1, q2);
	WIN_ASSERT_TRUE(q1.Qualified());
}
END_TEST


BEGIN_TEST(ARBQ_Comparison)
{
	ARB_Q na(ARB_Q::eQ_NA);
	ARB_Q q(ARB_Q::eQ_Q);
	ARB_Q nq(ARB_Q::eQ_NQ);
	ARB_Q e(ARB_Q::eQ_E);
	ARB_Q sq(ARB_Q::eQ_SuperQ);

	WIN_ASSERT_TRUE(na < e);
	WIN_ASSERT_TRUE(e < nq);
	WIN_ASSERT_TRUE(nq < q);
	WIN_ASSERT_TRUE(q < sq);
	WIN_ASSERT_TRUE(q <= q);
}
END_TEST


BEGIN_TEST(ARBQ_Qualified)
{
	ARB_Q na(ARB_Q::eQ_NA);
	WIN_ASSERT_FALSE(na.Qualified());
	ARB_Q q(ARB_Q::eQ_Q);
	WIN_ASSERT_TRUE(q.Qualified());
	ARB_Q nq(ARB_Q::eQ_NQ);
	WIN_ASSERT_FALSE(nq.Qualified());
	ARB_Q e(ARB_Q::eQ_E);
	WIN_ASSERT_FALSE(e.Qualified());
	ARB_Q sq(ARB_Q::eQ_SuperQ);
	WIN_ASSERT_TRUE(sq.Qualified());
}
END_TEST


BEGIN_TEST(ARBQ_Load)
{
	ARB_Q q;
	tstring errmsg;
	ARBErrorCallback callback(errmsg);
	ARBVersion ver(1, 0);
	WIN_ASSERT_TRUE(q.Load(_T("SQ"), ver, callback));
	WIN_ASSERT_STRING_EQUAL(_T("SQ"), q.str().c_str());
}
END_TEST


BEGIN_TEST(ARBQ_LoadFail)
{
	ARB_Q q(ARB_Q::eQ_Q);
	tstring errmsg;
	ARBErrorCallback callback(errmsg);
	ARBVersion ver(1, 0);
	WIN_ASSERT_FALSE(q.Load(_T("attrib"), ver, callback));
	WIN_ASSERT_STRING_EQUAL(_T("NA"), q.str().c_str());
}
END_TEST


BEGIN_TEST(ARBQ_Save)
{
	ARB_Q q;
	ElementNodePtr ele = ElementNode::New(_T("test"));
	WIN_ASSERT_TRUE(q.Save(ele, _T("attrib")));
	tstring str;
	WIN_ASSERT_TRUE(ElementNode::eFound == ele->GetAttrib(_T("attrib"), str));
	WIN_ASSERT_STRING_EQUAL(_T("NA"), str.c_str());
}
END_TEST
