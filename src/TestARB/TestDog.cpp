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


BEGIN_TEST(Dog_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//static ARBDogPtr New();
}
END_TEST


BEGIN_TEST(Dog_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//ARBDogPtr Clone() const;
}
END_TEST


BEGIN_TEST(Dog_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//ARBDog& operator=(ARBDog const& rhs);
}
END_TEST


BEGIN_TEST(Dog_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool operator==(ARBDog const& rhs) const;
	//bool operator!=(ARBDog const& rhs) const
}
END_TEST


BEGIN_TEST(Dog_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(Dog_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Load(
	//		ARBConfig const& inConfig,
	//		ElementNodePtr inTree,
	//		ARBVersion const& inVersion,
	//		ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(Dog_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(Dog_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameVenue(
	//		tstring const& inOldVenue,
	//		tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(Dog_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(Dog_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameDivision(
	//		ARBConfigVenuePtr inVenue,
	//		tstring const& inOldDiv,
	//		tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(Dog_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteDivision(
	//		ARBConfig const& inConfig,
	//		tstring const& inVenue,
	//		tstring const& inDiv);
}
END_TEST


BEGIN_TEST(DogList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Load(
	//		ARBConfig const& inConfig,
	//		ElementNodePtr inTree,
	//		ARBVersion const& inVersion,
	//		ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogList_SetMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//void SetMultiQs(ARBConfig const& inConfig);
}
END_TEST


BEGIN_TEST(DogList_NumExistingPointsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumExistingPointsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogList_NumRegNumsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumRegNumsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogList_NumTitlesInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumTitlesInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogList_NumTrialsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumTrialsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogList_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameVenue(
	//		tstring const& inOldVenue,
	//		tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(DogList_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogList_NumOtherPointsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumOtherPointsInUse(tstring const& inOther) const;
}
END_TEST


BEGIN_TEST(DogList_RenameOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameOtherPoints(
	//		tstring const& inOldOther,
	//		tstring const& inNewOther);
}
END_TEST


BEGIN_TEST(DogList_DeleteOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteOtherPoints(tstring const& inOther);
}
END_TEST


BEGIN_TEST(DogList_NumMultiQsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumMultiQsInUse(
	//		tstring const& inVenue,
	//		tstring const& inMultiQ) const;
}
END_TEST


BEGIN_TEST(DogList_RenameMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameMultiQs(
	//		tstring const& inVenue,
	//		tstring const& inOldMultiQ,
	//		tstring const& inNewMultiQ);
}
END_TEST


BEGIN_TEST(DogList_DeleteMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteMultiQs(
	//		ARBConfig const& inConfig,
	//		tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogList_NumMultiHostedTrialsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumMultiHostedTrialsInDivision(
	//		ARBConfig const& inConfig,
	//		tstring const& inVenue,
	//		tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogList_NumExistingPointsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumExistingPointsInDivision(
	//		ARBConfigVenuePtr inVenue,
	//		tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogList_NumRunsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumRunsInDivision(
	//		ARBConfigVenuePtr inVenue,
	//		tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogList_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameDivision(
	//		ARBConfigVenuePtr inVenue,
	//		tstring const& inOldDiv,
	//		tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(DogList_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteDivision(
	//		ARBConfig const& inConfig,
	//		tstring const& inVenue,
	//		tstring const& inDiv);
}
END_TEST


BEGIN_TEST(DogList_NumLevelsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumLevelsInUse(
	//		tstring const& inVenue,
	//		tstring const& inDiv,
	//		tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(DogList_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameLevel(
	//		tstring const& inVenue,
	//		tstring const& inDiv,
	//		tstring const& inOldLevel,
	//		tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(DogList_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteLevel(
	//		tstring const& inVenue,
	//		tstring const& inDiv,
	//		tstring const& inLevel);
}
END_TEST


BEGIN_TEST(DogList_NumTitlesInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumTitlesInUse(
	//		tstring const& inVenue,
	//		tstring const& inTitle) const;
}
END_TEST


BEGIN_TEST(DogList_RenameTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameTitle(
	//		tstring const& inVenue,
	//		tstring const& inOldTitle,
	//		tstring const& inNewTitle);
}
END_TEST


BEGIN_TEST(DogList_DeleteTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteTitle(
	//		tstring const& inVenue,
	//		tstring const& inTitle);
}
END_TEST


BEGIN_TEST(DogList_NumEventsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int NumEventsInUse(
	//		tstring const& inVenue,
	//		tstring const& inEvent) const;
}
END_TEST


BEGIN_TEST(DogList_RenameEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int RenameEvent(
	//		tstring const& inVenue,
	//		tstring const& inOldEvent,
	//		tstring const& inNewEvent);
}
END_TEST


BEGIN_TEST(DogList_DeleteEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int DeleteEvent(
	//		tstring const& inVenue,
	//		tstring const& inEvent);
}
END_TEST


BEGIN_TEST(DogList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool AddDog(ARBDogPtr inDog);
}
END_TEST
