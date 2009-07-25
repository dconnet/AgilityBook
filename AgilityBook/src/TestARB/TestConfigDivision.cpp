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


SUITE(TestConfigDivision)
{
	TEST(New)
	{
		TODO_TEST
		//static ARBConfigDivisionPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
		//ARBConfigDivisionPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigDivision const& rhs) const;
	//	bool operator!=(ARBConfigDivision const& rhs) const
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
	//			ARBConfigVenue& ioVenue,
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
	//			ARBConfigDivisionPtr inDivNew,
	//			tstring& ioInfo);
	}
}


SUITE(TestConfigDivisionList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigVenue& ioVenue,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Reorder)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigDivisionList const& inList);
		// Make sure the target config has a division that the source config does not
	}


	TEST(VerifyLevel)
	{
		TODO_TEST
	//	bool VerifyLevel(
	//			tstring const& inDiv,
	//			tstring const& inLevel) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	bool FindDivision(
	//			tstring const& inDiv,
	//			ARBConfigDivisionPtr* outDiv = NULL) const;
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddDivision(
	//			tstring const& inDiv,
	//			ARBConfigDivisionPtr* outDiv = NULL);
	//	bool AddDivision(ARBConfigDivisionPtr inDiv);
	//	int DeleteDivision(
	//			tstring const& inDiv,
	//			ARBConfigEventList& ioEvents);
	}
}
