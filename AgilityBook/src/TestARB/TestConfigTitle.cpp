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


SUITE(TestConfigTitle)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigTitlePtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigTitlePtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigTitle const& rhs) const;
	//	bool operator!=(ARBConfigTitle const& rhs) const
	}


	TEST(Clear)
	{
		TODO_TEST
	//	void clear();
	}


	TEST(GenName)
	{
		TODO_TEST
	//	virtual tstring GetGenericName() const
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(IsValidOn)
	{
		TODO_TEST
	//	bool IsValidOn(ARBDate inDate) const
	}


	TEST(GetNiceName)
	{
		TODO_TEST
	//	tstring const& GetNiceName() const
	}


	TEST(GetCompleteName)
	{
		TODO_TEST
	//	tstring GetCompleteName(
	//			short inInstance = 0,
	//			bool bShowInstance = false,
	//			bool bAbbrevFirst = true,
	//			bool bAddDates = false) const;
	}
}


SUITE(TestConfigTitleList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback,
	//			bool inCheckDups = false);
	}


	TEST(ReorderBy)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigTitleList const& inList);
		// Make sure the target config has a title that the source config does not
	}


	TEST(FindTitleCompleteName)
	{
		TODO_TEST
	//	bool FindTitleCompleteName(
	//			tstring const& inName,
	//			short inInstance,
	//			bool bShowInstance,
	//			bool bAbbrevFirst = true,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindTitle(
	//			tstring const& inName,
	//			ARBConfigTitlePtr* outTitle = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddTitle(
	//			tstring const& inName,
	//			ARBConfigTitlePtr* outTitle = NULL);
	//	bool AddTitle(ARBConfigTitlePtr inTitle);
	//	bool DeleteTitle(tstring const& inName);
	}
}
