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

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogTitle.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
		//	virtual std::wstring GetGenericName() const;
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
		//	int NumTitlesInVenue(std::wstring const& inVenue) const;
		}
	}


	TEST(FindTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool FindTitle(
		//			std::wstring const& inVenue,
		//			std::wstring const& inTitle,
		//			ARBDogTitlePtr* outTitle = NULL) const;
		}
	}


	TEST(FindMaxInstance)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	short FindMaxInstance(
		//			std::wstring const& inVenue,
		//			std::wstring const& inTitle) const;
		}
	}


	TEST(RenameVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameVenue(
		//			std::wstring const& inOldVenue,
		//			std::wstring const& inNewVenue);
		}
	}


	TEST(DeleteVenue)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int DeleteVenue(std::wstring const& inVenue);
		}
	}


	TEST(NumTitlesInUse)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int NumTitlesInUse(
		//			std::wstring const& inVenue,
		//			std::wstring const& inTitle) const;
		}
	}


	TEST(RenameTitle)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	int RenameTitle(
		//			std::wstring const& inVenue,
		//			std::wstring const& inOldTitle,
		//			std::wstring const& inNewTitle);
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
