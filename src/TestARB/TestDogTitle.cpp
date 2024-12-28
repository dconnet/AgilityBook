/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDogTitle.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("DogTitle")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBDogTitlePtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogTitlePtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogTitle& operator=(ARBDogTitle const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogTitle const& rhs) const;
			//	bool operator!=(ARBDogTitle const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	virtual wxString GetGenericName() const;
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


TEST_CASE("DogTitleList")
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


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void sort();
		}
	}


	SECTION("NumTitlesInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumTitlesInVenue(wxString const& inVenue) const;
		}
	}


	SECTION("FindTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindTitle(
			//			wxString const& inVenue,
			//			wxString const& inTitle,
			//			ARBDogTitlePtr* outTitle = nullptr) const;
		}
	}


	SECTION("FindMaxInstance")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	short FindMaxInstance(
			//			wxString const& inVenue,
			//			wxString const& inTitle) const;
		}
	}


	SECTION("RenameVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameVenue(
			//			wxString const& inOldVenue,
			//			wxString const& inNewVenue);
		}
	}


	SECTION("DeleteVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteVenue(wxString const& inVenue);
		}
	}


	SECTION("NumTitlesInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumTitlesInUse(
			//			wxString const& inVenue,
			//			wxString const& inTitle) const;
		}
	}


	SECTION("RenameTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameTitle(
			//			wxString const& inVenue,
			//			wxString const& inOldTitle,
			//			wxString const& inNewTitle);
		}
	}


	SECTION("AddTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddTitle(ARBDogTitlePtr inTitle);
		}
	}


	SECTION("DeleteTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool DeleteTitle(ARBDogTitlePtr inTitle);
		}
	}
}

} // namespace dconSoft
