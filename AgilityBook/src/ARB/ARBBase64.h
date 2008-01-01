#pragma once

/*
 * Copyright © 2004-2008 David Connet. All Rights Reserved.
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
 * Note, we are using tstring's mainly for consistency.
 * The actual contained data is still 8bit chars.
 *
 * Revision History
 * @li 2004-03-06 DRC Created
 */

#include "ARBTypes.h"

class ARBBase64
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
			tstring const& inBase64,
			char*& outBinData,
			size_t& outBytes);

	/**
	 * Deallocate memory allocated in Decode()
	 * @param inBinData Binary data allocated in Decode()
	 */
	static void Release(char*& inBinData);

	/**
	 * Encode data
	 * @param inBinData Buffer to encode
	 * @param inBytes Length of inBinData
	 * @param outData Encoded data
	 * @return Success
	 */
	static bool Encode(
			char const* inBinData,
			size_t inBytes,
			tstring& outData);
};
