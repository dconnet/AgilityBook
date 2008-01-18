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

BEGIN_TEST(ConfigLifetimePoints_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigLifetimePointsPtr New();
//	static ARBConfigLifetimePointsPtr New(double inPoints, double inFaults);
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigLifetimePointsPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints const& rhs);
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigLifetimePoints const& rhs) const;
//	bool operator!=(ARBConfigLifetimePoints const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const;
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigLifetimePoints_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigLifetimePointsList_)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(ConfigLifetimePointsList_SortAddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
//	bool AddLifetimePoints(
//			double inPoints,
//			double inFaults,
//			ARBConfigLifetimePointsPtr* outPoints = NULL);
//	bool DeleteLifetimePoints(double inFaults);
}
END_TEST


BEGIN_TEST(ConfigLifetimePointsList_GetLifetimePoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	double GetLifetimePoints(double inFaults) const;
}
END_TEST


BEGIN_TEST(ConfigLifetimePointsList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindLifetimePoints(
//			double inFaults,
//			ARBConfigLifetimePointsPtr* outPoints = NULL) const;
}
END_TEST
