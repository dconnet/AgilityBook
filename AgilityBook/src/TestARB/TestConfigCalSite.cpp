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
 * @brief Test ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigCalSite.h"
#include "Element.h"


BEGIN_TEST(ConfigCalSite_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//static ARBConfigCalSitePtr New();
}
END_TEST


BEGIN_TEST(ConfigCalSite_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//ARBConfigCalSitePtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigCalSite_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//ARBConfigCalSite& operator=(ARBConfigCalSite const& rhs);
}
END_TEST


BEGIN_TEST(ConfigCalSite_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool operator==(ARBConfigCalSite const& rhs) const;
	//bool operator!=(ARBConfigCalSite const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigCalSite_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Load(
	//		ElementNodePtr inTree,
	//		ARBVersion const& inVersion,
	//		ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigCalSite_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigCalSite_GetFormattedURL)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//tstring GetFormattedURL(
	//		std::vector<tstring> const& inLocCodes,
	//		std::vector<tstring> const& inVenueCodes) const;
}
END_TEST


BEGIN_TEST(ConfigCalSite_LocationCodes)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool HasLocationCode(tstring const& inCode) const;
	//bool AddLocationCode(tstring const& inCode, tstring const& inName);
	//bool RemoveLocationCode(tstring const& inCode);
	//bool RemoveAllLocationCodes();
}
END_TEST


BEGIN_TEST(ConfigCalSite_VenueCodes)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool HasVenueCode(tstring const& inCode) const;
	//bool AddVenueCode(tstring const& inCode, tstring const& inVenue);
	//bool RemoveVenueCode(tstring const& inCode);
	//bool RemoveAllVenueCodes();
}
END_TEST


BEGIN_TEST(ConfigCalSiteList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Load(
	//		ElementNodePtr inTree,
	//		ARBVersion const& inVersion,
	//		ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigCalSiteList_sort)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//void sort();
}
END_TEST


BEGIN_TEST(ConfigCalSiteList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool FindSite(
	//		tstring const& inSite,
	//		ARBConfigCalSitePtr* outSite = NULL) const;
}
END_TEST


BEGIN_TEST(ConfigCalSiteList_AddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool AddSite(
	//		tstring const& inSite,
	//		ARBConfigCalSitePtr* outSite = NULL);
	//bool AddSite(ARBConfigCalSitePtr inSite);
	//int DeleteSite(tstring const& inSite);
}
END_TEST
