#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2013-05-19 DRC Make last div/level/height/handler context aware.
 * @li 2010-03-28 DRC Added ability to import/export program settings.
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

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <set>
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
class CVersionNum;


struct CFontInfo
{
	std::wstring name;
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
	static bool ImportSettings(ElementNodePtr tree);
	static ElementNodePtr ExportSettings();

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
		eCalColorPast,
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
	typedef enum
	{
		// Fixed values as they are stored in the registry
		eViewRunsByTrial = 1,
		eViewRunsByList = 0,
		eViewAllRunsByList = 2,
	} ViewRunsStyle;
	static ViewRunsStyle GetViewRunsStyle();
	static void SetViewRunsStyle(ViewRunsStyle style);
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
	static bool GetRunTimeInOPS();
	static void SetRunTimeInOPS(bool bSet);
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
	static std::wstring GetLastEnteredDivision(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);
	static void SetLastEnteredDivision(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue,
			wchar_t const* inLast);
	static std::wstring GetLastEnteredLevel(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);
	static void SetLastEnteredLevel(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue,
			wchar_t const* inLast);
	static std::wstring GetLastEnteredHeight(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);
	static void SetLastEnteredHeight(
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue,
			wchar_t const* inLast);
	static std::wstring GetLastEnteredRefHeight();
	static void SetLastEnteredRefHeight(wchar_t const* inLast);
	static std::wstring GetLastEnteredJudge();
	static void SetLastEnteredJudge(wchar_t const* inLast);
	static std::wstring GetLastEnteredHandler(ARBDogPtr pDog);
	static void SetLastEnteredHandler(
			ARBDogPtr pDog,
			wchar_t const* inLast);
	static void CleanLastItems(std::wstring const& callName);
	static void CleanLastItems(std::wstring const& oldCallName, std::wstring const& newCallName);
	static void CleanLastItems(ARBConfig const& inConfig);
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
			std::wstring& delimiter);
	static void SetImportExportDelimiters(
			bool bImport,
			long delim,
			std::wstring const& delimiter);
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
		eAllColumns		= 0x01ff
	} ColumnOrder;
	// General program options
	static long GetMRUFileCount();
	static void SetMRUFileCount(long nFiles);
	static bool GetAutoUpdateCheck();
	static void SetAutoUpdateCheck(bool bSet);
	static long GetNumBackupFiles();
	static void SetNumBackupFiles(long nFiles);
	static bool AutoShowPropertiesOnNewTitle();
	static void AutoShowPropertiesOnNewTitle(bool bShow);
	static bool ShowHtmlPoints();
	static void SetShowHtmlPoints(bool bSet);
	// Internet things
	// -username/pw for accessing URLs thru ReadHTTP.cpp
	static std::wstring GetUserName(std::wstring const& hint);
	static void SetUserName(std::wstring const& hint, std::wstring const& userName);
	static bool GetUseProxy();
	static void SetUseProxy(bool inUseProxy);
	static std::wstring GetProxyServer();
	static void SetProxyServer(std::wstring const& inProxy);
	static std::wstring GetProxy();
	// -CalSite suppression options
	static bool IsCalSiteVisible(
			std::wstring const& filename,
			CVersionNum const& inVer); // Version number of current calsite
	static void SuppressCalSite(
			std::wstring const& filename,
			bool bSuppress);
	static CVersionNum GetCalSitePermanentStatus(std::wstring const& filename);
	static void SuppressCalSitePermanently(
			std::wstring const& filename,
			CVersionNum const& inVer,
			bool bSuppress = true);

protected:
	friend class CDlgAssignColumns;
	// CDlgAssignColumns has some structs for used to ensure validity.
	static void GetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::wstring const& namedColumn,
			std::vector<long>& outValues,
			bool bDefaultValues = false);
	static void SetColumnOrder(
			ColumnOrder eOrder,
			size_t idxColumn,
			std::wstring const& namedColumn,
			std::vector<long> const& inValues);
};
