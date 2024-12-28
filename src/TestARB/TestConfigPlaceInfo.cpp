/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigPlaceInfo and ARBConfigPlaceInfoList class.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigPlaceInfo.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("ConfigPlaceInfo")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigPlaceInfoPtr New();
			//	static ARBConfigPlaceInfoPtr New(short inPlace, double inValue, bool bMustQ);
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigPlaceInfoPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigPlaceInfo const& rhs) const;
			//	bool operator!=(ARBConfigPlaceInfo const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	virtual wxString GetGenericName() const;
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


TEST_CASE("ConfigPlaceInfoList")
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


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void sort();
		}
	}


	SECTION("GetPlaceInfo")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool GetPlaceInfo(short inPlace, double &outValue) const;
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindPlaceInfo(
			//			short inPlace,
			//			ARBConfigPlaceInfoPtr* outPlace = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddPlaceInfo(
			//			short inPlace,
			//			double inValue,
			//			bool inMustQ,
			//			ARBConfigPlaceInfoPtr* outPlace = nullptr);
			//	bool DeletePlaceInfo(short inPlace);
		}
	}
}

} // namespace dconSoft
