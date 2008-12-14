/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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

#include "ARBBase64.h"
#ifdef WXWIDGETS
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/zstream.h>
#else // WXWIDGETS
#include "zlib.h"
#endif // WXWIDGETS

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

#define CHUNK	131072

BinaryData::BinaryData()
{
}


bool BinaryData::Decode(
		tstring const& inBase64,
		unsigned char*& outBinData,
		size_t& outBytes)
{
	outBinData = NULL;
	outBytes = 0;

	unsigned char* pData;
	size_t len;
	if (!ARBBase64::Decode(inBase64, pData, len))
		return false;

#ifdef WXWIDGETS
	wxZlibInputStream strm(new wxMemoryInputStream(pData, len), wxZLIB_ZLIB);
	wxMemoryOutputStream output;
	output.Write(input);
	output.Close();
	outBytes = output.GetSize();
	outBinData = new unsigned char[outBytes];
	output.CopyTo(outBinData, outBytes);

#else // WXWIDGETS
	z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    if (Z_OK != inflateInit(&strm))
	{
		ARBBase64::Release(pData);
        return false;
	}

	// Originally used boost::scoped_ptr, but not available in TR1
	tr1::shared_ptr<unsigned char> out(new unsigned char[CHUNK]);
	strm.avail_in = static_cast<uInt>(len);
	strm.next_in = reinterpret_cast<Bytef*>(pData);
	do
	{
        strm.avail_out = CHUNK;
        strm.next_out = (Bytef*)out.get();
        int ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret)
		{
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&strm);
			ARBBase64::Release(pData);
            return false;
        }
        size_t have = CHUNK - strm.avail_out;
		if (outBinData)
		{
			unsigned char* tmp = new unsigned char[have + outBytes];
			memcpy(tmp, outBinData, outBytes);
			memcpy(tmp + outBytes, out.get(), have);
			outBytes += have;
			Release(outBinData);
			outBinData = tmp;
		}
		else
		{
			outBinData = new unsigned char[have];
			outBytes = have;
			memcpy(outBinData, out.get(), have);
		}
	} while (strm.avail_out == 0);
    inflateEnd(&strm);
#endif // WXWIDGETS

	ARBBase64::Release(pData);

	return true;
}


void BinaryData::Release(unsigned char* inBinData)
{
	delete [] inBinData;
}


bool BinaryData::Encode(
		unsigned char const* inBinData,
		size_t inBytes,
		tstring& outBase64)
{
	outBase64.empty();

	size_t nData = 0;
	unsigned char* pData = NULL;

#ifdef WXWIDGETS
	wxMemoryOutputStream output;
	wxZlibOutputStream strm(output);
	strm.Write(inBinData, inBytes);
	strm.Close();
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

#else // WXWIDGETS
	tr1::shared_ptr<unsigned char> out(new unsigned char[CHUNK]);
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (Z_OK != deflateInit(&strm, Z_DEFAULT_COMPRESSION))
        return false;

    strm.avail_in = (uInt)inBytes;
    strm.next_in = (Bytef*)inBinData;
    do
	{
        strm.avail_out = CHUNK;
        strm.next_out = (Bytef*)out.get();
        /*int ret =*/ deflate(&strm, Z_FINISH);    /* no bad return value */
        //assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        size_t have = CHUNK - strm.avail_out;
		if (pData)
		{
			unsigned char* tmp = new unsigned char[have + nData];
			memcpy(tmp, pData, nData);
			memcpy(tmp + nData, out.get(), have);
			nData += have;
			delete [] pData;
			pData = tmp;
		}
		else
		{
			pData = new unsigned char[have];
			nData = have;
			memcpy(pData, out.get(), have);
		}
    } while (strm.avail_out == 0);
    deflateEnd(&strm);
#endif // WXWIDGETS

	bool bOk = ARBBase64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}


bool BinaryData::Encode(
		FILE* inData,
		tstring& outBase64)
{
	outBase64.empty();

	size_t nData = 0;
	unsigned char* pData = NULL;

#ifdef WXWIDGETS
	wxMemoryOutputStream output;
	wxZlibOutputStream strm(output);
	wxFileInputStream instrm(_fileno(inData));
	strm.Write(instrm);
	strm.Close();
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

#else // WXWIDGETS

	tr1::shared_ptr<unsigned char> in(new unsigned char[CHUNK]);
	tr1::shared_ptr<unsigned char> out(new unsigned char[CHUNK]);
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (Z_OK != deflateInit(&strm, Z_DEFAULT_COMPRESSION))
        return false;

	int flush;
	do
	{
		strm.avail_in = (uInt)fread(in.get(), 1, CHUNK, inData);
		if (ferror(inData))
		{
			deflateEnd(&strm);
			return false;
		}
		flush = feof(inData) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = (Bytef*)in.get();

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = (Bytef*)out.get();
			/*int ret =*/ deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			size_t have = CHUNK - strm.avail_out;
			if (pData)
			{
				unsigned char* tmp = new unsigned char[have + nData];
				memcpy(tmp, pData, nData);
				memcpy(tmp + nData, out.get(), have);
				nData += have;
				delete [] pData;
				pData = tmp;
			}
			else
			{
				pData = new unsigned char[have];
				nData = have;
				memcpy(pData, out.get(), have);
			}
		} while (strm.avail_out == 0);
	} while (flush != Z_FINISH);
    deflateEnd(&strm);
#endif // WXWIDGETS

	bool bOk = ARBBase64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}


bool BinaryData::DecodeString(
		tstring const& inBase64,
		tstring& outData)
{
	unsigned char* data;
	size_t len;
	if (!BinaryData::Decode(inBase64, data, len))
		return false;
	// TODO: Better conversion
#ifdef UNICODE
	outData = tstringUtil::Convert(reinterpret_cast<char*>(data), len);
#else
	outData = tstring(reinterpret_cast<char*>(data), len);
#endif
	Release(data);
	return true;
}


bool BinaryData::EncodeString(
		tstring const& inData,
		tstring& outBase64)
{
	outBase64.empty();
	if (inData.empty())
		return false;
	// Do not include the null terminator. Otherwise decoding includes it into
	// the output string - which when streamed, then includes the null. Which
	// in an ostringstream, terminates the string on output of the stream.
	// TODO: Better conversion
#ifdef UNICODE
	std::string tmp(tstringUtil::Convert(inData));
	return BinaryData::Encode(reinterpret_cast<unsigned char const*>(tmp.c_str()), tmp.length(), outBase64);
#else
	return BinaryData::Encode(reinterpret_cast<unsigned char const*>(inData.c_str()), inData.length(), outBase64);
#endif
}
