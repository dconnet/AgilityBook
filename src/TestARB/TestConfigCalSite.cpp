/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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


SUITE(TestConfigCalSite)
{
	TEST(New)
	{
		TODO_TEST
		//static ARBConfigCalSitePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
		//ARBConfigCalSitePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
		//ARBConfigCalSite& operator=(ARBConfigCalSite const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
		//bool operator==(ARBConfigCalSite const& rhs) const;
		//bool operator!=(ARBConfigCalSite const& rhs) const
	}


	TEST(Load)
	{
		TODO_TEST
		//bool Load(
		//		ElementNodePtr inTree,
		//		ARBVersion const& inVersion,
		//		ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
		//bool Save(ElementNodePtr ioTree) const;
	}


	TEST(GetFormattedURL)
	{
		TODO_TEST
		//tstring GetFormattedURL(
		//		std::vector<tstring> const& inLocCodes,
		//		std::vector<tstring> const& inVenueCodes) const;
	}


	TEST(LocationCodes)
	{
		TODO_TEST
		//bool HasLocationCode(tstring const& inCode) const;
		//bool AddLocationCode(tstring const& inCode, tstring const& inName);
		//bool RemoveLocationCode(tstring const& inCode);
		//bool RemoveAllLocationCodes();
	}


	TEST(VenueCodes)
	{
		TODO_TEST
		//bool HasVenueCode(tstring const& inCode) const;
		//bool AddVenueCode(tstring const& inCode, tstring const& inVenue);
		//bool RemoveVenueCode(tstring const& inCode);
		//bool RemoveAllVenueCodes();
	}
}


SUITE(TestConfigCalSiteList)
{
	TEST(Load)
	{
		TODO_TEST
		//bool Load(
		//		ElementNodePtr inTree,
		//		ARBVersion const& inVersion,
		//		ARBErrorCallback& ioCallback);
	}


	TEST(sort)
	{
		TODO_TEST
		//void sort();
	}


	TEST(Find)
	{
		TODO_TEST
		//bool FindSite(
		//		tstring const& inSite,
		//		ARBConfigCalSitePtr* outSite = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
		//bool AddSite(
		//		tstring const& inSite,
		//		ARBConfigCalSitePtr* outSite = NULL);
		//bool AddSite(ARBConfigCalSitePtr inSite);
		//int DeleteSite(tstring const& inSite);
	}
}
