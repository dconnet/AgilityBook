/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfigScoring and ARBConfigScoringList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigScoring.h"
#include "Element.h"


SUITE(TestConfigScoring)
{
	TEST(GetScoringStyleStr)
	{
		TODO_TEST
	//	static wxString GetScoringStyleStr(ScoringStyle inStyle);
	}


	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigScoringPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigScoringPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigScoring& operator=(ARBConfigScoring const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigScoring const& rhs) const;
	//	bool operator!=(ARBConfigScoring const& rhs) const
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
	//			ARBConfigDivisionList const& inDivisions,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(IsValidOn)
	{
		TODO_TEST
	//	bool IsValidOn(ARBDate inDate) const
	}
}


SUITE(TestConfigScoringList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigDivisionList const& inDivisions,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(FindAllEvents)
	{
		TODO_TEST
	//	size_t FindAllEvents(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate,
	//			bool inTitlePoints,
	//			ARBVector<ARBConfigScoringPtr>& outList) const;
	}


	TEST(FindEvent)
	{
		TODO_TEST
	//	bool FindEvent(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate,
	//			ARBConfigScoringPtr* outEvent = NULL) const;
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			wxString const& inDivision,
	//			wxString const& inLevel,
	//			ARBDate const& inDate) const;
	}


	TEST(Add)
	{
		TODO_TEST
	//	ARBConfigScoringPtr AddScoring();
	}
}
