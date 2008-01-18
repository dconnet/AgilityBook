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
 * @brief Test ARBDogExistingPoints and ARBDogExistingPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogExistingPoints.h"
#include "Element.h"

#if 0
class ARBDogExistingPoints : public ARBBase
{
	static ARBDogExistingPointsPtr New();
	ARBDogExistingPointsPtr Clone() const;
	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
	bool operator==(ARBDogExistingPoints const& rhs) const;
	bool operator!=(ARBDogExistingPoints const& rhs) const
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	static tstring GetPointTypeName(PointType inType);
};

class ARBDogExistingPointsList : public ARBVector<ARBDogExistingPointsPtr>
{
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	void sort();
	bool HasPoints(tstring const& inVenue) const;
	bool HasPoints(
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent,
			ARBDate inDateFrom,
			ARBDate inDateTo,
			bool inHasLifetime) const;
	double ExistingPoints(
			ARBDogExistingPoints::PointType inType,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent,
			ARBDate inDateFrom,
			ARBDate inDateTo) const;
	int NumExistingPointsInVenue(tstring const& inVenue) const;
	int RenameVenue(
			tstring const& inOldVenue,
			tstring const& inNewVenue);
	int DeleteVenue(tstring const& inVenue);
	int NumExistingPointsInDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inDiv) const;
	int RenameDivision(
			tstring const& inVenue,
			tstring const& inOldDiv,
			tstring const& inNewDiv);
	int DeleteDivision(
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
	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
