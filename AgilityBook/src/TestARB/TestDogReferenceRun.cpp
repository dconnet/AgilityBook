/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogReferenceRun and ARBDogReferenceRunList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogReferenceRun.h"
#include "Element.h"


SUITE(TestDogReferenceRun)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogReferenceRunPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogReferenceRunPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogReferenceRun& operator=(ARBDogReferenceRun const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogReferenceRun const& rhs) const;
	//	bool operator!=(ARBDogReferenceRun const& rhs) const
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


SUITE(TestDogReferenceRunList)
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


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddReferenceRun(ARBDogReferenceRunPtr inRef);
	//	bool DeleteReferenceRun(ARBDogReferenceRunPtr inRef);
	}
}
