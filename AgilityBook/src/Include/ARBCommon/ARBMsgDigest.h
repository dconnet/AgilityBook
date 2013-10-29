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
		ARBDigestMD5,
		ARBDigestSha1,
		ARBDigestSha2,
	} ARBDigest;

	static std::wstring Compute(
			std::istream& inFile,
			size_t* outSize = NULL,
			ARBDigest type = ARBDigestMD5);
};
