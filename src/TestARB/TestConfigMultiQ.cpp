/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigMultiQItem, ARBConfigMultiQ and ARBConfigMultiQList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigMultiQ.h"
#include "Element.h"


SUITE(TestConfigMultiQ)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigMultiQPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigMultiQPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigMultiQ const& rhs) const;
	//	bool operator!=(ARBConfigMultiQ const& rhs) const
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
	//			ARBConfigVenue const& inVenue,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(Match)
	{
		TODO_TEST
	//	bool Match(
	//			std::vector<ARBDogRunPtr>& ioRuns,
	//			std::vector<ARBDogRunPtr>& outRuns) const;
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
	//			wxString const& inDiv,
	//			wxString const& inOldLevel,
	//			wxString const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(wxString const& inLevel);
	}


	TEST(RenameEvent)
	{
		TODO_TEST
	//	int RenameEvent(
	//			wxString const& inOldEvent,
	//			wxString const& inNewEvent);
	}


	TEST(DeleteEvent)
	{
		TODO_TEST
	//	int DeleteEvent(wxString const& inEvent);
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddItem(
	//			wxString const& inDiv,
	//			wxString const& inLevel,
	//			wxString const& inEvent);
	//	bool RemoveItem(
	//			wxString const& inDiv,
	//			wxString const& inLevel,
	//			wxString const& inEvent);
	//	bool RemoveAllItems();
	}
}


SUITE(TestConfigMultiQList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigVenue const& inVenue,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindMultiQ(
	//			wxString const& inName,
	//			bool inUseShortName = false,
	//			ARBConfigMultiQPtr* outMultiQ = NULL) const;
	//	bool FindMultiQ(
	//			ARBConfigMultiQ const& inMultiQ,
	//			ARBConfigMultiQPtr* outMultiQ = NULL) const;
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
	//			wxString const& inDiv,
	//			wxString const& inOldLevel,
	//			wxString const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(wxString const& inLevel);
	}


	TEST(RenameEvent)
	{
		TODO_TEST
	//	int RenameEvent(
	//			wxString const& inOldEvent,
	//			wxString const& inNewEvent);
	}


	TEST(DeleteEvent)
	{
		TODO_TEST
	//	int DeleteEvent(wxString const& inEvent);
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);
	//	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
	}
}
