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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigLevel.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("ConfigLevel")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigLevelPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigLevelPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigLevel& operator=(ARBConfigLevel const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigLevel const& rhs) const;
			//	bool operator!=(ARBConfigLevel const& rhs) const
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
			//	virtual wxString GetGenericName() const
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


	SECTION("Update")
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


TEST_CASE("ConfigLevelList")
{
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


	SECTION("VerifyLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyLevel(
			//			wxString const& inName,
			//			bool inAllowWildCard = true) const;
		}
	}


	SECTION("FindLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindLevel(
			//			wxString const& inName,
			//			ARBConfigLevelPtr* outLevel = nullptr);
		}
	}


	SECTION("FindSubLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindSubLevel(
			//			wxString const& inName,
			//			ARBConfigLevelPtr* outLevel = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddLevel(
			//			wxString const& inName,
			//			ARBConfigLevelPtr* outLevel = nullptr);
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

} // namespace dconSoft
