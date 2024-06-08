/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogRunScoring class.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDogRunScoring.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;


TEST_CASE("DogRunScoring")
{
	SECTION("TranslateConfigScoring")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBScoringType TranslateConfigScoring(ARBScoringStyle inType);
		}
	}


	SECTION("ctor")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogRunScoring();
			//	ARBDogRunScoring(ARBDogRunScoring const& rhs);
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogRunScoring const& rhs) const;
			//	bool operator!=(ARBDogRunScoring const& rhs) const
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//			short inConfigVersion,
			//			ARBConfigEventPtr inEvent,
			//			ARBConfigScoringPtr inEventScoring,
			//			ElementNodePtr inTree,
			//			ARBVersion const& inVersion,
			//			ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("GetMinYPS")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool GetMinYPS(
			//			bool inTableInYPS,
			//			double& outYPS) const;
		}
	}


	SECTION("GetYPS")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool GetYPS(
			//			bool inTableInYPS,
			//			double& outYPS) const;
		}
	}


	SECTION("GetObstaclesPS")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool GetObstaclesPS(bool inTableInYPS, bool inRunTimeInOPS, double& outOPS) const;
		}
	}


	SECTION("MilesPerHour")
	{
		if (!g_bMicroTest)
		{
			constexpr double yards = 100.0;
			constexpr double time100 = 20.1;
			constexpr wchar_t mph[] = L"10.18";

			// AKC FCAT and CPE DragRace100
			ARBDogRunScoring scoring;
			scoring.SetType(ARBScoringType::BySpeed, false);

			scoring.SetYards(yards);
			scoring.SetTime(time100);
			double ops = 0.0;
			int prec = 2;
			REQUIRE(scoring.GetObstaclesPS(false, false, ops, prec));
			REQUIRE(ARBDouble::ToString(ops, 2) == mph);

			// CPE DragRace50
			scoring.SetYards(yards / 2);
			scoring.SetTime(time100 / 2);
			ops = 0.0;
			prec = 2;
			REQUIRE(scoring.GetObstaclesPS(false, false, ops, prec));
			REQUIRE(ARBDouble::ToString(ops, 2) == mph);
		}
	}


	SECTION("GetTimeFaults")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;
		}
	}
}

} // namespace dconSoft
