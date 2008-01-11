/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBDate.h"


BEGIN_TEST(Date_New)
{
	ARBDate d;
	WIN_ASSERT_FALSE(d.IsValid());
}
END_TEST


BEGIN_TEST(Date_Valid)
{
	ARBDate d(1999, 3, 27);
	WIN_ASSERT_EQUAL(1999, d.GetYear());
	WIN_ASSERT_EQUAL(3, d.GetMonth());
	WIN_ASSERT_EQUAL(27, d.GetDay());
}
END_TEST


BEGIN_TEST(Date_Bad)
{
	ARBDate d(1999, 3, 42);
	WIN_ASSERT_FALSE(d.IsValid());
}
END_TEST


BEGIN_TEST(Date_Equality)
{
	ARBDate d1(1999, 3, 27);
	ARBDate d2(1999, 3, 27);
	WIN_ASSERT_EQUAL(d1, d2);
}
END_TEST


BEGIN_TEST(Date_Less)
{
	ARBDate d1(1999, 3, 27);
	ARBDate d2(1999, 4, 27);
	WIN_ASSERT_TRUE(d1 < d2);
}
END_TEST


BEGIN_TEST(Date_Between)
{
	ARBDate d1(1999, 3, 27);
	ARBDate d2(1999, 4, 27);
	ARBDate d3(1999, 5, 27);
	WIN_ASSERT_TRUE(d2.isBetween(d1, d3));
	WIN_ASSERT_TRUE(d2.isBetween(d3, d1));
	WIN_ASSERT_FALSE(d1.isBetween(d2, d3));
	WIN_ASSERT_FALSE(d1.isBetween(d3, d2));
}
END_TEST


BEGIN_TEST(Date_String)
{
	ARBDate d(1999, 3, 2);
	WIN_ASSERT_STRING_EQUAL(_T("03/02/1999"), d.GetString(ARBDate::eDefault).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("03-02-1999"), d.GetString(ARBDate::eDashMMDDYYYY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("03/02/1999"), d.GetString(ARBDate::eSlashMMDDYYYY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("1999-03-02"), d.GetString(ARBDate::eDashYYYYMMDD).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("1999/03/02"), d.GetString(ARBDate::eSlashYYYYMMDD).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("02-03-1999"), d.GetString(ARBDate::eDashDDMMYYYY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("02/03/1999"), d.GetString(ARBDate::eSlashDDMMYYYY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("3-2-1999"), d.GetString(ARBDate::eDashMDY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("3/2/1999"), d.GetString(ARBDate::eSlashMDY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("1999-3-2"), d.GetString(ARBDate::eDashYMD).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("1999/3/2"), d.GetString(ARBDate::eSlashYMD).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("2-3-1999"), d.GetString(ARBDate::eDashDMY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("2/3/1999"), d.GetString(ARBDate::eSlashDMY).c_str());
	WIN_ASSERT_STRING_EQUAL(_T("19990302"), d.GetString(ARBDate::eYYYYMMDD).c_str());
}
END_TEST


BEGIN_TEST(Date_Add)
{
	ARBDate d1(1999, 3, 27);
	ARBDate d2(1999, 3, 30);
	long days = 3;
	ARBDate dt1 = d1 + days;
	WIN_ASSERT_EQUAL(dt1, d2);
	ARBDate dt2 = d1;
	dt2 += days;
	WIN_ASSERT_EQUAL(dt2, d2);
	WIN_ASSERT_EQUAL(dt1, dt2);
}
END_TEST


BEGIN_TEST(Date_Subtract)
{
	ARBDate d1(1999, 3, 30);
	ARBDate d2(1999, 3, 27);
	long days = 3;
	ARBDate dt1 = d1 - days;
	WIN_ASSERT_EQUAL(dt1, d2);
	ARBDate dt2 = d1;
	dt2 -= days;
	WIN_ASSERT_EQUAL(dt2, d2);
	WIN_ASSERT_EQUAL(dt1, dt2);
}
END_TEST
