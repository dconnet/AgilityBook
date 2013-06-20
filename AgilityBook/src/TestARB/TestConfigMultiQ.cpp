/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigMultiQ.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
		//	virtual std::wstring GetGenericName() const
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
		//			std::wstring const& inDiv,
		//			std::wstring const& inOldLevel,
		//			std::wstring const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(std::wstring const& inLevel);
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			std::wstring const& inOldEvent,
		//			std::wstring const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(std::wstring const& inEvent);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddItem(
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel,
		//			std::wstring const& inEvent);
		//	bool RemoveItem(
		//			std::wstring const& inDiv,
		//			std::wstring const& inLevel,
		//			std::wstring const& inEvent);
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
		//			std::wstring const& inName,
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
		//			std::wstring const& inDiv,
		//			std::wstring const& inOldLevel,
		//			std::wstring const& inNewLevel);
		}
	}


	TEST(DeleteLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteLevel(std::wstring const& inLevel);
		}
	}


	TEST(RenameEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameEvent(
		//			std::wstring const& inOldEvent,
		//			std::wstring const& inNewEvent);
		}
	}


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(std::wstring const& inEvent);
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
