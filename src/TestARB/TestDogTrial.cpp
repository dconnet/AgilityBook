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
 * @brief Test ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogTrial.h"
#include "Element.h"


SUITE(TestDogTrial)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogTrialPtr New();
	//	static ARBDogTrialPtr New(ARBCalendar const& inCal);
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogTrialPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogTrial& operator=(ARBDogTrial const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogTrial const& rhs) const;
	//	bool operator!=(ARBDogTrial const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual wxString GetGenericName() const
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


	TEST(SetMultiQs)
	{
		TODO_TEST
	//	void SetMultiQs(ARBConfig const& inConfig);
	}


	TEST(GetSpeedPoints)
	{
		TODO_TEST
	//	short GetSpeedPoints(
	//			ARBConfig const& inConfig,
	//			wxString const& inDiv,
	//			wxString const& inLevel) const;
	}


	TEST(HasVenue)
	{
		TODO_TEST
	//	bool HasVenue(wxString const& inVenue) const;
	}
}


SUITE(TestDogTrialList)
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
	//	void sort(bool inDescending);
	}


	TEST(NumTrialsInVenue)
	{
		TODO_TEST
	//	int NumTrialsInVenue(wxString const& inVenue) const;
	}


	TEST(RenameVenue)
	{
		TODO_TEST
	//	int RenameVenue(
	//			wxString const& inOldVenue,
	//			wxString const& inNewVenue);
	}


	TEST(DeleteVenue)
	{
		TODO_TEST
	//	int DeleteVenue(wxString const& inVenue);
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
	//			wxString const& inOldOther,
	//			wxString const& inNewOther);
	}


	TEST(DeleteOtherPoints)
	{
		TODO_TEST
	//	int DeleteOtherPoints(wxString const& inOther);
	}


	TEST(NumMultiHostedTrialsInDivision)
	{
		TODO_TEST
	//	int NumMultiHostedTrialsInDivision(
	//			ARBConfig const& inConfig,
	//			wxString const& inVenue,
	//			wxString const& inDiv) const;
	}


	TEST(NumRunsInDivision)
	{
		TODO_TEST
	//	int NumRunsInDivision(
	//			ARBConfigVenuePtr inVenue,
	//			wxString const& inDiv) const;
	}


	TEST(RenameDivision)
	{
		TODO_TEST
	//	int RenameDivision(
	//			ARBConfigVenuePtr inVenue,
	//			wxString const& inOldDiv,
	//			wxString const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
	//	int DeleteDivision(
	//			ARBConfig const& inConfig,
	//			wxString const& inVenue,
	//			wxString const& inDiv);
	}


	TEST(NumLevelsInUse)
	{
		TODO_TEST
	//	int NumLevelsInUse(
	//			wxString const& inVenue,
	//			wxString const& inDiv,
	//			wxString const& inLevel) const;
	}


	TEST(RenameLevel)
	{
		TODO_TEST
	//	int RenameLevel(
	//			wxString const& inVenue,
	//			wxString const& inDiv,
	//			wxString const& inOldLevel,
	//			wxString const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(
	//			wxString const& inVenue,
	//			wxString const& inDiv,
	//			wxString const& inLevel);
	}


	TEST(NumEventsInUse)
	{
		TODO_TEST
	//	int NumEventsInUse(
	//			wxString const& inVenue,
	//			wxString const& inEvent) const;
	}


	TEST(RenameEvent)
	{
		TODO_TEST
	//	int RenameEvent(
	//			wxString const& inVenue,
	//			wxString const& inOldEvent,
	//			wxString const& inNewEvent);
	}


	TEST(DeleteEvent)
	{
		TODO_TEST
	//	int DeleteEvent(
	//			wxString const& inVenue,
	//			wxString const& inEvent);
	}


	TEST(AddTrial)
	{
		TODO_TEST
	//	bool AddTrial(ARBDogTrialPtr inTrial);
	}


	TEST(DeleteTrial)
	{
		TODO_TEST
	//	bool DeleteTrial(ARBDogTrialPtr inTrial);
	}
}
