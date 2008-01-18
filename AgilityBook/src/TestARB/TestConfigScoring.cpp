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
 * @brief Test ARBConfigScoring and ARBConfigScoringList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigScoring.h"
#include "Element.h"

BEGIN_TEST(ConfigScoring_GetScoringStyleStr)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static tstring GetScoringStyleStr(ScoringStyle inStyle);
}
END_TEST


BEGIN_TEST(ConfigScoring_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigScoringPtr New();
}
END_TEST


BEGIN_TEST(ConfigScoring_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigScoringPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigScoring_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigScoring& operator=(ARBConfigScoring const& rhs);
}
END_TEST


BEGIN_TEST(ConfigScoring_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigScoring const& rhs) const;
//	bool operator!=(ARBConfigScoring const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigScoring_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigScoring_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigDivisionList const& inDivisions,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigScoring_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigScoring_IsValidOn)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool IsValidOn(ARBDate inDate) const
}
END_TEST


BEGIN_TEST(ConfigScoringList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigDivisionList const& inDivisions,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigScoringList_FindAllEvents)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	size_t FindAllEvents(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			bool inTitlePoints,
//			ARBVector<ARBConfigScoringPtr>& outList) const;
}
END_TEST


BEGIN_TEST(ConfigScoringList_FindEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindEvent(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			ARBConfigScoringPtr* outEvent = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigScoringList_VerifyEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyEvent(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate) const;
}
END_TEST


BEGIN_TEST(ConfigScoringList_Add)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigScoringPtr AddScoring();
}
END_TEST
