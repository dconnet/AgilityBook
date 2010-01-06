/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include "ARBStructure.h"
#include "ARBDogRunScoring.h"
#include "Element.h"


SUITE(TestDogRunScoring)
{
	TEST(TranslateConfigScoring)
	{
		TODO_TEST
	//	static ARBDogRunScoring::ScoringType TranslateConfigScoring(ARBConfigScoring::ScoringStyle inType);
	}


	TEST(ctor)
	{
		TODO_TEST
	//	ARBDogRunScoring();
	//	ARBDogRunScoring(ARBDogRunScoring const& rhs);
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogRunScoring const& rhs) const;
	//	bool operator!=(ARBDogRunScoring const& rhs) const
	}


	TEST(Load)
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


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(GetMinYPS)
	{
		TODO_TEST
	//	bool GetMinYPS(
	//			bool inTableInYPS,
	//			double& outYPS) const;
	}


	TEST(GetYPS)
	{
		TODO_TEST
	//	bool GetYPS(
	//			bool inTableInYPS,
	//			double& outYPS) const;
	}


	TEST(GetObstaclesPS)
	{
		TODO_TEST
	//	bool GetObstaclesPS(bool inTableInYPS, double& outOPS) const;
	}


	TEST(GetTimeFaults)
	{
		TODO_TEST
	//	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;
	}
}
