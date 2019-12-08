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
 * 2019-12-08 Add fmt number formatting tests.
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2015-12-22 Added tests for changed ARBDouble parameter.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-12 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/format.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


static std::wstring FormNumber(std::wstring const& d1, std::wstring const& dec, std::wstring const& d2)
{
	return fmt::format(L"{}{}{}", d1, dec, d2);
}


static void RunDblTests(bool bUseLocale)
{
	std::wstring decimalPt = L".";
	if (bUseLocale)
		decimalPt = CLocaleWrapper::GetDecimalPt();

	// ARBDouble always strips 0s unless prec ==2, unless =".00"
	double p = 3.14159265358979323846;
	std::wstring s = ARBDouble::ToString(p, 2, bUseLocale);
	REQUIRE(FormNumber(L"3", decimalPt, L"14") == s);
	s = ARBDouble::ToString(p, 4, bUseLocale);
	REQUIRE(FormNumber(L"3", decimalPt, L"1416") == s);

	// eCompatible

	p = 2.1;
	s = ARBDouble::ToString(p, 0, bUseLocale);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale);
	REQUIRE(FormNumber(L"2", decimalPt, L"10") == s);
	s = ARBDouble::ToString(p, 3, bUseLocale);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, bUseLocale);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1, bUseLocale);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 2, bUseLocale);
	REQUIRE(L"2" == s);

	p = 42;
	s = ARBDouble::ToString(p, 0, bUseLocale);
	REQUIRE(std::wstring(L"42") == s);
	s = ARBDouble::ToString(p, 1, bUseLocale);
	REQUIRE(std::wstring(L"42") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale);
	REQUIRE(std::wstring(L"42") == s);

	p = 2002; // ARB alpha 1 release year!
	s = ARBDouble::ToString(p, 0, bUseLocale);
	REQUIRE(std::wstring(L"2002") == s);
	s = ARBDouble::ToString(p, 1, bUseLocale);
	REQUIRE(std::wstring(L"2002") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale);
	REQUIRE(std::wstring(L"2002") == s);

	// eStrip

	p = 2.1;
	s = ARBDouble::ToString(p, 0, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 3, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 2, bUseLocale, ARBDouble::ZeroStrip::Strip);
	REQUIRE(L"2" == s);

	// eAsIs

	p = 2.1;
	s = ARBDouble::ToString(p, 0, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"1") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"10") == s);
	s = ARBDouble::ToString(p, 3, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"100") == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(L"2" == s);
	s = ARBDouble::ToString(p, 1, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"0") == s);
	s = ARBDouble::ToString(p, 2, bUseLocale, ARBDouble::ZeroStrip::AsIs);
	REQUIRE(FormNumber(L"2", decimalPt, L"00") == s);
}


TEST_CASE("Double")
{
	SECTION("fmtCheck")
	{
		static const struct
		{
			double val;
			int prec;
			wchar_t const* expected;
			wchar_t const* format;
			wchar_t const* formatv6;
		} tests[] = {
			{42.34, 0, L"42.34", L"{:g}",    L"{:n}"},
			{42.0,  0, L"42",    L"{:g}",    L"{:n}"},
			{42.34, 1, L"42.3",  L"{:.{}f}", L"{:.{}n}"},
			{42.0,  1, L"42.0",  L"{:.{}f}", L"{:.{}n}"},
		};

		for (auto& test : tests)
		{
			std::wstring s;
			if (test.prec > 0)
				s = fmt::format(test.format, test.val, test.prec);
			else
				s = fmt::format(test.format, test.val);
			REQUIRE(s == test.expected);
		}

		if (FMT_VERSION >= 60000)
		{
			// FMT v6: Formatting changed to locale-independent.
			// New 'n' option is required for locale-specific formatting.
			for (auto& test : tests)
			{
				std::wstring s;
				if (test.prec > 0)
					s = fmt::format(test.formatv6, test.val, test.prec);
				else
					s = fmt::format(test.formatv6, test.val);
				REQUIRE(s == test.expected);
			}
		}
	}


	SECTION("strPrecUS")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__) && !USE_CRT
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
			CLocaleWrapper locale(LC_ALL, "english-us");
#endif
			RunDblTests(true);
			RunDblTests(false);
		}
	}


	SECTION("strPrecFR")
	{
		if (!g_bMicroTest)
		{
			// Using French since the default decimal separator is a comma.
#if defined(__WXWINDOWS__) && !USE_CRT
			wxLocale locale(wxLANGUAGE_FRENCH);
#else
			CLocaleWrapper locale(LC_ALL, "french");
#endif
			RunDblTests(true);
			RunDblTests(false);
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
