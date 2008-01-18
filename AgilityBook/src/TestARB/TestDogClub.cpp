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
 * @brief Test ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogClub.h"
#include "Element.h"

#if 0
class ARBDogClub : public ARBBase
{
	static ARBDogClubPtr New();
	ARBDogClubPtr Clone() const;
	ARBDogClub& operator=(ARBDogClub const& rhs);
	bool operator==(ARBDogClub const& rhs) const;
	bool operator!=(ARBDogClub const& rhs) const
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
};

class ARBDogClubList : public ARBVector<ARBDogClubPtr>
{
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;
	tstring GetPrimaryClubName() const;
	tstring GetPrimaryClubVenue() const;
	bool FindEvent(
			ARBConfig const& inConfig,
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			ARBErrorCallback& ioCallback,
			ARBConfigEventPtr* outEvent = NULL,
			ARBConfigScoringPtr* outScoring = NULL) const;
	bool FindVenue(
			tstring const& inVenue,
			ARBDogClubPtr* outClub = NULL) const;
	bool AddClub(
			tstring const& inName,
			tstring const& inVenue,
			ARBDogClubPtr* outClub = NULL);
	bool DeleteClub(
			tstring const& inName,
			tstring const& inVenue);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
