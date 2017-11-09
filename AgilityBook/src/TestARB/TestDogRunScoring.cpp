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
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogRunScoring.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("DogRunScoring")
{
	SECTION("TranslateConfigScoring")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogRunScoring::ScoringType TranslateConfigScoring(ARBConfigScoring::ScoringStyle inType);
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


	SECTION("GetTimeFaults")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;
		}
	}
}
