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
 * @brief Test ARBConfigMultiQItem, ARBConfigMultiQ and ARBConfigMultiQList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigMultiQ.h"
#include "Element.h"


BEGIN_TEST(ConfigMultiQ_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigMultiQPtr New();
}
END_TEST


BEGIN_TEST(ConfigMultiQ_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigMultiQPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigMultiQ_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigMultiQ const& rhs) const;
//	bool operator!=(ARBConfigMultiQ const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigMultiQ_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigMultiQ_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigVenue const& inVenue,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigMultiQ_Match)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Match(
//			std::vector<ARBDogRunPtr>& ioRuns,
//			std::vector<ARBDogRunPtr>& outRuns) const;
}
END_TEST


BEGIN_TEST(ConfigMultiQ_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameDivision(
//			tstring const& inOldDiv,
//			tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteDivision(tstring const& inDiv);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameLevel(
//			tstring const& inDiv,
//			tstring const& inOldLevel,
//			tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteLevel(tstring const& inLevel);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_RenameEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameEvent(
//			tstring const& inOldEvent,
//			tstring const& inNewEvent);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_DeleteEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteEvent(tstring const& inEvent);
}
END_TEST


BEGIN_TEST(ConfigMultiQ_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddItem(
//			tstring const& inDiv,
//			tstring const& inLevel,
//			tstring const& inEvent);
//	bool RemoveItem(
//			tstring const& inDiv,
//			tstring const& inLevel,
//			tstring const& inEvent);
//	bool RemoveAllItems();
}
END_TEST


BEGIN_TEST(ConfigMultiQList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigVenue const& inVenue,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindMultiQ(
//			tstring const& inName,
//			bool inUseShortName = false,
//			ARBConfigMultiQPtr* outMultiQ = NULL) const;
//	bool FindMultiQ(
//			ARBConfigMultiQ const& inMultiQ,
//			ARBConfigMultiQPtr* outMultiQ = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigMultiQList_RenameDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameDivision(
//			tstring const& inOldDiv,
//			tstring const& inNewDiv);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_DeleteDivision)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteDivision(tstring const& inDiv);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_RenameLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameLevel(
//			tstring const& inDiv,
//			tstring const& inOldLevel,
//			tstring const& inNewLevel);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_DeleteLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteLevel(tstring const& inLevel);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_RenameEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int RenameEvent(
//			tstring const& inOldEvent,
//			tstring const& inNewEvent);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_DeleteEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	int DeleteEvent(tstring const& inEvent);
}
END_TEST


BEGIN_TEST(ConfigMultiQList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);
//	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
}
END_TEST
