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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBConfigEvent.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("ConfigEvent")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBConfigEventPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigEventPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBConfigEvent const& rhs) const;
			//	bool operator!=(ARBConfigEvent const& rhs) const
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
			//			ARBConfigDivisionList const& inDivisions,
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


	SECTION("Update")
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


	SECTION("FindAllEvents")
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


	SECTION("VerifyEvent")
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


	SECTION("FindEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool FindEvent(
			//			wxString const& inDivision,
			//			wxString const& inLevel,
			//			ARBDate const& inDate,
			//			ARBConfigScoringPtr* outScoring = nullptr) const
		}
	}
}


TEST_CASE("ConfigEventList")
{
	SECTION("Load")
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


	SECTION("ReorderBy")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void ReorderBy(ARBConfigEventList const& inList);
			// Make sure the target config has an event that the source config does not
		}
	}


	SECTION("VerifyEvent")
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


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	 bool FindEvent(
			//			wxString const& inEvent,
			//			wxString const& inDivision,
			//			wxString const& inLevel,
			//			ARBDate const& inDate,
			//			ARBConfigEventPtr* outEvent = nullptr,
			//			ARBConfigScoringPtr* outScoring = nullptr) const;
			//	bool FindEvent(
			//			wxString const& inEvent,
			//			ARBConfigEventPtr* outEvent = nullptr) const;
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
			//			wxString const& inOldDiv,
			//			wxString const& inOldLevel,
			//			wxString const& inNewLevel);
		}
	}


	SECTION("DeleteLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteLevel(
			//			wxString const& inDiv,
			//			wxString const& inLevel);
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddEvent(ARBConfigEventPtr inEvent);
			//	bool DeleteEvent(wxString const& inEvent);
		}
	}
}

} // namespace dconSoft
