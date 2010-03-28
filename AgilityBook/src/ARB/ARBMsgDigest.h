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
 * @li 2010-02-07 DRC Created
 */

#include <wx/string.h>


class ARBMsgDigest
{
	ARBMsgDigest();
public:
	static wxString ComputeBuffer(
			char const* inData,
			size_t nData);
	static wxString ComputeFile(
			wxString const& inFileName,
			size_t* outSize = NULL);
};
