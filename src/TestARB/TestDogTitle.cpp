/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief Test ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
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
		TODO_TEST
	//	static ARBDogTitlePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogTitlePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogTitle& operator=(ARBDogTitle const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogTitle const& rhs) const;
	//	bool operator!=(ARBDogTitle const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual tstring GetGenericName() const;
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}
}


SUITE(TestDogTitleList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfig const& inConfig,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	}


	TEST(sort)
	{
		TODO_TEST
	//	void sort();
	}


	TEST(NumTitlesInVenue)
	{
		TODO_TEST
	//	int NumTitlesInVenue(tstring const& inVenue) const;
	}


	TEST(FindTitle)
	{
		TODO_TEST
	//	bool FindTitle(
	//			tstring const& inVenue,
	//			tstring const& inTitle,
	//			ARBDogTitlePtr* outTitle = NULL) const;
	}


	TEST(FindMaxInstance)
	{
		TODO_TEST
	//	short FindMaxInstance(
	//			tstring const& inVenue,
	//			tstring const& inTitle) const;
	}


	TEST(RenameVenue)
	{
		TODO_TEST
	//	int RenameVenue(
	//			tstring const& inOldVenue,
	//			tstring const& inNewVenue);
	}


	TEST(DeleteVenue)
	{
		TODO_TEST
	//	int DeleteVenue(tstring const& inVenue);
	}


	TEST(NumTitlesInUse)
	{
		TODO_TEST
	//	int NumTitlesInUse(
	//			tstring const& inVenue,
	//			tstring const& inTitle) const;
	}


	TEST(RenameTitle)
	{
		TODO_TEST
	//	int RenameTitle(
	//			tstring const& inVenue,
	//			tstring const& inOldTitle,
	//			tstring const& inNewTitle);
	}


	TEST(AddTitle)
	{
		TODO_TEST
	//	bool AddTitle(ARBDogTitlePtr inTitle);
	}


	TEST(DeleteTitle)
	{
		TODO_TEST
	//	bool DeleteTitle(ARBDogTitlePtr inTitle);
	}
}
