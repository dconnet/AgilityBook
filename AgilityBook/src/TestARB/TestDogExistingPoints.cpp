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
 * @brief Test ARBDogExistingPoints and ARBDogExistingPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogExistingPoints.h"
#include "Element.h"


BEGIN_TEST(DogExistingPoints_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogExistingPointsPtr New();
}
END_TEST


BEGIN_TEST(DogExistingPoints_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogExistingPointsPtr Clone() const;
}
END_TEST


BEGIN_TEST(DogExistingPoints_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
}
END_TEST


BEGIN_TEST(DogExistingPoints_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogExistingPoints const& rhs) const;
//	bool operator!=(ARBDogExistingPoints const& rhs) const
}
END_TEST


BEGIN_TEST(DogExistingPoints_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(DogExistingPoints_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogExistingPoints_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogExistingPoints_GetPointTypeName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static tstring GetPointTypeName(PointType inType);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogExistingPointsList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
}
END_TEST


BEGIN_TEST(DogExistingPointsList_HasPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool HasPoints(tstring const& inVenue) const;
//	bool HasPoints(
//			ARBConfigVenuePtr inVenue,
//			ARBConfigDivisionPtr inDiv,
//			ARBConfigLevelPtr inLevel,
//			ARBConfigEventPtr inEvent,
//			ARBDate inDateFrom,
//			ARBDate inDateTo,
//			bool inHasLifetime) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_ExistingPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	double ExistingPoints(
//			ARBDogExistingPoints::PointType inType,
//			ARBConfigVenuePtr inVenue,
//			ARBConfigMultiQPtr inMultiQ,
//			ARBConfigDivisionPtr inDiv,
//			ARBConfigLevelPtr inLevel,
//			ARBConfigEventPtr inEvent,
//			ARBDate inDateFrom,
//			ARBDate inDateTo) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumExistingPointsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumExistingPointsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameVenue(
//			tstring const& inOldVenue,
//			tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumExistingPointsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumExistingPointsInDivision(
//			ARBConfigVenuePtr inVenue,
//			tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameDivision(
//			tstring const& inVenue,
//			tstring const& inOldDiv,
//			tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteDivision(
//			tstring const& inVenue,
//			tstring const& inDiv);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumLevelsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumLevelsInUse(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameLevel(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inOldLevel,
//			tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteLevel(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inLevel);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumEventsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumEventsInUse(
//			tstring const& inVenue,
//			tstring const& inEvent) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameEvent(
//			tstring const& inVenue,
//			tstring const& inOldEvent,
//			tstring const& inNewEvent);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteEvent(
//			tstring const& inVenue,
//			tstring const& inEvent);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumOtherPointsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumOtherPointsInUse(tstring const& inOther) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameOtherPoints(
//			tstring const& inOldOther,
//			tstring const& inNewOther);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteOtherPoints(tstring const& inOther);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_NumMultiQsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumMultiQsInUse(
//			tstring const& inVenue,
//			tstring const& inMultiQ) const;
}
END_TEST


BEGIN_TEST(DogExistingPointsList_RenameMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameMultiQs(
//			tstring const& inVenue,
//			tstring const& inOldMultiQ,
//			tstring const& inNewMultiQ);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_DeleteMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteMultiQs(
//			ARBConfig const& inConfig,
//			tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogExistingPointsList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
//	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
}
END_TEST
