#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Definitions of configuration (registry) entries.
 * @author David Connet
 *
 * wxConfig must be set up first before using these.
 *
 * Revision History
 * 2020-01-07 ColumnOrder and ReportListHeader settings are incompatible - separate.
 * 2018-12-25 Convert to fmt.
 * 2018-10-11 Separated from RegItems.h.
 */

#include "fmt/xchar.h"

// Sorting:
#define CFG_KEY_SORTING L"Sorting"
//	ST [key]Order (ColumnOrder.cpp)
inline std::wstring CFG_SORTING_ORDER(std::wstring const& item)
{
	return fmt::format(L"{}/{}Order", CFG_KEY_SORTING, item);
}
//	ST [key]Sort (ColumnOrder.cpp)
inline std::wstring CFG_SORTING_SORT(std::wstring const& item)
{
	return fmt::format(L"{}/{}Sort", CFG_KEY_SORTING, item);
}
//	ST [key]Order2 (ReportListHeader.cpp)
inline std::wstring CFG_SORTING_ORDER2(std::wstring const& item)
{
	return fmt::format(L"{}/{}Order2", CFG_KEY_SORTING, item);
}
//	ST [key]Vis (ReportListHeader.cpp)
inline std::wstring CFG_SORTING_VISIBLE(std::wstring const& item)
{
	return fmt::format(L"{}/{}Vis", CFG_KEY_SORTING, item);
}
inline std::wstring CFG_COLUMN_WIDTHS(std::wstring const& item)
{
	return fmt::format(L"{}/{}Width", CFG_KEY_SORTING, item);
}
inline std::wstring CFG_SORT_COLUMN(std::wstring const& item)
{
	return fmt::format(L"{}/{}Column", CFG_KEY_SORTING, item);
}

#define CFG_KEY_ACCELERATORS L"Accelerators"
// ST
#define CFG_ACCEL_KEYCODE L"KeyCode"
// ST
#define CFG_ACCEL_ID L"id"
// DW (bitmask for alt/ctrl/shift modifiers)
#define CFG_ACCEL_MOD L"mod"

#define CFG_KEY_RECENT_FILES L"Recent File List" // Named this way for compatibility with existing MFC app
