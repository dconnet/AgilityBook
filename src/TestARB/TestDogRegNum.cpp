/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRegNum.h"
#include "Element.h"


SUITE(TestDogRegNum)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogRegNumPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogRegNumPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogRegNum const& rhs) const;
	//	bool operator!=(ARBDogRegNum const& rhs) const
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


SUITE(TestDogRegNumList)
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


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(NumRegNumsInVenue)
	{
		TODO_TEST
	//	int NumRegNumsInVenue(wxString const& inVenue) const;
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


	TEST(FindRegNum)
	{
		TODO_TEST
	//	bool FindRegNum(
	//			wxString const& inVenue,
	//			ARBDogRegNumPtr* outRegNum = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddRegNum(
	//			wxString const& inVenue,
	//			wxString const& inNumber,
	//			ARBDogRegNumPtr* outRegNum = NULL);
	//	bool AddRegNum(ARBDogRegNumPtr inRegNum);
	//	int DeleteRegNum(
	//			wxString const& inVenue,
	//			wxString const& inNumber);
	}
}
