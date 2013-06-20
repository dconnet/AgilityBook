/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigCalSite.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestConfigCalSite)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//static ARBConfigCalSitePtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//ARBConfigCalSitePtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//ARBConfigCalSite& operator=(ARBConfigCalSite const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool operator==(ARBConfigCalSite const& rhs) const;
			//bool operator!=(ARBConfigCalSite const& rhs) const
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Load(
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Save(ElementNodePtr ioTree) const;
		}
	}


	TEST(GetFormattedURL)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//std::wstring GetFormattedURL(
			//		std::vector<std::wstring> const& inLocCodes,
			//		std::vector<std::wstring> const& inVenueCodes) const;
		}
	}


	TEST(LocationCodes)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool HasLocationCode(std::wstring const& inCode) const;
			//bool AddLocationCode(std::wstring const& inCode, std::wstring const& inName);
			//bool RemoveLocationCode(std::wstring const& inCode);
			//bool RemoveAllLocationCodes();
		}
	}


	TEST(VenueCodes)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool HasVenueCode(std::wstring const& inCode) const;
			//bool AddVenueCode(std::wstring const& inCode, std::wstring const& inVenue);
			//bool RemoveVenueCode(std::wstring const& inCode);
			//bool RemoveAllVenueCodes();
		}
	}
}


SUITE(TestConfigCalSiteList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Load(
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//void sort();
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool FindSite(
			//		std::wstring const& inSite,
			//		ARBConfigCalSitePtr* outSite = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool AddSite(
			//		std::wstring const& inSite,
			//		ARBConfigCalSitePtr* outSite = NULL);
			//bool AddSite(ARBConfigCalSitePtr inSite);
			//int DeleteSite(std::wstring const& inSite);
		}
	}
}
