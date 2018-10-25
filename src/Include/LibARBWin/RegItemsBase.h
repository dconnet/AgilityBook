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
 * 2018-10-11 Separated from RegItems.h.
 */


//Sorting:
#define CFG_KEY_SORTING					L"Sorting"
//	ST [key]Order (ColumnOrder.cpp)
inline wxString CFG_SORTING_ORDER(wxString const& item)
{
	wxString order;
	order << CFG_KEY_SORTING L"/" << item << L"Order";
	return order;
}
//	ST [key]Sort (ColumnOrder.cpp)
inline wxString CFG_SORTING_SORT(wxString const& item)
{
	wxString order;
	order << CFG_KEY_SORTING L"/" << item << L"Sort";
	return order;
}

#define CFG_KEY_ACCELERATORS	L"Accelerators"
// ST
#define CFG_ACCEL_KEYCODE		L"KeyCode"
// ST
#define CFG_ACCEL_ID			L"id"
// DW (bitmask for alt/ctrl/shift modifiers)
#define CFG_ACCEL_MOD			L"mod"

#define CFG_KEY_RECENT_FILES	L"Recent File List" // Named this way for compatibility with existing MFC app
