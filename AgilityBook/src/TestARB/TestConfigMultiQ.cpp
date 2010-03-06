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
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigMultiQPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigMultiQPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigMultiQ const& rhs) const;
		//	bool operator!=(ARBConfigMultiQ const& rhs) const
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
		//			ARBConfigVenue const& inVenue,
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


	TEST(Match)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Match(
		//			std::vector<ARBDogRunPtr>& ioRuns,
		//			std::vector<ARBDogRunPtr>& outRuns) const;
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
		//			wxString const& inDiv,
		//			wxString const& inOldLevel,
		//			wxString const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(wxString const& inLevel);
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			wxString const& inOldEvent,
		//			wxString const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(wxString const& inEvent);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
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
}


SUITE(TestConfigMultiQList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfigVenue const& inVenue,
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
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
		//			wxString const& inDiv,
		//			wxString const& inOldLevel,
		//			wxString const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(wxString const& inLevel);
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			wxString const& inOldEvent,
		//			wxString const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(wxString const& inEvent);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);
		//	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
		}
	}
}
