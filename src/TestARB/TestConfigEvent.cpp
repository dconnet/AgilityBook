/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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


SUITE(TestConfigEvent)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigEventPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigEventPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigEvent const& rhs) const;
	//	bool operator!=(ARBConfigEvent const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual wxString GetGenericName() const
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigDivisionList const& inDivisions,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(Update)
	{
		TODO_TEST
	//	bool Update(
	//			int indent,
	//			ARBConfigEventPtr inEventNew,
	//			wxString& ioInfo);
	}


	TEST(FindAllEvents)
	{
		TODO_TEST
	//	size_t FindAllEvents(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate,
	//			bool inTitlePoints,
	//			ARBVector<ARBConfigScoringPtr>& outList) const
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate) const
	}


	TEST(FindEvent)
	{
		TODO_TEST
	//	bool FindEvent(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate,
	//			ARBConfigScoringPtr* outScoring = NULL) const
	}
}


SUITE(TestConfigEventList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigDivisionList const& inDivisions,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(ReorderBy)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigEventList const& inList);
		// Make sure the target config has an event that the source config does not
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			wxString const& inEvent,
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate) const;
	}


	TEST(Find)
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


	TEST(RenameDivision)
	{
		TODO_TEST
	//	int RenameDivision(
	//			wxString const& inOldDiv,
	//			wxString const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
	//	int DeleteDivision(wxString const& inDiv);
	}


	TEST(RenameLevel)
	{
		TODO_TEST
	//	int RenameLevel(
	//			wxString const& inOldDiv,
	//			wxString const& inOldLevel,
	//			wxString const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(
	//			wxString const& inDiv,
	//			wxString const& inLevel);
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddEvent(ARBConfigEventPtr inEvent);
	//	bool DeleteEvent(wxString const& inEvent);
	}
}
