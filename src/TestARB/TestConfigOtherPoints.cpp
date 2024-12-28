/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigOtherPoints and ARBConfigOtherPointsList class.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigOtherPoints.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("ConfigOtherPoints")
{
	SECTION("GetTallyValidValues")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static void GetTallyValidValues(std::vector<wxString>& outValues);
		}
	}


	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigOtherPointsPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigOtherPointsPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigOtherPoints const& rhs) const;
			//	bool operator!=(ARBConfigOtherPoints const& rhs) const
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
}


TEST_CASE("ConfigOtherPointsList")
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


	SECTION("VerifyOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyOtherPoints(wxString const& inName) const;
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindOtherPoints(
			//			wxString const& inName,
			//			ARBConfigOtherPointsPtr* outPoints = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddOtherPoints(ARBConfigOtherPointsPtr inOther);
			//	bool DeleteOtherPoints(wxString const& inName);
		}
	}
}

} // namespace dconSoft
