/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigDivision and ARBConfigDivisionList class.
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
#include "ARB/ARBConfigDivision.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("ConfigDivision")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//static ARBConfigDivisionPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//ARBConfigDivisionPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigDivision const& rhs) const;
		//	bool operator!=(ARBConfigDivision const& rhs) const
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
		//			ARBConfigVenue& ioVenue,
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


	SECTION("Update")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Update(
		//			int indent,
		//			ARBConfigDivisionPtr inDivNew,
		//			std::wstring& ioInfo);
		}
	}
}


TEST_CASE("ConfigDivisionList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfigVenue& ioVenue,
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Reorder")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void ReorderBy(ARBConfigDivisionList const& inList);
			// Make sure the target config has a division that the source config does not
		}
	}


	SECTION("VerifyLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyLevel(
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel) const;
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindDivision(
		//			std::wstring const& inDiv,
		//			ARBConfigDivisionPtr* outDiv = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddDivision(
		//			std::wstring const& inDiv,
		//			ARBConfigDivisionPtr* outDiv = nullptr);
		//	bool AddDivision(ARBConfigDivisionPtr inDiv);
		//	int DeleteDivision(
		//			std::wstring const& inDiv,
		//			ARBConfigEventList& ioEvents);
		}
	}
}
