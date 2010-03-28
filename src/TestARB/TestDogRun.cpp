/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogRunPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRunPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRun& operator=(ARBDogRun const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogRun const& rhs) const;
		//	bool operator!=(ARBDogRun const& rhs) const
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	virtual wxString GetGenericName() const;
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig const& inConfig,
		//			ARBDogClubList const& inClubs,
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


	TEST(NumOtherPointsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumOtherPointsInUse(wxString const& inOther) const;
		}
	}


	TEST(RenameOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameOtherPoints(
		//			wxString const& inOldName,
		//			wxString const& inNewName);
		}
	}


	TEST(DeleteOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteOtherPoints(wxString const& inName);
		}
	}


	TEST(GetSpeedPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	short GetSpeedPoints(ARBConfigScoringPtr inScoring) const;
		}
	}


	TEST(GetTitlePoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	double GetTitlePoints(
		//			ARBConfigScoringPtr inScoring,
		//			bool* outClean = NULL,
		//			double* outLifeTime = NULL,
		//			double* outPlacement = NULL) const;
		}
	}


	TEST(GetScore)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	double GetScore(ARBConfigScoringPtr inScoring) const;
		}
	}
}


SUITE(TestDogRunList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig const& inConfig,
		//			ARBDogClubList const& inClubs,
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
		}
	}


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void sort();
		}
	}


	TEST(GetStartDate)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDate GetStartDate() const;
		}
	}


	TEST(GetEndDate)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDate GetEndDate() const;
		}
	}


	TEST(AddRun)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddRun(ARBDogRunPtr inRun);
		}
	}


	TEST(DeleteRun)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool DeleteRun(ARBDogRunPtr inRun);
		}
	}
}
