/*
 * Copyright � 2008 David Connet. All Rights Reserved.
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
 * @brief Test ARBCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCalendar.h"
#include "ARBStructure.h"
#include "Element.h"

FIXTURE(Calendar)


namespace
{
	ElementNodePtr CalData1; // v1.0
	ElementNodePtr CalData2; // v2.0+
	ElementNodePtr CalData1a; // v1.0 with v2 data
}


static ElementNodePtr CreateNode()
{
	ElementNodePtr data = ElementNode::New(TREE_CALENDAR);
	data->SetValue(_T("These are some notes"));
	data->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 3, 4));
	data->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 3, 5));
	data->AddAttrib(ATTRIB_CAL_LOCATION, _T("Sunnyvale, CA"));
	data->AddAttrib(ATTRIB_CAL_CLUB, _T("Bay Team"));
	data->AddAttrib(ATTRIB_CAL_VENUE, _T("USDAA"));
	return data;
}


SETUP(Calendar)
{
	WIN_ASSERT_TRUE(CommonSetup());

	CalData1 = CreateNode();
	CalData1a = CreateNode();
	CalData2 = CreateNode();

	// Specialize the v1/v2 objs.
	CalData1->AddAttrib(_T("PlanOn"), _T("y"));
	CalData1a->AddAttrib(ATTRIB_CAL_ENTERED, _T("P"));
	CalData2->AddAttrib(ATTRIB_CAL_ENTERED, _T("P"));
}


TEARDOWN(Calendar)
{
	CalData1.reset();
	CalData2.reset();
	CalData1a.reset();

	WIN_ASSERT_TRUE(CommonTeardown());
}


BEGIN_TEST(Calendar_New)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	WIN_ASSERT_NOT_NULL(cal.get());
}
END_TEST


BEGIN_TEST(Calendar_Clone)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	cal->SetStartDate(ARBDate(2007, 9, 1));
	cal->SetLocation(_T("Here"));
	ARBCalendarPtr cal2 = cal->Clone();
	WIN_ASSERT_NOT_NULL(cal2.get());
	WIN_ASSERT_NOT_EQUAL(cal.get(), cal2.get());
	WIN_ASSERT_EQUAL(*cal, *cal2);
	WIN_ASSERT_EQUAL(cal->GetStartDate(), cal2->GetStartDate());
	WIN_ASSERT_EQUAL(cal->GetLocation(), cal2->GetLocation());
	cal->SetLocation(_T("Here2"));
	WIN_ASSERT_NOT_EQUAL(cal->GetLocation(), cal2->GetLocation());
}
END_TEST


BEGIN_TEST(Calendar_OpEqual)
{
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2007, 9, 1));
	cal1->SetLocation(_T("Here"));
	ARBCalendarPtr cal2 = ARBCalendar::New();
	WIN_ASSERT_NOT_EQUAL(*cal1, *cal2);
	*cal1 = *cal2;
	WIN_ASSERT_EQUAL(*cal1, *cal2);
}
END_TEST


BEGIN_TESTF(Calendar_Compare, Calendar)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	tstring errs;
	ARBErrorCallback callback(errs);
    cal->Load(CalData2, ARBVersion(2, 0), callback);
	ARBCalendarPtr cal2 = cal->Clone();
	WIN_ASSERT_NOT_EQUAL(cal.get(), cal2.get());
	WIN_ASSERT_EQUAL(*cal, *cal2);
	ARBDate n = ARBDate::Today();
	cal2->SetStartDate(n);
	cal2->SetEndDate(n+1);
	WIN_ASSERT_TRUE(*cal < *cal2);
	WIN_ASSERT_TRUE(*cal < cal2->GetStartDate());
	WIN_ASSERT_NOT_EQUAL(*cal, *cal2);
	WIN_ASSERT_FALSE(*cal > *cal2);
	WIN_ASSERT_FALSE(*cal > cal2->GetStartDate());
}
END_TESTF


BEGIN_TEST(Calendar_GenName)
{
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetClub(_T("Way"));
	cal1->SetVenue(_T("No"));
	cal1->SetLocation(_T("There"));
	WIN_ASSERT_STRING_EQUAL(_T("No Way There"), cal1->GetGenericName().c_str());
}
END_TEST


BEGIN_TESTF(Calendar_Load1, Calendar)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(cal->Load(CalData1, ARBVersion(1, 0), callback));
	WIN_ASSERT_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
	tstring name = cal->GetGenericName();
	WIN_ASSERT_FALSE(name.empty());
}
END_TESTF


BEGIN_TESTF(Calendar_Load2, Calendar)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(cal->Load(CalData2, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
}
END_TESTF


BEGIN_TESTF(Calendar_Load3, Calendar)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(cal->Load(CalData1a, ARBVersion(1, 0), callback));
	// These are not equal as 'Entered' isn't parsed in 1.0.
	WIN_ASSERT_NOT_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
}
END_TESTF


BEGIN_TESTF(Calendar_Save, Calendar)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	tstring errs;
	ARBErrorCallback callback(errs);
    cal->Load(CalData2, ARBVersion(2, 0), callback);
	ElementNodePtr ele = ElementNode::New();
	WIN_ASSERT_TRUE(cal->Save(ele));
}
END_TESTF


BEGIN_TEST(Calendar_IsBefore)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	cal->SetStartDate(ARBDate(2007, 9, 1));
	cal->SetEndDate(ARBDate(2007, 9, 3));
	WIN_ASSERT_FALSE(cal->IsBefore(ARBDate(2007, 8, 31)));
	WIN_ASSERT_FALSE(cal->IsBefore(cal->GetStartDate()));
	WIN_ASSERT_FALSE(cal->IsBefore(cal->GetEndDate()));
	WIN_ASSERT_TRUE(cal->IsBefore(ARBDate(2007, 9, 4)));
}
END_TEST


BEGIN_TEST(Calendar_InRange)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	cal->SetStartDate(ARBDate(2007, 9, 1));
	cal->SetEndDate(ARBDate(2007, 9, 3));
	WIN_ASSERT_TRUE(cal->InRange(cal->GetStartDate()));
	WIN_ASSERT_TRUE(cal->InRange(cal->GetEndDate()));
	WIN_ASSERT_TRUE(cal->InRange(ARBDate(2007, 9, 2)));
	WIN_ASSERT_FALSE(cal->InRange(ARBDate(2007, 9, 4)));
}
END_TEST


BEGIN_TEST(Calendar_IsRangeOverlapped)
{
	ARBCalendarPtr cal = ARBCalendar::New();
	cal->SetStartDate(ARBDate(2007, 9, 1));
	cal->SetEndDate(ARBDate(2007, 9, 3));
	WIN_ASSERT_TRUE(cal->IsRangeOverlapped(cal->GetStartDate(), cal->GetEndDate()));
	ARBDate d1(2007, 8, 31);
	ARBDate d2(2007, 9, 2);
	ARBDate d3(2007, 9, 4);
	ARBDate d4(2007, 9, 5);
	WIN_ASSERT_TRUE(cal->IsRangeOverlapped(d1, d2));
	WIN_ASSERT_TRUE(cal->IsRangeOverlapped(d2, d3));
	WIN_ASSERT_FALSE(cal->IsRangeOverlapped(d3, d4));
	WIN_ASSERT_TRUE(cal->IsRangeOverlapped(d1, d4));
}
END_TEST


BEGIN_TEST(Calendar_IsMatch)
{
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2007, 9, 1));
	cal1->SetEndDate(ARBDate(2007, 9, 3));
	cal1->SetVenue(_T("AKC"));
	cal1->SetClub(_T("club"));
	cal1->SetLocation(_T("There"));

	ARBCalendarPtr cal2;

	WIN_ASSERT_FALSE(cal1->IsMatch(cal2, true));

	cal2 = cal1->Clone();
	cal2->SetLocation(_T("Here"));
	WIN_ASSERT_FALSE(cal1->IsMatch(cal2, true));
	WIN_ASSERT_TRUE(cal1->IsMatch(cal2, false));
	cal2->SetEndDate(cal2->GetEndDate() + 1);
	WIN_ASSERT_FALSE(cal1->IsMatch(cal2, false));
}
END_TEST


BEGIN_TEST(Calendar_Update)
{
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2006, 9, 4));
	cal1->SetEndDate(ARBDate(2006, 9, 5));
	cal1->SetLocation(_T("Hollister"));
	cal1->SetClub(_T("PASA"));
	cal1->SetVenue(_T("ASCA"));
	ARBCalendarPtr cal2 = cal1->Clone();
	cal1->SetEndDate(ARBDate(2006, 9, 6));
	cal1->SetLocation(_T("Turlock"));
	cal1->SetClub(_T("PASA"));
	cal1->SetVenue(_T("ASCA"));
	WIN_ASSERT_NOT_EQUAL(*cal1, *cal2);
	WIN_ASSERT_TRUE(cal1->Update(cal2));
	WIN_ASSERT_EQUAL(*cal1, *cal2);
}
END_TEST


BEGIN_TESTF(CalendarList_Load, Calendar)
{
	ARBCalendarList callist;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(callist.Load(CalData1, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(callist.Load(CalData2, ARBVersion(2, 0), callback));
	ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
	ele->SetValue(_T("These are some notes"));
	ele->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 9, 4));
	ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
	ele->AddAttrib(ATTRIB_CAL_LOCATION, _T("Hollister, CA"));
	ele->AddAttrib(ATTRIB_CAL_CLUB, _T("PASA"));
	ele->AddAttrib(ATTRIB_CAL_VENUE, _T("ASCA"));
	ele->AddAttrib(_T("PlanOn"), _T("n"));
	WIN_ASSERT_FALSE(callist.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(2u, callist.size());
}
END_TESTF


BEGIN_TESTF(CalendarList_Load2, Calendar)
{
	ARBCalendarList callist;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(callist.Load(CalData1, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(callist.Load(CalData2, ARBVersion(2, 0), callback));
	ElementNodePtr ele = ElementNode::New(TREE_CALENDAR);
	ele->SetValue(_T("These are some notes"));
	ele->AddAttrib(_T("DateStrt"), ARBDate(2006, 9, 4));
	ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
	ele->AddAttrib(ATTRIB_CAL_LOCATION, _T("Hollister, CA"));
	ele->AddAttrib(ATTRIB_CAL_CLUB, _T("PASA"));
	ele->AddAttrib(ATTRIB_CAL_VENUE, _T("ASCA"));
	ele->AddAttrib(_T("PlanOn"), _T("n"));
	WIN_ASSERT_FALSE(callist.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(2u, callist.size());
}
END_TESTF


BEGIN_TEST(CalendarList_sort)
{
	ARBCalendarList callist;
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2006, 9, 4));
	cal1->SetEndDate(ARBDate(2006, 9, 5));
	cal1->SetLocation(_T("Hollister"));
	cal1->SetClub(_T("PASA"));
	cal1->SetVenue(_T("ASCA"));
	ARBCalendarPtr cal2 = cal1->Clone();
	cal2->SetStartDate(ARBDate(2005, 3, 26));
	cal2->SetEndDate(ARBDate(2005, 3, 27));
	callist.AddCalendar(cal1);
	callist.AddCalendar(cal2);

	ARBCalendarList callist2;
	callist.Clone(callist2);
	WIN_ASSERT_EQUAL(callist, callist2);
	callist.sort();
	WIN_ASSERT_NOT_EQUAL(callist, callist2);
}
END_TEST


BEGIN_TEST(CalendarList_Entered)
{
	ARBCalendarList callist;
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2005, 3, 26));
	cal1->SetEndDate(ARBDate(2005, 3, 27));
	callist.AddCalendar(cal1);
	ARBCalendarPtr cal2 = ARBCalendar::New();
	cal2->SetStartDate(ARBDate(2005, 4, 26));
	cal2->SetEndDate(ARBDate(2005, 4, 27));
	callist.AddCalendar(cal2);
	std::vector<ARBCalendarPtr> lst;
	WIN_ASSERT_EQUAL(0u, callist.GetAllEntered(lst));
	cal1->SetEntered(ARBCalendar::ePlanning);
	WIN_ASSERT_EQUAL(0u, callist.GetAllEntered(lst));
	cal1->SetEntered(ARBCalendar::eEntered);
	WIN_ASSERT_EQUAL(1u, callist.GetAllEntered(lst));
}
END_TEST


BEGIN_TEST(CalendarList_Trim)
{
	ARBCalendarList callist;
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2005, 3, 26));
	cal1->SetEndDate(ARBDate(2005, 3, 27));
	callist.AddCalendar(cal1);
	ARBCalendarPtr cal2 = ARBCalendar::New();
	cal2->SetStartDate(ARBDate(2005, 4, 26));
	cal2->SetEndDate(ARBDate(2005, 4, 27));
	callist.AddCalendar(cal2);
	callist.TrimEntries(ARBDate(2005, 3, 25));
	WIN_ASSERT_EQUAL(2u, callist.size());
	callist.TrimEntries(ARBDate(2005, 3, 27));
	WIN_ASSERT_EQUAL(2u, callist.size());
	callist.TrimEntries(ARBDate(2005, 3, 28));
	WIN_ASSERT_EQUAL(1u, callist.size());
}
END_TEST


BEGIN_TEST(CalendarList_Find)
{
	ARBCalendarList callist;
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2006, 9, 4));
	cal1->SetEndDate(ARBDate(2006, 9, 5));
	cal1->SetLocation(_T("Hollister"));
	cal1->SetClub(_T("PASA"));
	cal1->SetVenue(_T("ASCA"));
	ARBCalendarPtr cal2 = cal1->Clone();
	cal2->SetStartDate(ARBDate(2005, 3, 26));
	cal2->SetEndDate(ARBDate(2005, 3, 27));
	callist.AddCalendar(cal1);
	callist.AddCalendar(cal2);
	callist.sort();
	ARBCalendarPtr found;
	WIN_ASSERT_TRUE(callist.FindCalendar(cal1, true, &found));
	WIN_ASSERT_NOT_NULL(found.get());
	WIN_ASSERT_EQUAL(found.get(), cal1.get());
	ARBCalendarPtr clone = found->Clone();
	WIN_ASSERT_TRUE(callist.FindCalendar(clone, true, &found));
	WIN_ASSERT_NOT_NULL(found.get());
	WIN_ASSERT_NOT_EQUAL(found.get(), clone.get());
	WIN_ASSERT_EQUAL(*found, *clone);
}
END_TEST


BEGIN_TEST(CalendarList_AddDelete)
{
	ARBCalendarList callist;
	ARBCalendarPtr cal1 = ARBCalendar::New();
	cal1->SetStartDate(ARBDate(2006, 9, 4));
	cal1->SetEndDate(ARBDate(2006, 9, 5));
	cal1->SetLocation(_T("Hollister"));
	cal1->SetClub(_T("PASA"));
	cal1->SetVenue(_T("ASCA"));
	ARBCalendarPtr cal2 = cal1->Clone();
	cal2->SetStartDate(ARBDate(2005, 3, 26));
	cal2->SetEndDate(ARBDate(2005, 3, 27));
	callist.AddCalendar(cal1);
	callist.AddCalendar(cal2);
	callist.sort();
	ARBCalendarPtr cal3 = callist[0]->Clone();
	WIN_ASSERT_EQUAL(*cal2, *cal3);
	cal3->SetNote(_T("Test"));
	WIN_ASSERT_NOT_EQUAL(*cal2, *cal3);
	callist.AddCalendar(cal3);
	callist.AddCalendar(cal1->Clone());
	callist.sort();
	WIN_ASSERT_EQUAL(4u, callist.size());
	WIN_ASSERT_TRUE(callist.DeleteCalendar(cal1));
	WIN_ASSERT_EQUAL(3u, callist.size());
	WIN_ASSERT_TRUE(callist.DeleteCalendar(cal1));
	WIN_ASSERT_EQUAL(2u, callist.size());
	WIN_ASSERT_FALSE(callist.DeleteCalendar(cal1));
	WIN_ASSERT_EQUAL(2u, callist.size());
	WIN_ASSERT_NOT_EQUAL(*callist[0], *callist[1]);
	ARBCalendarList callist2;
	callist.Clone(callist2);
	WIN_ASSERT_EQUAL(callist, callist2);
	WIN_ASSERT_NOT_EQUAL(callist[0].get(), callist2[0].get());
	WIN_ASSERT_EQUAL(*callist[0], *callist2[0]);
}
END_TEST
