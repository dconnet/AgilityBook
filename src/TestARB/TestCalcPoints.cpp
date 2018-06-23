/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBCalcPoints class
 * @author David Connet
 *
 * Revision History
 * 2018-06-23 Added USDAA Top10.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2011-08-02 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBCalcPoints.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("CalcPoints")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			REQUIRE(ARBCalcPoints::New(ePointsTypeNormal));
			REQUIRE(ARBCalcPoints::New(ePointsTypeT2B));
			REQUIRE(ARBCalcPoints::New(ePointsTypeUKI));
			REQUIRE(ARBCalcPoints::New(ePointsTypeTop10USDAA));
			REQUIRE(!ARBCalcPoints::New(ePointsTypeMax));
		}
	}

	// GetPoints(double inPoints, double inTime, double inSCT, short inPlace, short inClass)

	SECTION("Normal")
	{
		if (!g_bMicroTest)
		{
			ARBCalcPointsPtr p = ARBCalcPoints::New(ePointsTypeNormal);
			REQUIRE(p->GetType() == ePointsTypeNormal);
			REQUIRE(p->AllowConfiguration());
			double pt = p->GetPoints(1.0, 10.0, 5.0, 0, 0);
			REQUIRE(pt == 1.0);
			// All fields ignored except first.
			pt = p->GetPoints(1.0, 0.0, 0.0, 0, 0);
			REQUIRE(pt == 1.0);
		}
	}


	SECTION("T2B")
	{
		if (!g_bMicroTest)
		{
			ARBCalcPointsPtr p = ARBCalcPoints::New(ePointsTypeT2B);
			REQUIRE(p->GetType() == ePointsTypeT2B);
			REQUIRE(!p->AllowConfiguration());
			// First field ignored.
			double pt = p->GetPoints(0.0, 28.73, 28.73, 1, 10);
			// Using the example from the AKC rule book.
			REQUIRE(pt == 10.0);
			pt = p->GetPoints(0.0, 28.73, 28.73, 2, 10);
			REQUIRE(pt == 9.0);
			pt = p->GetPoints(0.0, 31.0, 28.73, 2, 10);
			REQUIRE(pt == 9.0);
			pt = p->GetPoints(0.0, 31.99, 28.73, 2, 10);
			REQUIRE(pt == 9.0);
			pt = p->GetPoints(0.0, 32.0, 28.73, 2, 10);
			REQUIRE(pt == 8.0);
			pt = p->GetPoints(0.0, 34.99, 28.73, 2, 10);
			REQUIRE(pt == 8.0);
			pt = p->GetPoints(0.0, 35.0, 28.73, 2, 10);
			REQUIRE(pt == 7.0);
		}
	}


	SECTION("UKI")
	{
		if (!g_bMicroTest)
		{
			ARBCalcPointsPtr p = ARBCalcPoints::New(ePointsTypeUKI);
			REQUIRE(p->GetType() == ePointsTypeUKI);
			REQUIRE(!p->AllowConfiguration());
			// First 3 fields ignored.
			static struct
			{
				short place;
				short inCls;
				double pts;
			} points[] = {
				// From page 5 in the 2011 UKI manual
				{1, 10, 6.0},
				{2, 10, 5.0},
				{3, 10, 4.0},
				{4, 10, 4.0},
				//
				{1, 11, 12.0},
				{2, 11, 8.0},
				{3, 11, 7.0},
				{4, 11, 5.0},
				{5, 11, 4.0},
				//
				{1, 30, 12.0},
				{2, 30, 8.0},
				{3, 30, 7.0},
				{4, 30, 5.0},
				{5, 30, 4.0},
				//
				{1, 31, 12.0},
				{2, 31, 11.0},
				{3, 31, 10.0},
				{4, 31, 9.0},
				{5, 31, 8.0},
				{6, 31, 7.0},
				{7, 31, 6.0},
				{8, 31, 5.0},
				{9, 31, 4.0},
				{10, 31, 4.0},
			};
			static size_t nPoints = sizeof(points) / sizeof(points[0]);
			for (size_t i = 0; i < nPoints; ++i)
			{
				double pt = p->GetPoints(0.0, 0.0, 0.0, points[i].place, points[i].inCls);
				REQUIRE(pt == points[i].pts);
			}
		}
	}


	SECTION("USDAA")
	{
		if (!g_bMicroTest)
		{
			ARBCalcPointsPtr p = ARBCalcPoints::New(ePointsTypeTop10USDAA);
			REQUIRE(p->GetType() == ePointsTypeTop10USDAA);
			REQUIRE(!p->AllowConfiguration());
			// First 3 fields ignored.
			static struct
			{
				short place;
				short inCls;
				double pts;
			} points[] = {
				{1, 1, 0.0},
				//
				{1, 2, 1.0},
				{2, 2, 0.0},
				//
				{1, 3, 1.0},
				{2, 3, 0.0},
				//
				{1, 4, 3.0},
				{2, 4, 1.0},
				{3, 4, 0.0},
				//
				{1, 5, 3.0},
				{2, 5, 1.0},
				{3, 5, 0.0},
				//
				{1, 6, 5.0},
				{2, 6, 3.0},
				{3, 6, 1.0},
				{4, 6, 0.0},
				//
				{1, 10, 5.0},
				{2, 10, 3.0},
				{3, 10, 1.0},
				{4, 10, 0.0},
				//
				{1, 11, 7.0},
				{2, 11, 5.0},
				{3, 11, 3.0},
				{4, 11, 1.0},
				{5, 11, 0.0},
				//
				{1, 20, 7.0},
				{2, 20, 5.0},
				{3, 20, 3.0},
				{4, 20, 1.0},
				{5, 20, 0.0},
				//
				{1, 21, 9.0},
				{2, 21, 7.0},
				{3, 21, 5.0},
				{4, 21, 3.0},
				{5, 21, 1.0},
				{6, 21, 0.0},
				//
				{1, 30, 9.0},
				{2, 30, 7.0},
				{3, 30, 5.0},
				{4, 30, 3.0},
				{5, 30, 1.0},
				{6, 30, 0.0},
				//
				{1, 31, 11.0},
				{2, 31, 9.0},
				{3, 31, 7.0},
				{4, 31, 5.0},
				{5, 31, 3.0},
				{6, 31, 1.0},
				{7, 31, 0.0},
				//
				{1, 40, 11.0},
				{2, 40, 9.0},
				{3, 40, 7.0},
				{4, 40, 5.0},
				{5, 40, 3.0},
				{6, 40, 1.0},
				{7, 40, 0.0},
			};
			static size_t nPoints = sizeof(points) / sizeof(points[0]);
			for (size_t i = 0; i < nPoints; ++i)
			{
				double pt = p->GetPoints(0.0, 0.0, 0.0, points[i].place, points[i].inCls);
				REQUIRE(pt == points[i].pts);
			}
		}
	}
}
