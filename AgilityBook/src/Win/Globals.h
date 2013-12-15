#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Globals
 * @author David Connet
 *
 * Revision History
 * 2012-10-06 Moved backup function from doc to here.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-05-04 Added FormatBytes
 * 2010-10-30 Moved BreakLine to a separate file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-14 Moved from AgilityBook.h
 */

#include "ARBCommon/ARBTypes.h"
class CListCtrl;
class wxDateTime;
class wxFileName;
class wxTreeCtrl;


/**
 * Create a backup file (and reorganize if some were deleted)
 * @param inFilename File name to backup.
 * @param nBackups Number of backups to keep.
 * @return Backup files reorganized.
 */
extern bool CreateBackupFile(
		wxString const& inFilename,
		int nBackups);


/**
 * Using wx to get the times on network files is really slow.
 * I suspect it's the win32 CreateFile/GetFileTime apis.
 * @param filename File name to access.
 * @param dtAccess Last access time, may be NULL.
 * @param dtMod Last modification time, may be NULL.
 * @param dtCreate Creation time, may be NULL.
 * @return Success
 */
extern bool GetFileTimes(
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
