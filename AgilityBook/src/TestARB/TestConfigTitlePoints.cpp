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
 * @brief Test ARBConfigTitlePoints and ARBConfigTitlePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigTitlePoints.h"
#include "Element.h"


BEGIN_TEST(ConfigTitlePoints_New)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	static ARBConfigTitlePointsPtr New();
//	static ARBConfigTitlePointsPtr New(double inPoints, double inFaults);
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_Clone)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigTitlePointsPtr Clone() const;
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_OpEqual)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints const& rhs);
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_Compare)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool operator==(ARBConfigTitlePoints const& rhs) const;
//	bool operator!=(ARBConfigTitlePoints const& rhs) const
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_GenName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	virtual tstring GetGenericName() const;
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback,
//			ARBConfigLifetimePointsList& ioLifetimePoints);
}
END_TEST


BEGIN_TEST(ConfigTitlePoints_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TEST(ConfigTitlePointsList_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool Load(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback,
//			ARBConfigLifetimePointsList& ioLifetimePoints);
}
END_TEST


BEGIN_TEST(ConfigTitlePointsList_SortAddDelete)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	void sort();
//	bool AddTitlePoints(
//			double inPoints,
//			double inFaults,
//			ARBConfigTitlePointsPtr* outPoints = NULL);
//	bool DeleteTitlePoints(double inFaults);
}
END_TEST


BEGIN_TEST(ConfigTitlePointsList_GetTitlePoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	double GetTitlePoints(double inFaults) const;
}
END_TEST


BEGIN_TEST(ConfigTitlePointsList_Find)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool FindTitlePoints(
//			double inFaults,
//			ARBConfigTitlePointsPtr* outPoints = NULL) const;
}
END_TEST
