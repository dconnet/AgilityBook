/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogRunOtherPoints and ARBDogRunOtherPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRunOtherPoints.h"
#include "Element.h"


SUITE(TestDogRunOtherPoints)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogRunOtherPointsPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogRunOtherPointsPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRunOtherPoints& operator=(ARBDogRunOtherPoints const& rhs);
	}


	TEST(Comparfe)
	{
		TODO_TEST
	//	bool operator==(ARBDogRunOtherPoints const& rhs) const;
	//	bool operator!=(ARBDogRunOtherPoints const& rhs) const
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


SUITE(TestDogRunOtherPointsList)
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


	TEST(AddOtherPoints)
	{
		TODO_TEST
	//	bool AddOtherPoints(ARBDogRunOtherPointsPtr inOther);
	}
}
