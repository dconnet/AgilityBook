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
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-12 DRC Encoding/decoding 0 bytes should fail.
 * @li 2008-12-13 DRC Added wxWidget support (zlib)
 * @li 2007-01-03 DRC Created
 */

#include "stdafx.h"
#include "BinaryData.h"

#include "ARBBase64.h"
#include "StringUtil.h"
#if defined(__WXWINDOWS__)
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/zstream.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define CHUNK	131072

BinaryData::BinaryData()
{
}


bool BinaryData::Decode(
		std::wstring const& inBase64,
		unsigned char*& outBinData,
		size_t& outBytes)
{
	outBinData = NULL;
	outBytes = 0;
	if (inBase64.empty())
		return false;

#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(implement non-wx version)
	return false;

#else

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
#endif
}


void BinaryData::Release(unsigned char* inBinData)
{
	delete [] inBinData;
}


bool BinaryData::Encode(
		unsigned char const* inBinData,
		size_t inBytes,
		std::wstring& outBase64)
{
	outBase64.erase();
	if (0 == inBytes)
		return false;

#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(implement non-wx version)
	return false;

#else

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

	std::wstring tmp;
	bool bOk = ARBBase64::Encode(pData, nData, tmp);
	outBase64 = tmp;
	delete [] pData;

	return bOk;
#endif
}


bool BinaryData::EncodeFile(
		std::wstring const& inFileName,
		std::wstring& outBase64)
{
	outBase64.erase();

#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(implement non-wx version)
	return false;
#endif

#if defined(__WXWINDOWS__)
	wxFFile file;
	if (!file.Open(inFileName.c_str(), L"rb"))
		return false;

	size_t nData = 0;
	unsigned char* pData = NULL;

	wxMemoryOutputStream output;
	{
		wxZlibOutputStream strm(output);
		wxFFileInputStream instrm(file);
		strm.Write(instrm);
		strm.Close();
	}
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

	std::wstring tmp;
	bool bOk = ARBBase64::Encode(pData, nData, tmp);
	outBase64 = tmp;
	delete [] pData;

	return bOk;
#endif
}


bool BinaryData::DecodeString(
		std::wstring const& inBase64,
		std::wstring& outData)
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
	std::string tmp(reinterpret_cast<char*>(data), len);
	Release(data);
	outData = StringUtil::stringW(tmp);
	return true;
}


bool BinaryData::EncodeString(
		std::wstring const& inData,
		std::wstring& outBase64)
{
	outBase64.erase();
	if (inData.empty())
		return false;
	// Do not include the null terminator. Otherwise decoding includes it into
	// the output string - which when streamed, then includes the null. Which
	// in an ostringstream, terminates the string on output of the stream.
	// TODO: Better conversion
	std::string tmp(StringUtil::stringA(inData));
	return BinaryData::Encode(reinterpret_cast<unsigned char const*>(tmp.c_str()), tmp.length(), outBase64);
}
