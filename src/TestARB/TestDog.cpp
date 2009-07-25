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
 * @brief Test ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDog.h"
#include "Element.h"


SUITE(TestDog)
{
	TEST(New)
	{
		TODO_TEST
		//static ARBDogPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
		//ARBDogPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
		//ARBDog& operator=(ARBDog const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
		//bool operator==(ARBDog const& rhs) const;
		//bool operator!=(ARBDog const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
		//virtual tstring GetGenericName() const
	}


	TEST(Load)
	{
		TODO_TEST
		//bool Load(
		//		ARBConfig const& inConfig,
		//		ElementNodePtr inTree,
		//		ARBVersion const& inVersion,
		//		ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
		//bool Save(ElementNodePtr ioTree) const;
	}


	TEST(RenameVenue)
	{
		TODO_TEST
		//int RenameVenue(
		//		tstring const& inOldVenue,
		//		tstring const& inNewVenue);
	}


	TEST(DeleteVenue)
	{
		TODO_TEST
		//int DeleteVenue(tstring const& inVenue);
	}


	TEST(RenameDivision)
	{
		TODO_TEST
		//int RenameDivision(
		//		ARBConfigVenuePtr inVenue,
		//		tstring const& inOldDiv,
		//		tstring const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
		//int DeleteDivision(
		//		ARBConfig const& inConfig,
		//		tstring const& inVenue,
		//		tstring const& inDiv);
	}
}


SUITE(TestDogList)
{
	TEST(Load)
	{
		TODO_TEST
		//bool Load(
		//		ARBConfig const& inConfig,
		//		ElementNodePtr inTree,
		//		ARBVersion const& inVersion,
		//		ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	}


	TEST(SetMultiQs)
	{
		TODO_TEST
		//void SetMultiQs(ARBConfig const& inConfig);
	}


	TEST(NumExistingPointsInVenue)
	{
		TODO_TEST
		//int NumExistingPointsInVenue(tstring const& inVenue) const;
	}


	TEST(NumRegNumsInVenue)
	{
		TODO_TEST
		//int NumRegNumsInVenue(tstring const& inVenue) const;
	}


	TEST(NumTitlesInVenue)
	{
		TODO_TEST
		//int NumTitlesInVenue(tstring const& inVenue) const;
	}


	TEST(NumTrialsInVenue)
	{
		TODO_TEST
		//int NumTrialsInVenue(tstring const& inVenue) const;
	}


	TEST(RenameVenue)
	{
		TODO_TEST
		//int RenameVenue(
		//		tstring const& inOldVenue,
		//		tstring const& inNewVenue);
	}


	TEST(DeleteVenue)
	{
		TODO_TEST
		//int DeleteVenue(tstring const& inVenue);
	}


	TEST(NumOtherPointsInUse)
	{
		TODO_TEST
		//int NumOtherPointsInUse(tstring const& inOther) const;
	}


	TEST(RenameOtherPoints)
	{
		TODO_TEST
		//int RenameOtherPoints(
		//		tstring const& inOldOther,
		//		tstring const& inNewOther);
	}


	TEST(DeleteOtherPoints)
	{
		TODO_TEST
		//int DeleteOtherPoints(tstring const& inOther);
	}


	TEST(NumMultiQsInUse)
	{
		TODO_TEST
		//int NumMultiQsInUse(
		//		tstring const& inVenue,
		//		tstring const& inMultiQ) const;
	}


	TEST(RenameMultiQs)
	{
		TODO_TEST
		//int RenameMultiQs(
		//		tstring const& inVenue,
		//		tstring const& inOldMultiQ,
		//		tstring const& inNewMultiQ);
	}


	TEST(DeleteMultiQs)
	{
		TODO_TEST
		//int DeleteMultiQs(
		//		ARBConfig const& inConfig,
		//		tstring const& inVenue);
	}


	TEST(NumMultiHostedTrialsInDivision)
	{
		TODO_TEST
		//int NumMultiHostedTrialsInDivision(
		//		ARBConfig const& inConfig,
		//		tstring const& inVenue,
		//		tstring const& inDiv) const;
	}


	TEST(NumExistingPointsInDivision)
	{
		TODO_TEST
		//int NumExistingPointsInDivision(
		//		ARBConfigVenuePtr inVenue,
		//		tstring const& inDiv) const;
	}


	TEST(NumRunsInDivision)
	{
		TODO_TEST
		//int NumRunsInDivision(
		//		ARBConfigVenuePtr inVenue,
		//		tstring const& inDiv) const;
	}


	TEST(RenameDivision)
	{
		TODO_TEST
		//int RenameDivision(
		//		ARBConfigVenuePtr inVenue,
		//		tstring const& inOldDiv,
		//		tstring const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
		//int DeleteDivision(
		//		ARBConfig const& inConfig,
		//		tstring const& inVenue,
		//		tstring const& inDiv);
	}


	TEST(NumLevelsInUse)
	{
		TODO_TEST
		//int NumLevelsInUse(
		//		tstring const& inVenue,
		//		tstring const& inDiv,
		//		tstring const& inLevel) const;
	}


	TEST(RenameLevel)
	{
		TODO_TEST
		//int RenameLevel(
		//		tstring const& inVenue,
		//		tstring const& inDiv,
		//		tstring const& inOldLevel,
		//		tstring const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
		//int DeleteLevel(
		//		tstring const& inVenue,
		//		tstring const& inDiv,
		//		tstring const& inLevel);
	}


	TEST(NumTitlesInUse)
	{
		TODO_TEST
		//int NumTitlesInUse(
		//		tstring const& inVenue,
		//		tstring const& inTitle) const;
	}


	TEST(RenameTitle)
	{
		TODO_TEST
		//int RenameTitle(
		//		tstring const& inVenue,
		//		tstring const& inOldTitle,
		//		tstring const& inNewTitle);
	}


	TEST(DeleteTitle)
	{
		TODO_TEST
		//int DeleteTitle(
		//		tstring const& inVenue,
		//		tstring const& inTitle);
	}


	TEST(NumEventsInUse)
	{
		TODO_TEST
		//int NumEventsInUse(
		//		tstring const& inVenue,
		//		tstring const& inEvent) const;
	}


	TEST(RenameEvent)
	{
		TODO_TEST
		//int RenameEvent(
		//		tstring const& inVenue,
		//		tstring const& inOldEvent,
		//		tstring const& inNewEvent);
	}


	TEST(DeleteEvent)
	{
		TODO_TEST
		//int DeleteEvent(
		//		tstring const& inVenue,
		//		tstring const& inEvent);
	}


	TEST(AddDelete)
	{
		TODO_TEST
		//bool AddDog(ARBDogPtr inDog);
	}
}
