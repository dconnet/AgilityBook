/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogRegNum.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("DogRegNum")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogRegNumPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRegNumPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogRegNum const& rhs) const;
		//	bool operator!=(ARBDogRegNum const& rhs) const
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


TEST_CASE("DogRegNumList")
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


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void sort();
		}
	}


	SECTION("NumRegNumsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumRegNumsInVenue(std::wstring const& inVenue) const;
		}
	}


	SECTION("RenameVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameVenue(
		//			std::wstring const& inOldVenue,
		//			std::wstring const& inNewVenue);
		}
	}


	SECTION("DeleteVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteVenue(std::wstring const& inVenue);
		}
	}


	SECTION("FindRegNum")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindRegNum(
		//			std::wstring const& inVenue,
		//			ARBDogRegNumPtr* outRegNum = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddRegNum(
		//			std::wstring const& inVenue,
		//			std::wstring const& inNumber,
		//			ARBDogRegNumPtr* outRegNum = nullptr);
		//	bool AddRegNum(ARBDogRegNumPtr inRegNum);
		//	int DeleteRegNum(
		//			std::wstring const& inVenue,
		//			std::wstring const& inNumber);
		}
	}
}
