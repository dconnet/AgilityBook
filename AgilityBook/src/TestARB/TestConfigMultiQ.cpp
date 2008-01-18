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
 * @brief Test ARBConfigMultiQItem, ARBConfigMultiQ and ARBConfigMultiQList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigMultiQ.h"
#include "Element.h"

#if 0
class ARBConfigMultiQ : public ARBBase
{
	static ARBConfigMultiQPtr New();
	ARBConfigMultiQPtr Clone() const;
	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
	bool operator==(ARBConfigMultiQ const& rhs) const;
	bool operator!=(ARBConfigMultiQ const& rhs) const
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ARBConfigVenue const& inVenue,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	bool Match(
			std::vector<ARBDogRunPtr>& ioRuns,
			std::vector<ARBDogRunPtr>& outRuns) const;
	int RenameDivision(
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(tstring const& inDiv);
	int RenameLevel(
			tstring const& inDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);
	int DeleteLevel(tstring const& inLevel);
	int RenameEvent(
			tstring const& inOldEvent,
			tstring const& inNewEvent);
	int DeleteEvent(tstring const& inEvent);
	bool AddItem(
			tstring const& inDiv,
			tstring const& inLevel,
			tstring const& inEvent);
	bool RemoveItem(
			tstring const& inDiv,
			tstring const& inLevel,
			tstring const& inEvent);
	bool RemoveAllItems();
};

class ARBConfigMultiQList : public ARBVector<ARBConfigMultiQPtr>
{
	bool Load(
			ARBConfigVenue const& inVenue,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool FindMultiQ(
			tstring const& inName,
			bool inUseShortName = false,
			ARBConfigMultiQPtr* outMultiQ = NULL) const;
	bool FindMultiQ(
			ARBConfigMultiQ const& inMultiQ,
			ARBConfigMultiQPtr* outMultiQ = NULL) const;
	int RenameDivision(
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(tstring const& inDiv);
	int RenameLevel(
			tstring const& inDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);
	int DeleteLevel(tstring const& inLevel);
	int RenameEvent(
			tstring const& inOldEvent,
			tstring const& inNewEvent);
	int DeleteEvent(tstring const& inEvent);
	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);
	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
