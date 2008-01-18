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
 * @brief Test ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigVenue.h"
#include "Element.h"

#if 0
class ARBConfigVenue : public ARBBase
{
	static ARBConfigVenuePtr New();
	ARBConfigVenuePtr Clone() const;
	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
	bool operator==(ARBConfigVenue const& rhs) const;
	bool operator!=(ARBConfigVenue const& rhs) const
	void clear();
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ARBConfig& ioConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	bool Update(
			int indent,
			ARBConfigVenuePtr inVenueNew,
			tstring& ioInfo);
};

class ARBConfigVenueList : public ARBVector<ARBConfigVenuePtr>
{
	bool Load(
			ARBConfig& ioConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	void sort();
	bool VerifyVenue(tstring const& inVenue) const
	bool VerifyMultiQ(
			tstring const& inVenue,
			tstring const& inMultiQ,
			bool inUseShortName = false) const;
	bool VerifyLevel(
			tstring const& inVenue,
			tstring const& inDivision,
			tstring const& inLevel) const;
	bool VerifyEvent(
			tstring const& inVenue,
			tstring const& inDivision,
			tstring const& inLevel,
			tstring const& inEvent,
			ARBDate const& inDate) const;
	bool FindTitleCompleteName(
			tstring const& inVenue,
			tstring const& inName,
			bool bShowInstance,
			bool bAbbrevFirst = true,
			ARBConfigTitlePtr* outTitle = NULL) const;
	bool FindTitle(
			tstring const& inVenue,
			tstring const& inTitle,
			ARBConfigTitlePtr* outTitle = NULL) const;
	bool DeleteTitle(tstring const& inTitle);
	bool FindVenue(
			tstring const& inVenue,
			ARBConfigVenuePtr* outVenue = NULL) const;
	bool AddVenue(
			tstring const& inVenue,
			ARBConfigVenuePtr* outVenue = NULL);
	bool AddVenue(ARBConfigVenuePtr inVenue);
	int DeleteVenue(tstring const& inVenue);
	bool FindEvent(
			tstring const& inVenue,
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigEventPtr* outEvent = NULL,
			ARBConfigScoringPtr* outScoring = NULL) const;
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
