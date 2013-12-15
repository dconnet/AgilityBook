/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigLevel and ARBConfigLevel classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigLevel.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
		//	virtual std::wstring GetGenericName() const
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
		//			std::wstring& ioInfo);
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
		//			std::wstring const& inName,
		//			bool inAllowWildCard = true) const;
		}
	}


	TEST(FindLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindLevel(
		//			std::wstring const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL);
		}
	}


	TEST(FindSubLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindSubLevel(
		//			std::wstring const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddLevel(
		//			std::wstring const& inName,
		//			ARBConfigLevelPtr* outLevel = NULL);
		//	bool AddLevel(ARBConfigLevelPtr inLevel);
		//	bool DeleteLevel(
		//			std::wstring const& inDiv,
		//			std::wstring const& inName,
		//			ARBConfigEventList& ioEvents);
		//	bool DeleteSubLevel(
		//			std::wstring const& inName,
		//			bool& outLevelModified);
		}
	}
}
