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
 * 2014-03-07 Add support for Poco's compression.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-12 Encoding/decoding 0 bytes should fail.
 * 2008-12-13 Added wxWidget support (zlib)
 * 2007-01-03 Created
 */

#include "stdafx.h"
#include "ARBCommon/BinaryData.h"

#include "ARBCommon/ARBBase64.h"
#include "ARBCommon/StringUtil.h"

#if defined(USE_POCO)
#include "Poco/Buffer.h"
#include "Poco/DeflatingStream.h"
#include "Poco/InflatingStream.h"
#include "Poco/MemoryStream.h"
#include "Poco/StreamCopier.h"
#include <fstream>

#elif defined(__WXWINDOWS__)
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/zstream.h>
#include <fstream>

#else
#ifdef WIN32
#pragma PRAGMA_MESSAGE("WARNING: Need Poco or wxWidgets")
#else
#pragma message("WARNING: Need Poco or wxWidgets")
#endif
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool BinaryData::Decode(
		std::wstring const& inBase64,
		unsigned char*& outBinData,
		size_t& outBytes)
{
	outBinData = nullptr;
	outBytes = 0;
	if (inBase64.empty())
		return false;

	bool bOk = false;
	unsigned char* pData;
	size_t len;
	if (ARBBase64::Decode(inBase64, pData, len))
	{
#if defined(USE_POCO)
		Poco::MemoryInputStream input(reinterpret_cast<char*>(pData), len);
		Poco::InflatingInputStream inflater(input);
		std::stringstream output;
		Poco::StreamCopier::copyStream(inflater, output);
		std::string const& data = output.str();
		outBytes = data.size();
		outBinData = new unsigned char[outBytes];
		memcpy(outBinData, data.data(), outBytes);
		bOk = true;

#elif defined(__WXWINDOWS__)
		wxMemoryOutputStream output;
		{
			wxZlibInputStream strm(new wxMemoryInputStream(pData, len), wxZLIB_ZLIB);
			output.Write(strm);
			output.Close();
		}
		outBytes = output.GetSize();
		outBinData = new unsigned char[outBytes];
		output.CopyTo(outBinData, outBytes);
		bOk = true;

#endif

		ARBBase64::Release(pData);
	}

	return bOk;
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
	outBase64.clear();
	if (0 == inBytes)
		return false;

	bool bOk = false;
	size_t nData = 0;
	unsigned char* pData = nullptr;

#if defined(USE_POCO)
	Poco::MemoryInputStream input(reinterpret_cast<char const*>(inBinData), inBytes);
	Poco::DeflatingInputStream deflater(input);
	std::stringstream output;
	Poco::StreamCopier::copyStream(deflater, output);
	std::string const& data = output.str();
	nData = data.size();
	pData = new unsigned char[nData];
	memcpy(pData, data.data(), nData);

#elif defined(__WXWINDOWS__)

	wxMemoryOutputStream output;
	{
		wxZlibOutputStream strm(output);
		strm.Write(inBinData, inBytes);
		strm.Close();
	}
	nData = output.GetSize();
	pData = new unsigned char[nData];
	output.CopyTo(pData, nData);

#endif

	if (pData)
	{
		std::wstring tmp;
		bOk = ARBBase64::Encode(pData, nData, tmp);
		outBase64 = tmp;
		delete[] pData;
	}

	return bOk;
}


bool BinaryData::EncodeFile(
		std::wstring const& inFileName,
		std::wstring& outBase64)
{
	outBase64.clear();

	bool bOk = false;
	size_t nData = 0;
	unsigned char* pData = nullptr;

#if defined(USE_POCO)
#ifdef ARB_HAS_ISTREAM_WCHAR
	std::ifstream file(inFileName, std::ios::binary);
#else
	std::string filename(StringUtil::stringA(inFileName));
	std::ifstream file(filename.c_str(), std::ios::binary);
#endif
	if (!file.good())
		return false;

	Poco::DeflatingInputStream deflater(file);
	std::stringstream output;
	Poco::StreamCopier::copyStream(deflater, output);
	std::string const& data = output.str();
	nData = data.size();
	pData = new unsigned char[nData];
	memcpy(pData, data.data(), nData);

#elif defined(__WXWINDOWS__)
	wxFFile file;
	if (!file.Open(inFileName.c_str(), L"rb"))
		return false;

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

#endif

	if (pData)
	{
		std::wstring tmp;
		bOk = ARBBase64::Encode(pData, nData, tmp);
		outBase64 = tmp;
		delete[] pData;
	}

	return bOk;
}


bool BinaryData::DecodeString(
		std::wstring const& inBase64,
		std::wstring& outData)
{
	if (inBase64.empty())
	{
		outData.clear();
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
	outBase64.clear();
	if (inData.empty())
		return false;
	// Do not include the null terminator. Otherwise decoding includes it into
	// the output string - which when streamed, then includes the null. Which
	// in an ostringstream, terminates the string on output of the stream.
	// TODO: Better conversion
	std::string tmp(StringUtil::stringA(inData));
	return BinaryData::Encode(reinterpret_cast<unsigned char const*>(tmp.c_str()), tmp.length(), outBase64);
}
