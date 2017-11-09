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

}
