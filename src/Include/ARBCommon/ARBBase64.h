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


namespace ARBBase64
{
/**
 * Decode a base64 string.
 * @param inBase64 Encoded buffer
 * @param outBinData Decoded buffer
 * @return Success
 */
ARBCOMMON_API bool Decode(std::wstring const& inBase64, std::vector<unsigned char>& outBinData);

/**
 * Encode data
 * @param inBinData Buffer to encode
 * @param outData Encoded data
 * @return Success
 */
ARBCOMMON_API bool Encode(std::vector<unsigned char> const& inBinData, std::wstring& outData);
} // namespace ARBBase64
