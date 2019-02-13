#pragma once

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
 * Note, we are using std::wstring's mainly for consistency.
 * The actual contained data is still 8bit chars.
 *
 * Revision History
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2004-03-06 Created
 */

#include "ARBTypes.h"
#include "LibwxARBCommon.h"


class ARBCOMMON_API ARBBase64
{
	ARBBase64();
public:
	/**
	 * Decode a base64 string.
	 * @param inBase64 Encoded buffer
	 * @param outBinData Decoded buffer
	 * @param outBytes Length of decoded data
	 * @return Success
	 * @note User must call Release() on outBinData
	 */
	static bool Decode(
			std::wstring const& inBase64,
			unsigned char*& outBinData,
			size_t& outBytes);

	/**
	 * Deallocate memory allocated in Decode()
	 * @param inBinData Binary data allocated in Decode()
	 */
	static void Release(unsigned char*& inBinData);

	/**
	 * Encode data
	 * @param inBinData Buffer to encode
	 * @param inBytes Length of inBinData
	 * @param outData Encoded data
	 * @return Success
	 */
	static bool Encode(
			unsigned char const* inBinData,
			size_t inBytes,
			std::wstring& outData);
};
