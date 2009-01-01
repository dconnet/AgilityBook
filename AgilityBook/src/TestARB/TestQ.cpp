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


SUITE(TestARBQ)
{
	TEST(ctor)
	{
		ARB_Q q;
		CHECK(_T("NA") == q.str());

		ARB_Q q1(ARB_Q::eQ_Q);
		CHECK(_T("Q") == q1.str());
	}


	TEST(Types)
	{
		CHECK_EQUAL(5, ARB_Q::GetNumValidTypes());
		CHECK(_T("NA, Q, NQ, E, SQ") == ARB_Q::GetValidTypes());
		CHECK(ARB_Q(ARB_Q::eQ_NA) == ARB_Q::GetValidType(0));
		CHECK(ARB_Q(ARB_Q::eQ_Q) == ARB_Q::GetValidType(1));
		CHECK(ARB_Q(ARB_Q::eQ_NQ) == ARB_Q::GetValidType(2));
		CHECK(ARB_Q(ARB_Q::eQ_E) == ARB_Q::GetValidType(3));
		CHECK(ARB_Q(ARB_Q::eQ_SuperQ) == ARB_Q::GetValidType(4));

		std::vector<tstring> types;
		ARB_Q::GetValidTypes(types);
		CHECK_EQUAL(5u, types.size());
	}


	TEST(Equality)
	{
		ARB_Q q1(ARB_Q::eQ_Q);
		ARB_Q q2(ARB_Q::eQ_NQ);
		CHECK(q1 != q2);
		q2 = ARB_Q::eQ_Q;
		CHECK(q1 == q2);
		CHECK(q1.Qualified());
	}


	TEST(Comparison)
	{
		ARB_Q na(ARB_Q::eQ_NA);
		ARB_Q q(ARB_Q::eQ_Q);
		ARB_Q nq(ARB_Q::eQ_NQ);
		ARB_Q e(ARB_Q::eQ_E);
		ARB_Q sq(ARB_Q::eQ_SuperQ);

		CHECK(na < e);
		CHECK(e < nq);
		CHECK(nq < q);
		CHECK(q < sq);
		CHECK(q <= q);
	}


	TEST(Qualified)
	{
		ARB_Q na(ARB_Q::eQ_NA);
		CHECK(!na.Qualified());
		ARB_Q q(ARB_Q::eQ_Q);
		CHECK(q.Qualified());
		ARB_Q nq(ARB_Q::eQ_NQ);
		CHECK(!nq.Qualified());
		ARB_Q e(ARB_Q::eQ_E);
		CHECK(!e.Qualified());
		ARB_Q sq(ARB_Q::eQ_SuperQ);
		CHECK(sq.Qualified());
	}


	TEST(Load)
	{
		ARB_Q q;
		tstring errmsg;
		ARBErrorCallback callback(errmsg);
		ARBVersion ver(1, 0);
		CHECK(q.Load(_T("SQ"), ver, callback));
		CHECK(_T("SQ") == q.str());
	}


	TEST(LoadFail)
	{
		ARB_Q q(ARB_Q::eQ_Q);
		tstring errmsg;
		ARBErrorCallback callback(errmsg);
		ARBVersion ver(1, 0);
		CHECK(!q.Load(_T("attrib"), ver, callback));
		CHECK(_T("NA") == q.str());
	}


	TEST(Save)
	{
		ARB_Q q;
		ElementNodePtr ele = ElementNode::New(_T("test"));
		CHECK(q.Save(ele, _T("attrib")));
		tstring str;
		CHECK(ElementNode::eFound == ele->GetAttrib(_T("attrib"), str));
		CHECK(_T("NA") == str);
	}
}
