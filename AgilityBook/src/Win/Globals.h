#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Globals
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-14 DRC Moved from AgilityBook.h
 */

#include "ARBTypes.h"
#include "PointsData.h"
#include <vector>


/**
 * Separate a line into substrings. Very simply routine that has no escape chars
 * @param inSep Separator character
 * @param inStr String to separate
 * @param outFields Separated pieces
 * @return Number of fields
 * @note In Globals.cpp
 */
extern size_t BreakLine(
		char inSep,
		tstring inStr,
		std::vector<tstring>& outFields);

/**
 * Print blank pages to enter runs on.
 * @param inConfig Configuration
 * @param inDog Dog's runs to print.
 * @param inRuns Runs to print, if empty, print blank pages.
 * @return Printed?
 * @note In Print.cpp
 */
extern bool PrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns);


/**
 * Find a window id within a sizer.
 * @param sizer The sizer to recursively search.
 * @param id Id of window to return.
 * @note In Globals.cpp
 */
extern wxWindow* FindWindowInSizer(
		wxSizer* sizer,
		int id);


/**
 * Return the text for a column.
 * This is just a simple wrapper around wxListItem/GetItem
 * @note index = -1 implies getting the column header text
 */
extern wxString GetListColumnText(
		wxListCtrl const* list,
		long index,
		long col);


/**
 * Set the text for a column.
 * This is just a simple wrapper around wxListItem/SetItem
 */
extern bool SetListColumnText(
		wxListCtrl* list,
		long index,
		long col,
		wxString const& text);


/**
 * Refresh tree data - in Windows we used getdispinfo, in wxWindows that doesn't
 * work. So the old code that simply invalidated the window no longer works.
 */
extern void RefreshTreeItem(
		wxTreeCtrl* tree,
		wxTreeItemId item,
		bool bRecurse = false);


/**
 * Draw text similar to VC DrawText with DT_CALCRECT and DT_WORDBREAK
 * @param pDC Input device context
 * @param inStr String to draw
 * @param rect if drawing, text will be clipped to input, on output (if bBreak), rect needed is returned
 * @param bBreak Draw with line breaks, else singleline
 * @param flags Flags to pass to DrawLabel
 * @param bCalc Determine rectangle or do actual drawing
 */
extern void DrawBetterLabel(
		wxDC* pDC,
		wxString const& inStr,
		wxRect& rect,
		int flags,
		bool bCalc);
