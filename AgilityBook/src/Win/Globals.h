#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Globals
 * @author David Connet
 *
 * Revision History
 * @li 2010-10-30 DRC Moved BreakLine to a separate file.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-14 DRC Moved from AgilityBook.h
 */

#include "ARBTypes.h"
#include "PointsData.h"
#include <wx/treectrl.h>
class CListCtrl;
class wxDateTime;
class wxFileName;
class wxTreeCtrl;


/**
 * Using wx to get the times on network files is really slow.
 * I suspect it's the win32 CreateFile/GetFileTime apis.
 * @param inPassword Password to decode
 * @param outBuffer Decoded password
 * @return Success
 */
bool GetFileTimes(
		wxFileName const& filename,
		wxDateTime* dtAccess,
		wxDateTime* dtMod,
		wxDateTime* dtCreate);


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
extern std::wstring GetListColumnText(
		CListCtrl const* list,
		long index,
		long col);


/**
 * Set the text for a column.
 * This is just a simple wrapper around wxListItem/SetItem
 */
extern bool SetListColumnText(
		CListCtrl* list,
		long index,
		long col,
		std::wstring const& text);


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
 * @param flags Flags to pass to DrawLabel
 * @param bCalc Determine rectangle or do actual drawing
 */
extern void DrawBetterLabel(
		wxDC* pDC,
		wxString const& inStr,
		wxRect& rect,
		int flags,
		bool bCalc);
