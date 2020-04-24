#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base64/zlib encoding/decoding class.
 * @author David Connet
 *
 * Note, we are using std::wstring's mainly for consistency.
 * The actual contained data is still 8bit chars.
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-01-03 Created
 */

#include "ARBTypes.h"
#include "LibwxARBCommon.h"


namespace BinaryData
{
/**
 * Decode base64 and decompress data.
 * @param inBase64 Base64 encoded data
 * @param outBinData Decoded/decompressed data
 * @post outBinData must be deallocated using Release()
 */
ARBCOMMON_API bool Decode(std::wstring const& inBase64, std::vector<unsigned char>& outBinData);

/**
 * Compress and base64 encode a chunk of data.
 * @param inBinData Data to encode.
 * @param outBase64 Base64 encoded string of compressed (zlib) data.
 */
ARBCOMMON_API bool Encode(std::vector<unsigned char> const& inBinData, std::wstring& outBase64);
ARBCOMMON_API bool EncodeFile(std::wstring const& inFileName, std::wstring& outBase64);

ARBCOMMON_API bool DecodeString(std::wstring const& inBase64, std::wstring& outData);
ARBCOMMON_API bool EncodeString(std::wstring const& inData, std::wstring& outBase64);
}; // namespace BinaryData
