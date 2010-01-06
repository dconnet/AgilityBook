/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigDivision and ARBConfigDivisionList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigDivision.h"
#include "Element.h"


SUITE(TestConfigDivision)
{
	TEST(New)
	{
		TODO_TEST
		//static ARBConfigDivisionPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
		//ARBConfigDivisionPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigDivision const& rhs) const;
	//	bool operator!=(ARBConfigDivision const& rhs) const
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
	//			ARBConfigVenue& ioVenue,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(Update)
	{
		TODO_TEST
	//	bool Update(
	//			int indent,
	//			ARBConfigDivisionPtr inDivNew,
	//			wxString& ioInfo);
	}
}


SUITE(TestConfigDivisionList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigVenue& ioVenue,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Reorder)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigDivisionList const& inList);
		// Make sure the target config has a division that the source config does not
	}


	TEST(VerifyLevel)
	{
		TODO_TEST
	//	bool VerifyLevel(
	//			wxString const& inDiv,
	//			wxString const& inLevel) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindDivision(
	//			wxString const& inDiv,
	//			ARBConfigDivisionPtr* outDiv = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddDivision(
	//			wxString const& inDiv,
	//			ARBConfigDivisionPtr* outDiv = NULL);
	//	bool AddDivision(ARBConfigDivisionPtr inDiv);
	//	int DeleteDivision(
	//			wxString const& inDiv,
	//			ARBConfigEventList& ioEvents);
	}
}
