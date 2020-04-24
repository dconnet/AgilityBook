/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDogClub.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("DogClub")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBDogClubPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogClubPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogClub& operator=(ARBDogClub const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogClub const& rhs) const;
			//	bool operator!=(ARBDogClub const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	virtual std::wstring GetGenericName() const
		}
	}


	SECTION("Load")
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


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Save(ElementNodePtr ioTree) const;
		}
	}
}


TEST_CASE("DogClubList")
{
	SECTION("Load")
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


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	SECTION("GetPrimaryClub")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool GetPrimaryClub(ARBDogClubPtr* outClub = nullptr) const;
		}
	}


	SECTION("GetPrimaryClubName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	std::wstring GetPrimaryClubName() const;
		}
	}


	SECTION("GetPrimaryClubVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	std::wstring GetPrimaryClubVenue() const;
		}
	}


	SECTION("FindEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindEvent(
			//			ARBConfig const& inConfig,
			//			std::wstring const& inEvent,
			//			std::wstring const& inDivision,
			//			std::wstring const& inLevel,
			//			ARBDate const& inDate,
			//			ARBErrorCallback& ioCallback,
			//			ARBConfigEventPtr* outEvent = nullptr,
			//			ARBConfigScoringPtr* outScoring = nullptr) const;
		}
	}


	SECTION("FindVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindVenue(
			//			std::wstring const& inVenue,
			//			ARBDogClubPtr* outClub = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddClub(
			//			std::wstring const& inName,
			//			std::wstring const& inVenue,
			//			ARBDogClubPtr* outClub = nullptr);
			//	bool DeleteClub(
			//			std::wstring const& inName,
			//			std::wstring const& inVenue);
		}
	}
}
