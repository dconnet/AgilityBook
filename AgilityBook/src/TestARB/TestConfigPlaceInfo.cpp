/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigPlaceInfo and ARBConfigPlaceInfoList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigPlaceInfo.h"
#include "Element.h"


SUITE(TestConfigPlaceInfo)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigPlaceInfoPtr New();
	//	static ARBConfigPlaceInfoPtr New(short inPlace, double inValue, bool bMustQ);
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigPlaceInfoPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigPlaceInfo const& rhs) const;
	//	bool operator!=(ARBConfigPlaceInfo const& rhs) const
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


SUITE(TestConfigPlaceInfoList)
{
	TEST(Load)
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


	TEST(GetPlaceInfo)
	{
		TODO_TEST
	//	bool GetPlaceInfo(short inPlace, double &outValue) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindPlaceInfo(
	//			short inPlace,
	//			ARBConfigPlaceInfoPtr* outPlace = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddPlaceInfo(
	//			short inPlace,
	//			double inValue,
	//			bool inMustQ,
	//			ARBConfigPlaceInfoPtr* outPlace = NULL);
	//	bool DeletePlaceInfo(short inPlace);
	}
}
