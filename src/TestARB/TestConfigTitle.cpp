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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigTitle.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("ConfigTitle")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigTitlePtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigTitlePtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigTitle const& rhs) const;
		//	bool operator!=(ARBConfigTitle const& rhs) const
		}
	}


	SECTION("Clear")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void clear();
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	virtual std::wstring GetGenericName() const
		}
	}


	SECTION("Load")
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


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("IsValidOn")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool IsValidOn(ARBDate inDate) const
		}
	}


	SECTION("GetNiceName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	std::wstring const& GetNiceName() const
		}
	}


	SECTION("GetCompleteName")
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


TEST_CASE("ConfigTitleList")
{
	SECTION("Load")
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


	SECTION("ReorderBy")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void ReorderBy(ARBConfigTitleList const& inList);
			// Make sure the target config has a title that the source config does not
		}
	}


	SECTION("FindTitleCompleteName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitleCompleteName(
		//			std::wstring const& inName,
		//			short inInstance,
		//			bool bShowInstance,
		//			bool bAbbrevFirst = true,
		//			ARBConfigTitlePtr* outTitle = nullptr) const;
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitle(
		//			std::wstring const& inName,
		//			ARBConfigTitlePtr* outTitle = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddTitle(
		//			std::wstring const& inName,
		//			ARBConfigTitlePtr* outTitle = nullptr);
		//	bool AddTitle(ARBConfigTitlePtr inTitle);
		//	bool DeleteTitle(std::wstring const& inName);
		}
	}
}
