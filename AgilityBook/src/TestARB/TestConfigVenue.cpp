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
 * @brief Test ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigVenue.h"
#include "Element.h"

BEGIN_TEST(ConfigVenue_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigVenuePtr New();
}
END_TEST


BEGIN_TEST(ConfigVenue_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigVenuePtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigVenue_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
}
END_TEST


BEGIN_TEST(ConfigVenue_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigVenue const& rhs) const;
//	bool operator!=(ARBConfigVenue const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigVenue_Clear)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void clear();
}
END_TEST


BEGIN_TEST(ConfigVenue_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const
}
END_TEST


BEGIN_TEST(ConfigVenue_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig& ioConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigVenue_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigVenue_Update)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Update(
//			int indent,
//			ARBConfigVenuePtr inVenueNew,
//			tstring& ioInfo);
}
END_TEST


BEGIN_TEST(ConfigVenueList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ARBConfig& ioConfig,
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigVenueList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
}
END_TEST


BEGIN_TEST(ConfigVenueList_VerifyVenue)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyVenue(tstring const& inVenue) const
}
END_TEST


BEGIN_TEST(ConfigVenueList_VerifyMultiQ)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyMultiQ(
//			tstring const& inVenue,
//			tstring const& inMultiQ,
//			bool inUseShortName = false) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_VerifyLevel)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyLevel(
//			tstring const& inVenue,
//			tstring const& inDivision,
//			tstring const& inLevel) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_VerifyEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool VerifyEvent(
//			tstring const& inVenue,
//			tstring const& inDivision,
//			tstring const& inLevel,
//			tstring const& inEvent,
//			ARBDate const& inDate) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_FindTitleCompleteName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitleCompleteName(
//			tstring const& inVenue,
//			tstring const& inName,
//			bool bShowInstance,
//			bool bAbbrevFirst = true,
//			ARBConfigTitlePtr* outTitle = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_FindTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitle(
//			tstring const& inVenue,
//			tstring const& inTitle,
//			ARBConfigTitlePtr* outTitle = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_DeleteTitle)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool DeleteTitle(tstring const& inTitle);
}
END_TEST


BEGIN_TEST(ConfigVenueList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindVenue(
//			tstring const& inVenue,
//			ARBConfigVenuePtr* outVenue = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_FindEvent)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindEvent(
//			tstring const& inVenue,
//			tstring const& inEvent,
//			tstring const& inDivision,
//			tstring const& inLevel,
//			ARBDate const& inDate,
//			ARBConfigEventPtr* outEvent = NULL,
//			ARBConfigScoringPtr* outScoring = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigVenueList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool AddVenue(
//			tstring const& inVenue,
//			ARBConfigVenuePtr* outVenue = NULL);
//	bool AddVenue(ARBConfigVenuePtr inVenue);
//	int DeleteVenue(tstring const& inVenue);
}
END_TEST
