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
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2010-02-07 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/ARBMsgDigest.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("MsgDigest")
{
	// String
	static char const* RawString = "This is a test of a string";
	static std::wstring DigestStringMD5(L"b36b52c4835d88fdb548087b9a829bf1");
	static std::wstring DigestStringSHA1(L"7d56b53d54b141cb77e465abcfe63436a35ae222");
	static std::wstring DigestStringSHA256(L"5d2819684143b99d8b9c9a254e1b5584529a3fe947862e8ae15e246eda292c37");


	SECTION("RawDecode")
	{
		if (!g_bMicroTest)
		{
			std::stringstream buffer(RawString);
			std::wstring digest = ARBMsgDigest::Compute(buffer, ARBMsgDigest::ARBDigest::MD5);
			REQUIRE(digest == DigestStringMD5);
		}
	}


	SECTION("RawDecodeSha1")
	{
		if (!g_bMicroTest)
		{
			std::stringstream buffer(RawString);
			std::wstring digest = ARBMsgDigest::Compute(buffer, ARBMsgDigest::ARBDigest::SHA1);
			REQUIRE(digest == DigestStringSHA1);
		}
	}


	SECTION("RawDecodeSha256")
	{
		if (!g_bMicroTest)
		{
			std::stringstream buffer(RawString);
			std::wstring digest = ARBMsgDigest::Compute(buffer, ARBMsgDigest::ARBDigest::SHA256);
			REQUIRE(digest == DigestStringSHA256);
		}
	}
}
