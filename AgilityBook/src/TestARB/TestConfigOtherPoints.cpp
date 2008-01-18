/*
 * Copyright � 2008 David Connet. All Rights Reserved.
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
 * @brief Test ARBConfigOtherPoints and ARBConfigOtherPointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigOtherPoints.h"
#include "Element.h"

#if 0
class ARBConfigOtherPoints : public ARBBase
{
	static void GetTallyValidValues(std::vector<tstring>& outValues);
	static ARBConfigOtherPointsPtr New();
	ARBConfigOtherPointsPtr Clone() const;
	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);
	bool operator==(ARBConfigOtherPoints const& rhs) const;
	bool operator!=(ARBConfigOtherPoints const& rhs) const
	void clear();
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
};

class ARBConfigOtherPointsList : public ARBVector<ARBConfigOtherPointsPtr>
{
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool VerifyOtherPoints(tstring const& inName) const;
	bool FindOtherPoints(
			tstring const& inName,
			ARBConfigOtherPointsPtr* outPoints = NULL) const;
	bool AddOtherPoints(ARBConfigOtherPointsPtr inOther);
	bool DeleteOtherPoints(tstring const& inName);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
