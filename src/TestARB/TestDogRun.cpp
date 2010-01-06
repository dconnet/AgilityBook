/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogRun and ARBDogRunList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRun.h"
#include "Element.h"


SUITE(TestDogRun)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogRunPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogRunPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRun& operator=(ARBDogRun const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogRun const& rhs) const;
	//	bool operator!=(ARBDogRun const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual wxString GetGenericName() const;
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
	//			ARBDogClubList const& inClubs,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(NumOtherPointsInUse)
	{
		TODO_TEST
	//	int NumOtherPointsInUse(wxString const& inOther) const;
	}


	TEST(RenameOtherPoints)
	{
		TODO_TEST
	//	int RenameOtherPoints(
	//			wxString const& inOldName,
	//			wxString const& inNewName);
	}


	TEST(DeleteOtherPoints)
	{
		TODO_TEST
	//	int DeleteOtherPoints(wxString const& inName);
	}


	TEST(GetSpeedPoints)
	{
		TODO_TEST
	//	short GetSpeedPoints(ARBConfigScoringPtr inScoring) const;
	}


	TEST(GetTitlePoints)
	{
		TODO_TEST
	//	double GetTitlePoints(
	//			ARBConfigScoringPtr inScoring,
	//			bool* outClean = NULL,
	//			double* outLifeTime = NULL,
	//			double* outPlacement = NULL) const;
	}


	TEST(GetScore)
	{
		TODO_TEST
	//	double GetScore(ARBConfigScoringPtr inScoring) const;
	}
}


SUITE(TestDogRunList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
	//			ARBDogClubList const& inClubs,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	}


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(GetStartDate)
	{
		TODO_TEST
	//	ARBDate GetStartDate() const;
	}


	TEST(GetEndDate)
	{
		TODO_TEST
	//	ARBDate GetEndDate() const;
	}


	TEST(AddRun)
	{
		TODO_TEST
	//	bool AddRun(ARBDogRunPtr inRun);
	}


	TEST(DeleteRun)
	{
		TODO_TEST
	//	bool DeleteRun(ARBDogRunPtr inRun);
	}
}
