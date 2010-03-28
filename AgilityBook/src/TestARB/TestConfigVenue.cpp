/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigVenue.h"
#include "Element.h"


SUITE(TestConfigVenue)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigVenuePtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigVenuePtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigVenue const& rhs) const;
		//	bool operator!=(ARBConfigVenue const& rhs) const
		}
	}


	TEST(Clear)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	void clear();
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
		//			ARBConfig& ioConfig,
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
		//			ARBConfigVenuePtr inVenueNew,
		//			wxString& ioInfo);
		}
	}
}


SUITE(TestConfigVenueList)
{
	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig& ioConfig,
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
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


	TEST(VerifyVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyVenue(wxString const& inVenue) const
		}
	}


	TEST(VerifyMultiQ)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyMultiQ(
		//			wxString const& inVenue,
		//			wxString const& inMultiQ,
		//			bool inUseShortName = false) const;
		}
	}


	TEST(VerifyLevel)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyLevel(
		//			wxString const& inVenue,
		//			wxString const& inDivision,
		//			wxString const& inLevel) const;
		}
	}


	TEST(VerifyEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyEvent(
		//			wxString const& inVenue,
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			wxString const& inEvent,
		//			ARBDate const& inDate) const;
		}
	}


	TEST(FindTitleCompleteName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitleCompleteName(
		//			wxString const& inVenue,
		//			wxString const& inName,
		//			bool bShowInstance,
		//			bool bAbbrevFirst = true,
		//			ARBConfigTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(FindTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitle(
		//			wxString const& inVenue,
		//			wxString const& inTitle,
		//			ARBConfigTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(DeleteTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool DeleteTitle(wxString const& inTitle);
		}
	}


	TEST(Find)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindVenue(
		//			wxString const& inVenue,
		//			ARBConfigVenuePtr* outVenue = NULL) const;
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			wxString const& inVenue,
		//			wxString const& inEvent,
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigEventPtr* outEvent = NULL,
		//			ARBConfigScoringPtr* outScoring = NULL) const;
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddVenue(
		//			wxString const& inVenue,
		//			ARBConfigVenuePtr* outVenue = NULL);
		//	bool AddVenue(ARBConfigVenuePtr inVenue);
		//	int DeleteVenue(wxString const& inVenue);
		}
	}
}
