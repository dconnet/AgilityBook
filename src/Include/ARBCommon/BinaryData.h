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
	 * @param outBytes Number of bytes in outBinData
	 * @post outBinData must be deallocated using Release()
	 */
	ARBCOMMON_API bool Decode(
			std::wstring const& inBase64,
			unsigned char*& outBinData,
			size_t& outBytes);

	/**
	 * Deallocate memory allocated in Decode()
	 * @param inBinData Binary data allocated in Decode()
	 */
	ARBCOMMON_API void Release(unsigned char* inBinData);

	/**
	 * Compress and base64 encode a chunk of data.
	 * @param inBinData Data to encode.
	 * @param inBytes Number of bytes in inBinData
	 * @param outBase64 Base64 encoded string of compressed (zlib) data.
	 */
	ARBCOMMON_API bool Encode(
			unsigned char const* inBinData,
			size_t inBytes,
			std::wstring& outBase64);
	ARBCOMMON_API bool EncodeFile(
			std::wstring const& inFileName,
			std::wstring& outBase64);

	ARBCOMMON_API bool DecodeString(
			std::wstring const& inBase64,
			std::wstring& outData);
	ARBCOMMON_API bool EncodeString(
			std::wstring const& inData,
			std::wstring& outBase64);
};
