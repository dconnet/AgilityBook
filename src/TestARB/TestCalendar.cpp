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
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
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


TEST_CASE("Calendar")
{
	CalData data;

	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			REQUIRE(!!cal.get());
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetLocation(L"Here");
			ARBCalendarPtr cal2 = cal->Clone();
			REQUIRE(!!cal2.get());
			REQUIRE(cal.get() != cal2.get());
			REQUIRE(*cal == *cal2);
			REQUIRE(cal->GetStartDate() == cal2->GetStartDate());
			REQUIRE(cal->GetLocation() == cal2->GetLocation());
			cal->SetLocation(L"Here2");
			REQUIRE(cal->GetLocation() != cal2->GetLocation());
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetStartDate(ARBDate(2007, 9, 1));
			cal1->SetLocation(L"Here");
			ARBCalendarPtr cal2 = ARBCalendar::New();
			REQUIRE(*cal1 != *cal2);
			*cal1 = *cal2;
			REQUIRE(*cal1 == *cal2);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			cal->Load(data.CalData2, ARBVersion(2, 0), callback);
			ARBCalendarPtr cal2 = cal->Clone();
			REQUIRE(cal.get() != cal2.get());
			REQUIRE(*cal == *cal2);
			ARBDate n = ARBDate::Today();
			cal2->SetStartDate(n);
			cal2->SetEndDate(n+1);
			REQUIRE(*cal < *cal2);
			REQUIRE(*cal < cal2->GetStartDate());
			REQUIRE(*cal != *cal2);
			REQUIRE(!(*cal > *cal2));
			REQUIRE(!(*cal > cal2->GetStartDate()));
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal1 = ARBCalendar::New();
			cal1->SetClub(L"Way");
			cal1->SetVenue(L"No");
			cal1->SetLocation(L"There");
			REQUIRE(L"No Way There" == cal1->GetGenericName());
		}
	}


	SECTION("Load1")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(cal->Load(data.CalData1, ARBVersion(1, 0), callback));
			REQUIRE(ARBCalendarEntry::Planning == cal->GetEntered());
			std::wstring name = cal->GetGenericName();
			REQUIRE(!name.empty());
		}
	}


	SECTION("Load2")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(cal->Load(data.CalData2, ARBVersion(2, 0), callback));
			REQUIRE(ARBCalendarEntry::Planning == cal->GetEntered());
		}
	}


	SECTION("Load3")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(cal->Load(data.CalData1a, ARBVersion(1, 0), callback));
			// These are not equal as 'Entered' isn't parsed in 1.0.
			REQUIRE(ARBCalendarEntry::Planning != cal->GetEntered());
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			cal->Load(data.CalData2, ARBVersion(2, 0), callback);
			ElementNodePtr ele = ElementNode::New();
			REQUIRE(cal->Save(ele));
		}
	}


	SECTION("IsBefore")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetEndDate(ARBDate(2007, 9, 3));
			REQUIRE(!cal->IsBefore(ARBDate(2007, 8, 31)));
			REQUIRE(!cal->IsBefore(cal->GetStartDate()));
			REQUIRE(!cal->IsBefore(cal->GetEndDate()));
			REQUIRE(cal->IsBefore(ARBDate(2007, 9, 4)));
		}
	}


	SECTION("InRange")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetEndDate(ARBDate(2007, 9, 3));
			REQUIRE(cal->InRange(cal->GetStartDate()));
			REQUIRE(cal->InRange(cal->GetEndDate()));
			REQUIRE(cal->InRange(ARBDate(2007, 9, 2)));
			REQUIRE(!cal->InRange(ARBDate(2007, 9, 4)));
		}
	}


	SECTION("IsRangeOverlapped")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarPtr cal = ARBCalendar::New();
			cal->SetStartDate(ARBDate(2007, 9, 1));
			cal->SetEndDate(ARBDate(2007, 9, 3));
			REQUIRE(cal->IsRangeOverlapped(cal->GetStartDate(), cal->GetEndDate()));
			ARBDate d1(2007, 8, 31);
			ARBDate d2(2007, 9, 2);
			ARBDate d3(2007, 9, 4);
			ARBDate d4(2007, 9, 5);
			REQUIRE(cal->IsRangeOverlapped(d1, d2));
			REQUIRE(cal->IsRangeOverlapped(d2, d3));
			REQUIRE(!cal->IsRangeOverlapped(d3, d4));
			REQUIRE(cal->IsRangeOverlapped(d1, d4));
		}
	}


	SECTION("IsMatch")
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

			REQUIRE(!cal1->IsMatch(cal2, true));

			cal2 = cal1->Clone();
			cal2->SetLocation(L"Here");
			REQUIRE(!cal1->IsMatch(cal2, true));
			REQUIRE(cal1->IsMatch(cal2, false));
			cal2->SetEndDate(cal2->GetEndDate() + 1);
			REQUIRE(!cal1->IsMatch(cal2, false));
		}
	}


	SECTION("Update")
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
			REQUIRE(*cal1 != *cal2);
			REQUIRE(cal1->Update(cal2));
			REQUIRE(*cal1 == *cal2);
		}
	}
}


TEST_CASE("CalendarList")
{
	CalData data;

	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(callist.Load(data.CalData1, ARBVersion(1, 0), callback));
			REQUIRE(callist.Load(data.CalData2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(ATTRIB_CAL_START, ARBDate(2006, 9, 4));
			ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
			ele->AddAttrib(ATTRIB_CAL_LOCATION, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_CAL_CLUB, L"PASA");
			ele->AddAttrib(ATTRIB_CAL_VENUE, L"ASCA");
			ele->AddAttrib(L"PlanOn", L"n");
			REQUIRE(!callist.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(2u == callist.size());
		}
	}


	SECTION("Load2")
	{
		if (!g_bMicroTest)
		{
			ARBCalendarList callist;
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(callist.Load(data.CalData1, ARBVersion(1, 0), callback));
			REQUIRE(callist.Load(data.CalData2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(TREE_CALENDAR);
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(L"DateStrt", ARBDate(2006, 9, 4));
			ele->AddAttrib(ATTRIB_CAL_END, ARBDate(2006, 9, 5));
			ele->AddAttrib(ATTRIB_CAL_LOCATION, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_CAL_CLUB, L"PASA");
			ele->AddAttrib(ATTRIB_CAL_VENUE, L"ASCA");
			ele->AddAttrib(L"PlanOn", L"n");
			REQUIRE(!callist.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(2u == callist.size());
		}
	}


	SECTION("sort")
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
			REQUIRE(callist == callist2);
			callist.sort();
			REQUIRE(callist != callist2);
		}
	}


	SECTION("Entered")
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
			REQUIRE(0u == callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendarEntry::Planning);
			REQUIRE(0u == callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendarEntry::Pending);
			REQUIRE(1u == callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendarEntry::Planning);
			REQUIRE(0u == callist.GetAllEntered(lst));
			cal1->SetEntered(ARBCalendarEntry::Entered);
			REQUIRE(1u == callist.GetAllEntered(lst));
		}
	}


	SECTION("Trim")
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
			REQUIRE(2u == callist.size());
			callist.TrimEntries(ARBDate(2005, 3, 27));
			REQUIRE(2u == callist.size());
			callist.TrimEntries(ARBDate(2005, 3, 28));
			REQUIRE(1u == callist.size());
		}
	}


	SECTION("Find")
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
			REQUIRE(callist.FindCalendar(cal1, true, &found));
			REQUIRE(!!found.get());
			REQUIRE(found.get() == cal1.get());
			ARBCalendarPtr clone = found->Clone();
			REQUIRE(callist.FindCalendar(clone, true, &found));
			REQUIRE(!!found.get());
			REQUIRE(found.get() != clone.get());
			REQUIRE(*found == *clone);
		}
	}


	SECTION("AddDelete")
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
			REQUIRE(*cal2 == *cal3);
			cal3->SetNote(L"Test");
			REQUIRE(*cal2 != *cal3);
			callist.AddCalendar(cal3);
			callist.AddCalendar(cal1->Clone());
			callist.sort();
			REQUIRE(4u == callist.size());
			REQUIRE(callist.DeleteCalendar(cal1));
			REQUIRE(3u == callist.size());
			REQUIRE(callist.DeleteCalendar(cal1));
			REQUIRE(2u == callist.size());
			REQUIRE(!callist.DeleteCalendar(cal1));
			REQUIRE(2u == callist.size());
			REQUIRE(*callist[0] != *callist[1]);
			ARBCalendarList callist2;
			callist.Clone(callist2);
			REQUIRE(callist == callist2);
			REQUIRE(callist[0].get() != callist2[0].get());
			REQUIRE(*callist[0] == *callist2[0]);
		}
	}
}
