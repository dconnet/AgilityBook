#pragma once

/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * Revision History
 * @li 2004-03-06 DRC Created
 */

#include <string>

class Base64
{
public:
	Base64(bool bDeleteDecodeBuffer = true);
	~Base64();

	/**
	 * Decode a base64 string.
	 * @param inBuffer Encoded buffer
	 * @param outBuffer Decoded buffer
	 * @param outLength Length of decoded data
	 * @return Success
	 * @note User must call "delete[]" on outBuffer if bDeleteDecodeBuffer is false.
	 */
	bool Decode(
			std::string const& inBuffer,
			char*& outBuffer,
			size_t& outLength);

	/**
	 * Encode data
	 * @param inBuffer Buffer to encode
	 * @param inLength Length of inBuffer
	 * @return Encoded data
	 */
	std::string Encode(
			char const* inBuffer,
			size_t inLength);

private:
	void SetBuffer(char* inBuffer);
	bool m_DeleteDecodeBuffer;
	char* m_DecodeBuffer;
};
