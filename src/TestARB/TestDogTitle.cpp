/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogTitle.h"
#include "Element.h"


SUITE(TestDogTitle)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBDogTitlePtr New();
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogTitlePtr Clone() const;
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogTitle& operator=(ARBDogTitle const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogTitle const& rhs) const;
		//	bool operator!=(ARBDogTitle const& rhs) const
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	virtual wxString GetGenericName() const;
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
}


SUITE(TestDogTitleList)
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


	TEST(NumTitlesInVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumTitlesInVenue(wxString const& inVenue) const;
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
		//			ARBDogTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(FindMaxInstance)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	short FindMaxInstance(
		//			wxString const& inVenue,
		//			wxString const& inTitle) const;
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


	TEST(NumTitlesInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumTitlesInUse(
		//			wxString const& inVenue,
		//			wxString const& inTitle) const;
		}
	}


	TEST(RenameTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameTitle(
		//			wxString const& inVenue,
		//			wxString const& inOldTitle,
		//			wxString const& inNewTitle);
		}
	}


	TEST(AddTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool AddTitle(ARBDogTitlePtr inTitle);
		}
	}


	TEST(DeleteTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool DeleteTitle(ARBDogTitlePtr inTitle);
		}
	}
}
