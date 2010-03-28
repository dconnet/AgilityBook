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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBAgilityRecordBook.h"
#include "Element.h"


SUITE(TestAgilityRecordBook)
{
	TEST(GetCurrentDocVersion)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//static ARBVersion const& GetCurrentDocVersion();
		}
	}


	TEST(ctor)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBAgilityRecordBook();
		}
	}


	TEST(clear)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void clear();
		}
	}


	TEST(Load)
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


	TEST(Save)
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


	TEST(Default)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void Default();
		}
	}


	TEST(Update)
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


	TEST(GetAllClubNames)
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


	TEST(GetAllTrialLocations)
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


	TEST(GetAllEventSubNames)
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


	TEST(GetAllHeights)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllHeights(std::set<wxString>& outHeights) const;
		}
	}


	TEST(GetAllCallNames)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllCallNames(std::set<wxString>& outNames) const;
		}
	}


	TEST(GetAllBreeds)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllBreeds(std::set<wxString>& outBreeds) const;
		}
	}


	TEST(GetAllJudges)
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


	TEST(GetAllHandlers)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t GetAllHandlers(std::set<wxString>& outHandlers) const;
		}
	}


	TEST(GetAllPartners)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void GetAllPartners(
		//			std::set<wxString>& outPartners,
		//			std::set<wxString>& outDogs) const;
		}
	}


	TEST(GetAllFaultTypes)
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

SUITE(TestHttp)
{
	TEST(Read)
	{
		if (g_bMicroTest)
		{
#if CHECK_LEAK
			wxURL::SetDefaultProxy(wxT("proxy.eng.vmware.com:3128"));
#endif
			wxURL url(wxT("http://www.agilityrecordbook.com/version.txt"));
#if !CHECK_LEAK
			url.SetProxy(wxT("proxy.eng.vmware.com:3128"));
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
}
#endif
