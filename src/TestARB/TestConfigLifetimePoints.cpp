/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigLifetimePoints and ARBConfigLifetimePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigLifetimePoints.h"
#include "Element.h"


SUITE(TestConfigLifetimePoints)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigLifetimePointsPtr New();
	//	static ARBConfigLifetimePointsPtr New(double inPoints, double inFaults);
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigLifetimePointsPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigLifetimePoints const& rhs) const;
	//	bool operator!=(ARBConfigLifetimePoints const& rhs) const
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


SUITE(TestConfigLifetimePointsList)
{
	TEST(New)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(GetLifetimePoints)
	{
		TODO_TEST
	//	double GetLifetimePoints(double inFaults) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindLifetimePoints(
	//			double inFaults,
	//			ARBConfigLifetimePointsPtr* outPoints = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddLifetimePoints(
	//			double inPoints,
	//			double inFaults,
	//			ARBConfigLifetimePointsPtr* outPoints = NULL);
	//	bool DeleteLifetimePoints(double inFaults);
	}
}
