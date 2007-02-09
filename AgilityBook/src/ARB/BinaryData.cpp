/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief Base64/zlib encoding/decoding class.
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-03 DRC Created
 */

#include "stdafx.h"
#include "BinaryData.h"

#include "Base64.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHUNK	131072

BinaryData::BinaryData()
{
}

bool BinaryData::Decode(
		ARBString const& inBase64,
		char*& outBinData,
		size_t& outBytes)
{
	outBinData = NULL;
	outBytes = 0;

	char* pData;
	size_t len;
	if (!Base64::Decode(inBase64, pData, len))
		return false;

	z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    if (Z_OK != inflateInit(&strm))
	{
		Base64::Release(pData);
        return false;
	}

	char out[CHUNK];
	strm.avail_in = static_cast<uInt>(len);
	strm.next_in = reinterpret_cast<Bytef*>(pData);
	do
	{
        strm.avail_out = CHUNK;
        strm.next_out = (Bytef*)out;
        int ret = inflate(&strm, Z_NO_FLUSH);
        ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret)
		{
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&strm);
			Base64::Release(pData);
            return false;
        }
        size_t have = CHUNK - strm.avail_out;
		if (outBinData)
		{
			char* tmp = new char[have + outBytes];
			memcpy(tmp, outBinData, outBytes);
			memcpy(tmp + outBytes, out, have);
			outBytes += have;
			Release(outBinData);
			outBinData = tmp;
		}
		else
		{
			outBinData = new char[have];
			outBytes = have;
			memcpy(outBinData, out, have);
		}
	} while (strm.avail_out == 0);
    inflateEnd(&strm);
	Base64::Release(pData);

	return true;
}

void BinaryData::Release(char* inBinData)
{
	delete [] inBinData;
}

bool BinaryData::Encode(
		char const* inBinData,
		size_t inBytes,
		ARBString& outBase64)
{
	outBase64.empty();

	char out[CHUNK];
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (Z_OK != deflateInit(&strm, Z_DEFAULT_COMPRESSION))
        return false;

	size_t nData = 0;
	char* pData = NULL;
    strm.avail_in = (uInt)inBytes;
    strm.next_in = (Bytef*)inBinData;
    do
	{
        strm.avail_out = CHUNK;
        strm.next_out = (Bytef*)out;
        /*int ret =*/ deflate(&strm, Z_FINISH);    /* no bad return value */
        //ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
        size_t have = CHUNK - strm.avail_out;
		if (pData)
		{
			char* tmp = new char[have + nData];
			memcpy(tmp, pData, nData);
			memcpy(tmp + nData, out, have);
			nData += have;
			delete [] pData;
			pData = tmp;
		}
		else
		{
			pData = new char[have];
			nData = have;
			memcpy(pData, out, have);
		}
    } while (strm.avail_out == 0);
    deflateEnd(&strm);

	bool bOk = Base64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}

bool BinaryData::Encode(
		FILE* inData,
		ARBString& outBase64)
{
	outBase64.empty();

	char in[CHUNK];
	char out[CHUNK];
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (Z_OK != deflateInit(&strm, Z_DEFAULT_COMPRESSION))
        return false;

	int flush;
	size_t nData = 0;
	char* pData = NULL;
	do
	{
		strm.avail_in = (uInt)fread(in, 1, CHUNK, inData);
		if (ferror(inData))
		{
			deflateEnd(&strm);
			return false;
		}
		flush = feof(inData) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = (Bytef*)in;

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = (Bytef*)out;
			/*int ret =*/ deflate(&strm, flush);    /* no bad return value */
			//ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
			size_t have = CHUNK - strm.avail_out;
			if (pData)
			{
				char* tmp = new char[have + nData];
				memcpy(tmp, pData, nData);
				memcpy(tmp + nData, out, have);
				nData += have;
				delete [] pData;
				pData = tmp;
			}
			else
			{
				pData = new char[have];
				nData = have;
				memcpy(pData, out, have);
			}
		} while (strm.avail_out == 0);
	} while (flush != Z_FINISH);
    deflateEnd(&strm);

	bool bOk = Base64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}

bool BinaryData::DecodeString(
		ARBString const& inBase64,
		ARBString& outData)
{
	char* data;
	size_t len;
	if (!BinaryData::Decode(inBase64, data, len))
		return false;
	// TODO: Better conversion
#ifdef UNICODE
	CString tmp(data, (int)len);
	outData = (LPCTSTR)tmp;
#else
	outData = ARBString(data, len);
#endif
	Release(data);
	return true;
}

bool BinaryData::EncodeString(
		ARBString const& inData,
		ARBString& outBase64)
{
	outBase64.empty();
	if (inData.empty())
		return false;
	size_t len = inData.length() + 1;
	// TODO: Better conversion
#ifdef UNICODE
	CStringA tmp(inData.c_str());
	return BinaryData::Encode((LPCSTR)tmp, len, outBase64);
#else
	return BinaryData::Encode(inData.c_str(), len, outBase64);
#endif
}
