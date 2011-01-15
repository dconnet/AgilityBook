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
 * @li 2009-10-30 DRC Add support for localized dates.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBDate.h"

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


	TEST(wxDate)
	{
		if (!g_bMicroTest)
		{
			ARBDate d(1999, 3, 27);
			wxDateTime wxD(d.GetDate());
			CHECK_EQUAL(1999, wxD.GetYear());
			CHECK_EQUAL(3, wxD.GetMonth() + 1); // 'Month' is an enum, starting at 0
			CHECK_EQUAL(27, wxD.GetDay());
		}
	}


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
			{
				wxLocale locale(wxLANGUAGE_ENGLISH_UK);
				CHECK(wxT("02/03/1999") == d.GetString(ARBDate::eCurrentLocale));
			}
			{
				wxLocale locale(wxLANGUAGE_ENGLISH_US);
#ifdef __WXMAC__
				CHECK(wxT("03/02/1999") == d.GetString(ARBDate::eCurrentLocale));
#else
				CHECK(wxT("3/2/1999") == d.GetString(ARBDate::eCurrentLocale));
#endif
			}
			CHECK(wxT("03-02-1999") == d.GetString(ARBDate::eDashMMDDYYYY));
			CHECK(wxT("03/02/1999") == d.GetString(ARBDate::eSlashMMDDYYYY));
			CHECK(wxT("1999-03-02") == d.GetString(ARBDate::eDashYYYYMMDD));
			CHECK(wxT("1999/03/02") == d.GetString(ARBDate::eSlashYYYYMMDD));
			CHECK(wxT("02-03-1999") == d.GetString(ARBDate::eDashDDMMYYYY));
			CHECK(wxT("02/03/1999") == d.GetString(ARBDate::eSlashDDMMYYYY));
			CHECK(wxT("3-2-1999") == d.GetString(ARBDate::eDashMDY));
			CHECK(wxT("3/2/1999") == d.GetString(ARBDate::eSlashMDY));
			CHECK(wxT("1999-3-2") == d.GetString(ARBDate::eDashYMD));
			CHECK(wxT("1999/3/2") == d.GetString(ARBDate::eSlashYMD));
			CHECK(wxT("2-3-1999") == d.GetString(ARBDate::eDashDMY));
			CHECK(wxT("2/3/1999") == d.GetString(ARBDate::eSlashDMY));
			CHECK(wxT("19990302") == d.GetString(ARBDate::eYYYYMMDD));
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
			ARBDate d = ARBDate::FromString(wxT("1999-3-27"), ARBDate::eDashYYYYMMDD);
			CHECK(d.IsValid());
			ARBDate d2(1999, 3, 27);
			CHECK(d == d2);
			d = ARBDate::FromString(wxT("1999-3-27"), ARBDate::eSlashYYYYMMDD); // Reading does not enforce 0-padding
			CHECK(!d.IsValid());
			//TODO: Add more complete tests (test each format, bad formats, etc)
		}
	}


	TEST(FromStringUK)
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
			wxLocale locale(wxLANGUAGE_ENGLISH_UK);
			ARBDate d = ARBDate::FromString(wxT("27/3/1999"), ARBDate::eCurrentLocale);
			CHECK(d.IsValid());
			CHECK(d == d2);
		}
	}


	TEST(FromStringUS)
	{
		if (!g_bMicroTest)
		{
			ARBDate d2(1999, 3, 27);
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
			ARBDate d = ARBDate::FromString(wxT("3/27/1999"), ARBDate::eCurrentLocale);
			CHECK(d.IsValid());
			CHECK(d == d2);
		}
	}


	TEST(ValidDateString)
	{
		if (!g_bMicroTest)
		{
			ARBDate d1(1999, 3, 30);
			ARBDate d2(1999, 3, 27);
			wxString s = ARBDate::GetValidDateString(d1, d2);
			CHECK(wxT("[1999-3-30-1999-3-27]") == s);
			d1.clear();
			s = ARBDate::GetValidDateString(d1, d2);
			CHECK(wxT("[*-1999-3-27]") == s);
			s = ARBDate::GetValidDateString(d2, d1);
			CHECK(wxT("[1999-3-27-*]") == s);
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


	TEST(DSTDate)
	{
		ARBDate d1(2010, 6, 1); // A date in DST
		ARBDate d2(2010, 12, 1); // A date not.
		time_t t1 = d1.GetDate();
		time_t t2 = d2.GetDate();
		CHECK(d1 == ARBDate(t1));
		CHECK(d2 == ARBDate(t2));
	}
}
