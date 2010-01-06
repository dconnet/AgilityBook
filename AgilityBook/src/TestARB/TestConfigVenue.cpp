/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
		TODO_TEST
	//	static ARBConfigVenuePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigVenuePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigVenue const& rhs) const;
	//	bool operator!=(ARBConfigVenue const& rhs) const
	}


	TEST(Clear)
	{
		TODO_TEST
	//	void clear();
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
	//			ARBConfig& ioConfig,
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
	//			ARBConfigVenuePtr inVenueNew,
	//			wxString& ioInfo);
	}
}


SUITE(TestConfigVenueList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig& ioConfig,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(VerifyVenue)
	{
		TODO_TEST
	//	bool VerifyVenue(wxString const& inVenue) const
	}


	TEST(VerifyMultiQ)
	{
		TODO_TEST
	//	bool VerifyMultiQ(
	//			wxString const& inVenue,
	//			wxString const& inMultiQ,
	//			bool inUseShortName = false) const;
	}


	TEST(VerifyLevel)
	{
		TODO_TEST
	//	bool VerifyLevel(
	//			wxString const& inVenue,
	//			wxString const& inDivision,
	//			wxString const& inLevel) const;
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			wxString const& inVenue,
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			wxString const& inEvent,
	//			ARBDate const& inDate) const;
	}


	TEST(FindTitleCompleteName)
	{
		TODO_TEST
	//	bool FindTitleCompleteName(
	//			wxString const& inVenue,
	//			wxString const& inName,
	//			bool bShowInstance,
	//			bool bAbbrevFirst = true,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(FindTitle)
	{
		TODO_TEST
	//	bool FindTitle(
	//			wxString const& inVenue,
	//			wxString const& inTitle,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(DeleteTitle)
	{
		TODO_TEST
	//	bool DeleteTitle(wxString const& inTitle);
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindVenue(
	//			wxString const& inVenue,
	//			ARBConfigVenuePtr* outVenue = NULL) const;
	}


	TEST(FindEvent)
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


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddVenue(
	//			wxString const& inVenue,
	//			ARBConfigVenuePtr* outVenue = NULL);
	//	bool AddVenue(ARBConfigVenuePtr inVenue);
	//	int DeleteVenue(wxString const& inVenue);
	}
}
