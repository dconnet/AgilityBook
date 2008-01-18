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
 * @brief Test ARBConfigLevel and ARBConfigLevel classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigLevel.h"
#include "Element.h"


BEGIN_TEST(ConfigLevel_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigLevelPtr New();
}
END_TEST


BEGIN_TEST(ConfigLevel_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigLevelPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigLevel_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigLevel& operator=(ARBConfigLevel const& rhs);
}
END_TEST


BEGIN_TEST(ConfigLevel_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigLevel const& rhs) const;
//	bool operator!=(ARBConfigLevel const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigLevel_Clear)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void clear();
}
END_TEST


BEGIN_TEST(ConfigLevel_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigLevel_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigLevel_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigLevel_Update)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Update(
//			int indent,
//			ARBConfigLevelPtr inLevelNew,
//			tstring& ioInfo);
}
END_TEST


BEGIN_TEST(ConfigLevelList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigLevelList_VerifyLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyLevel(
//			tstring const& inName,
//			bool inAllowWildCard = true) const;
}
END_TEST


BEGIN_TEST(ConfigLevelList_FindLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindLevel(
//			tstring const& inName,
//			ARBConfigLevelPtr* outLevel = NULL);
}
END_TEST


BEGIN_TEST(ConfigLevelList_FindSubLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindSubLevel(
//			tstring const& inName,
//			ARBConfigLevelPtr* outLevel = NULL) const;
}
END_TEST

BEGIN_TEST(ConfigLevelList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddLevel(
//			tstring const& inName,
//			ARBConfigLevelPtr* outLevel = NULL);
//	bool AddLevel(ARBConfigLevelPtr inLevel);
//	bool DeleteLevel(
//			tstring const& inDiv,
//			tstring const& inName,
//			ARBConfigEventList& ioEvents);
//	bool DeleteSubLevel(
//			tstring const& inName,
//			bool& outLevelModified);
}
END_TEST
