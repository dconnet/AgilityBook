/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDog.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestDog)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//static ARBDogPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//ARBDogPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//ARBDog& operator=(ARBDog const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool operator==(ARBDog const& rhs) const;
			//bool operator!=(ARBDog const& rhs) const
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual std::wstring GetGenericName() const
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Load(
			//		ARBConfig const& inConfig,
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Save(ElementNodePtr ioTree) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameVenue(
			//		std::wstring const& inOldVenue,
			//		std::wstring const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteVenue(std::wstring const& inVenue);
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inOldDiv,
			//		std::wstring const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv);
		}
	}
}


SUITE(TestDogList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Load(
			//		ARBConfig const& inConfig,
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	TEST(SetMultiQs)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//void SetMultiQs(ARBConfig const& inConfig);
		}
	}


	TEST(NumExistingPointsInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumExistingPointsInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(NumRegNumsInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumRegNumsInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(NumTitlesInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumTitlesInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(NumTrialsInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumTrialsInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameVenue(
			//		std::wstring const& inOldVenue,
			//		std::wstring const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteVenue(std::wstring const& inVenue);
		}
	}


	TEST(NumOtherPointsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumOtherPointsInUse(std::wstring const& inOther) const;
		}
	}


	TEST(RenameOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameOtherPoints(
			//		std::wstring const& inOldOther,
			//		std::wstring const& inNewOther);
		}
	}


	TEST(DeleteOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteOtherPoints(std::wstring const& inOther);
		}
	}


	TEST(NumMultiQsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumMultiQsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inMultiQ) const;
		}
	}


	TEST(RenameMultiQs)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameMultiQs(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldMultiQ,
			//		std::wstring const& inNewMultiQ);
		}
	}


	TEST(DeleteMultiQs)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteMultiQs(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue);
		}
	}


	TEST(NumMultiHostedTrialsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumMultiHostedTrialsInDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	TEST(NumExistingPointsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumExistingPointsInDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	TEST(NumRunsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumRunsInDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inOldDiv,
			//		std::wstring const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv);
		}
	}


	TEST(NumLevelsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumLevelsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inLevel) const;
		}
	}


	TEST(RenameLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameLevel(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inOldLevel,
			//		std::wstring const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteLevel(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inLevel);
		}
	}


	TEST(NumTitlesInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumTitlesInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inTitle) const;
		}
	}


	TEST(RenameTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameTitle(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldTitle,
			//		std::wstring const& inNewTitle);
		}
	}


	TEST(DeleteTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteTitle(
			//		std::wstring const& inVenue,
			//		std::wstring const& inTitle);
		}
	}


	TEST(NumEventsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int NumEventsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inEvent) const;
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int RenameEvent(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldEvent,
			//		std::wstring const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int DeleteEvent(
			//		std::wstring const& inVenue,
			//		std::wstring const& inEvent);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool AddDog(ARBDogPtr inDog);
		}
	}
}
