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
 * @brief Test ARBConfigEvent and ARBConfigEventList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigEvent.h"
#include "Element.h"


SUITE(TestConfigEvent)
{
	TEST(New)
	{
		TODO_TEST
	//	static ARBConfigEventPtr New();
	}


	TEST(Clone)
	{
		TODO_TEST
	//	ARBConfigEventPtr Clone() const;
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBConfigEvent const& rhs) const;
	//	bool operator!=(ARBConfigEvent const& rhs) const
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
	//			ARBConfigDivisionList const& inDivisions,
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
	//			ARBConfigEventPtr inEventNew,
	//			tstring& ioInfo);
	}


	TEST(FindAllEvents)
	{
		TODO_TEST
	//	size_t FindAllEvents(
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate,
	//			bool inTitlePoints,
	//			ARBVector<ARBConfigScoringPtr>& outList) const
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate) const
	}


	TEST(FindEvent)
	{
		TODO_TEST
	//	bool FindEvent(
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate,
	//			ARBConfigScoringPtr* outScoring = NULL) const
	}
}


SUITE(TestConfigEventList)
{
	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			ARBConfigDivisionList const& inDivisions,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(ReorderBy)
	{
		TODO_TEST
	//	void ReorderBy(ARBConfigEventList const& inList);
		// Make sure the target config has an event that the source config does not
	}


	TEST(VerifyEvent)
	{
		TODO_TEST
	//	bool VerifyEvent(
	//			tstring const& inEvent,
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate) const;
	}


	TEST(Find)
	{
		TODO_TEST
	//	 bool FindEvent(
	//			tstring const& inEvent,
	//			tstring const& inDivision,
	//			tstring const& inLevel,
	//			ARBDate const& inDate,
	//			ARBConfigEventPtr* outEvent = NULL,
	//			ARBConfigScoringPtr* outScoring = NULL) const;
	//	bool FindEvent(
	//			tstring const& inEvent,
	//			ARBConfigEventPtr* outEvent = NULL) const;
	}


	TEST(RenameDivision)
	{
		TODO_TEST
	//	int RenameDivision(
	//			tstring const& inOldDiv,
	//			tstring const& inNewDiv);
	}


	TEST(DeleteDivision)
	{
		TODO_TEST
	//	int DeleteDivision(tstring const& inDiv);
	}


	TEST(RenameLevel)
	{
		TODO_TEST
	//	int RenameLevel(
	//			tstring const& inOldDiv,
	//			tstring const& inOldLevel,
	//			tstring const& inNewLevel);
	}


	TEST(DeleteLevel)
	{
		TODO_TEST
	//	int DeleteLevel(
	//			tstring const& inDiv,
	//			tstring const& inLevel);
	}


	TEST(AddDelete)
	{
		TODO_TEST
	//	bool AddEvent(ARBConfigEventPtr inEvent);
	//	bool DeleteEvent(tstring const& inEvent);
	}
}
