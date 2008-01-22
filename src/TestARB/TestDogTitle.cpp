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


BEGIN_TEST(DogTitle_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogTitlePtr New();
}
END_TEST


BEGIN_TEST(DogTitle_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogTitlePtr Clone() const;
}
END_TEST


BEGIN_TEST(DogTitle_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogTitle& operator=(ARBDogTitle const& rhs);
}
END_TEST


BEGIN_TEST(DogTitle_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogTitle const& rhs) const;
//	bool operator!=(ARBDogTitle const& rhs) const
}
END_TEST


BEGIN_TEST(DogTitle_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const;
}
END_TEST


BEGIN_TEST(DogTitle_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogTitle_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogTitleList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogTitleList_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
}
END_TEST


BEGIN_TEST(DogTitleList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
}
END_TEST


BEGIN_TEST(DogTitleList_NumTitlesInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumTitlesInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogTitleList_FindTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitle(
//			tstring const& inVenue,
//			tstring const& inTitle,
//			ARBDogTitlePtr* outTitle = NULL) const;
}
END_TEST


BEGIN_TEST(DogTitleList_FindMaxInstance)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	short FindMaxInstance(
//			tstring const& inVenue,
//			tstring const& inTitle) const;
}
END_TEST


BEGIN_TEST(DogTitleList_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameVenue(
//			tstring const& inOldVenue,
//			tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(DogTitleList_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogTitleList_NumTitlesInUse)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumTitlesInUse(
//			tstring const& inVenue,
//			tstring const& inTitle) const;
}
END_TEST


BEGIN_TEST(DogTitleList_RenameTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameTitle(
//			tstring const& inVenue,
//			tstring const& inOldTitle,
//			tstring const& inNewTitle);
}
END_TEST


BEGIN_TEST(DogTitleList_AddTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddTitle(ARBDogTitlePtr inTitle);
}
END_TEST


BEGIN_TEST(DogTitleList_DeleteTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool DeleteTitle(ARBDogTitlePtr inTitle);
}
END_TEST
