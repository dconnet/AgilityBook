/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
		//	virtual std::wstring GetGenericName() const
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
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel) const;
		}
	}


	TEST(HasVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool HasVenue(std::wstring const& inVenue) const;
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
		//	int NumTrialsInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameVenue(
		//			std::wstring const& inOldVenue,
		//			std::wstring const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteVenue(std::wstring const& inVenue);
		}
	}


	TEST(NumOtherPointsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumOtherPointsInUse(std::wstring const& inOther) const;
		}
	}


	TEST(RenameOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameOtherPoints(
		//			std::wstring const& inOldOther,
		//			std::wstring const& inNewOther);
		}
	}


	TEST(DeleteOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteOtherPoints(std::wstring const& inOther);
		}
	}


	TEST(NumMultiHostedTrialsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumMultiHostedTrialsInDivision(
		//			ARBConfig const& inConfig,
		//			std::wstring const& inVenue,
		//			std::wstring const& inDiv) const;
		}
	}


	TEST(NumRunsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumRunsInDivision(
		//			ARBConfigVenuePtr inVenue,
		//			std::wstring const& inDiv) const;
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameDivision(
		//			ARBConfigVenuePtr inVenue,
		//			std::wstring const& inOldDiv,
		//			std::wstring const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteDivision(
		//			ARBConfig const& inConfig,
		//			std::wstring const& inVenue,
		//			std::wstring const& inDiv);
		}
	}


	TEST(NumLevelsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumLevelsInUse(
		//			std::wstring const& inVenue,
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel) const;
		}
	}


	TEST(RenameLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameLevel(
		//			std::wstring const& inVenue,
		//			std::wstring const& inDiv,
		//			std::wstring const& inOldLevel,
		//			std::wstring const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(
		//			std::wstring const& inVenue,
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel);
		}
	}


	TEST(NumEventsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumEventsInUse(
		//			std::wstring const& inVenue,
		//			std::wstring const& inEvent) const;
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			std::wstring const& inVenue,
		//			std::wstring const& inOldEvent,
		//			std::wstring const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(
		//			std::wstring const& inVenue,
		//			std::wstring const& inEvent);
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
