/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBDogRunPartner and ARBDogRunPartnerList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRunPartner.h"
#include "Element.h"


SUITE(TestDogRunPartner)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogRunPartnerPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogRunPartnerPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRunPartner& operator=(ARBDogRunPartner const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogRunPartner const& rhs) const;
	//	bool operator!=(ARBDogRunPartner const& rhs) const
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


SUITE(TestDogRunPartnerList)
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


	TEST(AddPartner)
	{
		TODO_TEST
	//	bool AddPartner(ARBDogRunPartnerPtr inPartner);
	}
}
