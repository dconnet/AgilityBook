/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test MD5 class
 * @author David Connet
 *
 * Revision History
 * @li 2010-02-07 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBMsgDigest.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestMsgDigest)
{
	// String
	static char const* RawString = "This is a test of a string";
	static std::wstring DigestString(L"b36b52c4835d88fdb548087b9a829bf1");


	TEST(RawDecode)
	{
		if (!g_bMicroTest)
		{
			std::wstring digest = ARBMsgDigest::ComputeBuffer(RawString, strlen(RawString));
			CHECK(digest == DigestString);
		}
	}


	TEST(RawDecodeStream)
	{
		if (!g_bMicroTest)
		{
			std::stringstream str(RawString);
			std::wstring digest = ARBMsgDigest::ComputeFile(str);
			CHECK(digest == DigestString);
		}
	}
}
