/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static wxString GetScoringStyleStr(ScoringStyle inStyle);
		}
	}


	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBConfigScoringPtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigScoringPtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigScoring& operator=(ARBConfigScoring const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBConfigScoring const& rhs) const;
		//	bool operator!=(ARBConfigScoring const& rhs) const
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
		//			ARBConfigDivisionList const& inDivisions,
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


	TEST(IsValidOn)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool IsValidOn(ARBDate inDate) const
		}
	}
}


SUITE(TestConfigScoringList)
{
	TEST(Load)
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


	TEST(FindAllEvents)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	size_t FindAllEvents(
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate,
		//			bool inTitlePoints,
		//			ARBVector<ARBConfigScoringPtr>& outList) const;
		}
	}


	TEST(FindEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindEvent(
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate,
		//			ARBConfigScoringPtr* outEvent = NULL) const;
		}
	}


	TEST(VerifyEvent)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool VerifyEvent(
		//			wxString const& inDivision,
		//			wxString const& inLevel,
		//			ARBDate const& inDate) const;
		}
	}


	TEST(Add)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBConfigScoringPtr AddScoring();
		}
	}
}
