/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2017-10-18 Add FindTitleCompleteName test.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ConfigHandler.h"

#include "ARB/ARBConfigVenue.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

TEST_CASE("ConfigVenue")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigVenuePtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigVenuePtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigVenue const& rhs) const;
			//	bool operator!=(ARBConfigVenue const& rhs) const
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
			//			ARBConfig& ioConfig,
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
			//			ARBConfigVenuePtr inVenueNew,
			//			wxString& ioInfo);
		}
	}
}


TEST_CASE("ConfigVenueList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//			ARBConfig& ioConfig,
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


	SECTION("VerifyVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyVenue(wxString const& inVenue) const
		}
	}


	SECTION("VerifyMultiQ")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyMultiQ(
			//			wxString const& inVenue,
			//			wxString const& inMultiQ,
			//			bool inUseShortName = false) const;
		}
	}


	SECTION("VerifyLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyLevel(
			//			wxString const& inVenue,
			//			wxString const& inDivision,
			//			wxString const& inLevel) const;
		}
	}


	SECTION("VerifyEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool VerifyEvent(
			//			wxString const& inVenue,
			//			wxString const& inDivision,
			//			wxString const& inLevel,
			//			wxString const& inEvent,
			//			ARBDate const& inDate) const;
		}
	}


	SECTION("FindTitleCompleteName")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			REQUIRE(config.GetVenues().FindTitleCompleteName(L"AKC", L"[NA] Novice Agility", true));
			REQUIRE(config.GetVenues().FindTitleCompleteName(L"AKC", L"Novice Agility [NA]", false));
			REQUIRE(config.GetVenues().FindTitleCompleteName(L"AKC", L"[MACH] Master Agility Champion", true));
			REQUIRE(config.GetVenues().FindTitleCompleteName(L"AKC", L"Master Agility Champion [MACH]", false));
		}
	}


	SECTION("FindTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindTitle(
			//			wxString const& inVenue,
			//			wxString const& inTitle,
			//			ARBConfigTitlePtr* outTitle = nullptr) const;
		}
	}


	SECTION("DeleteTitle")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool DeleteTitle(wxString const& inTitle);
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindVenue(
			//			wxString const& inVenue,
			//			ARBConfigVenuePtr* outVenue = nullptr) const;
		}
	}


	SECTION("FindEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindEvent(
			//			wxString const& inVenue,
			//			wxString const& inEvent,
			//			wxString const& inDivision,
			//			wxString const& inLevel,
			//			ARBDate const& inDate,
			//			ARBConfigEventPtr* outEvent = nullptr,
			//			ARBConfigScoringPtr* outScoring = nullptr) const;
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddVenue(
			//			wxString const& inVenue,
			//			ARBConfigVenuePtr* outVenue = nullptr);
			//	bool AddVenue(ARBConfigVenuePtr inVenue);
			//	int DeleteVenue(wxString const& inVenue);
		}
	}
}

} // namespace dconSoft
