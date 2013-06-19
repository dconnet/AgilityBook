/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBCalendar.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


static ElementNodePtr CreateNode()
{
	ElementNodePtr data = ElementNode::New(TREE_CALENDAR);
	data->SetValue(L"These are some notes");
	data->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 3, 4));
	data->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 3, 5));
	data->AddAttrib(ATTRIB_CAL_LOCATION, L"Sunnyvale, CA");
	data->AddAttrib(ATTRIB_CAL_CLUB, L"Bay Team");
	data->AddAttrib(ATTRIB_CAL_VENUE, L"USDAA");
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
	CalData1->AddAttrib(L"PlanOn", L"y");
	CalData1a->AddAttrib(ATTRIB_CAL_ENTERED, L"P");
	CalData2->AddAttrib(ATTRIB_CAL_ENTERED, L"P");
}


SUITE(TestCalendar)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			CHECK(NULL != cal.get());
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetLocation(L"Here");
			ARBCalendarPtr cal2 = cal->Clone();
			CHECK(NULL != cal2.get());
			CHECK(cal.get() != cal2.get());
			CHECK(*cal == *cal2);
			CHECK(cal->GetStartDate() == cal2->GetStartDate());
			CHECK(cal->GetLocation() == cal2->GetLocation());
			cal->SetLocation(L"Here2");
			CHECK(cal->GetLocation() != cal2->GetLocation());
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2007, 9, 1));
			cal1->SetLocation(L"Here");
			ARBCalendarPtr cal2 = ARBCalendar::New();
			CHECK(*cal1 != *cal2);
			*cal1 = *cal2;
			CHECK(*cal1 == *cal2);
		}
	}


	TEST_FIXTURE(CalData, Compare)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			std::wostringstream errs;
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
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetClub(L"Way");
			cal1->SetVenue(L"No");
			cal1->SetLocation(L"There");
			CHECK(L"No Way There" == cal1->GetGenericName());
		}
	}


	TEST_FIXTURE(CalData, Load1)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(cal->Load(CalData1, ARBVersion(1, 0), callback));
			CHECK_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
			std::wstring name = cal->GetGenericName();
			CHECK(!name.empty());
		}
	}


	TEST_FIXTURE(CalData, Load2)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(cal->Load(CalData2, ARBVersion(2, 0), callback));
			CHECK_EQUAL(ARBCalendar::ePlanning, cal->GetEntered());
		}
	}


	TEST_FIXTURE(CalData, Load3)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(cal->Load(CalData1a, ARBVersion(1, 0), callback));
			// These are not equal as 'Entered' isn't parsed in 1.0.
			CHECK(ARBCalendar::ePlanning != cal->GetEntered());
		}
	}


	TEST_FIXTURE(CalData, Save)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			cal->Load(CalData2, ARBVersion(2, 0), callback);
			ElementNodePtr ele = ElementNode::New();
			CHECK(cal->Save(ele));
		}
	}


	TEST(IsBefore)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetEndDate(ARBDate(2007, 9, 3));
			CHECK(!cal->IsBefore(ARBDate(2007, 8, 31)));
			CHECK(!cal->IsBefore(cal->GetStartDate()));
			CHECK(!cal->IsBefore(cal->GetEndDate()));
			CHECK(cal->IsBefore(ARBDate(2007, 9, 4)));
		}
	}


	TEST(InRange)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetEndDate(ARBDate(2007, 9, 3));
			CHECK(cal->InRange(cal->GetStartDate()));
			CHECK(cal->InRange(cal->GetEndDate()));
			CHECK(cal->InRange(ARBDate(2007, 9, 2)));
			CHECK(!cal->InRange(ARBDate(2007, 9, 4)));
		}
	}


	TEST(IsRangeOverlapped)
	{
		if (!g_bMicroTest)
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
	}


	TEST(IsMatch)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2007, 9, 1));
			cal1->SetEndDate(ARBDate(2007, 9, 3));
			cal1->SetVenue(L"AKC");
			cal1->SetClub(L"club");
			cal1->SetLocation(L"There");

			ARBCalendarPtr cal2;

			CHECK(!cal1->IsMatch(cal2, true));

			cal2 = cal1->Clone();
			cal2->SetLocation(L"Here");
			CHECK(!cal1->IsMatch(cal2, true));
			CHECK(cal1->IsMatch(cal2, false));
			cal2->SetEndDate(cal2->GetEndDate() + 1);
			CHECK(!cal1->IsMatch(cal2, false));
		}
	}


	TEST(Update)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2006, 9, 4));
			cal1->SetEndDate(ARBDate(2006, 9, 5));
			cal1->SetLocation(L"Hollister");
			cal1->SetClub(L"PASA");
			cal1->SetVenue(L"ASCA");
			ARBCalendarPtr cal2 = cal1->Clone();
			cal1->SetEndDate(ARBDate(2006, 9, 6));
			cal1->SetLocation(L"Turlock");
			cal1->SetClub(L"PASA");
			cal1->SetVenue(L"ASCA");
			CHECK(*cal1 != *cal2);
			CHECK(cal1->Update(cal2));
			CHECK(*cal1 == *cal2);
		}
	}
}


SUITE(TestCalendarList)
{
	TEST_FIXTURE(CalData, Load)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(callist.Load(CalData1, ARBVersion(1, 0), callback));
			CHECK(callist.Load(CalData2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 9, 4));
			ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
			ele->AddAttrib(ATTRIB_CAL_LOCATION, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_CAL_CLUB, L"PASA");
			ele->AddAttrib(ATTRIB_CAL_VENUE, L"ASCA");
			ele->AddAttrib(L"PlanOn", L"n");
			CHECK(!callist.Load(ele, ARBVersion(2, 0), callback));
			CHECK_EQUAL(2u, callist.size());
		}
	}


	TEST_FIXTURE(CalData, Load2)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(callist.Load(CalData1, ARBVersion(1, 0), callback));
			CHECK(callist.Load(CalData2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(TREE_CALENDAR);
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(L"DateStrt", ARBDate(2006, 9, 4));
			ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
			ele->AddAttrib(ATTRIB_CAL_LOCATION, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_CAL_CLUB, L"PASA");
			ele->AddAttrib(ATTRIB_CAL_VENUE, L"ASCA");
			ele->AddAttrib(L"PlanOn", L"n");
			CHECK(!callist.Load(ele, ARBVersion(2, 0), callback));
			CHECK_EQUAL(2u, callist.size());
		}
	}


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2006, 9, 4));
			cal1->SetEndDate(ARBDate(2006, 9, 5));
			cal1->SetLocation(L"Hollister");
			cal1->SetClub(L"PASA");
			cal1->SetVenue(L"ASCA");
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
	}


	TEST(Entered)
	{
		if (!g_bMicroTest)
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
			cal1->SetEntered(ARBCalendar::ePending);
			CHECK_EQUAL(1u, callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendar::ePlanning);
			CHECK_EQUAL(0u, callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendar::eEntered);
			CHECK_EQUAL(1u, callist.GetAllEntered(lst));
		}
	}


	TEST(Trim)
	{
		if (!g_bMicroTest)
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
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2006, 9, 4));
			cal1->SetEndDate(ARBDate(2006, 9, 5));
			cal1->SetLocation(L"Hollister");
			cal1->SetClub(L"PASA");
			cal1->SetVenue(L"ASCA");
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
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2006, 9, 4));
			cal1->SetEndDate(ARBDate(2006, 9, 5));
			cal1->SetLocation(L"Hollister");
			cal1->SetClub(L"PASA");
			cal1->SetVenue(L"ASCA");
			ARBCalendarPtr cal2 = cal1->Clone();
			cal2->SetStartDate(ARBDate(2005, 3, 26));
			cal2->SetEndDate(ARBDate(2005, 3, 27));
			callist.AddCalendar(cal1);
			callist.AddCalendar(cal2);
			callist.sort();
			ARBCalendarPtr cal3 = callist[0]->Clone();
			CHECK(*cal2 == *cal3);
			cal3->SetNote(L"Test");
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
}
