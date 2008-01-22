/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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


BEGIN_TEST(DogRun_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogRunPtr New();
}
END_TEST


BEGIN_TEST(DogRun_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogRunPtr Clone() const;
}
END_TEST


BEGIN_TEST(DogRun_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogRun& operator=(ARBDogRun const& rhs);
}
END_TEST


BEGIN_TEST(DogRun_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogRun const& rhs) const;
//	bool operator!=(ARBDogRun const& rhs) const
}
END_TEST


BEGIN_TEST(DogRun_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const;
}
END_TEST


BEGIN_TEST(DogRun_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ARBDogClubList const& inClubs,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogRun_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogRun_NumOtherPointsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumOtherPointsInUse(tstring const& inOther) const;
}
END_TEST


BEGIN_TEST(DogRun_RenameOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameOtherPoints(
//			tstring const& inOldName,
//			tstring const& inNewName);
}
END_TEST


BEGIN_TEST(DogRun_DeleteOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteOtherPoints(tstring const& inName);
}
END_TEST


BEGIN_TEST(DogRun_GetSpeedPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	short GetSpeedPoints(ARBConfigScoringPtr inScoring) const;
}
END_TEST


BEGIN_TEST(DogRun_GetTitlePoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	double GetTitlePoints(
//			ARBConfigScoringPtr inScoring,
//			bool* outClean = NULL,
//			double* outLifeTime = NULL,
//			double* outPlacement = NULL) const;
}
END_TEST


BEGIN_TEST(DogRun_GetScore)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	double GetScore(ARBConfigScoringPtr inScoring) const;
}
END_TEST


BEGIN_TEST(DogRunList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ARBDogClubList const& inClubs,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogRunList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogRunList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
}
END_TEST


BEGIN_TEST(DogRunList_GetStartDate)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDate GetStartDate() const;
}
END_TEST


BEGIN_TEST(DogRunList_GetEndDate)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDate GetEndDate() const;
}
END_TEST


BEGIN_TEST(DogRunList_AddRun)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddRun(ARBDogRunPtr inRun);
}
END_TEST


BEGIN_TEST(DogRunList_DeleteRun)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool DeleteRun(ARBDogRunPtr inRun);
}
END_TEST
