/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDogTrial.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("DogTrial")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBDogTrialPtr New();
			//	static ARBDogTrialPtr New(ARBCalendar const& inCal);
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogTrialPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogTrial& operator=(ARBDogTrial const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogTrial const& rhs) const;
			//	bool operator!=(ARBDogTrial const& rhs) const
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
			//			ARBConfig const& inConfig,
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


	SECTION("SetMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void SetMultiQs(ARBConfig const& inConfig);
		}
	}


	SECTION("GetSpeedPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	short GetSpeedPoints(
			//			ARBConfig const& inConfig,
			//			wxString const& inDiv,
			//			wxString const& inLevel) const;
		}
	}


	SECTION("HasVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool HasVenue(wxString const& inVenue) const;
		}
	}
}


TEST_CASE("DogTrialList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//			ARBConfig const& inConfig,
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
		}
	}


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void sort(bool inDescending);
		}
	}


	SECTION("NumTrialsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumTrialsInVenue(wxString const& inVenue) const;
		}
	}


	SECTION("RenameVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameVenue(
			//			wxString const& inOldVenue,
			//			wxString const& inNewVenue);
		}
	}


	SECTION("DeleteVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteVenue(wxString const& inVenue);
		}
	}


	SECTION("NumOtherPointsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumOtherPointsInUse(wxString const& inOther) const;
		}
	}


	SECTION("RenameOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameOtherPoints(
			//			wxString const& inOldOther,
			//			wxString const& inNewOther);
		}
	}


	SECTION("DeleteOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteOtherPoints(wxString const& inOther);
		}
	}


	SECTION("NumMultiHostedTrialsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumMultiHostedTrialsInDivision(
			//			ARBConfig const& inConfig,
			//			wxString const& inVenue,
			//			wxString const& inDiv) const;
		}
	}


	SECTION("NumRunsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumRunsInDivision(
			//			ARBConfigVenuePtr inVenue,
			//			wxString const& inDiv) const;
		}
	}


	SECTION("RenameDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameDivision(
			//			ARBConfigVenuePtr inVenue,
			//			wxString const& inOldDiv,
			//			wxString const& inNewDiv);
		}
	}


	SECTION("DeleteDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteDivision(
			//			ARBConfig const& inConfig,
			//			wxString const& inVenue,
			//			wxString const& inDiv);
		}
	}


	SECTION("NumLevelsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumLevelsInUse(
			//			wxString const& inVenue,
			//			wxString const& inDiv,
			//			wxString const& inLevel) const;
		}
	}


	SECTION("RenameLevel")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameLevel(
			//			wxString const& inVenue,
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
			//	int DeleteLevel(
			//			wxString const& inVenue,
			//			wxString const& inDiv,
			//			wxString const& inLevel);
		}
	}


	SECTION("NumEventsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumEventsInUse(
			//			wxString const& inVenue,
			//			wxString const& inEvent) const;
		}
	}


	SECTION("RenameEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameEvent(
			//			wxString const& inVenue,
			//			wxString const& inOldEvent,
			//			wxString const& inNewEvent);
		}
	}


	SECTION("DeleteEvent")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteEvent(
			//			wxString const& inVenue,
			//			wxString const& inEvent);
		}
	}


	SECTION("AddTrial")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddTrial(ARBDogTrialPtr inTrial);
		}
	}


	SECTION("DeleteTrial")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool DeleteTrial(ARBDogTrialPtr inTrial);
		}
	}
}

} // namespace dconSoft
