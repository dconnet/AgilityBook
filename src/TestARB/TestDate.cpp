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


SUITE(TestDate)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			ARBDate d;
			CHECK(!d.IsValid());
		}
	}


#if defined(__WXWINDOWS__)
	TEST(wxDate)
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 27);
			wxDateTime wxD;
			d.GetDate(wxD);
			CHECK_EQUAL(1999, wxD.GetYear());
			CHECK_EQUAL(3, wxD.GetMonth() + 1); // 'Month' is an enum, starting at 0
			CHECK_EQUAL(27, wxD.GetDay());
		}
	}
#endif


	TEST(Valid)
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 27);
			CHECK_EQUAL(1999, d.GetYear());
			CHECK_EQUAL(3, d.GetMonth());
			CHECK_EQUAL(27, d.GetDay());
		}
	}


	TEST(Bad)
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 42);
			CHECK(!d.IsValid());
		}
	}


	TEST(Equality)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 3, 27);
			CHECK(d1 == d2);
		}
	}


	TEST(Less)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 4, 27);
			CHECK(d1 < d2);
		}
	}


	TEST(Between)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 4, 27);
			ARBDate d3(1999, 5, 27);
			CHECK(d2.isBetween(d1, d3));
			CHECK(d2.isBetween(d3, d1));
			CHECK(!d1.isBetween(d2, d3));
			CHECK(!d1.isBetween(d3, d2));
		}
	}


	TEST(String)
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
				CHECK(setlocale(LC_ALL, "english-uk"));
#endif
				CHECK(L"02/03/1999" == d.GetString(ARBDate::eCurrentLocale));
			}
			{
#if defined(__WXWINDOWS__)
				wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
				CHECK(setlocale(LC_ALL, "english-us"));
#endif
#ifdef __WXMAC__
				CHECK(L"03/02/1999" == d.GetString(ARBDate::eCurrentLocale));
#else
				CHECK(L"3/2/1999" == d.GetString(ARBDate::eCurrentLocale));
#endif
			}
#endif
			CHECK(L"03-02-1999" == d.GetString(ARBDate::eDashMMDDYYYY));
			CHECK(L"03/02/1999" == d.GetString(ARBDate::eSlashMMDDYYYY));
			CHECK(L"1999-03-02" == d.GetString(ARBDate::eDashYYYYMMDD));
			CHECK(L"1999/03/02" == d.GetString(ARBDate::eSlashYYYYMMDD));
			CHECK(L"02-03-1999" == d.GetString(ARBDate::eDashDDMMYYYY));
			CHECK(L"02/03/1999" == d.GetString(ARBDate::eSlashDDMMYYYY));
			CHECK(L"3-2-1999" == d.GetString(ARBDate::eDashMDY));
			CHECK(L"3/2/1999" == d.GetString(ARBDate::eSlashMDY));
			CHECK(L"1999-3-2" == d.GetString(ARBDate::eDashYMD));
			CHECK(L"1999/3/2" == d.GetString(ARBDate::eSlashYMD));
			CHECK(L"2-3-1999" == d.GetString(ARBDate::eDashDMY));
			CHECK(L"2/3/1999" == d.GetString(ARBDate::eSlashDMY));
			CHECK(L"19990302" == d.GetString(ARBDate::eYYYYMMDD));
		}
	}


	TEST(Add)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 27);
			ARBDate d2(1999, 3, 30);
			long days = 3;
			ARBDate dt1 = d1 + days;
			CHECK(dt1 == d2);
			ARBDate dt2 = d1;
			dt2 += days;
			CHECK(dt2 == d2);
			CHECK(dt1 == dt2);
		}
	}


	TEST(Subtract)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 30);
			ARBDate d2(1999, 3, 27);
			long days = 3;
			ARBDate dt1 = d1 - days;
			CHECK(dt1 == d2);
			ARBDate dt2 = d1;
			dt2 -= days;
			CHECK(dt2 == d2);
			CHECK(dt1 == dt2);
		}
	}


	TEST(FromString)
	{
		if (!g_bMicroTest)
		{
			ARBDate d = ARBDate::FromString(L"1999-3-27", ARBDate::eDashYYYYMMDD);
			CHECK(d.IsValid());
			ARBDate d2(1999, 3, 27);
			CHECK(d == d2);
			d = ARBDate::FromString(L"1999-3-27", ARBDate::eSlashYYYYMMDD); // Reading does not enforce 0-padding
			CHECK(!d.IsValid());
			//TODO: Add more complete tests (test each format, bad formats, etc)
		}
	}


#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else
	TEST(FromStringUK)
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_ENGLISH_UK);
#else
			CHECK(setlocale(LC_ALL, "english-uk"));
#endif
			ARBDate d = ARBDate::FromString(L"27/3/1999", ARBDate::eCurrentLocale);
			CHECK(d.IsValid());
			CHECK(d == d2);
		}
	}
#endif


#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else
	TEST(FromStringUS)
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
#if defined(__WXWINDOWS__)
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
#else
			CHECK(setlocale(LC_ALL, "english-us"));
#endif
			ARBDate d = ARBDate::FromString(L"3/27/1999", ARBDate::eCurrentLocale);
			CHECK(d.IsValid());
			CHECK(d == d2);
		}
	}
#endif


	TEST(ValidDateString)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 30);
			ARBDate d2(1999, 3, 27);
			std::wstring s = ARBDate::GetValidDateString(d1, d2);
			CHECK(L"[1999-3-30-1999-3-27]" == s);
			d1.clear();
			s = ARBDate::GetValidDateString(d1, d2);
			CHECK(L"[*-1999-3-27]" == s);
			s = ARBDate::GetValidDateString(d2, d1);
			CHECK(L"[1999-3-27-*]" == s);
		}
	}


	TEST(DayOfWeek)
	{
		if (!g_bMicroTest)
		{
			ARBDate dSun(2009, 4, 12);
			ARBDate dMon(2009, 4, 13);
			CHECK(0 == dSun.GetDayOfWeek(ARBDate::eSunday));
			CHECK(1 == dMon.GetDayOfWeek(ARBDate::eSunday));
			CHECK(0 == dMon.GetDayOfWeek(ARBDate::eMonday));
		}
	}


	TEST(DayOfYear)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(2009, 1, 1);
			ARBDate d2(2009, 2, 1);
			ARBDate d3(2009, 12, 31);
			ARBDate d4(2004, 12, 31);
			CHECK(1 == d1.GetDayOfYear());
			CHECK(32 == d2.GetDayOfYear());
			CHECK(365 == d3.GetDayOfYear());
			CHECK(366 == d4.GetDayOfYear());
		}
	}


	TEST(DSTDate)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(2010, 6, 1); // A date in DST
			ARBDate d2(2010, 12, 1); // A date not.
			time_t t1;
			d1.GetDate(t1);
			time_t t2;
			d2.GetDate(t2);
			CHECK(d1 == ARBDate(t1));
			CHECK(d2 == ARBDate(t2));
		}
	}

	TEST(LeapYear)
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
			for (size_t i = 0; i < ARRAY_SIZE(sDates); ++i)
			{
				ARBDate d1(sDates[i].yr, 1, 1);
				CHECK(d1.isLeap() == sDates[i].isLeep);
			}
		}
	}
}
