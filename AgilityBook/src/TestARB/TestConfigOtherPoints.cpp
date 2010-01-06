/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigOtherPoints and ARBConfigOtherPointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigOtherPoints.h"
#include "Element.h"

SUITE(TestConfigOtherPoints)
{
	TEST(GetTallyValidValues)
	{
		TODO_TEST
	//	static void GetTallyValidValues(std::vector<wxString>& outValues);
	}


	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigOtherPointsPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigOtherPointsPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigOtherPoints const& rhs) const;
	//	bool operator!=(ARBConfigOtherPoints const& rhs) const
	}


	TEST(Clear)
	{
		TODO_TEST
	//	void clear();
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


SUITE(TestConfigOtherPointsList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(VerifyOtherPoints)
	{
		TODO_TEST
	//	bool VerifyOtherPoints(wxString const& inName) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindOtherPoints(
	//			wxString const& inName,
	//			ARBConfigOtherPointsPtr* outPoints = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddOtherPoints(ARBConfigOtherPointsPtr inOther);
	//	bool DeleteOtherPoints(wxString const& inName);
	}
}
