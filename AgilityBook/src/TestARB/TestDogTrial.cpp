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
 * @brief Test ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogTrial.h"
#include "Element.h"


BEGIN_TEST(DogTrial_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogTrialPtr New();
//	static ARBDogTrialPtr New(ARBCalendar const& inCal);
}
END_TEST


BEGIN_TEST(DogTrial_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogTrialPtr Clone() const;
}
END_TEST


BEGIN_TEST(DogTrial_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogTrial& operator=(ARBDogTrial const& rhs);
}
END_TEST


BEGIN_TEST(DogTrial_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogTrial const& rhs) const;
//	bool operator!=(ARBDogTrial const& rhs) const
}
END_TEST


BEGIN_TEST(DogTrial_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(DogTrial_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogTrial_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogTrial_SetMultiQs)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void SetMultiQs(ARBConfig const& inConfig);
}
END_TEST


BEGIN_TEST(DogTrial_GetSpeedPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	short GetSpeedPoints(
//			ARBConfig const& inConfig,
//			tstring const& inDiv,
//			tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(DogTrial_HasVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool HasVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogTrialList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogTrialList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogTrialList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort(bool inDescending);
}
END_TEST


BEGIN_TEST(DogTrialList_NumTrialsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumTrialsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogTrialList_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameVenue(
//			tstring const& inOldVenue,
//			tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogTrialList_NumOtherPointsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumOtherPointsInUse(tstring const& inOther) const;
}
END_TEST


BEGIN_TEST(DogTrialList_RenameOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameOtherPoints(
//			tstring const& inOldOther,
//			tstring const& inNewOther);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteOtherPoints(tstring const& inOther);
}
END_TEST


BEGIN_TEST(DogTrialList_NumMultiHostedTrialsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumMultiHostedTrialsInDivision(
//			ARBConfig const& inConfig,
//			tstring const& inVenue,
//			tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogTrialList_NumRunsInDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumRunsInDivision(
//			ARBConfigVenuePtr inVenue,
//			tstring const& inDiv) const;
}
END_TEST


BEGIN_TEST(DogTrialList_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameDivision(
//			ARBConfigVenuePtr inVenue,
//			tstring const& inOldDiv,
//			tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteDivision(
//			ARBConfig const& inConfig,
//			tstring const& inVenue,
//			tstring const& inDiv);
}
END_TEST


BEGIN_TEST(DogTrialList_NumLevelsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumLevelsInUse(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(DogTrialList_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameLevel(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inOldLevel,
//			tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteLevel(
//			tstring const& inVenue,
//			tstring const& inDiv,
//			tstring const& inLevel);
}
END_TEST


BEGIN_TEST(DogTrialList_NumEventsInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumEventsInUse(
//			tstring const& inVenue,
//			tstring const& inEvent) const;
}
END_TEST


BEGIN_TEST(DogTrialList_RenameEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameEvent(
//			tstring const& inVenue,
//			tstring const& inOldEvent,
//			tstring const& inNewEvent);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteEvent(
//			tstring const& inVenue,
//			tstring const& inEvent);
}
END_TEST


BEGIN_TEST(DogTrialList_AddTrial)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddTrial(ARBDogTrialPtr inTrial);
}
END_TEST


BEGIN_TEST(DogTrialList_DeleteTrial)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool DeleteTrial(ARBDogTrialPtr inTrial);
}
END_TEST
