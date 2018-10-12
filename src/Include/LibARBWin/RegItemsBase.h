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
//	ST [key]Order (ColumnOrder.cpp)
inline wxString CFG_SORTING_ORDER(wxString const& item)
{
	return L"Sorting/" + item + L"Order";
}
//	ST [key]Sort (ColumnOrder.cpp)
inline wxString CFG_SORTING_SORT(wxString const& item)
{
	return L"Sorting/" + item + L"Sort";
}
