/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base64 encoding/decoding class.
 * @author David Connet
 *
 * Base on code found on www.codeproject.com:
 *  http://www.codeproject.com/string/ammimeutils.asp
 *  AMMimeUtils, posted 3/23/2001 by Anders Molin
 *
 * Note: The original code wrote lines with "\r\n". We don't.
 *
 * Revision History
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-12 DRC Encoding/decoding 0 bytes should fail.
 * @li 2004-04-27 DRC Decoding: Did not properly compute the data length.
 *                    Encoding: Problem encoding 1 and 2 byte buffers.
 * @li 2004-03-06 DRC Created
 */

#include "stdafx.h"
#include "ARBCommon/ARBBase64.h"

#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static unsigned int const MaxLineLength = 76;

static char const base64chars[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', // 0-7
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 8
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', // 16
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', // 24
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', // 32
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', // 40
	'w', 'x', 'y', 'z', '0', '1', '2', '3', // 48
	'4', '5', '6', '7', '8', '9', '+', '/'  // 56
};

#define SKIP '\202'

static char const base64map[] =
{
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, 62,   SKIP, SKIP, SKIP, 63,
	52,   53,   54,   55,   56,   57,   58,   59,
	60,   61,   SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, 0,    1 ,   2 ,   3 ,   4 ,   5 ,   6 ,
	7,    8 ,   9 ,   10,   11,   12,   13,   14,
	15,   16,   17,   18,   19,   20,   21,   22,
	23,   24,   25,   SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, 26,   27,   28,   29,   30,   31,   32,
	33,   34,   35,   36,   37,   38,   39,   40,
	41,   42,   43,   44,   45,   46,   47,   48,
	49,   50,   51,   SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
	SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP
};

/////////////////////////////////////////////////////////////////////////////

ARBBase64::ARBBase64()
{
}


bool ARBBase64::Decode(
		std::wstring const& inBase64,
		unsigned char*& outBinData,
		size_t& outBytes)
{
	outBinData = NULL;
	outBytes = 0;
	if (inBase64.empty())
		return false;

	size_t bufsize = inBase64.length();
	outBinData = new unsigned char[bufsize];
	unsigned char* result = outBinData;

	int std = 0;
	size_t count = 1;
	size_t nChar = 0;

	while (count <= bufsize && inBase64[nChar] != '=')
	{
		//check to see if it's a legal base64 char...
		while (SKIP == base64map[inBase64[nChar]])
		{
			if (inBase64[nChar] != '\r' && inBase64[nChar] != '\n')
			{
				// error in the encoded data.
				delete [] outBinData;
				outBinData = NULL;
				return false;
			}
			++nChar;
			--bufsize;
			if (count >= bufsize)
				break;
		}

		//add the base64 char to std...
		std |= base64map[inBase64[nChar++] & 0xFF];
		std <<= 6;
		if (count % 4 == 0) //we have 3 more real chars...
		{
			//put std in the next 3 chars in finalresult
			int tmp;
			std >>= 6;
			tmp = std;
			tmp >>= 16;
			tmp &= 0xFF;
			*(result++) = static_cast<unsigned char>(tmp);
			tmp = std;
			tmp >>= 8;
			tmp &= 0xFF;
			*(result++) = static_cast<unsigned char>(tmp);
			tmp = std;
			tmp &= 0xFF;
			*(result++) = static_cast<unsigned char>(tmp);
			std = 0; //empty std
			outBytes += 3;
		}
		++count;
	}

	//find and decode the remaining chars, if any...
	--count;
	if (count % 4 != 0)
	{
		// pad out the buffer
		for (size_t i = 0; i < 4 - (count % 4); ++i)
			std <<= 6;
		size_t nBits = (count % 4) * 6;
		outBytes += nBits / 8;
		int tmp;
		std >>= 6;
		tmp = std;
		tmp >>= 16;
		tmp &= 0xFF;
		*(result++) = static_cast<unsigned char>(tmp);
		tmp = std;
		tmp >>= 8;
		tmp &= 0xFF;
		*(result++) = static_cast<unsigned char>(tmp);
		tmp = std;
		tmp &= 0xFF;
		*(result++) = static_cast<unsigned char>(tmp);
	}
	return true;
}


void ARBBase64::Release(unsigned char*& inBinData)
{
	delete [] inBinData;
	inBinData = NULL;
}


bool ARBBase64::Encode(
		unsigned char const* inBinData,
		size_t inBytes,
		std::wstring& outData)
{
	outData.erase();
	if (0 == inBytes || !inBinData || !*inBinData)
		return false;
	unsigned char* encoded = NULL;
	size_t alsize = (inBytes * 4) / 3;
	encoded = new unsigned char[alsize + ((alsize / 76) * 2) + 10];
	size_t count = 0;
	size_t LineLen = 0;
	unsigned char* fresult = encoded;
	unsigned char const* s = inBinData;
	int tmp = 0;
	//let's step through the buffer and encode it...
	if (inBytes > 2) // A really small buffer causes problems with the logic below
	{
		while (count <= inBytes)
		{
			if (count % 3 == 0 && count != 0)
			{
				tmp >>= 8;
				tmp &= 0xFFFFFF;
				//we have 4 new b64 chars, add them to encoded
				int mid = tmp;
				mid >>= 18;
				mid &= 0x3F;
				*(fresult++) = base64chars[mid];
				++LineLen;
				mid = tmp;
				mid >>= 12;
				mid &= 0x3F;
				*(fresult++) = base64chars[mid];
				++LineLen;
				mid = tmp;
				mid >>= 6;
				mid &= 0x3F;
				*(fresult++) = base64chars[mid];
				++LineLen;
				mid = tmp;
				mid &= 0x3F;
				*(fresult++) = base64chars[mid];
				++LineLen;
				//reset tmp
				tmp = 0;
				// Should we break the line?
				if (LineLen >= MaxLineLength)
				{
					*(fresult++) = '\n';
					LineLen = 0;
				}
				if (inBytes - count < 3)
					break;
			}
			unsigned char mid = (256 - (0 - *s)) & 0xff;
			tmp |= mid;
			tmp <<= 8;
			++count;
			++s;
		}
	}
	//do we have some chars left...
	size_t rest = (inBytes - count) % 3;
	if (rest != 0)
	{
		tmp = 0;
		for (size_t i = 0; i < 3; ++i)
		{
			if (i < rest)
			{
				unsigned char mid = (256 - (0 - *s)) & 0xff;
				tmp |= mid;
				tmp <<= 8;
				++count;
				++s;
			}
			else
			{
				tmp |= 0;
				tmp <<= 8;
			}
		}
		tmp >>= 8;
		tmp &= 0xFFFFFF;
		//we have some new b64 chars, add them to encoded
		int mid = tmp;
		if (rest >= 1)
		{
			mid >>= 18;
			mid &= 0x3F;
			*(fresult++) = base64chars[mid];
			mid = tmp;
			mid >>= 12;
			mid &= 0x3F;
			*(fresult++) = base64chars[mid];
		}
		if (rest >= 2)
		{
			mid = tmp;
			mid >>= 6;
			mid &= 0x3F;
			*(fresult++) = base64chars[mid];
		}
		if (rest >= 3)
		{
			mid = tmp;
			mid &= 0x3F;
			*(fresult++) = base64chars[mid];
		}
		for (size_t c = 3; c > rest; --c)
		{
			*(fresult++) = '=';
		}
	}
	*(fresult++) = 0;
	bool bOk = false;
	if (encoded)
	{
		bOk = true;
		char const* signedChar = reinterpret_cast<char*>(encoded);
		outData = StringUtil::stringW(signedChar, strlen(signedChar));
		delete [] encoded;
	}
	return bOk;
}
