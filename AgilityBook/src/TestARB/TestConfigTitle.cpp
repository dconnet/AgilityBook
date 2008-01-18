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
 * @brief Test ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigTitle.h"
#include "Element.h"


BEGIN_TEST(ConfigTitle_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigTitlePtr New();
}
END_TEST


BEGIN_TEST(ConfigTitle_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigTitlePtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigTitle_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
}
END_TEST


BEGIN_TEST(ConfigTitle_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigTitle const& rhs) const;
//	bool operator!=(ARBConfigTitle const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigTitle_Clear)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void clear();
}
END_TEST


BEGIN_TEST(ConfigTitle_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigTitle_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigTitle_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigTitle_IsValidOn)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool IsValidOn(ARBDate inDate) const
}
END_TEST


BEGIN_TEST(ConfigTitle_GetNiceName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	tstring const& GetNiceName() const
}
END_TEST


BEGIN_TEST(ConfigTitle_GetCompleteName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	tstring GetCompleteName(
//			short inInstance = 0,
//			bool bShowInstance = false,
//			bool bAbbrevFirst = true,
//			bool bAddDates = false) const;
}
END_TEST


BEGIN_TEST(ConfigTitleList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback,
//			bool inCheckDups = false);
}
END_TEST


BEGIN_TEST(ConfigTitleList_ReorderBy)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void ReorderBy(ARBConfigTitleList const& inList);
}
END_TEST


BEGIN_TEST(ConfigTitleList_FindTitleCompleteName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitleCompleteName(
//			tstring const& inName,
//			short inInstance,
//			bool bShowInstance,
//			bool bAbbrevFirst = true,
//			ARBConfigTitlePtr* outTitle = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigTitleList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitle(
//			tstring const& inName,
//			ARBConfigTitlePtr* outTitle = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigTitleList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddTitle(
//			tstring const& inName,
//			ARBConfigTitlePtr* outTitle = NULL);
//	bool AddTitle(ARBConfigTitlePtr inTitle);
//	bool DeleteTitle(tstring const& inName);
}
END_TEST
