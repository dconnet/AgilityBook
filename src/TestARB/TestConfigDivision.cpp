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
 * @brief Test ARBConfigDivision and ARBConfigDivisionList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigDivision.h"
#include "Element.h"


BEGIN_TEST(ConfigDivision_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//static ARBConfigDivisionPtr New();
}
END_TEST


BEGIN_TEST(ConfigDivision_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//ARBConfigDivisionPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigDivision_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);
}
END_TEST


BEGIN_TEST(ConfigDivision_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigDivision const& rhs) const;
//	bool operator!=(ARBConfigDivision const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigDivision_Clear)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void clear();
}
END_TEST


BEGIN_TEST(ConfigDivision_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigDivision_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigVenue& ioVenue,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigDivision_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigDivision_Update)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Update(
//			int indent,
//			ARBConfigDivisionPtr inDivNew,
//			tstring& ioInfo);
}
END_TEST


BEGIN_TEST(ConfigDivisionList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfigVenue& ioVenue,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigDivisionList_Reorder)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void ReorderBy(ARBConfigDivisionList const& inList);
	// Make sure the target config has a division that the source config does not
}
END_TEST


BEGIN_TEST(ConfigDivisionList_VerifyLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyLevel(
//			tstring const& inDiv,
//			tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(ConfigDivisionList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindDivision(
//			tstring const& inDiv,
//			ARBConfigDivisionPtr* outDiv = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigDivisionList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddDivision(
//			tstring const& inDiv,
//			ARBConfigDivisionPtr* outDiv = NULL);
//	bool AddDivision(ARBConfigDivisionPtr inDiv);
//	int DeleteDivision(
//			tstring const& inDiv,
//			ARBConfigEventList& ioEvents);
}
END_TEST
