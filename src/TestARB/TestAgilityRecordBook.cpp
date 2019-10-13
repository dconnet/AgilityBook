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

#include "ARB/ARBStructure.h"
#include "ARB/ARBAgilityRecordBook.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("AgilityRecordBook")
{
	//TODO: Setup

	SECTION("GetCurrentDocVersion")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//static ARBVersion const& GetCurrentDocVersion();
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
		//		std::wstring const& inPgmVer,
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
		//			std::wstring& ioInfo,
		//			IConfigActionCallback& ioCallBack);
		}
	}


	SECTION("GetAllClubNames")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllClubNames(
		//			std::set<std::wstring>& outClubs,
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
		//			std::set<std::wstring>& outLocations,
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
		//			std::wstring const& inVenue,
		//			ARBConfigEventPtr inEvent,
		//			std::set<std::wstring>& outNames) const;
		}
	}


	SECTION("GetAllHeights")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllHeights(std::set<std::wstring>& outHeights) const;
		}
	}


	SECTION("GetAllCallNames")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllCallNames(std::set<std::wstring>& outNames) const;
		}
	}


	SECTION("GetAllBreeds")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllBreeds(std::set<std::wstring>& outBreeds) const;
		}
	}


	SECTION("GetAllJudges")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllJudges(
		//			std::set<std::wstring>& outJudges,
		//			bool bInfo,
		//			bool bVisibleOnly) const;
		}
	}


	SECTION("GetAllHandlers")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllHandlers(std::set<std::wstring>& outHandlers) const;
		}
	}


	SECTION("GetAllPartners")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void GetAllPartners(
		//			std::set<std::wstring>& outPartners,
		//			std::set<std::wstring>& outDogs) const;
		}
	}


	SECTION("GetAllFaultTypes")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllFaultTypes(std::set<std::wstring>& outFaults) const;
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
		wxURL url(L"http://www.agilityrecordbook.com/version.txt");
#if !CHECK_LEAK
		url.SetProxy(L"proxy.eng.vmware.com:3128");
#endif
		wxInputStream* stream = url.GetInputStream();
		CHECK(stream && stream->IsOk());
		std::wstring res;
		wxStringOutputStream outStream(&res);
		stream->Read(outStream);
		delete stream;
		CHECK(!res.empty());
	}
}
#endif
