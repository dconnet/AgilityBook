/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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


SUITE(TestConfigLevel)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigLevelPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigLevelPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigLevel& operator=(ARBConfigLevel const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigLevel const& rhs) const;
	//	bool operator!=(ARBConfigLevel const& rhs) const
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


	TEST(Update)
	{
		TODO_TEST
	//	bool Update(
	//			int indent,
	//			ARBConfigLevelPtr inLevelNew,
	//			tstring& ioInfo);
	}
}


SUITE(TestConfigLevelList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(VerifyLevel)
	{
		TODO_TEST
	//	bool VerifyLevel(
	//			tstring const& inName,
	//			bool inAllowWildCard = true) const;
	}


	TEST(FindLevel)
	{
		TODO_TEST
	//	bool FindLevel(
	//			tstring const& inName,
	//			ARBConfigLevelPtr* outLevel = NULL);
	}


	TEST(FindSubLevel)
	{
		TODO_TEST
	//	bool FindSubLevel(
	//			tstring const& inName,
	//			ARBConfigLevelPtr* outLevel = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
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
}
