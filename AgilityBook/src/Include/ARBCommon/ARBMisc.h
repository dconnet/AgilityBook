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
 * @li 2013-07-17 DRC Created
 */

#include <string>


/**
 * Make a string safe for HTML viewing.
 * @param inRawData String to sanitize
 * @param bConvertCR Convert \n to html-breaks.
 */
std::wstring SanitizeStringForHTML(
		std::wstring const& inRawData,
		bool bConvertCR = true);


/**
 * Get OS version.
 * @param verMajor OS major version number.
 * @param verMinor OS minor version number.
 */
bool GetOSInfo(int& verMajor, int& verMinor);


/**
 * Get a user-friendly OS info string.
 * @param bVerbose Get multi-line info.
 */
std::wstring GetOSInfo(bool bVerbose = true);
