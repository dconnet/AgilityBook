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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogClub.h"
#include "Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestDogClub)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogClubPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogClubPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogClub& operator=(ARBDogClub const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogClub const& rhs) const;
		//	bool operator!=(ARBDogClub const& rhs) const
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
}


SUITE(TestDogClubList)
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


	TEST(GetPrimaryClub)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;
		}
	}


	TEST(GetPrimaryClubName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	wxString GetPrimaryClubName() const;
		}
	}


	TEST(GetPrimaryClubVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	wxString GetPrimaryClubVenue() const;
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			ARBConfig const& inConfig,
		//			wxString const& inEvent,
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate,
		//			ARBErrorCallback& ioCallback,
		//			ARBConfigEventPtr* outEvent = NULL,
		//			ARBConfigScoringPtr* outScoring = NULL) const;
		}
	}


	TEST(FindVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindVenue(
		//			wxString const& inVenue,
		//			ARBDogClubPtr* outClub = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddClub(
		//			wxString const& inName,
		//			wxString const& inVenue,
		//			ARBDogClubPtr* outClub = NULL);
		//	bool DeleteClub(
		//			wxString const& inName,
		//			wxString const& inVenue);
		}
	}
}
