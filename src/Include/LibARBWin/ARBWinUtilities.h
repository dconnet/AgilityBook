#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Utilities
 * @author David Connet
 *
 * Revision History
 * 2018-10-30 Moved some utils to ARBCommon.
 * 2018-10-11 Moved to Win LibARBWin
 * 2015-11-01 Added ShouldComputeScore.
 * 2012-10-06 Moved backup function from doc to here.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-05-04 Added FormatBytes
 * 2010-10-30 Moved BreakLine to a separate file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-14 Moved from AgilityBook.h
 */

#include "LibwxARBWin.h"

#include "ARBCommon/ARBTypes.h"
#include <wx/treebase.h>
class CListCtrl;
class wxDateTime;
class wxFileName;
class wxTreeCtrl;


/**
 * Find a window id within a sizer.
 * @param sizer The sizer to recursively search.
 * @param id Id of window to return.
 */
ARBWIN_API wxWindow* FindWindowInSizer(wxSizer* sizer, int id);


/**
 * Return the text for a column.
 * This is just a simple wrapper around wxListItem/GetItem
 * @note index = -1 implies getting the column header text
 */
ARBWIN_API std::wstring GetListColumnText(CListCtrl const* list, long index, long col);


/**
 * Set the text for a column.
 * This is just a simple wrapper around wxListItem/SetItem
 */
ARBWIN_API bool SetListColumnText(CListCtrl* list, long index, long col, std::wstring const& text);


/**
 * Refresh tree data - in Windows we used getdispinfo, in wxWindows that doesn't
 * work. So the old code that simply invalidated the window no longer works.
 */
ARBWIN_API void RefreshTreeItem(wxTreeCtrl* tree, wxTreeItemId item, bool bRecurse = false);


/**
 * Draw text similar to VC DrawText with DT_CALCRECT and DT_WORDBREAK
 * @param pDC Input device context
 * @param inStr String to draw
 * @param rect if drawing, text will be clipped to input, on output (if bBreak), rect needed is returned
 * @param flags Flags to pass to DrawLabel
 * @param bCalc Determine rectangle or do actual drawing
 */
ARBWIN_API void DrawBetterLabel(wxDC* pDC, wxString const& inStr, wxRect& rect, int flags, bool bCalc);
