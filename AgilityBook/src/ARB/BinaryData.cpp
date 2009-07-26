/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-12 DRC Encoding/decoding 0 bytes should fail.
 * @li 2008-12-13 DRC Added wxWidget support (zlib)
 * @li 2007-01-03 DRC Created
 */

#include "stdafx.h"
#include "BinaryData.h"

#include "ARBBase64.h"
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/zstream.h>

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
	if (inBase64.empty())
		return false;

	unsigned char* pData;
	size_t len;
	if (!ARBBase64::Decode(inBase64, pData, len))
		return false;

	wxMemoryOutputStream output;
	{
		wxZlibInputStream strm(new wxMemoryInputStream(pData, len), wxZLIB_ZLIB);
		output.Write(strm);
		output.Close();
	}
	outBytes = output.GetSize();
	outBinData = new unsigned char[outBytes];
	output.CopyTo(outBinData, outBytes);

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
	outBase64.erase();
	if (0 == inBytes)
		return false;

	size_t nData = 0;
	unsigned char* pData = NULL;

	wxMemoryOutputStream output;
	{
		wxZlibOutputStream strm(output);
		strm.Write(inBinData, inBytes);
		strm.Close();
	}
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

	bool bOk = ARBBase64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}


bool BinaryData::Encode(
		wxFFile& inData,
		tstring& outBase64)
{
	outBase64.erase();

	if (!inData.IsOpened())
		return false;

	size_t nData = 0;
	unsigned char* pData = NULL;

	wxMemoryOutputStream output;
	{
		wxZlibOutputStream strm(output);
		wxFFileInputStream instrm(inData);
		strm.Write(instrm);
		strm.Close();
	}
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

	bool bOk = ARBBase64::Encode(pData, nData, outBase64);
	delete [] pData;

	return bOk;
}


bool BinaryData::DecodeString(
		tstring const& inBase64,
		tstring& outData)
{
	if (inBase64.empty())
	{
		outData.erase();
		return false;
	}
	unsigned char* data;
	size_t len;
	if (!BinaryData::Decode(inBase64, data, len))
		return false;
	// TODO: Better conversion
	outData = tstringUtil::TString(reinterpret_cast<char*>(data), len);
	Release(data);
	return true;
}


bool BinaryData::EncodeString(
		tstring const& inData,
		tstring& outBase64)
{
	outBase64.erase();
	if (inData.empty())
		return false;
	// Do not include the null terminator. Otherwise decoding includes it into
	// the output string - which when streamed, then includes the null. Which
	// in an ostringstream, terminates the string on output of the stream.
	// TODO: Better conversion
	std::string tmp(tstringUtil::tstringA(inData));
	return BinaryData::Encode(reinterpret_cast<unsigned char const*>(tmp.c_str()), tmp.length(), outBase64);
}
