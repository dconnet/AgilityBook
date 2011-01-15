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

#include "ARBStructure.h"
#include "ARBConfigEvent.h"
#include "Element.h"

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
		//	virtual wxString GetGenericName() const
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
		//			wxString& ioInfo);
		}
	}


	TEST(FindAllEvents)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t FindAllEvents(
		//			wxString const& inDivision,
		//			wxString const& inLevel,
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
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate) const
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			wxString const& inDivision,
		//			wxString const& inLevel,
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
		//			wxString const& inEvent,
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate) const;
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	 bool FindEvent(
		//			wxString const& inEvent,
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigEventPtr* outEvent = NULL,
		//			ARBConfigScoringPtr* outScoring = NULL) const;
		//	bool FindEvent(
		//			wxString const& inEvent,
		//			ARBConfigEventPtr* outEvent = NULL) const;
		}
	}


	TEST(RenameDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameDivision(
		//			wxString const& inOldDiv,
		//			wxString const& inNewDiv);
		}
	}


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteDivision(wxString const& inDiv);
		}
	}


	TEST(RenameLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameLevel(
		//			wxString const& inOldDiv,
		//			wxString const& inOldLevel,
		//			wxString const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(
		//			wxString const& inDiv,
		//			wxString const& inLevel);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddEvent(ARBConfigEventPtr inEvent);
		//	bool DeleteEvent(wxString const& inEvent);
		}
	}
}
