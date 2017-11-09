/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test BinaryData class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/BinaryData.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(need non-wx support in libarb - currently it asserts)
#else

TEST_CASE("BinaryData")
{
	// This is an small gif file.
	static const size_t RawDataSize = 84;
	static const unsigned char RawData[RawDataSize] =
	{
		 71,  73,  70,  56,  57,  97,   5,   0,   5,   0,
		179,   0,   0,   0,   0,   0, 128,   0,   0,   0,
		128,   0, 128, 128,   0,   0,   0, 128, 128,   0,
		128,   0, 128, 128, 192, 192, 192, 128, 128, 128,
		255,   0,   0,   0, 255,   0, 255, 255,   0,   0,
		  0, 255, 255,   0, 255,   0, 255, 255, 255, 255,
		255,  44,   0,   0,   0,   0,   5,   0,   5,   0,
		  0,   4,   9,  16, 200, 247,  36, 160,  22, 207,
		186,  35,   0,  59
	};
	// compressed, encoded gif file (Note, '\n' is just formatting - but needed to check for equality)
	static const std::wstring EncodedData(L"eJxz93SzsExkZWBl2MwAAg1g3ACmGsCsAwcONDQ0/AcK/Gf4D6b+g1lAoAPSAdLLwMIpcOK7ygKx\n87uUGawBGnIbbQ==");
	// String
	static const std::wstring RawString(L"This is a test of a string");
	// compressed, encoded string
	static const std::wstring EncodedString(L"eJwLycgsVgCiRIWS1OIShfw0IKu4pCgzLx0AeIAJIw==");


	SECTION("RawDecode")
	{
		if (!g_bMicroTest)
		{
			unsigned char* outData = nullptr;
			size_t bytes;
			REQUIRE(BinaryData::Decode(EncodedData, outData, bytes));
			REQUIRE(!!outData);
			REQUIRE(RawDataSize == bytes);
			REQUIRE(0 == memcmp(RawData, outData, bytes));
			BinaryData::Release(outData);
		}
	}


	SECTION("RawEncode")
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			REQUIRE(BinaryData::Encode(RawData, RawDataSize, str));
			REQUIRE(EncodedData == str);
		}
	}


	//TODO: Test Encode(FILE*, std::wstring& outdata)


	SECTION("StringDecode")
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			REQUIRE(BinaryData::DecodeString(EncodedString, str));
			REQUIRE(RawString == str);
		}
	}


	SECTION("StringEncode")
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			REQUIRE(BinaryData::EncodeString(RawString, str));
			REQUIRE(EncodedString == str);
		}
	}

	SECTION("RoundTrip")
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			REQUIRE(BinaryData::EncodeString(RawString, str));
			REQUIRE(EncodedString == str);
			std::wstring str2;
			REQUIRE(BinaryData::DecodeString(str, str2));
			REQUIRE(RawString == str2);
		}
	}
}
#endif
