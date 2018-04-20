/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDouble functions
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2015-12-22 Added tests for changed ARBDouble parameter.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-12 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


static std::wstring FormNumber(std::wstring const& d1, std::wstring const& dec, std::wstring const& d2)
{
	std::wostringstream val;
	val << d1 << dec << d2;
	return val.str();
}


static void RunDblTests()
{
#if defined(__WXWINDOWS__)
	std::wstring decimalPt = StringUtil::stringW(wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER));
#else
#pragma PRAGMA_TODO(Write localization)
	std::wstring decimalPt = L".";
#endif

	// ARBDouble always strips 0s unless prec ==2, unless =".00"
	double p = 3.14159265358979323846;
	std::wstring s = ARBDouble::ToString(p, 2);
	REQUIRE(FormNumber(L"3", decimalPt, L"14") == s);
	s = ARBDouble::ToString(p, 4);
	REQUIRE(FormNumber(L"3", decimalPt, L"1416") == s);

	// eCompatible

	p = 2.1;
	s = ARBDouble::ToString(p, 0);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2);
	REQUIRE(FormNumber(L"2", decimalPt, L"10") == s);
	s = ARBDouble::ToString(p, 3);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 2);
	REQUIRE(L"2" == s);

	// eStrip

	p = 2.1;
	s = ARBDouble::ToString(p, 0, ARBDouble::eStrip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eStrip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 3, ARBDouble::eStrip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, ARBDouble::eStrip);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1, ARBDouble::eStrip);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eStrip);
	REQUIRE(L"2" == s);

	// eAsIs

	p = 2.1;
	s = ARBDouble::ToString(p, 0, ARBDouble::eAsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eAsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"10") == s);
	s = ARBDouble::ToString(p, 3, ARBDouble::eAsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"100") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, ARBDouble::eAsIs);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1, ARBDouble::eAsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"0") == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eAsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"00") == s);
}


TEST_CASE("Double")
{
	SECTION("strPrecUS")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
			REQUIRE(setlocale(LC_ALL, "english-us"));
#endif
			RunDblTests();
		}
	}


	SECTION("strPrecFR")
	{
		if (!g_bMicroTest)
		{
			// Using French since the default decimal separator is a comma.
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_FRENCH);
#else
			REQUIRE(setlocale(LC_ALL, "french"));
#endif
			RunDblTests();
		}
	}


	SECTION("equal")
	{
		if (!g_bMicroTest)
		{
			double p1 = 3.14159265;
			double p2 = 3.141592;
			double p3 = 3.141592657;
			REQUIRE(ARBDouble::equal(p1, p2, 0.00001));
			REQUIRE(!ARBDouble::equal(p1, p2, 0.00000001));
			REQUIRE(ARBDouble::equal(p1, p3, 0.00000001));
		}
	}
}
