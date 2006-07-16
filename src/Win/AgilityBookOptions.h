#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2006-07-16 DRC Added PointsViewSort
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-05-04 DRC Added IncludeCRCDImage
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2004-08-31 DRC Added AutoShowSplashScreen
 * @li 2004-06-16 DRC Added options to remember date formats.
 * @li 2004-06-06 DRC Added additional clipboard formats.
 * @li 2004-05-16 DRC Added IsLevelVisible.
 * @li 2004-04-08 DRC Added general program options.
 * @li 2004-03-13 DRC Added GetViewHiddenTitles.
 * @li 2004-01-04 DRC Added GetImportExportDateFormat.
 * @li 2003-12-11 DRC Added options for import/export wizard.
 */

#include <set>
#include <vector>
#include "ARBDate.h"
#include "ARBTypes.h"

struct CFontInfo
{
	CString name;
	int size;
	bool italic;
	bool bold;
	void CreateFont(
			CFont& font,
			CDC* pDC = NULL);
	void CreateFont(
			CFontDialog const& dlg,
			CFont& font,
			CDC* pDC = NULL);
};

class CAgilityBookOptions
{
public:
	// Calendar options
	static bool AutoDeleteCalendarEntries();
	static void SetAutoDeleteCalendarEntries(bool bAuto);
	static bool ViewAllCalendarEntries();
	static void SetViewAllCalendarEntries(bool bView);
	static bool ViewAllCalendarOpening();
	static void SetViewAllCalendarOpening(bool bView);
	static bool ViewAllCalendarClosing();
	static void SetViewAllCalendarClosing(bool bView);
	static int DaysTillEntryIsPast();
	static void SetDaysTillEntryIsPast(int nDays);
	static bool HideOverlappingCalendarEntries();
	static void SetHideOverlappingCalendarEntries(bool bHide);
	static CSize GetCalendarEntrySize();
	static void SetCalendarEntrySize(CSize const& sz);
	typedef enum
	{
		eCalColorNotEntered,
		eCalColorPlanning,
		eCalColorOpening,
		eCalColorClosing,
		eCalColorEntered
	} CalendarColorItem;
	static COLORREF CalendarColor(CalendarColorItem inItem);
	static void SetCalendarColor(
			CalendarColorItem inItem,
			COLORREF inColor);
	static COLORREF CalendarClosingColor();
	static void SetCalendarClosingColor(COLORREF inColor);
	static int CalendarOpeningNear();
	static void SetCalendarOpeningNear(int inDays);
	static int CalendarClosingNear();
	static void SetCalendarClosingNear(int inDays);
	static COLORREF CalendarOpeningNearColor();
	static void SetCalendarOpeningNearColor(COLORREF inColor);
	static COLORREF CalendarClosingNearColor();
	static void SetCalendarClosingNearColor(COLORREF inColor);
	// Common options
	static ARBDate::DayOfWeek GetFirstDayOfWeek();
	static void SetFirstDayOfWeek(ARBDate::DayOfWeek day);
	// Runs/points options
	static bool GetViewRunsByTrial();
	static void SetViewRunsByTrial(bool bView);
	static bool GetNewestDatesFirst();
	static void SetNewestDatesFirst(bool bNewest);
	typedef enum
	{
		// Fixed values as they are stored in the registry
		ePointsViewSortUnknown = 0,
		ePointsViewSortDivision = 1,
		ePointsViewSortLevel = 2,
		ePointsViewSortEvent = 3
	} PointsViewSort;
	static void GetPointsViewSort(
			PointsViewSort& outPrimary,
			PointsViewSort& outSecondary,
			PointsViewSort& outTertiary);
	static void SetPointsViewSort(
			PointsViewSort inPrimary,
			PointsViewSort inSecondary,
			PointsViewSort inTertiary);
	static bool GetViewHiddenTitles();
	static void SetViewHiddenTitles(bool bSet);
	static bool GetTableInYPS();
	static void SetTableInYPS(bool bSet);
	static bool GetIncludeCRCDImage();
	static void SetIncludeCRCDImage(bool bSet);
	// Font options
	static void GetPrinterFontInfo(CFontInfo& info);
	static void SetPrinterFontInfo(CFontInfo const& info);
	static void GetCalendarFontInfo(CFontInfo& info);
	static void SetCalendarFontInfo(CFontInfo const& info);
	// Last entered options
	static CString GetLastEnteredDivision();
	static void SetLastEnteredDivision(TCHAR const* inLast);
	static CString GetLastEnteredLevel();
	static void SetLastEnteredLevel(TCHAR const* inLast);
	static CString GetLastEnteredHeight();
	static void SetLastEnteredHeight(TCHAR const* inLast);
	static CString GetLastEnteredRefHeight();
	static void SetLastEnteredRefHeight(TCHAR const* inLast);
	static CString GetLastEnteredJudge();
	static void SetLastEnteredJudge(TCHAR const* inLast);
	static CString GetLastEnteredHandler();
	static void SetLastEnteredHandler(TCHAR const* inLast);
	// Import/Export options
	enum
	{
		eDelimTab		= 1,
		eDelimSpace		= 2,
		eDelimColon		= 3,
		eDelimSemicolon	= 4,
		eDelimComma		= 5,
		eDelimOther		= 6
	};
	static int GetImportStartRow();
	static void SetImportStartRow(int row);
	static void GetImportExportDelimiters(
			bool bImport,
			int& delim,
			CString& delimiter);
	static void SetImportExportDelimiters(
			bool bImport,
			int delim,
			CString const& delimiter);
	static void GetImportExportDateFormat(
			bool bImport,
			ARBDate::DateFormat& outFormat);
	static void SetImportExportDateFormat(
			bool bImport,
			ARBDate::DateFormat inFormat);
	typedef enum
	{
		eUnknown		= 0x0000,
		eRunsImport		= 0x0001,
		eRunsExport		= 0x0002,
		eCalImport		= 0x0004,
		eCalExport		= 0x0008,
		eCalExportAppt	= 0x0010,
		eCalExportTask	= 0x0020,
		eLogImport		= 0x0040,
		eLogExport		= 0x0080,
		eViewTree		= 0x0100,
		eViewRuns		= 0x0200,
		eViewCal		= 0x0400,
		eViewLog		= 0x0800,
	} ColumnOrder;
	// General program options
	static bool GetAutoUpdateCheck();
	static void SetAutoUpdateCheck(bool bSet);
	static int GetNumBackupFiles();
	static void SetNumBackupFiles(int nFiles);
	static bool AutoShowSplashScreen();
	static void AutoShowSplashScreen(bool bAutoShow);
	static CString GetSplashImage();
	static void SetSplashImage(CString const& filename);
	static bool AutoShowPropertiesOnNewTitle();
	static void AutoShowPropertiesOnNewTitle(bool bShow);
	typedef enum
	{
		eRunTree	= 0,
		eRunList	= 1,
		ePoints		= 2,
		eCalList	= 3,
		eCalendar	= 4,
		eTraining	= 5,
	} FormattedDate;
	static ARBDate::DateFormat GetDateFormat(FormattedDate inItem);
	static void SetDateFormat(
			FormattedDate inItem,
			ARBDate::DateFormat inFormat);

protected:
	friend class CDlgAssignColumns;
	// CDlgAssignColumns has some structs for used to ensure validity.
	static void GetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::vector<int>& outValues);
	static void SetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::vector<int> const& inValues);
public:
	// Special clipboard formats
	typedef enum
	{
		eFormatDog,
		eFormatTrial,
		eFormatRun,
		eFormatCalendar,
		eFormatiCalendar,
		eFormatLog,
	} eClipFormat;
	static UINT GetClipboardFormat(eClipFormat fmt);
};
