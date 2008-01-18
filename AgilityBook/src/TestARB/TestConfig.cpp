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
 * @brief Test ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfig.h"
#include "Element.h"


FIXTURE(Config)


SETUP(Config)
{
	WIN_ASSERT_TRUE(CommonSetup());
}


TEARDOWN(Config)
{
	WIN_ASSERT_TRUE(CommonTeardown());
}


BEGIN_TESTF(Config_Equality, Config)
{
	ARBConfig config1, config2;
	WIN_ASSERT_EQUAL(config1, config2);
	config1.Default();
	WIN_ASSERT_NOT_EQUAL(config1, config2);
	ARBConfig config3(config1);
	WIN_ASSERT_EQUAL(config1, config3);
}
END_TESTF


BEGIN_TESTF(Config_Clear, Config)
{
	ARBConfig config1, config2;
	config1.Default();
	WIN_ASSERT_NOT_EQUAL(config1, config2);
	config1.clear();
	WIN_ASSERT_EQUAL(config1, config2);
}
END_TESTF


BEGIN_TEST(Config_LoadFault)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool LoadFault(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(Config_LoadOtherPoints)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
//	bool LoadOtherPoints(
//			ElementNodePtr inTree,
//			ARBVersion const& inVersion,
//			ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(Config_Load)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Load(
	//		ElementNodePtr inTree,
	//		ARBVersion const& inVersion,
	//		ARBErrorCallback& ioCallback);
}
END_TEST


BEGIN_TEST(Config_Save)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Save(ElementNodePtr ioTree) const;
}
END_TEST


BEGIN_TESTF(Config_Default, Config)
{
	ARBConfig config;
	WIN_ASSERT_EQUAL(0u, config.GetVenues().size());
	config.Default();
	WIN_ASSERT_EQUAL(13u, config.GetVenues().size());
}
END_TESTF


BEGIN_TEST(Config_GetDTD)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//static std::string GetDTD(bool bNormalizeCRNL = true);
}
END_TEST


BEGIN_TEST(Config_GetTitleNiceName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//tstring GetTitleNiceName(
	//		tstring const& inVenue,
	//		tstring const& inTitle) const;
}
END_TEST


BEGIN_TEST(Config_GetTitleCompleteName)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//tstring GetTitleCompleteName(
	//		ARBDogTitlePtr inTitle,
	//		bool bAbbrevFirst = true) const;
}
END_TEST


BEGIN_TEST(Config_Update)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Update(
	//		int indent,
	//		ARBConfig const& inConfigNew,
	//		otstringstream& ioInfo);
}
END_TEST
