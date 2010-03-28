/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigSubLevel and ARBConfigSubLevelList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigSubLevel.h"
#include "Element.h"


SUITE(TestConfigSubLevel)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigSubLevelPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigSubLevelPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigSubLevel& operator=(ARBConfigSubLevel const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigSubLevel const& rhs) const;
		//	bool operator!=(ARBConfigSubLevel const& rhs) const
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	virtual wxString GetGenericName() const
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Save(ElementNodePtr ioTree) const;
		}
	}
}


SUITE(TestConfigSubLevelList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindSubLevel(wxString const& inName) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddSubLevel(
		//			wxString const& inName,
		//			ARBConfigSubLevelPtr* outLevel = NULL);
		//	bool DeleteSubLevel(wxString const& inName);
		}
	}
}
