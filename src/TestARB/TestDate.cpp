/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDate class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2013-10-07 Added leap year tests.
 * 2012-10-26 Changed ARBDate::GetTime to avoid time_t when possible.
 * 2009-10-30 Add support for localized dates.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/ARBDate.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("Date")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			ARBDate d;
			REQUIRE(!d.IsValid());
		}
	}


#if defined(__WXWINDOWS__)
	SECTION("wxDate")
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 27);
			wxDateTime wxD;
			d.GetDate(wxD);
			REQUIRE(1999 == wxD.GetYear());
			REQUIRE(3 == wxD.GetMonth() + 1); // 'Month' is an enum, starting at 0
			REQUIRE(27 == wxD.GetDay());
		}
	}
#endif


	SECTION("Valid")
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 27);
			REQUIRE(1999 == d.GetYear());
			REQUIRE(3 == d.GetMonth());
			REQUIRE(27 == d.GetDay());
		}
	}


	SECTION("Bad")
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 42);
			REQUIRE(!d.IsValid());
		}
	}


	SECTION("Equality")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 3, 27);
			REQUIRE(d1 == d2);
		}
	}


	SECTION("Less")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 4, 27);
			REQUIRE(d1 < d2);
		}
	}


	SECTION("Between")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 4, 27);
			ARBDate d3(1999, 5, 27);
			REQUIRE(d2.isBetween(d1, d3));
			REQUIRE(d2.isBetween(d3, d1));
			REQUIRE(!d1.isBetween(d2, d3));
			REQUIRE(!d1.isBetween(d3, d2));
		}
	}


	SECTION("String")
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 2);
#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else
			{
#if defined(__WXWINDOWS__)
				wxLocale locale(wxLANGUAGE_ENGLISH_UK);
#else
				CLocaleWrapper locale(LC_ALL, "english-uk");
#endif
				REQUIRE(L"02/03/1999" == d.GetString(ARBDate::eLocale));
			}
			{
#if defined(__WXWINDOWS__)
				wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
				CLocaleWrapper locale(LC_ALL, "english-us");
#endif
#ifdef __WXMAC__
				REQUIRE(L"03/02/1999" == d.GetString(ARBDate::eLocale));
#else
				REQUIRE(L"3/2/1999" == d.GetString(ARBDate::eLocale));
#endif
			}
#endif
			REQUIRE(L"03-02-1999" == d.GetString(ARBDate::eDashMMDDYYYY));
			REQUIRE(L"03/02/1999" == d.GetString(ARBDate::eSlashMMDDYYYY));
			REQUIRE(L"1999-03-02" == d.GetString(ARBDate::eDashYYYYMMDD));
			REQUIRE(L"1999/03/02" == d.GetString(ARBDate::eSlashYYYYMMDD));
			REQUIRE(L"02-03-1999" == d.GetString(ARBDate::eDashDDMMYYYY));
			REQUIRE(L"02/03/1999" == d.GetString(ARBDate::eSlashDDMMYYYY));
			REQUIRE(L"3-2-1999" == d.GetString(ARBDate::eDashMDY));
			REQUIRE(L"3/2/1999" == d.GetString(ARBDate::eSlashMDY));
			REQUIRE(L"1999-3-2" == d.GetString(ARBDate::eDashYMD));
			REQUIRE(L"1999/3/2" == d.GetString(ARBDate::eSlashYMD));
			REQUIRE(L"2-3-1999" == d.GetString(ARBDate::eDashDMY));
			REQUIRE(L"2/3/1999" == d.GetString(ARBDate::eSlashDMY));
			REQUIRE(L"19990302" == d.GetString(ARBDate::eYYYYMMDD));
		}
	}


	SECTION("Add")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 3, 30);
			long days = 3;
			ARBDate dt1 = d1 + days;
			REQUIRE(dt1 == d2);
			ARBDate dt2 = d1;
			dt2 += days;
			REQUIRE(dt2 == d2);
			REQUIRE(dt1 == dt2);
		}
	}


	SECTION("Subtract")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 30);
			ARBDate d2(1999, 3, 27);
			long days = 3;
			ARBDate dt1 = d1 - days;
			REQUIRE(dt1 == d2);
			ARBDate dt2 = d1;
			dt2 -= days;
			REQUIRE(dt2 == d2);
			REQUIRE(dt1 == dt2);
		}
	}


	SECTION("FromString")
	{
		if (!g_bMicroTest)
		{
			ARBDate d = ARBDate::FromString(L"1999-3-27", ARBDate::eDashYYYYMMDD);
			REQUIRE(d.IsValid());
			ARBDate d2(1999, 3, 27);
			REQUIRE(d == d2);
			d = ARBDate::FromString(L"1999-3-27", ARBDate::eSlashYYYYMMDD); // Reading does not enforce 0-padding
			REQUIRE(!d.IsValid());
			//TODO: Add more complete tests (test each format, bad formats, etc)
		}
	}


#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else
	SECTION("FromStringUK")
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_ENGLISH_UK);
#else
			CLocaleWrapper locale(LC_ALL, "english-uk");
#endif
			ARBDate d = ARBDate::FromString(L"27/3/1999", ARBDate::eLocale);
			REQUIRE(d.IsValid());
			REQUIRE(d == d2);
		}
	}
#endif


#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else
	SECTION("FromStringUS")
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
			CLocaleWrapper locale(LC_ALL, "english-us");
#endif
			ARBDate d = ARBDate::FromString(L"3/27/1999", ARBDate::eLocale);
			REQUIRE(d.IsValid());
			REQUIRE(d == d2);
		}
	}
#endif


	SECTION("ValidDateString")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 30);
			ARBDate d2(1999, 3, 27);
			std::wstring s = ARBDate::GetValidDateString(d1, d2);
			REQUIRE(L"[1999-3-30-1999-3-27]" == s);
			d1.clear();
			s = ARBDate::GetValidDateString(d1, d2);
			REQUIRE(L"[*-1999-3-27]" == s);
			s = ARBDate::GetValidDateString(d2, d1);
			REQUIRE(L"[1999-3-27-*]" == s);
		}
	}


	SECTION("DayOfWeek")
	{
		if (!g_bMicroTest)
		{
			ARBDate dSun(2009, 4, 12);
			ARBDate dMon(2009, 4, 13);
			REQUIRE(0 == dSun.GetDayOfWeek(ARBDate::eSunday));
			REQUIRE(1 == dMon.GetDayOfWeek(ARBDate::eSunday));
			REQUIRE(0 == dMon.GetDayOfWeek(ARBDate::eMonday));
		}
	}


	SECTION("DayOfYear")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(2009, 1, 1);
			ARBDate d2(2009, 2, 1);
			ARBDate d3(2009, 12, 31);
			ARBDate d4(2004, 12, 31);
			REQUIRE(1 == d1.GetDayOfYear());
			REQUIRE(32 == d2.GetDayOfYear());
			REQUIRE(365 == d3.GetDayOfYear());
			REQUIRE(366 == d4.GetDayOfYear());
		}
	}


	SECTION("DSTDate")
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(2010, 6, 1); // A date in DST
			ARBDate d2(2010, 12, 1); // A date not.
			time_t t1;
			d1.GetDate(t1);
			time_t t2;
			d2.GetDate(t2);
			REQUIRE(d1 == ARBDate(t1));
			REQUIRE(d2 == ARBDate(t2));
		}
	}


	SECTION("LeapYear")
	{
		if (!g_bMicroTest)
		{
			static struct
			{
				int yr;
				bool isLeep;
			} sDates[] = {
				{1900, false},
				{2000, true},
				{2001, false},
				{2004, true},
			};
			for (size_t i = 0; i < _countof(sDates); ++i)
			{
				ARBDate d1(sDates[i].yr, 1, 1);
				REQUIRE(d1.isLeap() == sDates[i].isLeep);
			}
		}
	}
}
