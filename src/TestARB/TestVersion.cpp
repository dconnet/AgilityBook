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
 * @brief Test ARBVersion class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-12 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBTypes.h"


BEGIN_TEST(Version_ctor)
{
	unsigned short maj = 0, minor = 0;
	ARBVersion v;
	WIN_ASSERT_EQUAL(maj, v.Major());
	WIN_ASSERT_EQUAL(minor, v.Minor());

	maj = 1;
	minor = 2;
	ARBVersion v2(maj, minor);
	WIN_ASSERT_EQUAL(maj, v2.Major());
	WIN_ASSERT_EQUAL(minor, v2.Minor());
}
END_TEST


BEGIN_TEST(Version_str)
{
	ARBVersion v(1, 2);
	WIN_ASSERT_STRING_EQUAL(_T("1.2"), v.str().c_str());
}
END_TEST


BEGIN_TEST(Version_Comparison)
{
	ARBVersion v1(1, 2);
	ARBVersion v2(2, 1);
	WIN_ASSERT_TRUE(v1 < v2);
}
END_TEST
