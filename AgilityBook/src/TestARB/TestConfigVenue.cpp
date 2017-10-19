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
 * 2017-10-18 Add FindTitleCompleteName test.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestConfigVenue)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigVenuePtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigVenuePtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigVenue const& rhs) const;
		//	bool operator!=(ARBConfigVenue const& rhs) const
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
		//			ARBConfig& ioConfig,
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
		//			ARBConfigVenuePtr inVenueNew,
		//			std::wstring& ioInfo);
		}
	}
}


SUITE(TestConfigVenueList)
{
	TEST(Load)
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


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void sort();
		}
	}


	TEST(VerifyVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyVenue(std::wstring const& inVenue) const
		}
	}


	TEST(VerifyMultiQ)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyMultiQ(
		//			std::wstring const& inVenue,
		//			std::wstring const& inMultiQ,
		//			bool inUseShortName = false) const;
		}
	}


	TEST(VerifyLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyLevel(
		//			std::wstring const& inVenue,
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel) const;
		}
	}


	TEST(VerifyEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyEvent(
		//			std::wstring const& inVenue,
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			std::wstring const& inEvent,
		//			ARBDate const& inDate) const;
		}
	}


	TEST(FindTitleCompleteName)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			CHECK(config.GetVenues().FindTitleCompleteName(L"AKC", L"[NA] Novice Agility", true));
			CHECK(config.GetVenues().FindTitleCompleteName(L"AKC", L"Novice Agility [NA]", false));
			CHECK(config.GetVenues().FindTitleCompleteName(L"AKC", L"[MACH] Master Agility Champion", true));
			CHECK(config.GetVenues().FindTitleCompleteName(L"AKC", L"Master Agility Champion [MACH]", false));
		}
	}


	TEST(FindTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitle(
		//			std::wstring const& inVenue,
		//			std::wstring const& inTitle,
		//			ARBConfigTitlePtr* outTitle = nullptr) const;
		}
	}


	TEST(DeleteTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool DeleteTitle(std::wstring const& inTitle);
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindVenue(
		//			std::wstring const& inVenue,
		//			ARBConfigVenuePtr* outVenue = nullptr) const;
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			std::wstring const& inVenue,
		//			std::wstring const& inEvent,
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigEventPtr* outEvent = nullptr,
		//			ARBConfigScoringPtr* outScoring = nullptr) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddVenue(
		//			std::wstring const& inVenue,
		//			ARBConfigVenuePtr* outVenue = nullptr);
		//	bool AddVenue(ARBConfigVenuePtr inVenue);
		//	int DeleteVenue(std::wstring const& inVenue);
		}
	}
}
