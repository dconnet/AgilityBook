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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigMultiQ.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("ConfigMultiQ")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigMultiQPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigMultiQPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigMultiQ const& rhs) const;
			//	bool operator!=(ARBConfigMultiQ const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	virtual wxString GetGenericName() const
		}
	}


	SECTION("Load")
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


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("Match")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Match(
			//			std::vector<ARBDogRunPtr>& ioRuns,
			//			std::vector<ARBDogRunPtr>& outRuns) const;
		}
	}


	SECTION("RenameDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameDivision(
			//			wxString const& inOldDiv,
			//			wxString const& inNewDiv);
		}
	}


	SECTION("DeleteDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteDivision(wxString const& inDiv);
		}
	}


	SECTION("RenameLevel")
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


	SECTION("DeleteLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteLevel(wxString const& inLevel);
		}
	}


	SECTION("RenameEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameEvent(
			//			wxString const& inOldEvent,
			//			wxString const& inNewEvent);
		}
	}


	SECTION("DeleteEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteEvent(wxString const& inEvent);
		}
	}


	SECTION("AddDelete")
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


TEST_CASE("ConfigMultiQList")
{
	SECTION("Load")
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


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindMultiQ(
			//			wxString const& inName,
			//			bool inUseShortName = false,
			//			ARBConfigMultiQPtr* outMultiQ = nullptr) const;
			//	bool FindMultiQ(
			//			ARBConfigMultiQ const& inMultiQ,
			//			ARBConfigMultiQPtr* outMultiQ = nullptr) const;
		}
	}


	SECTION("RenameDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameDivision(
			//			wxString const& inOldDiv,
			//			wxString const& inNewDiv);
		}
	}


	SECTION("DeleteDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteDivision(wxString const& inDiv);
		}
	}


	SECTION("RenameLevel")
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


	SECTION("DeleteLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteLevel(wxString const& inLevel);
		}
	}


	SECTION("RenameEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameEvent(
			//			wxString const& inOldEvent,
			//			wxString const& inNewEvent);
		}
	}


	SECTION("DeleteEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteEvent(wxString const& inEvent);
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);
			//	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
		}
	}
}

} // namespace dconSoft
