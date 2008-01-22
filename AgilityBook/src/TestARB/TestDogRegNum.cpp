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
 * @brief Test ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRegNum.h"
#include "Element.h"


BEGIN_TEST(DogRegNum_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBDogRegNumPtr New();
}
END_TEST


BEGIN_TEST(DogRegNum_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogRegNumPtr Clone() const;
}
END_TEST


BEGIN_TEST(DogRegNum_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);
}
END_TEST


BEGIN_TEST(DogRegNum_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBDogRegNum const& rhs) const;
//	bool operator!=(ARBDogRegNum const& rhs) const
}
END_TEST


BEGIN_TEST(DogRegNum_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(DogRegNum_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogRegNum_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(DogRegNumList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig const& inConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(DogRegNumList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
}
END_TEST


BEGIN_TEST(DogRegNumList_NumRegNumsInVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int NumRegNumsInVenue(tstring const& inVenue) const;
}
END_TEST


BEGIN_TEST(DogRegNumList_RenameVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameVenue(
//			tstring const& inOldVenue,
//			tstring const& inNewVenue);
}
END_TEST


BEGIN_TEST(DogRegNumList_DeleteVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteVenue(tstring const& inVenue);
}
END_TEST


BEGIN_TEST(DogRegNumList_FindRegNum)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindRegNum(
//			tstring const& inVenue,
//			ARBDogRegNumPtr* outRegNum = NULL) const;
}
END_TEST


BEGIN_TEST(DogRegNumList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddRegNum(
//			tstring const& inVenue,
//			tstring const& inNumber,
//			ARBDogRegNumPtr* outRegNum = NULL);
//	bool AddRegNum(ARBDogRegNumPtr inRegNum);
//	int DeleteRegNum(
//			tstring const& inVenue,
//			tstring const& inNumber);
}
END_TEST
