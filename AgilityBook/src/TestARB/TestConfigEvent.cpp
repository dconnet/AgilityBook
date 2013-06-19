/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigEvent and ARBConfigEventList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigEvent.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestConfigEvent)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigEventPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigEventPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigEvent const& rhs) const;
		//	bool operator!=(ARBConfigEvent const& rhs) const
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
		//			ARBConfigDivisionList const& inDivisions,
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
		//			ARBConfigEventPtr inEventNew,
		//			std::wstring& ioInfo);
		}
	}


	TEST(FindAllEvents)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t FindAllEvents(
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate,
		//			bool inTitlePoints,
		//			ARBVector<ARBConfigScoringPtr>& outList) const
		}
	}


	TEST(VerifyEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyEvent(
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate) const
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigScoringPtr* outScoring = NULL) const
		}
	}
}


SUITE(TestConfigEventList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfigDivisionList const& inDivisions,
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	TEST(ReorderBy)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void ReorderBy(ARBConfigEventList const& inList);
			// Make sure the target config has an event that the source config does not
		}
	}


	TEST(VerifyEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyEvent(
		//			std::wstring const& inEvent,
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate) const;
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	 bool FindEvent(
		//			std::wstring const& inEvent,
		//			std::wstring const& inDivision,
		//			std::wstring const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigEventPtr* outEvent = NULL,
		//			ARBConfigScoringPtr* outScoring = NULL) const;
		//	bool FindEvent(
		//			std::wstring const& inEvent,
		//			ARBConfigEventPtr* outEvent = NULL) const;
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameDivision(
		//			std::wstring const& inOldDiv,
		//			std::wstring const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteDivision(std::wstring const& inDiv);
		}
	}


	TEST(RenameLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameLevel(
		//			std::wstring const& inOldDiv,
		//			std::wstring const& inOldLevel,
		//			std::wstring const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddEvent(ARBConfigEventPtr inEvent);
		//	bool DeleteEvent(std::wstring const& inEvent);
		}
	}
}
