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
 * 2015-05-06 Updated for changed Q order.
 * 2015-03-15 Fixed Unknown-Q usage.
 * 2015-02-13 Added Unknown Q type.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-11 Created
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
			CHECK(L"" == q.str());

			ARB_Q q1(ARB_Q::eQ_Q);
			CHECK(L"Q" == q1.str());
		}
	}


	TEST(Types)
	{
		if (!g_bMicroTest)
		{
			// FYI: None of the code assumes any order. This is just to make
			// sure I didn't add/remove something and forget.
			CHECK_EQUAL(7, ARB_Q::GetNumValidTypes());
			CHECK(L"SQ, Q, NQ, E, DNR, NA" == ARB_Q::GetValidTypes());
			CHECK(ARB_Q(ARB_Q::eQ_UNK) == ARB_Q::GetValidType(0));
			CHECK(ARB_Q(ARB_Q::eQ_NA) == ARB_Q::GetValidType(6));
			CHECK(ARB_Q(ARB_Q::eQ_Q) == ARB_Q::GetValidType(2));
			CHECK(ARB_Q(ARB_Q::eQ_NQ) == ARB_Q::GetValidType(3));
			CHECK(ARB_Q(ARB_Q::eQ_E) == ARB_Q::GetValidType(4));
			CHECK(ARB_Q(ARB_Q::eQ_DNR) == ARB_Q::GetValidType(5));
			CHECK(ARB_Q(ARB_Q::eQ_SuperQ) == ARB_Q::GetValidType(1));

			std::vector<std::wstring> types;
			ARB_Q::GetValidTypes(types);
			CHECK_EQUAL(7u, types.size());
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
#if defined(__WXWINDOWS__)
			ARB_Q q;
			std::wostringstream errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			CHECK(q.Load(L"SQ", ver, callback));
			CHECK(L"SQ" == q.str());
#else
#pragma PRAGMA_TODO(implement non-wx version)
#endif
		}
	}


	TEST(LoadFail)
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			ARB_Q q(ARB_Q::eQ_Q);
			std::wostringstream errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			CHECK(!q.Load(L"attrib", ver, callback));
			CHECK(L"" == q.str());
#else
#pragma PRAGMA_TODO(implement non-wx version)
#endif
		}
	}


	TEST(SaveValid)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q(ARB_Q::eQ_Q);
			ElementNodePtr ele = ElementNode::New(L"test");
			CHECK(q.Save(ele, L"attrib"));
			std::wstring str;
			CHECK(ElementNode::eFound == ele->GetAttrib(L"attrib", str));
			CHECK(L"Q" == str);
		}
	}


	TEST(SaveUnknown)
	{
		if (!g_bMicroTest)
		{
			ARB_Q q;
			ElementNodePtr ele = ElementNode::New(L"test");
			CHECK(q.Save(ele, L"attrib"));
			std::wstring str;
			CHECK(ElementNode::eNotFound == ele->GetAttrib(L"attrib", str));
			CHECK(L"" == str);
		}
	}
}
