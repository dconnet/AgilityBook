#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief MD5 algorithm, based on RSA Data Security, Inc.
 *
 * Revision History
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2010-02-07 DRC Created
 */

class ARBMsgDigest
{
	ARBMsgDigest();
public:
	static std::wstring ComputeBuffer(
			char const* inData,
			size_t nData);
	static std::wstring ComputeFile(
			wchar_t const* const inFileName,
			size_t* outSize = NULL);
	static std::wstring ComputeFile(
			std::wstring const& inFileName,
			size_t* outSize = NULL);
};
