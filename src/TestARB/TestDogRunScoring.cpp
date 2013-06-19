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
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogRunScoring.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestDogRunScoring)
{
	TEST(TranslateConfigScoring)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogRunScoring::ScoringType TranslateConfigScoring(ARBConfigScoring::ScoringStyle inType);
		}
	}


	TEST(ctor)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRunScoring();
		//	ARBDogRunScoring(ARBDogRunScoring const& rhs);
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogRunScoring const& rhs) const;
		//	bool operator!=(ARBDogRunScoring const& rhs) const
		}
	}


	TEST(Load)
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


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Save(ElementNodePtr ioTree) const;
		}
	}


	TEST(GetMinYPS)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool GetMinYPS(
		//			bool inTableInYPS,
		//			double& outYPS) const;
		}
	}


	TEST(GetYPS)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool GetYPS(
		//			bool inTableInYPS,
		//			double& outYPS) const;
		}
	}


	TEST(GetObstaclesPS)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool GetObstaclesPS(bool inTableInYPS, bool inRunTimeInOPS, double& outOPS) const;
		}
	}


	TEST(GetTimeFaults)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;
		}
	}
}
