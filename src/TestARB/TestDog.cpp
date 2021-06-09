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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("Dog")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// static ARBDogPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// ARBDogPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// ARBDog& operator=(ARBDog const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool operator==(ARBDog const& rhs) const;
			// bool operator!=(ARBDog const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// virtual std::wstring GetGenericName() const
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Load(
			//		ARBConfig const& inConfig,
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("RenameVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameVenue(
			//		std::wstring const& inOldVenue,
			//		std::wstring const& inNewVenue);
		}
	}


	SECTION("DeleteVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteVenue(std::wstring const& inVenue);
		}
	}


	SECTION("RenameDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inOldDiv,
			//		std::wstring const& inNewDiv);
		}
	}


	SECTION("DeleteDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv);
		}
	}
}


TEST_CASE("DogList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Load(
			//		ARBConfig const& inConfig,
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	SECTION("SetMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// void SetMultiQs(ARBConfig const& inConfig);
		}
	}


	SECTION("NumExistingPointsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumExistingPointsInVenue(std::wstring const& inVenue) const;
		}
	}


	SECTION("NumRegNumsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumRegNumsInVenue(std::wstring const& inVenue) const;
		}
	}


	SECTION("NumTitlesInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumTitlesInVenue(std::wstring const& inVenue) const;
		}
	}


	SECTION("NumTrialsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumTrialsInVenue(std::wstring const& inVenue) const;
		}
	}


	SECTION("RenameVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameVenue(
			//		std::wstring const& inOldVenue,
			//		std::wstring const& inNewVenue);
		}
	}


	SECTION("DeleteVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteVenue(std::wstring const& inVenue);
		}
	}


	SECTION("NumOtherPointsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumOtherPointsInUse(std::wstring const& inOther) const;
		}
	}


	SECTION("RenameOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameOtherPoints(
			//		std::wstring const& inOldOther,
			//		std::wstring const& inNewOther);
		}
	}


	SECTION("DeleteOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteOtherPoints(std::wstring const& inOther);
		}
	}


	SECTION("NumMultiQsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumMultiQsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inMultiQ) const;
		}
	}


	SECTION("RenameMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameMultiQs(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldMultiQ,
			//		std::wstring const& inNewMultiQ);
		}
	}


	SECTION("DeleteMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteMultiQs(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue);
		}
	}


	SECTION("NumMultiHostedTrialsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumMultiHostedTrialsInDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	SECTION("NumExistingPointsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumExistingPointsInDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	SECTION("NumRunsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumRunsInDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inDiv) const;
		}
	}


	SECTION("RenameDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameDivision(
			//		ARBConfigVenuePtr inVenue,
			//		std::wstring const& inOldDiv,
			//		std::wstring const& inNewDiv);
		}
	}


	SECTION("DeleteDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteDivision(
			//		ARBConfig const& inConfig,
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv);
		}
	}


	SECTION("NumLevelsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumLevelsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inLevel) const;
		}
	}


	SECTION("RenameLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameLevel(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inOldLevel,
			//		std::wstring const& inNewLevel);
		}
	}


	SECTION("DeleteLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteLevel(
			//		std::wstring const& inVenue,
			//		std::wstring const& inDiv,
			//		std::wstring const& inLevel);
		}
	}


	SECTION("NumTitlesInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumTitlesInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inTitle) const;
		}
	}


	SECTION("RenameTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameTitle(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldTitle,
			//		std::wstring const& inNewTitle);
		}
	}


	SECTION("DeleteTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteTitle(
			//		std::wstring const& inVenue,
			//		std::wstring const& inTitle);
		}
	}


	SECTION("NumEventsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int NumEventsInUse(
			//		std::wstring const& inVenue,
			//		std::wstring const& inEvent) const;
		}
	}


	SECTION("RenameEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int RenameEvent(
			//		std::wstring const& inVenue,
			//		std::wstring const& inOldEvent,
			//		std::wstring const& inNewEvent);
		}
	}


	SECTION("DeleteEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// int DeleteEvent(
			//		std::wstring const& inVenue,
			//		std::wstring const& inEvent);
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool AddDog(ARBDogPtr inDog);
		}
	}
}
