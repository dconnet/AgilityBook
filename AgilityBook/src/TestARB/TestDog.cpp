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
 * @brief Test ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDog.h"
#include "Element.h"

#if 0
class ARBDog : public ARBBase
{
	static ARBDogPtr New();
	ARBDogPtr Clone() const;
	ARBDog& operator=(ARBDog const& rhs);
	bool operator==(ARBDog const& rhs) const;
	bool operator!=(ARBDog const& rhs) const
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	int RenameVenue(
			tstring const& inOldVenue,
			tstring const& inNewVenue);
	int DeleteVenue(tstring const& inVenue);
	int RenameDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(
			ARBConfig const& inConfig,
			tstring const& inVenue,
			tstring const& inDiv);
};

class ARBDogList : public ARBVector<ARBDogPtr>
{
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	void SetMultiQs(ARBConfig const& inConfig);
	int NumExistingPointsInVenue(tstring const& inVenue) const;
	int NumRegNumsInVenue(tstring const& inVenue) const;
	int NumTitlesInVenue(tstring const& inVenue) const;
	int NumTrialsInVenue(tstring const& inVenue) const;
	int RenameVenue(
			tstring const& inOldVenue,
			tstring const& inNewVenue);
	int DeleteVenue(tstring const& inVenue);
	int NumOtherPointsInUse(tstring const& inOther) const;
	int RenameOtherPoints(
			tstring const& inOldOther,
			tstring const& inNewOther);
	int DeleteOtherPoints(tstring const& inOther);
	int NumMultiQsInUse(
			tstring const& inVenue,
			tstring const& inMultiQ) const;
	int RenameMultiQs(
			tstring const& inVenue,
			tstring const& inOldMultiQ,
			tstring const& inNewMultiQ);
	int DeleteMultiQs(
			ARBConfig const& inConfig,
			tstring const& inVenue);
	int NumMultiHostedTrialsInDivision(
			ARBConfig const& inConfig,
			tstring const& inVenue,
			tstring const& inDiv) const;
	int NumExistingPointsInDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inDiv) const;
	int NumRunsInDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inDiv) const;
	int RenameDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(
			ARBConfig const& inConfig,
			tstring const& inVenue,
			tstring const& inDiv);
	int NumLevelsInUse(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel) const;
	int RenameLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);
	int DeleteLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel);
	int NumTitlesInUse(
			tstring const& inVenue,
			tstring const& inTitle) const;
	int RenameTitle(
			tstring const& inVenue,
			tstring const& inOldTitle,
			tstring const& inNewTitle);
	int DeleteTitle(
			tstring const& inVenue,
			tstring const& inTitle);
	int NumEventsInUse(
			tstring const& inVenue,
			tstring const& inEvent) const;
	int RenameEvent(
			tstring const& inVenue,
			tstring const& inOldEvent,
			tstring const& inNewEvent);
	int DeleteEvent(
			tstring const& inVenue,
			tstring const& inEvent);
	bool AddDog(ARBDogPtr inDog);
	bool DeleteDog(ARBDogPtr inDog);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
