/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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


SUITE(TestDogClub)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogClubPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogClubPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogClub& operator=(ARBDogClub const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogClub const& rhs) const;
	//	bool operator!=(ARBDogClub const& rhs) const
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
}


SUITE(TestDogClubList)
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


	TEST(GetPrimaryClub)
	{
		TODO_TEST
	//	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;
	}


	TEST(GetPrimaryClubName)
	{
		TODO_TEST
	//	wxString GetPrimaryClubName() const;
	}


	TEST(GetPrimaryClubVenue)
	{
		TODO_TEST
	//	wxString GetPrimaryClubVenue() const;
	}


	TEST(FindEvent)
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


	TEST(FindVenue)
	{
		TODO_TEST
	//	bool FindVenue(
	//			wxString const& inVenue,
	//			ARBDogClubPtr* outClub = NULL) const;
	}


	TEST(AddDelete)
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
