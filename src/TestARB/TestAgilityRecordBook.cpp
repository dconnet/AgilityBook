/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBAgilityRecordBook class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("AgilityRecordBook")
{
	// TODO: Setup

	SECTION("GetCurrentDocVersion")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			// static ARBVersion const& GetCurrentDocVersion();
		}
	}


	SECTION("ctor")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBAgilityRecordBook();
		}
	}


	SECTION("clear")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void clear();
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//		ElementNodePtr inTree,
			//		bool inCalendar,
			//		bool inTraining,
			//		bool inConfig,
			//		bool inInfo,
			//		bool inDogs,
			//		ARBErrorCallback& ioCallback);
			//	bool Load(ElementNodePtr inTree, ARBErrorCallback& ioCallback)
			//	{
			//		return Load(inTree, true, true, true, true, true, ioCallback);
			//	}
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Save(
			//		ElementNodePtr outTree,
			//		wxString const& inPgmVer,
			//		bool inCalendar,
			//		bool inTraining,
			//		bool inConfig,
			//		bool inInfo,
			//		bool inDogs) const;
		}
	}


	SECTION("Default")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void Default();
		}
	}


	SECTION("Update")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Update(
			//			int indent,
			//			ARBConfig const& inConfigNew,
			//			wxString& ioInfo,
			//			IConfigActionCallback& ioCallBack);
		}
	}


	SECTION("GetAllClubNames")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllClubNames(
			//			std::set<wxString>& outClubs,
			//			bool bInfo,
			//			bool bVisibleOnly) const;
		}
	}


	SECTION("GetAllTrialLocations")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllTrialLocations(
			//			std::set<wxString>& outLocations,
			//			bool bInfo,
			//			bool bVisibleOnly) const;
		}
	}


	SECTION("GetAllEventSubNames")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllEventSubNames(
			//			wxString const& inVenue,
			//			ARBConfigEventPtr inEvent,
			//			std::set<wxString>& outNames) const;
		}
	}


	SECTION("GetAllHeights")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllHeights(std::set<wxString>& outHeights) const;
		}
	}


	SECTION("GetAllCallNames")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllCallNames(std::set<wxString>& outNames) const;
		}
	}


	SECTION("GetAllBreeds")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllBreeds(std::set<wxString>& outBreeds) const;
		}
	}


	SECTION("GetAllJudges")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllJudges(
			//			std::set<wxString>& outJudges,
			//			bool bInfo,
			//			bool bVisibleOnly) const;
		}
	}


	SECTION("GetAllHandlers")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllHandlers(std::set<wxString>& outHandlers) const;
		}
	}


	SECTION("GetAllPartners")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void GetAllPartners(
			//			std::set<wxString>& outPartners,
			//			std::set<wxString>& outDogs) const;
		}
	}


	SECTION("GetAllFaultTypes")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	size_t GetAllFaultTypes(std::set<wxString>& outFaults) const;
		}
	}
}


#if 0
#include <wx/sstream.h>
#include <wx/url.h>
#define CHECK_LEAK 1

TEST_CASE("HttpRead")
{
	if (g_bMicroTest)
	{
#if CHECK_LEAK
		wxURL::SetDefaultProxy(L"proxy.eng.vmware.com:3128");
#endif
		wxURL url(L"https://www.agilityrecordbook.com/version.txt");
#if !CHECK_LEAK
		url.SetProxy(L"proxy.eng.vmware.com:3128");
#endif
		wxInputStream* stream = url.GetInputStream();
		CHECK(stream && stream->IsOk());
		wxString res;
		wxStringOutputStream outStream(&res);
		stream->Read(outStream);
		delete stream;
		CHECK(!res.empty());
	}
}
#endif

} // namespace dconSoft
