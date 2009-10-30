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


SUITE(TestDate)
{
	TEST(New)
	{
		ARBDate d;
		CHECK(!d.IsValid());
	}


	TEST(wxDate)
	{
		ARBDate d(1999, 3, 27);
		wxDateTime wxD(d.GetDate());
		CHECK_EQUAL(1999, wxD.GetYear());
		CHECK_EQUAL(3, wxD.GetMonth() + 1); // 'Month' is an enum, starting at 0
		CHECK_EQUAL(27, wxD.GetDay());
	}


	TEST(Valid)
	{
		ARBDate d(1999, 3, 27);
		CHECK_EQUAL(1999, d.GetYear());
		CHECK_EQUAL(3, d.GetMonth());
		CHECK_EQUAL(27, d.GetDay());
	}


	TEST(Bad)
	{
		ARBDate d(1999, 3, 42);
		CHECK(!d.IsValid());
	}


	TEST(Equality)
	{
		ARBDate d1(1999, 3, 27);
		ARBDate d2(1999, 3, 27);
		CHECK(d1 == d2);
	}


	TEST(Less)
	{
		ARBDate d1(1999, 3, 27);
		ARBDate d2(1999, 4, 27);
		CHECK(d1 < d2);
	}


	TEST(Between)
	{
		ARBDate d1(1999, 3, 27);
		ARBDate d2(1999, 4, 27);
		ARBDate d3(1999, 5, 27);
		CHECK(d2.isBetween(d1, d3));
		CHECK(d2.isBetween(d3, d1));
		CHECK(!d1.isBetween(d2, d3));
		CHECK(!d1.isBetween(d3, d2));
	}


	TEST(String)
	{
		ARBDate d(1999, 3, 2);
		{
			wxLocale locale(wxLANGUAGE_ENGLISH_UK);
			CHECK(wxT("02/03/1999") == d.GetString(ARBDate::eCurrentLocale));
		}
		{
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
			CHECK(wxT("3/2/1999") == d.GetString(ARBDate::eCurrentLocale));
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


	TEST(Add)
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


	TEST(Subtract)
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


	TEST(FromString)
	{
		ARBDate d = ARBDate::FromString(wxT("1999-3-27"), ARBDate::eDashYYYYMMDD);
		CHECK(d.IsValid());
		ARBDate d2(1999, 3, 27);
		CHECK(d == d2);
		d = ARBDate::FromString(wxT("1999-3-27"), ARBDate::eSlashYYYYMMDD); // Reading does not enforce 0-padding
		CHECK(!d.IsValid());
		//TODO: Add more complete tests (test each format, bad formats, etc)
	}


	TEST(FromStringUK)
	{
		ARBDate d2(1999, 3, 27);
		wxLocale locale(wxLANGUAGE_ENGLISH_UK);
		ARBDate d = ARBDate::FromString(wxT("27/3/1999"), ARBDate::eCurrentLocale);
		CHECK(d.IsValid());
		CHECK(d == d2);
	}


	TEST(FromStringUS)
	{
		ARBDate d2(1999, 3, 27);
		wxLocale locale(wxLANGUAGE_ENGLISH_US);
		ARBDate d = ARBDate::FromString(wxT("3/27/1999"), ARBDate::eCurrentLocale);
		CHECK(d.IsValid());
		CHECK(d == d2);
	}


	TEST(ValidDateString)
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


	TEST(DayOfWeek)
	{
		ARBDate dSun(2009, 4, 12);
		ARBDate dMon(2009, 4, 13);
		CHECK(0 == dSun.GetDayOfWeek(ARBDate::eSunday));
		CHECK(1 == dMon.GetDayOfWeek(ARBDate::eSunday));
		CHECK(0 == dMon.GetDayOfWeek(ARBDate::eMonday));
	}
}
