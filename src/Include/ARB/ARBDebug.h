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
 * 2018-12-16 Convert to fmt.
 * 2018-01-28 Created
 */

#include "fmt/format.h"


/**
 * General information to be reported
 */
class ARBDebug
{
public:
	static std::wstring GetSystemInfo();
	static std::wstring GetRegistryInfo();

	/**
	 * Dump registry information
	 * @param inGroup Registry item (relative to wxConfig current path)
	 * @param outData Output for all registry data
	 * @param outItems Output for string items (only)
	 * @return Number of registry items added to outData.
	 */
	static size_t DumpRegistryGroup(
		wxString const& inGroup,
		fmt::wmemory_buffer* outData,
		std::vector<std::wstring>* outItems);
};
