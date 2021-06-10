#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Type definitions.
 * @author David Connet
 *
 * Revision History
 * 2019-01-31 Moved GetOSInfo to LibARBWin/ARBDebug.
 * 2014-06-19 Added IsWin7OrBetter.
 * 2013-07-17 Created
 */

#include "LibwxARBCommon.h"

#include <string>


/**
 * Make a string safe for HTML viewing.
 * @param inRawData String to sanitize
 * @param bConvertCR Convert \n to html-breaks.
 */
ARBCOMMON_API std::wstring SanitizeStringForHTML(std::wstring const& inRawData, bool bConvertCR = true);

/**
 * These are the strings we recognize as platforms.
 */
ARBCOMMON_API std::wstring GetARBArch();


/**
 * Get OS version.
 * @param verMajor OS major version number.
 * @param verMinor OS minor version number.
 */
ARBCOMMON_API bool GetOSInfo(int& verMajor, int& verMinor);


/**
 * Are we running on Win7 or better?
 */
ARBCOMMON_API bool IsWin7OrBetter();


/**
 * Translate a short to a Roman numeral
 */
ARBCOMMON_API std::wstring ShortToRoman(short value);
