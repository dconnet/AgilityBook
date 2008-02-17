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


struct CalData
{
	ElementNodePtr CalData1; // v1.0
	ElementNodePtr CalData2; // v2.0+
	ElementNodePtr CalData1a; // v1.0 with v2 data
	CalData();
};


CalData::CalData()
{
	CalData1 = CreateNode();
	CalData1a = CreateNode();
	CalData2 = CreateNode();
	// Specialize the v1/v2 objs.
	CalData1->AddAttrib(_T("PlanOn"), _T("y"));
	CalData1a->AddAttrib(ATTRIB_CAL_ENTERED, _T("P"));
	CalData2->AddAttrib(ATTRIB_CAL_ENTERED, _T("P"));
}


SUITE(TestCalendar)
{
	TEST(New)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		CHECK(NULL != cal.get());
	}


	TEST(Clone)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		cal->SetStartDate(ARBDate(2007, 9, 1));
		cal->SetLocation(_T("Here"));
		ARBCalendarPtr cal2 = cal->Clone();
		CHECK(NULL != cal2.get());
		CHECK(cal.get() != cal2.get());
		CHECK(*cal == *cal2);
		CHECK(cal->GetStartDate() == cal2->GetStartDate());
		CHECK(cal->GetLocation() == cal2->GetLocation());
		cal->SetLocation(_T("Here2"));
		CHECK(cal->GetLocation() != cal2->GetLocation());
	}


	TEST(OpEqual)
	{
		ARBCalendarPtr cal1 = ARBCalendar::New();
		cal1->SetStartDate(ARBDate(2007, 9, 1));
		cal1->SetLocation(_T("Here"));
		ARBCalendarPtr cal2 = ARBCalendar::New();
		CHECK(*cal1 != *cal2);
		*cal1 = *cal2;
		CHECK(*cal1 == *cal2);
	}


	TEST_FIXTURE(CalData, Compare)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		cal->Load(CalData2, ARBVersion(2, 0), callback);
		ARBCalendarPtr cal2 = cal->Clone();
		CHECK(cal.get() != cal2.get());
		CHECK(*cal == *cal2);
		ARBDate n = ARBDate::Today();
		cal2->SetStartDate(n);
		cal2->SetEndDate(n+1);
		CHECK(*cal < *cal2);
		CHECK(*cal < cal2->GetStartDate());
		CHECK(*cal != *cal2);
		CHECK(!(*cal > *cal2));
		CHECK(!(*cal > cal2->GetStartDate()));
	}


	TEST(GenName)
	{
		ARBCalendarPtr cal1 = ARBCalendar::New();
		cal1->SetClub(_T("Way"));
		cal1->SetVenue(_T("No"));
		cal1->SetLocation(_T("There"));
		CHECK(_T("No Way There") == cal1->GetGenericName());
	}


	TEST_FIXTURE(CalData, Load1)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(cal->Load(CalData1, ARBVersion(1, 0), callback));
		CHECK_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
		tstring name = cal->GetGenericName();
		CHECK(!name.empty());
	}


	TEST_FIXTURE(CalData, Load2)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(cal->Load(CalData2, ARBVersion(2, 0), callback));
		CHECK_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
	}


	TEST_FIXTURE(CalData, Load3)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(cal->Load(CalData1a, ARBVersion(1, 0), callback));
		// These are not equal as 'Entered' isn't parsed in 1.0.
		CHECK(ARBCalendar::ePlanning != cal->GetEntered());
	}


	TEST_FIXTURE(CalData, Save)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		cal->Load(CalData2, ARBVersion(2, 0), callback);
		ElementNodePtr ele = ElementNode::New();
		CHECK(cal->Save(ele));
	}


	TEST(IsBefore)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		cal->SetStartDate(ARBDate(2007, 9, 1));
		cal->SetEndDate(ARBDate(2007, 9, 3));
		CHECK(!cal->IsBefore(ARBDate(2007, 8, 31)));
		CHECK(!cal->IsBefore(cal->GetStartDate()));
		CHECK(!cal->IsBefore(cal->GetEndDate()));
		CHECK(cal->IsBefore(ARBDate(2007, 9, 4)));
	}


	TEST(InRange)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		cal->SetStartDate(ARBDate(2007, 9, 1));
		cal->SetEndDate(ARBDate(2007, 9, 3));
		CHECK(cal->InRange(cal->GetStartDate()));
		CHECK(cal->InRange(cal->GetEndDate()));
		CHECK(cal->InRange(ARBDate(2007, 9, 2)));
		CHECK(!cal->InRange(ARBDate(2007, 9, 4)));
	}


	TEST(IsRangeOverlapped)
	{
		ARBCalendarPtr cal = ARBCalendar::New();
		cal->SetStartDate(ARBDate(2007, 9, 1));
		cal->SetEndDate(ARBDate(2007, 9, 3));
		CHECK(cal->IsRangeOverlapped(cal->GetStartDate(), cal->GetEndDate()));
		ARBDate d1(2007, 8, 31);
		ARBDate d2(2007, 9, 2);
		ARBDate d3(2007, 9, 4);
		ARBDate d4(2007, 9, 5);
		CHECK(cal->IsRangeOverlapped(d1, d2));
		CHECK(cal->IsRangeOverlapped(d2, d3));
		CHECK(!cal->IsRangeOverlapped(d3, d4));
		CHECK(cal->IsRangeOverlapped(d1, d4));
	}


	TEST(IsMatch)
	{
		ARBCalendarPtr cal1 = ARBCalendar::New();
		cal1->SetStartDate(ARBDate(2007, 9, 1));
		cal1->SetEndDate(ARBDate(2007, 9, 3));
		cal1->SetVenue(_T("AKC"));
		cal1->SetClub(_T("club"));
		cal1->SetLocation(_T("There"));

		ARBCalendarPtr cal2;

		CHECK(!cal1->IsMatch(cal2, true));

		cal2 = cal1->Clone();
		cal2->SetLocation(_T("Here"));
		CHECK(!cal1->IsMatch(cal2, true));
		CHECK(cal1->IsMatch(cal2, false));
		cal2->SetEndDate(cal2->GetEndDate() + 1);
		CHECK(!cal1->IsMatch(cal2, false));
	}


	TEST(Update)
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
		CHECK(*cal1 != *cal2);
		CHECK(cal1->Update(cal2));
		CHECK(*cal1 == *cal2);
	}
}


SUITE(TestCalendarList)
{
	TEST_FIXTURE(CalData, Load)
	{
		ARBCalendarList callist;
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(callist.Load(CalData1, ARBVersion(1, 0), callback));
		CHECK(callist.Load(CalData2, ARBVersion(2, 0), callback));
		ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
		ele->SetValue(_T("These are some notes"));
		ele->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 9, 4));
		ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
		ele->AddAttrib(ATTRIB_CAL_LOCATION, _T("Hollister, CA"));
		ele->AddAttrib(ATTRIB_CAL_CLUB, _T("PASA"));
		ele->AddAttrib(ATTRIB_CAL_VENUE, _T("ASCA"));
		ele->AddAttrib(_T("PlanOn"), _T("n"));
		CHECK(!callist.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(2u, callist.size());
	}


	TEST_FIXTURE(CalData, Load2)
	{
		ARBCalendarList callist;
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(callist.Load(CalData1, ARBVersion(1, 0), callback));
		CHECK(callist.Load(CalData2, ARBVersion(2, 0), callback));
		ElementNodePtr ele = ElementNode::New(TREE_CALENDAR);
		ele->SetValue(_T("These are some notes"));
		ele->AddAttrib(_T("DateStrt"), ARBDate(2006, 9, 4));
		ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
		ele->AddAttrib(ATTRIB_CAL_LOCATION, _T("Hollister, CA"));
		ele->AddAttrib(ATTRIB_CAL_CLUB, _T("PASA"));
		ele->AddAttrib(ATTRIB_CAL_VENUE, _T("ASCA"));
		ele->AddAttrib(_T("PlanOn"), _T("n"));
		CHECK(!callist.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(2u, callist.size());
	}


	TEST(sort)
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
		CHECK(callist == callist2);
		callist.sort();
		CHECK(callist != callist2);
	}


	TEST(Entered)
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
		CHECK_EQUAL(0u, callist.GetAllEntered(lst));
		cal1->SetEntered(ARBCalendar::ePlanning);
		CHECK_EQUAL(0u, callist.GetAllEntered(lst));
		cal1->SetEntered(ARBCalendar::eEntered);
		CHECK_EQUAL(1u, callist.GetAllEntered(lst));
	}


	TEST(Trim)
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
		CHECK_EQUAL(2u, callist.size());
		callist.TrimEntries(ARBDate(2005, 3, 27));
		CHECK_EQUAL(2u, callist.size());
		callist.TrimEntries(ARBDate(2005, 3, 28));
		CHECK_EQUAL(1u, callist.size());
	}


	TEST(Find)
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
		CHECK(callist.FindCalendar(cal1, true, &found));
		CHECK(NULL != found.get());
		CHECK(found.get() == cal1.get());
		ARBCalendarPtr clone = found->Clone();
		CHECK(callist.FindCalendar(clone, true, &found));
		CHECK(NULL != found.get());
		CHECK(found.get() != clone.get());
		CHECK(*found == *clone);
	}


	TEST(AddDelete)
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
		CHECK(*cal2 == *cal3);
		cal3->SetNote(_T("Test"));
		CHECK(*cal2 != *cal3);
		callist.AddCalendar(cal3);
		callist.AddCalendar(cal1->Clone());
		callist.sort();
		CHECK_EQUAL(4u, callist.size());
		CHECK(callist.DeleteCalendar(cal1));
		CHECK_EQUAL(3u, callist.size());
		CHECK(callist.DeleteCalendar(cal1));
		CHECK_EQUAL(2u, callist.size());
		CHECK(!callist.DeleteCalendar(cal1));
		CHECK_EQUAL(2u, callist.size());
		CHECK(*callist[0] != *callist[1]);
		ARBCalendarList callist2;
		callist.Clone(callist2);
		CHECK(callist == callist2);
		CHECK(callist[0].get() != callist2[0].get());
		CHECK(*callist[0] == *callist2[0]);
	}
}
