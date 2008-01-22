/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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


BEGIN_TEST(DogClub_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogClubPtr New();
}
END_TEST


BEGIN_TEST(DogClub_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogClubPtr Clone() const;
}
END_TEST


BEGIN_TEST(DogClub_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogClub& operator=(ARBDogClub const& rhs);
}
END_TEST


BEGIN_TEST(DogClub_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogClub const& rhs) const;
//	bool operator!=(ARBDogClub const& rhs) const
}
END_TEST


BEGIN_TEST(DogClub_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(DogClub_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogClub_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogClubList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogClubList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogClubList_GetPrimaryClub)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;
}
END_TEST


BEGIN_TEST(DogClubList_GetPrimaryClubName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	tstring GetPrimaryClubName() const;
}
END_TEST


BEGIN_TEST(DogClubList_GetPrimaryClubVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	tstring GetPrimaryClubVenue() const;
}
END_TEST


BEGIN_TEST(DogClubList_FindEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
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
END_TEST


BEGIN_TEST(DogClubList_FindVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindVenue(
//			tstring const& inVenue,
//			ARBDogClubPtr* outClub = NULL) const;
}
END_TEST


BEGIN_TEST(DogClubList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddClub(
//			tstring const& inName,
//			tstring const& inVenue,
//			ARBDogClubPtr* outClub = NULL);
//	bool DeleteClub(
//			tstring const& inName,
//			tstring const& inVenue);
}
END_TEST
