/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARB_Q class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARB_Q.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestARBQ)
{
	TEST(ctor)
	{
		// Note: If this fails, it's likely the language catalog is not
		// properly setup.
		if (!g_bMicroTest)
		{
			ARB_Q q;
			CHECK(L"NA" == q.str());

			ARB_Q q1(ARB_Q::eQ_Q);
			CHECK(L"Q" == q1.str());
		}
	}


	TEST(Types)
	{
		if (!g_bMicroTest)
		{
			CHECK_EQUAL(6, ARB_Q::GetNumValidTypes());
			CHECK(L"NA, Q, NQ, E, DNR, SQ" == ARB_Q::GetValidTypes());
			CHECK(ARB_Q(ARB_Q::eQ_NA) == ARB_Q::GetValidType(0));
			CHECK(ARB_Q(ARB_Q::eQ_Q) == ARB_Q::GetValidType(1));
			CHECK(ARB_Q(ARB_Q::eQ_NQ) == ARB_Q::GetValidType(2));
			CHECK(ARB_Q(ARB_Q::eQ_E) == ARB_Q::GetValidType(3));
			CHECK(ARB_Q(ARB_Q::eQ_DNR) == ARB_Q::GetValidType(4));
			CHECK(ARB_Q(ARB_Q::eQ_SuperQ) == ARB_Q::GetValidType(5));

			std::vector<std::wstring> types;
			ARB_Q::GetValidTypes(types);
			CHECK_EQUAL(6u, types.size());
		}
	}


	TEST(Equality)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q1(ARB_Q::eQ_Q);
			ARB_Q q2(ARB_Q::eQ_NQ);
			CHECK(q1 != q2);
			q2 = ARB_Q::eQ_Q;
			CHECK(q1 == q2);
			CHECK(q1.Qualified());
		}
	}


	TEST(Comparison)
	{
		if (!g_bMicroTest)
		{
			ARB_Q na(ARB_Q::eQ_NA);
			ARB_Q q(ARB_Q::eQ_Q);
			ARB_Q nq(ARB_Q::eQ_NQ);
			ARB_Q e(ARB_Q::eQ_E);
			ARB_Q dnr(ARB_Q::eQ_DNR);
			ARB_Q sq(ARB_Q::eQ_SuperQ);

			CHECK(na < dnr);
			CHECK(dnr < e);
			CHECK(e < nq);
			CHECK(nq < q);
			CHECK(q < sq);
			CHECK(q <= q);
		}
	}


	TEST(Qualified)
	{
		if (!g_bMicroTest)
		{
			ARB_Q na(ARB_Q::eQ_NA);
			CHECK(!na.Qualified());
			ARB_Q q(ARB_Q::eQ_Q);
			CHECK(q.Qualified());
			ARB_Q nq(ARB_Q::eQ_NQ);
			CHECK(!nq.Qualified());
			ARB_Q e(ARB_Q::eQ_E);
			CHECK(!e.Qualified());
			ARB_Q dnr(ARB_Q::eQ_DNR);
			CHECK(!dnr.Qualified());
			ARB_Q sq(ARB_Q::eQ_SuperQ);
			CHECK(sq.Qualified());
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q;
			std::wostringstream errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			CHECK(q.Load(L"SQ", ver, callback));
			CHECK(L"SQ" == q.str());
		}
	}


	TEST(LoadFail)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q(ARB_Q::eQ_Q);
			std::wostringstream errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			CHECK(!q.Load(L"attrib", ver, callback));
			CHECK(L"NA" == q.str());
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q;
			ElementNodePtr ele = ElementNode::New(L"test");
			CHECK(q.Save(ele, L"attrib"));
			std::wstring str;
			CHECK(ElementNode::eFound == ele->GetAttrib(L"attrib", str));
			CHECK(L"NA" == str);
		}
	}
}
