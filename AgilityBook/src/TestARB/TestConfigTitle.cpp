/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigTitle.h"
#include "Element.h"


SUITE(TestConfigTitle)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigTitlePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigTitlePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigTitle const& rhs) const;
	//	bool operator!=(ARBConfigTitle const& rhs) const
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


	TEST(IsValidOn)
	{
		TODO_TEST
	//	bool IsValidOn(ARBDate inDate) const
	}


	TEST(GetNiceName)
	{
		TODO_TEST
	//	wxString const& GetNiceName() const
	}


	TEST(GetCompleteName)
	{
		TODO_TEST
	//	wxString GetCompleteName(
	//			short inInstance = 0,
	//			bool bShowInstance = false,
	//			bool bAbbrevFirst = true,
	//			bool bAddDates = false) const;
	}
}


SUITE(TestConfigTitleList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback,
	//			bool inCheckDups = false);
	}


	TEST(ReorderBy)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigTitleList const& inList);
		// Make sure the target config has a title that the source config does not
	}


	TEST(FindTitleCompleteName)
	{
		TODO_TEST
	//	bool FindTitleCompleteName(
	//			wxString const& inName,
	//			short inInstance,
	//			bool bShowInstance,
	//			bool bAbbrevFirst = true,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindTitle(
	//			wxString const& inName,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddTitle(
	//			wxString const& inName,
	//			ARBConfigTitlePtr* outTitle = NULL);
	//	bool AddTitle(ARBConfigTitlePtr inTitle);
	//	bool DeleteTitle(wxString const& inName);
	}
}
