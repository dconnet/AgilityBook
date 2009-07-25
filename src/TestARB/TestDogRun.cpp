/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief Test ARBDogRun and ARBDogRunList classes.
 * @author David Connet
 *
 * Revision History
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
	//	virtual tstring GetGenericName() const;
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
	//	int NumOtherPointsInUse(tstring const& inOther) const;
	}


	TEST(RenameOtherPoints)
	{
		TODO_TEST
	//	int RenameOtherPoints(
	//			tstring const& inOldName,
	//			tstring const& inNewName);
	}


	TEST(DeleteOtherPoints)
	{
		TODO_TEST
	//	int DeleteOtherPoints(tstring const& inName);
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
