#pragma once

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
 * @li 2013-10-29 DRC Added sha1/sha256
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2010-02-07 DRC Created
 */

#include <string>
#include <istream>


class ARBMsgDigest
{
	ARBMsgDigest();
public:
	typedef enum
	{
		ARBDigestUnknown,
		ARBDigestMD5,
		ARBDigestSHA1,
		ARBDigestSHA256,
	} ARBDigest;

	static std::wstring Compute(
			wchar_t const* const inFileName,
			ARBDigest type,
			size_t* outSize = NULL);
	static std::wstring Compute(
			std::wstring const& inFileName,
			ARBDigest type,
			size_t* outSize = NULL);
	static std::wstring Compute(
			std::istream& inFile,
			ARBDigest type,
			size_t* outSize = NULL);
};
