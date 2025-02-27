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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2015-05-06 Updated for changed Q order.
 * 2015-03-15 Fixed Unknown-Q usage.
 * 2015-02-13 Added Unknown Q type.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARB_Q.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

TEST_CASE("ARBQ")
{
	SECTION("ctor")
	{
		// Note: If this fails, it's likely the language catalog is not
		// properly setup.
		if (!g_bMicroTest)
		{
			ARB_Q q;
			REQUIRE(L"" == q.str());

			ARB_Q q1(Q::Q);
			REQUIRE(L"Q" == q1.str());
		}
	}


	SECTION("Types")
	{
		if (!g_bMicroTest)
		{
			// FYI: None of the code assumes any order. This is just to make
			// sure I didn't add/remove something and forget.
			REQUIRE(8 == ARB_Q::GetNumValidTypes());
			REQUIRE(L"SQ, Q, NQ, E, FEO, DNR, NA" == ARB_Q::GetValidTypes());
			REQUIRE(ARB_Q(Q::UNK) == ARB_Q::GetValidType(0));
			REQUIRE(ARB_Q(Q::SuperQ) == ARB_Q::GetValidType(1));
			REQUIRE(ARB_Q(Q::Q) == ARB_Q::GetValidType(2));
			REQUIRE(ARB_Q(Q::NQ) == ARB_Q::GetValidType(3));
			REQUIRE(ARB_Q(Q::E) == ARB_Q::GetValidType(4));
			REQUIRE(ARB_Q(Q::FEO) == ARB_Q::GetValidType(5));
			REQUIRE(ARB_Q(Q::DNR) == ARB_Q::GetValidType(6));
			REQUIRE(ARB_Q(Q::NA) == ARB_Q::GetValidType(7));

			std::vector<wxString> types;
			ARB_Q::GetValidTypes(types);
			REQUIRE(8u == types.size());
		}
	}


	SECTION("Equality")
	{
		if (!g_bMicroTest)
		{
			ARB_Q q1(Q::Q);
			ARB_Q q2(Q::NQ);
			REQUIRE(q1 != q2);
			q2 = Q::Q;
			REQUIRE(q1 == q2);
			REQUIRE(q1.Qualified());
		}
	}


	SECTION("Comparison")
	{
		if (!g_bMicroTest)
		{
			ARB_Q na(Q::NA);
			ARB_Q q(Q::Q);
			ARB_Q nq(Q::NQ);
			ARB_Q e(Q::E);
			ARB_Q dnr(Q::DNR);
			ARB_Q sq(Q::SuperQ);

			REQUIRE(na < dnr);
			REQUIRE(dnr < e);
			REQUIRE(e < nq);
			REQUIRE(nq < q);
			REQUIRE(q < sq);
			REQUIRE(q <= q);
		}
	}


	SECTION("Qualified")
	{
		if (!g_bMicroTest)
		{
			ARB_Q na(Q::NA);
			REQUIRE(!na.Qualified());
			ARB_Q q(Q::Q);
			REQUIRE(q.Qualified());
			ARB_Q nq(Q::NQ);
			REQUIRE(!nq.Qualified());
			ARB_Q e(Q::E);
			REQUIRE(!e.Qualified());
			ARB_Q dnr(Q::DNR);
			REQUIRE(!dnr.Qualified());
			ARB_Q sq(Q::SuperQ);
			REQUIRE(sq.Qualified());
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			ARB_Q q;
			wxString errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			REQUIRE(q.Load(L"SQ", ver, callback));
			REQUIRE(L"SQ" == q.str());
#else
#pragma PRAGMA_TODO(implement non - wx version)
#endif
		}
	}


	SECTION("LoadFail")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			ARB_Q q(Q::Q);
			wxString errmsg;
			ARBErrorCallback callback(errmsg);
			ARBVersion ver(1, 0);
			REQUIRE(!q.Load(L"attrib", ver, callback));
			REQUIRE(L"" == q.str());
#else
#pragma PRAGMA_TODO(implement non - wx version)
#endif
		}
	}


	SECTION("SaveValid")
	{
		if (!g_bMicroTest)
		{
			ARB_Q q(Q::Q);
			ElementNodePtr ele = ElementNode::New(L"test");
			REQUIRE(q.Save(ele, L"attrib"));
			wxString str;
			REQUIRE(ARBAttribLookup::Found == ele->GetAttrib(L"attrib", str));
			REQUIRE(L"Q" == str);
		}
	}


	SECTION("SaveUnknown")
	{
		if (!g_bMicroTest)
		{
			ARB_Q q;
			ElementNodePtr ele = ElementNode::New(L"test");
			REQUIRE(q.Save(ele, L"attrib"));
			wxString str;
			REQUIRE(ARBAttribLookup::NotFound == ele->GetAttrib(L"attrib", str));
			REQUIRE(L"" == str);
		}
	}
}

} // namespace dconSoft
