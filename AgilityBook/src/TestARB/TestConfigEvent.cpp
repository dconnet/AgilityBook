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

#if 0
class ARBConfigEvent : public ARBBase
{
	static ARBConfigEventPtr New();
	ARBConfigEventPtr Clone() const;
	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
	bool operator==(ARBConfigEvent const& rhs) const;
	bool operator!=(ARBConfigEvent const& rhs) const
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	bool Update(
			int indent,
			ARBConfigEventPtr inEventNew,
			tstring& ioInfo);
	size_t FindAllEvents(
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			bool inTitlePoints,
			ARBVector<ARBConfigScoringPtr>& outList) const
	bool VerifyEvent(
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate) const
	bool FindEvent(
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigScoringPtr* outScoring = NULL) const
};

class ARBConfigEventList : public ARBVector<ARBConfigEventPtr>
{
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	void ReorderBy(ARBConfigEventList const& inList);
	bool VerifyEvent(
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate) const;
	 bool FindEvent(
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigEventPtr* outEvent = NULL,
			ARBConfigScoringPtr* outScoring = NULL) const;
	int RenameDivision(
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(tstring const& inDiv);
	int RenameLevel(
			tstring const& inOldDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);
	int DeleteLevel(
			tstring const& inDiv,
			tstring const& inLevel);
	bool FindEvent(
			tstring const& inEvent,
			ARBConfigEventPtr* outEvent = NULL) const;
	bool AddEvent(ARBConfigEventPtr inEvent);
	bool DeleteEvent(tstring const& inEvent);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
