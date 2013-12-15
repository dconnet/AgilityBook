/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigTitle.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestConfigTitle)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigTitlePtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigTitlePtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigTitle const& rhs) const;
		//	bool operator!=(ARBConfigTitle const& rhs) const
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


	TEST(IsValidOn)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool IsValidOn(ARBDate inDate) const
		}
	}


	TEST(GetNiceName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	std::wstring const& GetNiceName() const
		}
	}


	TEST(GetCompleteName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	std::wstring GetCompleteName(
		//			short inInstance = 0,
		//			bool bShowInstance = false,
		//			bool bAbbrevFirst = true,
		//			bool bAddDates = false) const;
		}
	}
}


SUITE(TestConfigTitleList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback,
		//			bool inCheckDups = false);
		}
	}


	TEST(ReorderBy)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void ReorderBy(ARBConfigTitleList const& inList);
			// Make sure the target config has a title that the source config does not
		}
	}


	TEST(FindTitleCompleteName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitleCompleteName(
		//			std::wstring const& inName,
		//			short inInstance,
		//			bool bShowInstance,
		//			bool bAbbrevFirst = true,
		//			ARBConfigTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitle(
		//			std::wstring const& inName,
		//			ARBConfigTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddTitle(
		//			std::wstring const& inName,
		//			ARBConfigTitlePtr* outTitle = NULL);
		//	bool AddTitle(ARBConfigTitlePtr inTitle);
		//	bool DeleteTitle(std::wstring const& inName);
		}
	}
}
