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
 * 2013-10-29 Added sha1/sha256
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2010-02-07 Created
 */

#include "LibwxARBCommon.h"

#include <istream>
#include <string>


namespace ARBMsgDigest
{
	typedef enum
	{
		ARBDigestUnknown,
		ARBDigestMD5,
		ARBDigestSHA1,
		ARBDigestSHA256,
	} ARBDigest;

	ARBCOMMON_API std::wstring Compute(
			std::istream& inFile,
			ARBDigest type,
			size_t* outSize = nullptr);
};
