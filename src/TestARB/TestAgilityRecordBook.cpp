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
 * @brief Test ARBAgilityRecordBook class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBAgilityRecordBook.h"
#include "Element.h"

#if 0
class ARBAgilityRecordBook
{
	static ARBVersion const& GetCurrentDocVersion();
	ARBAgilityRecordBook();
	void clear();
	bool Load(
		ElementNodePtr inTree,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inInfo,
		bool inDogs,
		ARBErrorCallback& ioCallback);
	bool Load(ElementNodePtr inTree, ARBErrorCallback& ioCallback)
	{
		return Load(inTree, true, true, true, true, true, ioCallback);
	}
	bool Save(
		ElementNodePtr outTree,
		tstring const& inPgmVer,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inInfo,
		bool inDogs) const;
	void Default();
	bool Update(
			int indent,
			ARBConfig const& inConfigNew,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack);
	size_t GetAllClubNames(
			std::set<tstring>& outClubs,
			bool bInfo,
			bool bVisibleOnly) const;
	size_t GetAllTrialLocations(
			std::set<tstring>& outLocations,
			bool bInfo,
			bool bVisibleOnly) const;
	size_t GetAllEventSubNames(
			tstring const& inVenue,
			ARBConfigEventPtr inEvent,
			std::set<tstring>& outNames) const;
	size_t GetAllHeights(std::set<tstring>& outHeights) const;
	size_t GetAllCallNames(std::set<tstring>& outNames) const;
	size_t GetAllBreeds(std::set<tstring>& outBreeds) const;
	size_t GetAllJudges(
			std::set<tstring>& outJudges,
			bool bInfo,
			bool bVisibleOnly) const;
	size_t GetAllHandlers(std::set<tstring>& outHandlers) const;
	void GetAllPartners(
			std::set<tstring>& outPartners,
			std::set<tstring>& outDogs) const;
	size_t GetAllFaultTypes(std::set<tstring>& outFaults) const;
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
