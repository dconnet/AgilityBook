/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @li 2004-04-27 DRC Decoding: Did not properly compute the data length.
 *                    Encoding: Problem encoding 1 and 2 byte buffers.
 * @li 2004-03-06 DRC Created
 */

#include "stdafx.h"
#include "Base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

Base64::Base64(bool bDeleteDecodeBuffer)
	: m_DeleteDecodeBuffer(bDeleteDecodeBuffer)
	, m_DecodeBuffer(NULL)
{
}

Base64::~Base64()
{
	SetBuffer(NULL);
}

void Base64::SetBuffer(char* inBuffer)
{
	if (m_DeleteDecodeBuffer)
	{
		delete [] m_DecodeBuffer;
		m_DecodeBuffer = inBuffer;
	}
}

bool Base64::Decode(std::string const& inBuffer, char*& outBuffer, size_t& outLength)
{
	outBuffer = NULL;
	outLength = 0;
	if (0 == inBuffer.length())
		return false;

	size_t bufsize = inBuffer.length();
	outBuffer = new char[bufsize];
	SetBuffer(outBuffer);
	char *result = outBuffer;

	int std = 0;
	size_t count = 1;
	size_t nChar = 0;

	while (inBuffer[nChar] != '=' && count <= bufsize)
	{
		//check to see if it's a legal base64 char...
		while (SKIP == base64map[inBuffer[nChar]])
		{
			if (inBuffer[nChar] != '\r' && inBuffer[nChar] != '\n')
			{
				// error in the encoded data.
				delete [] outBuffer;
				outBuffer = NULL;
				m_DecodeBuffer = NULL;
				return false;
			}
			++nChar;
			--bufsize;
			if (count >= bufsize)
				break;
		}

		//add the base64 char to std...
		std |= base64map[inBuffer[nChar++] & 0xFF];
		std <<= 6;
		if (count % 4 == 0) //we have 3 more real chars...
		{
			//put std in the next 3 chars in finalresult
			int tmp;
			std >>= 6;
			tmp = std;
			tmp >>= 16;
			tmp &= 0xFF;
			*(result++) = static_cast<char>(tmp);
			tmp = std;
			tmp >>= 8;
			tmp &= 0xFF;
			*(result++) = static_cast<char>(tmp);
			tmp = std;
			tmp &= 0xFF;
			*(result++) = static_cast<char>(tmp);
			std = 0; //empty std
			outLength += 3;
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
		outLength += nBits / 8;
		int tmp;
		std >>= 6;
		tmp = std;
		tmp >>= 16;
		tmp &= 0xFF;
		*(result++) = static_cast<char>(tmp);
		tmp = std;
		tmp >>= 8;
		tmp &= 0xFF;
		*(result++) = static_cast<char>(tmp);
		tmp = std;
		tmp &= 0xFF;
		*(result++) = static_cast<char>(tmp);
	}
	return true;
}

std::string Base64::Encode(char const* inBuffer, size_t inLength)
{
	char *encoded = NULL;
	if (inBuffer != NULL && 0 < inLength)
	{
		size_t alsize = (inLength * 4) / 3;
		encoded = new char[alsize + ((alsize / 76) * 2) + 10];
		size_t count = 0;
		size_t LineLen = 0;
		char* fresult = encoded;
		char const *s = inBuffer;
		int tmp = 0;
		//let's step through the buffer and encode it...
		if (inLength > 2) // A really small buffer causes problems with the logic below
		{
			while (count <= inLength)
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
					if (inLength - count < 3)
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
		size_t rest = (inLength - count) % 3;
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
	}
	std::string result;
	if (encoded)
	{
		result = encoded;
		delete [] encoded;
	}
	return result;
}
