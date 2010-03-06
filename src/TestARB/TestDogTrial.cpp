/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogTrialPtr New();
		//	static ARBDogTrialPtr New(ARBCalendar const& inCal);
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogTrialPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogTrial& operator=(ARBDogTrial const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogTrial const& rhs) const;
		//	bool operator!=(ARBDogTrial const& rhs) const
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	virtual wxString GetGenericName() const
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig const& inConfig,
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


	TEST(SetMultiQs)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void SetMultiQs(ARBConfig const& inConfig);
		}
	}


	TEST(GetSpeedPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	short GetSpeedPoints(
		//			ARBConfig const& inConfig,
		//			wxString const& inDiv,
		//			wxString const& inLevel) const;
		}
	}


	TEST(HasVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool HasVenue(wxString const& inVenue) const;
		}
	}
}


SUITE(TestDogTrialList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig const& inConfig,
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
		//	void sort(bool inDescending);
		}
	}


	TEST(NumTrialsInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumTrialsInVenue(wxString const& inVenue) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameVenue(
		//			wxString const& inOldVenue,
		//			wxString const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteVenue(wxString const& inVenue);
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
		//			wxString const& inOldOther,
		//			wxString const& inNewOther);
		}
	}


	TEST(DeleteOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteOtherPoints(wxString const& inOther);
		}
	}


	TEST(NumMultiHostedTrialsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumMultiHostedTrialsInDivision(
		//			ARBConfig const& inConfig,
		//			wxString const& inVenue,
		//			wxString const& inDiv) const;
		}
	}


	TEST(NumRunsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumRunsInDivision(
		//			ARBConfigVenuePtr inVenue,
		//			wxString const& inDiv) const;
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameDivision(
		//			ARBConfigVenuePtr inVenue,
		//			wxString const& inOldDiv,
		//			wxString const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteDivision(
		//			ARBConfig const& inConfig,
		//			wxString const& inVenue,
		//			wxString const& inDiv);
		}
	}


	TEST(NumLevelsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumLevelsInUse(
		//			wxString const& inVenue,
		//			wxString const& inDiv,
		//			wxString const& inLevel) const;
		}
	}


	TEST(RenameLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameLevel(
		//			wxString const& inVenue,
		//			wxString const& inDiv,
		//			wxString const& inOldLevel,
		//			wxString const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(
		//			wxString const& inVenue,
		//			wxString const& inDiv,
		//			wxString const& inLevel);
		}
	}


	TEST(NumEventsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumEventsInUse(
		//			wxString const& inVenue,
		//			wxString const& inEvent) const;
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			wxString const& inVenue,
		//			wxString const& inOldEvent,
		//			wxString const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(
		//			wxString const& inVenue,
		//			wxString const& inEvent);
		}
	}


	TEST(AddTrial)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddTrial(ARBDogTrialPtr inTrial);
		}
	}


	TEST(DeleteTrial)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool DeleteTrial(ARBDogTrialPtr inTrial);
		}
	}
}
