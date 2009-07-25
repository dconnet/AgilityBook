/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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

#pragma PRAGMA_TODO("Enable DNR")
// When DNR is enabled, remove NO_DNR define and fix code below
#define NO_DNR

SUITE(TestARBQ)
{
	TEST(ctor)
	{
		ARB_Q q;
		CHECK(wxT("NA") == q.str());

		ARB_Q q1(ARB_Q::eQ_Q);
		CHECK(wxT("Q") == q1.str());
	}


	TEST(Types)
	{
		CHECK_EQUAL(5, ARB_Q::GetNumValidTypes());
		CHECK(wxT("NA, Q, NQ, E, SQ") == ARB_Q::GetValidTypes());
		CHECK(ARB_Q(ARB_Q::eQ_NA) == ARB_Q::GetValidType(0));
		CHECK(ARB_Q(ARB_Q::eQ_Q) == ARB_Q::GetValidType(1));
		CHECK(ARB_Q(ARB_Q::eQ_NQ) == ARB_Q::GetValidType(2));
		CHECK(ARB_Q(ARB_Q::eQ_E) == ARB_Q::GetValidType(3));
#ifdef NO_DNR
		CHECK(ARB_Q(ARB_Q::eQ_SuperQ) == ARB_Q::GetValidType(4));
#else
		CHECK(ARB_Q(ARB_Q::eQ_DNR) == ARB_Q::GetValidType(4));
		CHECK(ARB_Q(ARB_Q::eQ_SuperQ) == ARB_Q::GetValidType(5));
#endif

		std::vector<tstring> types;
		ARB_Q::GetValidTypes(types);
#ifdef NO_DNR
		CHECK_EQUAL(5u, types.size());
#else
		CHECK_EQUAL(6u, types.size());
#endif
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
#ifndef NO_DNR
		ARB_Q dnr(ARB_Q::eQ_DNR);
#endif
		ARB_Q sq(ARB_Q::eQ_SuperQ);

#ifdef NO_DNR
		CHECK(na < e);
#else
		CHECK(na < dnr);
		CHECK(dnr < e);
#endif
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
#ifndef NO_DNR
		ARB_Q dnr(ARB_Q::eQ_DNR);
		CHECK(!dnr.Qualified());
#endif
		ARB_Q sq(ARB_Q::eQ_SuperQ);
		CHECK(sq.Qualified());
	}


	TEST(Load)
	{
		ARB_Q q;
		tstring errmsg;
		ARBErrorCallback callback(errmsg);
		ARBVersion ver(1, 0);
		CHECK(q.Load(wxT("SQ"), ver, callback));
		CHECK(wxT("SQ") == q.str());
	}


	TEST(LoadFail)
	{
		ARB_Q q(ARB_Q::eQ_Q);
		tstring errmsg;
		ARBErrorCallback callback(errmsg);
		ARBVersion ver(1, 0);
		CHECK(!q.Load(wxT("attrib"), ver, callback));
		CHECK(wxT("NA") == q.str());
	}


	TEST(Save)
	{
		ARB_Q q;
		ElementNodePtr ele = ElementNode::New(wxT("test"));
		CHECK(q.Save(ele, wxT("attrib")));
		tstring str;
		CHECK(ElementNode::eFound == ele->GetAttrib(wxT("attrib"), str));
		CHECK(wxT("NA") == str);
	}
}
