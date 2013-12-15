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


SUITE(TestVersion)
{
	TEST(ctor)
	{
		if (!g_bMicroTest)
		{
			unsigned short maj = 0, minor = 0;
			ARBVersion v;
			CHECK(maj == v.Major());
			CHECK(minor == v.Minor());

			maj = 1;
			minor = 2;
			ARBVersion v2(maj, minor);
			CHECK(maj == v2.Major());
			CHECK(minor == v2.Minor());
		}
	}


	TEST(str)
	{
		if (!g_bMicroTest)
		{
			ARBVersion v(1, 2);
			CHECK(L"1.2" == v.str());
		}
	}


	TEST(Comparison)
	{
		if (!g_bMicroTest)
		{
			ARBVersion v1(1, 2);
			ARBVersion v2(2, 1);
			CHECK(v1 < v2);
		}
	}


	TEST(Bounds)
	{
		if (!g_bMicroTest)
		{
			unsigned short maj = 0xefff;
			unsigned short min = 0xeffe;
			ARBVersion v1(maj, min);
			CHECK(v1.Major() == maj);
			CHECK(v1.Minor() == min);
		}
	}

	TEST(VerParse)
	{
		if (!g_bMicroTest)
		{
			CVersionNum v(false);
			CHECK(v.Parse(L"1.2.3.4"));
			CVersionNum::VERSION_NUMBER ver;
			v.GetVersion(ver);
			CHECK(ver.part1 == 1);
			CHECK(ver.part2 == 2);
			CHECK(ver.part3 == 3);
			CHECK(ver.part4 == 4);
		}
	}
}
