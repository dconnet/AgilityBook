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
 * @li 2013-10-29 DRC Created
 */

#include "stdafx.h"
#include "ARBMsgDigestImpl.h"

#include "ARBCommon/StringUtil.h"
#include <sstream>
#if defined(__WXWINDOWS__)
#include <wx/file.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

// Note, error checking of arguments handled in ARBMsgDigest::Compute

std::wstring ARBMsgDigestComputeSHA256(
		std::istream& inFile,
		size_t* outSize)
{
#pragma PRAGMA_TODO(implement sha256)
	return std::wstring();
}
