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
 * @li 2008-01-12 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBTypes.h"

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
			CHECK(wxT("1.2") == v.str());
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
}
