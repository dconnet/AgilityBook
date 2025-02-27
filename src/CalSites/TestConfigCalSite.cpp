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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigCalSite.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("ConfigCalSite")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// static ARBConfigCalSitePtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// ARBConfigCalSitePtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// ARBConfigCalSite& operator=(ARBConfigCalSite const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool operator==(ARBConfigCalSite const& rhs) const;
			// bool operator!=(ARBConfigCalSite const& rhs) const
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Load(
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("GetFormattedURL")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// wxString GetFormattedURL(
			//		std::vector<wxString> const& inLocCodes,
			//		std::vector<wxString> const& inVenueCodes) const;
		}
	}


	SECTION("LocationCodes")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool HasLocationCode(wxString const& inCode) const;
			// bool AddLocationCode(wxString const& inCode, wxString const& inName);
			// bool RemoveLocationCode(wxString const& inCode);
			// bool RemoveAllLocationCodes();
		}
	}


	SECTION("VenueCodes")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool HasVenueCode(wxString const& inCode) const;
			// bool AddVenueCode(wxString const& inCode, wxString const& inVenue);
			// bool RemoveVenueCode(wxString const& inCode);
			// bool RemoveAllVenueCodes();
		}
	}
}


TEST_CASE("ConfigCalSiteList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool Load(
			//		ElementNodePtr inTree,
			//		ARBVersion const& inVersion,
			//		ARBErrorCallback& ioCallback);
		}
	}


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// void sort();
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool FindSite(
			//		wxString const& inSite,
			//		ARBConfigCalSitePtr* outSite = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// bool AddSite(
			//		wxString const& inSite,
			//		ARBConfigCalSitePtr* outSite = nullptr);
			// bool AddSite(ARBConfigCalSitePtr inSite);
			// int DeleteSite(wxString const& inSite);
		}
	}
}
