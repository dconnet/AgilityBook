#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2009-07-19 DRC Implement proxy support. 
 * @li 2008-12-24 DRC Ported to wxWidgets.
 * @li 2007-08-03 DRC Added UserNames
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

#include "ARBDate.h"
#include "ARBTypes.h"
#include <set>
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
class CVersionNum;


struct CFontInfo
{
	wxString name;
	int size;
	bool italic;
	bool bold;
	void CreateFont(wxFont& font);
	void CreateFont(
			wxFontDialog& dlg,
			wxFont& font);
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
	static long DaysTillEntryIsPast();
	static void SetDaysTillEntryIsPast(long nDays);
	static bool HideOverlappingCalendarEntries();
	static void SetHideOverlappingCalendarEntries(bool bHide);
	typedef enum
	{
		eCalColorNotEntered,
		eCalColorPlanning,
		eCalColorPending,
		eCalColorEntered,
		eCalColorOpening,
		eCalColorClosing,
	} CalendarColorItem;
	static wxColour CalendarColor(CalendarColorItem inItem);
	static void SetCalendarColor(
			CalendarColorItem inItem,
			wxColour inColor);
	static wxColour CalendarClosingColor();
	static void SetCalendarClosingColor(wxColour inColor);
	static long CalendarOpeningNear();
	static void SetCalendarOpeningNear(long inDays);
	static long CalendarClosingNear();
	static void SetCalendarClosingNear(long inDays);
	static wxColour CalendarOpeningNearColor();
	static void SetCalendarOpeningNearColor(wxColour inColor);
	static wxColour CalendarClosingNearColor();
	static void SetCalendarClosingNearColor(wxColour inColor);
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
	static bool GetViewLifetimePointsByEvent();
	static void SetViewLifetimePointsByEvent(bool bSet);
	static bool GetTableInYPS();
	static void SetTableInYPS(bool bSet);
	static bool GetIncludeCRCDImage();
	static void SetIncludeCRCDImage(bool bSet);
	// Font options
	static void GetPrinterFontInfo(CFontInfo& info);
	static void SetPrinterFontInfo(CFontInfo const& info);
	static bool GetUnitsAsMM();
	static void SetUnitsAsMM(bool bAsMM);
	// Get margins in .01 inches or MM (or pixels, if DC is set)
	static void GetPrinterMargins(
			bool bAsMM, // In .01in or millimeters, ignored it DC is set
			long& outLeft,
			long& outRight,
			long& outTop,
			long& outBottom,
			wxDC* pDC); // Used to convert to logical units
	static void SetPrinterMargins(
			bool bAsMM, // In .01 inches or millimeters
			long inLeft,
			long inRight,
			long inTop,
			long inBottom);
	static void GetRunPageSize(
			bool bAsMM, // In .01in or millimeters, ignored it DC is set
			long& outWidth,
			long& outHeight,
			wxDC* pDC); // Used to convert to logical units
	static void SetRunPageSize(
			bool bAsMM, // In .01 inches or millimeters
			long inWidth,
			long inHeight);
	static void GetCalendarFontInfo(CFontInfo& info);
	static void SetCalendarFontInfo(CFontInfo const& info);
	// Last entered options
	static wxString GetLastEnteredDivision();
	static void SetLastEnteredDivision(wxChar const* inLast);
	static wxString GetLastEnteredLevel();
	static void SetLastEnteredLevel(wxChar const* inLast);
	static wxString GetLastEnteredHeight();
	static void SetLastEnteredHeight(wxChar const* inLast);
	static wxString GetLastEnteredRefHeight();
	static void SetLastEnteredRefHeight(wxChar const* inLast);
	static wxString GetLastEnteredJudge();
	static void SetLastEnteredJudge(wxChar const* inLast);
	static wxString GetLastEnteredHandler();
	static void SetLastEnteredHandler(wxChar const* inLast);
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
	static long GetImportStartRow();
	static void SetImportStartRow(long row);
	static void GetImportExportDelimiters(
			bool bImport,
			long& delim,
			wxString& delimiter);
	static void SetImportExportDelimiters(
			bool bImport,
			long delim,
			wxString const& delimiter);
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
		eView			= 0x0100,
	} ColumnOrder;
	// General program options
	static long GetMRUFileCount();
	static void SetMRUFileCount(long nFiles);
	static bool GetAutoUpdateCheck();
	static void SetAutoUpdateCheck(bool bSet);
	static long GetNumBackupFiles();
	static void SetNumBackupFiles(long nFiles);
	static bool AutoShowSplashScreen();
	static void AutoShowSplashScreen(bool bAutoShow);
	static wxString GetSplashImage();
	static void SetSplashImage(wxString const& filename);
	static bool AutoShowPropertiesOnNewTitle();
	static void AutoShowPropertiesOnNewTitle(bool bShow);
	static bool ShowHtmlPoints();
	static void SetShowHtmlPoints(bool bSet);
	// Internet things
	// -username/pw for accessing URLs thru ReadHTTP.cpp
	static wxString GetUserName(wxString const& hint);
	static void SetUserName(wxString const& hint, wxString const& userName);
	static bool GetUseProxy();
	static void SetUseProxy(bool inUseProxy);
	static wxString GetProxyServer();
	static void SetProxyServer(wxString const& inProxy);
	static wxString GetProxy();
	// -CalSite suppression options
	static bool IsCalSiteVisible(
			wxString const& filename,
			CVersionNum const& inVer); // Version number of current calsite
	static void SuppressCalSite(
			wxString const& filename,
			bool bSuppress);
	static CVersionNum GetCalSitePermanentStatus(wxString const& filename);
	static void SuppressCalSitePermanently(
			wxString const& filename,
			CVersionNum const& inVer,
			bool bSuppress = true);

protected:
	friend class CDlgAssignColumns;
	// CDlgAssignColumns has some structs for used to ensure validity.
	static void GetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::vector<long>& outValues,
			bool bDefaultValues = false);
	static void SetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::vector<long> const& inValues);
};
