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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogExistingPoints.h"
#include "Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestDogExistingPoints)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogExistingPointsPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogExistingPointsPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogExistingPoints const& rhs) const;
		//	bool operator!=(ARBDogExistingPoints const& rhs) const
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
		//			ARBConfig const& inConfig,
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


	TEST(GetPointTypeName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static wxString GetPointTypeName(PointType inType);
		}
	}
}


SUITE(TestDogExistingPointsList)
{
	TEST(Load)
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


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	TEST(sort)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void sort();
		}
	}


	TEST(HasPoints)
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


	TEST(ExistingPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	double ExistingPoints(
		//			ARBDogExistingPoints::PointType inType,
		//			ARBConfigVenuePtr inVenue,
		//			ARBConfigMultiQPtr inMultiQ,
		//			ARBConfigDivisionPtr inDiv,
		//			ARBConfigLevelPtr inLevel,
		//			ARBConfigEventPtr inEvent,
		//			ARBDate inDateFrom,
		//			ARBDate inDateTo) const;
		}
	}


	TEST(NumExistingPointsInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumExistingPointsInVenue(wxString const& inVenue) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameVenue(
		//			wxString const& inOldVenue,
		//			wxString const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteVenue(wxString const& inVenue);
		}
	}


	TEST(NumExistingPointsInDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumExistingPointsInDivision(
		//			ARBConfigVenuePtr inVenue,
		//			wxString const& inDiv) const;
		}
	}


	TEST(RenameDivision)
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


	TEST(DeleteDivision)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteDivision(
		//			wxString const& inVenue,
		//			wxString const& inDiv);
		}
	}


	TEST(NumLevelsInUse)
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


	TEST(RenameLevel)
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


	TEST(DeleteLevel)
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


	TEST(NumEventsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumEventsInUse(
		//			wxString const& inVenue,
		//			wxString const& inEvent) const;
		}
	}


	TEST(RenameEvent)
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


	TEST(DeleteEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteEvent(
		//			wxString const& inVenue,
		//			wxString const& inEvent);
		}
	}


	TEST(NumOtherPointsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumOtherPointsInUse(wxString const& inOther) const;
		}
	}


	TEST(RenameOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameOtherPoints(
		//			wxString const& inOldOther,
		//			wxString const& inNewOther);
		}
	}


	TEST(DeleteOtherPoints)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteOtherPoints(wxString const& inOther);
		}
	}


	TEST(NumMultiQsInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumMultiQsInUse(
		//			wxString const& inVenue,
		//			wxString const& inMultiQ) const;
		}
	}


	TEST(RenameMultiQs)
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


	TEST(DeleteMultiQs)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteMultiQs(
		//			ARBConfig const& inConfig,
		//			wxString const& inVenue);
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
		//	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
		}
	}
}
