#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Base64/zlib encoding/decoding class.
 * @author David Connet
 *
 * Note, we are using wxString's mainly for consistency.
 * The actual contained data is still 8bit chars.
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2007-01-03 DRC Created
 */

#include "ARBTypes.h"
class wxFFile;


class BinaryData
{
	BinaryData();
public:
	/**
	 * Decode base64 and decompress data.
	 * @param inBase64 Base64 encoded data
	 * @param outBinData Decoded/decompressed data
	 * @param outBytes Number of bytes in outBinData
	 * @post outBinData must be deallocated using Release()
	 */
	static bool Decode(
			wxString const& inBase64,
			unsigned char*& outBinData,
			size_t& outBytes);

	/**
	 * Deallocate memory allocated in Decode()
	 * @param inBinData Binary data allocated in Decode()
	 */
	static void Release(unsigned char* inBinData);

	/**
	 * Compress and base64 encode a chunk of data.
	 * @param inBinData Data to encode.
	 * @param inBytes Number of bytes in inBinData
	 * @param outBase64 Base64 encoded string of compressed (zlib) data.
	 */
	static bool Encode(
			unsigned char const* inBinData,
			size_t inBytes,
			wxString& outBase64);
	static bool Encode(
			wxFFile& inData,
			wxString& outBase64);

	static bool DecodeString(
			wxString const& inBase64,
			wxString& outData);
	static bool EncodeString(
			wxString const& inData,
			wxString& outBase64);
};
