/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Test ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogClub.h"
#include "Element.h"


SUITE(TestDogClub)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBDogClubPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBDogClubPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogClub& operator=(ARBDogClub const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogClub const& rhs) const;
	//	bool operator!=(ARBDogClub const& rhs) const
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual tstring GetGenericName() const
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


SUITE(TestDogClubList)
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


	TEST(GetPrimaryClub)
	{
		TODO_TEST
	//	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;
	}


	TEST(GetPrimaryClubName)
	{
		TODO_TEST
	//	tstring GetPrimaryClubName() const;
	}


	TEST(GetPrimaryClubVenue)
	{
		TODO_TEST
	//	tstring GetPrimaryClubVenue() const;
	}


	TEST(FindEvent)
	{
		TODO_TEST
	//	bool FindEvent(
	//			ARBConfig const& inConfig,
	//			tstring const& inEvent,
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate,
	//			ARBErrorCallback& ioCallback,
	//			ARBConfigEventPtr* outEvent = NULL,
	//			ARBConfigScoringPtr* outScoring = NULL) const;
	}


	TEST(FindVenue)
	{
		TODO_TEST
	//	bool FindVenue(
	//			tstring const& inVenue,
	//			ARBDogClubPtr* outClub = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddClub(
	//			tstring const& inName,
	//			tstring const& inVenue,
	//			ARBDogClubPtr* outClub = NULL);
	//	bool DeleteClub(
	//			tstring const& inName,
	//			tstring const& inVenue);
	}
}
