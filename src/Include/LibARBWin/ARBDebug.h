#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDebug class
 * @author David Connet
 *
 * Revision History
 * 2019-01-31 Moved from ARB.
 * 2018-12-16 Convert to fmt.
 * 2018-01-28 Created
 */

#include "LibwxARBWin.h"

#include "ARBCommon/VersionNum.h"
#include "fmt/xchar.h"
#include <vector>


/**
 * General information to be reported
 */
namespace ARBDebug
{
// These are for consistent reporting in About dialog and sysinfo.
// The libraries and exe are always compiled together at one time.
ARBWIN_API std::wstring GetCompileDate();
ARBWIN_API std::wstring GetCompileTime();

ARBWIN_API std::wstring GetOSName();
ARBWIN_API std::wstring GetArchName();
ARBWIN_API std::wstring GetEndiannessName();

ARBWIN_API std::wstring GetSystemInfo(wxWindow const* pWindow, CVersionNum const& ver);
ARBWIN_API std::wstring GetRegistryInfo();

/**
 * Dump registry information
 * @param inGroup Registry item (relative to wxConfig current path)
 * @param outData Output for all registry data
 * @param outItems Output for string items (only)
 * @return Number of registry items added to outData.
 */
ARBWIN_API size_t
DumpRegistryGroup(wxString const& inGroup, fmt::wmemory_buffer* outData, std::vector<std::wstring>* outItems);
} // namespace ARBDebug
