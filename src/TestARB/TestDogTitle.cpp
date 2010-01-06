/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogTitle.h"
#include "Element.h"


SUITE(TestDogTitle)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogTitlePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogTitlePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogTitle& operator=(ARBDogTitle const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogTitle const& rhs) const;
	//	bool operator!=(ARBDogTitle const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual wxString GetGenericName() const;
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


SUITE(TestDogTitleList)
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
	//	void sort();
	}


	TEST(NumTitlesInVenue)
	{
		TODO_TEST
	//	int NumTitlesInVenue(wxString const& inVenue) const;
	}


	TEST(FindTitle)
	{
		TODO_TEST
	//	bool FindTitle(
	//			wxString const& inVenue,
	//			wxString const& inTitle,
	//			ARBDogTitlePtr* outTitle = NULL) const;
	}


	TEST(FindMaxInstance)
	{
		TODO_TEST
	//	short FindMaxInstance(
	//			wxString const& inVenue,
	//			wxString const& inTitle) const;
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


	TEST(NumTitlesInUse)
	{
		TODO_TEST
	//	int NumTitlesInUse(
	//			wxString const& inVenue,
	//			wxString const& inTitle) const;
	}


	TEST(RenameTitle)
	{
		TODO_TEST
	//	int RenameTitle(
	//			wxString const& inVenue,
	//			wxString const& inOldTitle,
	//			wxString const& inNewTitle);
	}


	TEST(AddTitle)
	{
		TODO_TEST
	//	bool AddTitle(ARBDogTitlePtr inTitle);
	}


	TEST(DeleteTitle)
	{
		TODO_TEST
	//	bool DeleteTitle(ARBDogTitlePtr inTitle);
	}
}
