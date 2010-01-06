/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigTitlePoints and ARBConfigTitlePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigTitlePoints.h"
#include "Element.h"


SUITE(TestConfigTitlePoints)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigTitlePointsPtr New();
	//	static ARBConfigTitlePointsPtr New(double inPoints, double inFaults);
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigTitlePointsPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigTitlePoints const& rhs) const;
	//	bool operator!=(ARBConfigTitlePoints const& rhs) const
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
	//			ARBErrorCallback& ioCallback,
	//			ARBConfigLifetimePointsList& ioLifetimePoints);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}
}


SUITE(TestConfigTitlePointsList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback,
	//			ARBConfigLifetimePointsList& ioLifetimePoints);
	}


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(GetTitlePoints)
	{
		TODO_TEST
	//	double GetTitlePoints(double inFaults) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindTitlePoints(
	//			double inFaults,
	//			ARBConfigTitlePointsPtr* outPoints = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddTitlePoints(
	//			double inPoints,
	//			double inFaults,
	//			ARBConfigTitlePointsPtr* outPoints = NULL);
	//	bool DeleteTitlePoints(double inFaults);
	}
}
