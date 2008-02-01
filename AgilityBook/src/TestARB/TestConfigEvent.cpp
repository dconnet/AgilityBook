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
 * @brief Test ARBConfigEvent and ARBConfigEventList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigEvent.h"
#include "Element.h"


BEGIN_TEST(ConfigEvent_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigEventPtr New();
}
END_TEST


BEGIN_TEST(ConfigEvent_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigEventPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigEvent_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
}
END_TEST


BEGIN_TEST(ConfigEvent_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigEvent const& rhs) const;
//	bool operator!=(ARBConfigEvent const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigEvent_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigEvent_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigDivisionList const& inDivisions,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigEvent_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigEvent_Update)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Update(
//			int indent,
//			ARBConfigEventPtr inEventNew,
//			tstring& ioInfo);
}
END_TEST


BEGIN_TEST(ConfigEvent_FindAllEvents)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	size_t FindAllEvents(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			bool inTitlePoints,
//			ARBVector<ARBConfigScoringPtr>& outList) const
}
END_TEST


BEGIN_TEST(ConfigEvent_VerifyEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyEvent(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate) const
}
END_TEST


BEGIN_TEST(ConfigEvent_FindEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindEvent(
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			ARBConfigScoringPtr* outScoring = NULL) const
}
END_TEST


BEGIN_TEST(ConfigEventList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigDivisionList const& inDivisions,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigEventList_ReorderBy)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void ReorderBy(ARBConfigEventList const& inList);
	// Make sure the target config has an event that the source config does not
}
END_TEST


BEGIN_TEST(ConfigEventList_VerifyEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyEvent(
//			tstring const& inEvent,
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate) const;
}
END_TEST


BEGIN_TEST(ConfigEventList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	 bool FindEvent(
//			tstring const& inEvent,
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			ARBConfigEventPtr* outEvent = NULL,
//			ARBConfigScoringPtr* outScoring = NULL) const;
//	bool FindEvent(
//			tstring const& inEvent,
//			ARBConfigEventPtr* outEvent = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigEventList_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameDivision(
//			tstring const& inOldDiv,
//			tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(ConfigEventList_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteDivision(tstring const& inDiv);
}
END_TEST


BEGIN_TEST(ConfigEventList_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameLevel(
//			tstring const& inOldDiv,
//			tstring const& inOldLevel,
//			tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(ConfigEventList_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteLevel(
//			tstring const& inDiv,
//			tstring const& inLevel);
}
END_TEST


BEGIN_TEST(ConfigEventList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddEvent(ARBConfigEventPtr inEvent);
//	bool DeleteEvent(tstring const& inEvent);
}
END_TEST
