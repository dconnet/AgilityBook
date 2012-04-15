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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "BinaryData.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestBinaryData)
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
	static const std::wstring EncodedData(wxT("eJxz93SzsExkZWBl2MwAAg1g3ACmGsCsAwcONDQ0/AcK/Gf4D6b+g1lAoAPSAdLLwMIpcOK7ygKx\n87uUGawBGnIbbQ=="));
	// String
	static const std::wstring RawString(wxT("This is a test of a string"));
	// compressed, encoded string
	static const std::wstring EncodedString(wxT("eJwLycgsVgCiRIWS1OIShfw0IKu4pCgzLx0AeIAJIw=="));


	TEST(RawDecode)
	{
		if (!g_bMicroTest)
		{
			unsigned char* outData = NULL;
			size_t bytes;
			CHECK(BinaryData::Decode(EncodedData, outData, bytes));
			CHECK(outData != NULL);
			CHECK_EQUAL(RawDataSize, bytes);
			CHECK_EQUAL(0, memcmp(RawData, outData, bytes));
			BinaryData::Release(outData);
		}
	}


	TEST(RawEncode)
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			CHECK(BinaryData::Encode(RawData, RawDataSize, str));
			CHECK(EncodedData == str);
		}
	}


	//TODO: Test Encode(FILE*, std::wstring& outdata)


	TEST(StringDecode)
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			CHECK(BinaryData::DecodeString(EncodedString, str));
			CHECK(RawString == str);
		}
	}


	TEST(StringEncode)
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			CHECK(BinaryData::EncodeString(RawString, str));
			CHECK(EncodedString == str);
		}
	}

	TEST(RoundTrip)
	{
		if (!g_bMicroTest)
		{
			std::wstring str;
			CHECK(BinaryData::EncodeString(RawString, str));
			CHECK(EncodedString == str);
			std::wstring str2;
			CHECK(BinaryData::DecodeString(str, str2));
			CHECK(RawString == str2);
		}
	}
}
