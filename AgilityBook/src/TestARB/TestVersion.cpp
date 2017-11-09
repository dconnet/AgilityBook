/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBVersion class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2012-07-30 Add CVersionNum tests.
 * 2011-08-22 ARBVersion was only using 16 instead of 32bits.
 * 2008-01-12 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBTypes2.h"
#include "ARBCommon/VersionNum.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("Version")
{
	SECTION("ctor")
	{
		if (!g_bMicroTest)
		{
			unsigned short maj = 0, minor = 0;
			ARBVersion v;
			REQUIRE(maj == v.Major());
			REQUIRE(minor == v.Minor());

			maj = 1;
			minor = 2;
			ARBVersion v2(maj, minor);
			REQUIRE(maj == v2.Major());
			REQUIRE(minor == v2.Minor());
		}
	}


	SECTION("str")
	{
		if (!g_bMicroTest)
		{
			ARBVersion v(1, 2);
			REQUIRE(L"1.2" == v.str());
		}
	}


	SECTION("Comparison")
	{
		if (!g_bMicroTest)
		{
			ARBVersion v1(1, 2);
			ARBVersion v2(2, 1);
			REQUIRE(v1 < v2);
		}
	}


	SECTION("Bounds")
	{
		if (!g_bMicroTest)
		{
			unsigned short maj = 0xefff;
			unsigned short min = 0xeffe;
			ARBVersion v1(maj, min);
			REQUIRE(v1.Major() == maj);
			REQUIRE(v1.Minor() == min);
		}
	}

	SECTION("VerParse")
	{
		if (!g_bMicroTest)
		{
			CVersionNum v(false);
			REQUIRE(v.Parse(L"1.2.3.4"));
			CVersionNum::VERSION_NUMBER ver;
			v.GetVersion(ver);
			REQUIRE(ver.part1 == 1);
			REQUIRE(ver.part2 == 2);
			REQUIRE(ver.part3 == 3);
			REQUIRE(ver.part4 == 4);
		}
	}
}
