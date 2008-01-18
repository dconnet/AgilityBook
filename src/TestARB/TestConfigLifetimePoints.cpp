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
 * @brief Test ARBConfigLifetimePoints and ARBConfigLifetimePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigLifetimePoints.h"
#include "Element.h"

#if 0
class ARBConfigLifetimePoints : public ARBBase
{
	static ARBConfigLifetimePointsPtr New();
	static ARBConfigLifetimePointsPtr New(double inPoints, double inFaults);
	ARBConfigLifetimePointsPtr Clone() const;
	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints const& rhs);
	bool operator==(ARBConfigLifetimePoints const& rhs) const;
	bool operator!=(ARBConfigLifetimePoints const& rhs) const
	virtual tstring GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
};

class ARBConfigLifetimePointsList : public ARBVector<ARBConfigLifetimePointsPtr>
{
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	void sort();
	double GetLifetimePoints(double inFaults) const;
	bool FindLifetimePoints(
			double inFaults,
			ARBConfigLifetimePointsPtr* outPoints = NULL) const;
	bool AddLifetimePoints(
			double inPoints,
			double inFaults,
			ARBConfigLifetimePointsPtr* outPoints = NULL);
	bool DeleteLifetimePoints(double inFaults);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
