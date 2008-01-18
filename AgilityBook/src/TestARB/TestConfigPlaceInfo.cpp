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
 * @brief Test ARBConfigPlaceInfo and ARBConfigPlaceInfoList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigPlaceInfo.h"
#include "Element.h"

BEGIN_TEST(ConfigPlaceInfo_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigPlaceInfoPtr New();
//	static ARBConfigPlaceInfoPtr New(short inPlace, double inValue, bool bMustQ);
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigPlaceInfoPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo const& rhs);
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigPlaceInfo const& rhs) const;
//	bool operator!=(ARBConfigPlaceInfo const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const;
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigPlaceInfo_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigPlaceInfoList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigPlaceInfoList_SortAddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
//	bool AddPlaceInfo(
//			short inPlace,
//			double inValue,
//			bool inMustQ,
//			ARBConfigPlaceInfoPtr* outPlace = NULL);
//	bool DeletePlaceInfo(short inPlace);
}
END_TEST


BEGIN_TEST(ConfigPlaceInfoList_GetPlaceInfo)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool GetPlaceInfo(short inPlace, double &outValue) const;
}
END_TEST


BEGIN_TEST(ConfigPlaceInfoList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindPlaceInfo(
//			short inPlace,
//			ARBConfigPlaceInfoPtr* outPlace = NULL) const;
}
END_TEST
