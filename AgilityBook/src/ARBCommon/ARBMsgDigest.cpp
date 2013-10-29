/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief File hashing algorithms
 *
 * Revision History
 * @li 2013-10-29 DRC Added more hash algorithms
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2010-02-07 DRC Created
 */

#include "stdafx.h"
#include "ARBCommon/ARBMsgDigest.h"

#include "ARBMsgDigestImpl.h"

#include "ARBCommon/StringUtil.h"
#include <sstream>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


std::wstring ARBMsgDigest::Compute(
		wchar_t const* const inFileName,
		ARBDigest type,
		size_t* outSize)
{
	if (outSize)
		*outSize = 0;
	if (!inFileName)
		return std::wstring();

	switch (type)
	{
	case ARBDigestMD5:
		return ARBMsgDigestComputeMD5(inFileName, outSize);

	case ARBDigestSHA1:
		return ARBMsgDigestComputeSHA1(inFileName, outSize);

	case ARBDigestSHA256:
		return ARBMsgDigestComputeSHA256(inFileName, outSize);

	default:
		assert(0);
		return std::wstring();
	}
}


std::wstring ARBMsgDigest::Compute(
		std::wstring const& inFileName,
		ARBDigest type,
		size_t* outSize)
{
	return Compute(inFileName.c_str(), type, outSize);
}


std::wstring ARBMsgDigest::Compute(
		std::istream& inFile,
		ARBDigest type,
		size_t* outSize)
{
	if (outSize)
		*outSize = 0;
	if (!inFile.good())
		return std::wstring();

	switch (type)
	{
	case ARBDigestMD5:
		return ARBMsgDigestComputeMD5(inFile, outSize);

	case ARBDigestSHA1:
		return ARBMsgDigestComputeSHA1(inFile, outSize);

	case ARBDigestSHA256:
		return ARBMsgDigestComputeSHA256(inFile, outSize);

	default:
		assert(0);
		return std::wstring();
	}
}
