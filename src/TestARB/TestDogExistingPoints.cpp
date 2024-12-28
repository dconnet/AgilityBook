/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogExistingPoints and ARBDogExistingPointsList classes.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBDogExistingPoints.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

TEST_CASE("DogExistingPoints")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBDogExistingPointsPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogExistingPointsPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogExistingPoints const& rhs) const;
			//	bool operator!=(ARBDogExistingPoints const& rhs) const
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


	SECTION("GetPointTypeName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static wxString GetPointTypeName(ARBExistingPointType inType);
		}
	}
}


TEST_CASE("DogExistingPointsList")
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
			//	void sort();
		}
	}


	SECTION("HasPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool HasPoints(wxString const& inVenue) const;
			//	bool HasPoints(
			//			ARBConfigVenuePtr inVenue,
			//			ARBConfigDivisionPtr inDiv,
			//			ARBConfigLevelPtr inLevel,
			//			ARBConfigEventPtr inEvent,
			//			ARBDate inDateFrom,
			//			ARBDate inDateTo,
			//			bool inHasLifetime) const;
		}
	}


	SECTION("ExistingPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	double ExistingPoints(
			//			ARBExistingPointType inType,
			//			ARBConfigVenuePtr inVenue,
			//			ARBConfigMultiQPtr inMultiQ,
			//			ARBConfigDivisionPtr inDiv,
			//			ARBConfigLevelPtr inLevel,
			//			ARBConfigEventPtr inEvent,
			//			ARBDate inDateFrom,
			//			ARBDate inDateTo) const;
		}
	}


	SECTION("NumExistingPointsInVenue")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumExistingPointsInVenue(wxString const& inVenue) const;
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


	SECTION("NumExistingPointsInDivision")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumExistingPointsInDivision(
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
			//			wxString const& inVenue,
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


	SECTION("NumMultiQsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumMultiQsInUse(
			//			wxString const& inVenue,
			//			wxString const& inMultiQ) const;
		}
	}


	SECTION("RenameMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameMultiQs(
			//			wxString const& inVenue,
			//			wxString const& inOldMultiQ,
			//			wxString const& inNewMultiQ);
		}
	}


	SECTION("DeleteMultiQs")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteMultiQs(
			//			ARBConfig const& inConfig,
			//			wxString const& inVenue);
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
			//	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
		}
	}
}

} // namespace dconSoft
