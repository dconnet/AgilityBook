/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigLevel and ARBConfigLevel classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigLevel.h"
#include "Element.h"


SUITE(TestConfigLevel)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigLevelPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigLevelPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigLevel& operator=(ARBConfigLevel const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigLevel const& rhs) const;
		//	bool operator!=(ARBConfigLevel const& rhs) const
		}
	}


	TEST(Clear)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void clear();
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


	TEST(Update)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Update(
		//			int indent,
		//			ARBConfigLevelPtr inLevelNew,
		//			wxString& ioInfo);
		}
	}
}


SUITE(TestConfigLevelList)
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


	TEST(VerifyLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyLevel(
		//			wxString const& inName,
		//			bool inAllowWildCard = true) const;
		}
	}


	TEST(FindLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindLevel(
		//			wxString const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL);
		}
	}


	TEST(FindSubLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindSubLevel(
		//			wxString const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddLevel(
		//			wxString const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL);
		//	bool AddLevel(ARBConfigLevelPtr inLevel);
		//	bool DeleteLevel(
		//			wxString const& inDiv,
		//			wxString const& inName,
		//			ARBConfigEventList& ioEvents);
		//	bool DeleteSubLevel(
		//			wxString const& inName,
		//			bool& outLevelModified);
		}
	}
}
