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


SUITE(TestDogExistingPoints)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogExistingPointsPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogExistingPointsPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogExistingPoints const& rhs) const;
	//	bool operator!=(ARBDogExistingPoints const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual tstring GetGenericName() const
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(GetPointTypeName)
	{
		TODO_TEST
	//	static tstring GetPointTypeName(PointType inType);
	}
}


SUITE(TestDogExistingPointsList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
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


	TEST(HasPoints)
	{
		TODO_TEST
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


	TEST(ExistingPoints)
	{
		TODO_TEST
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


	TEST(NumExistingPointsInVenue)
	{
		TODO_TEST
	//	int NumExistingPointsInVenue(tstring const& inVenue) const;
	}


	TEST(RenameVenue)
	{
		TODO_TEST
	//	int RenameVenue(
	//			tstring const& inOldVenue,
	//			tstring const& inNewVenue);
	}


	TEST(DeleteVenue)
	{
		TODO_TEST
	//	int DeleteVenue(tstring const& inVenue);
	}


	TEST(NumExistingPointsInDivision)
	{
		TODO_TEST
	//	int NumExistingPointsInDivision(
	//			ARBConfigVenuePtr inVenue,
	//			tstring const& inDiv) const;
	}


	TEST(RenameDivision)
	{
		TODO_TEST
	//	int RenameDivision(
	//			tstring const& inVenue,
	//			tstring const& inOldDiv,
	//			tstring const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
	//	int DeleteDivision(
	//			tstring const& inVenue,
	//			tstring const& inDiv);
	}


	TEST(NumLevelsInUse)
	{
		TODO_TEST
	//	int NumLevelsInUse(
	//			tstring const& inVenue,
	//			tstring const& inDiv,
	//			tstring const& inLevel) const;
	}


	TEST(RenameLevel)
	{
		TODO_TEST
	//	int RenameLevel(
	//			tstring const& inVenue,
	//			tstring const& inDiv,
	//			tstring const& inOldLevel,
	//			tstring const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(
	//			tstring const& inVenue,
	//			tstring const& inDiv,
	//			tstring const& inLevel);
	}


	TEST(NumEventsInUse)
	{
		TODO_TEST
	//	int NumEventsInUse(
	//			tstring const& inVenue,
	//			tstring const& inEvent) const;
	}


	TEST(RenameEvent)
	{
		TODO_TEST
	//	int RenameEvent(
	//			tstring const& inVenue,
	//			tstring const& inOldEvent,
	//			tstring const& inNewEvent);
	}


	TEST(DeleteEvent)
	{
		TODO_TEST
	//	int DeleteEvent(
	//			tstring const& inVenue,
	//			tstring const& inEvent);
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
	//			tstring const& inOldOther,
	//			tstring const& inNewOther);
	}


	TEST(DeleteOtherPoints)
	{
		TODO_TEST
	//	int DeleteOtherPoints(tstring const& inOther);
	}


	TEST(NumMultiQsInUse)
	{
		TODO_TEST
	//	int NumMultiQsInUse(
	//			tstring const& inVenue,
	//			tstring const& inMultiQ) const;
	}


	TEST(RenameMultiQs)
	{
		TODO_TEST
	//	int RenameMultiQs(
	//			tstring const& inVenue,
	//			tstring const& inOldMultiQ,
	//			tstring const& inNewMultiQ);
	}


	TEST(DeleteMultiQs)
	{
		TODO_TEST
	//	int DeleteMultiQs(
	//			ARBConfig const& inConfig,
	//			tstring const& inVenue);
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
	//	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
	}
}
