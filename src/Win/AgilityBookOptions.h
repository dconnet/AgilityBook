#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * 2020-01-27 Add alternate row color setting.
 * 2013-05-19 Make last div/level/height/handler context aware.
 * 2010-03-28 Added ability to import/export program settings.
 * 2009-07-19 Implement proxy support.
 * 2008-12-24 Ported to wxWidgets.
 * 2007-08-03 Added UserNames
 * 2006-07-16 Added ARBPointsViewSort
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-05-04 Added IncludeCRCDImage
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2004-08-31 Added AutoShowSplashScreen
 * 2004-06-16 Added options to remember date formats.
 * 2004-06-06 Added additional clipboard formats.
 * 2004-05-16 Added IsLevelVisible.
 * 2004-04-08 Added general program options.
 * 2004-03-13 Added GetViewHiddenTitles.
 * 2004-01-04 Added GetImportExportDateFormat.
 * 2003-12-11 Added options for import/export wizard.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
#include <set>
#include <vector>

namespace dconSoft
{
namespace ARBCommon
{
class CVersionNum;
} // namespace ARBCommon


enum class ARBCalColorItem
{
	Past,
	NotEntered,
	Planning,
	Pending,
	Entered,
	Opening,
	Closing,
};
enum class ARBViewRuns
{
	// Fixed values as they are stored in the registry
	RunsByTrial = 1,
	RunsByList = 0,
	AllRunsByList = 2,
};
enum class ARBPointsViewSort
{
	// Fixed values as they are stored in the registry
	Unknown = 0,
	Division = 1,
	Level = 2,
	Event = 3
};
enum class ARBImportExportDelim
{
	// Fixed values as they are stored in the registry
	Tab = 1,
	Space = 2,
	Colon = 3,
	Semicolon = 4,
	Comma = 5,
	Other = 6
};


struct CFontInfo
{
	std::wstring name;
	int size;
	bool italic;
	bool bold;
	CFontInfo()
		: name()
		, size(0)
		, italic(false)
		, bold(false)
	{
	}
	void CreateFont(wxFont& font);
	void CreateFont(wxFontDialog& dlg, wxFont& font);
};


class CAgilityBookOptions
{
public:
	static bool ImportSettings(ARBCommon::ElementNodePtr const& inTree);
	static ARBCommon::ElementNodePtr ExportSettings();

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
	static wxColour CalendarColor(ARBCalColorItem inItem);
	static void SetCalendarColor(ARBCalColorItem inItem, wxColour inColor);
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
	static ARBCommon::ARBDayOfWeek GetFirstDayOfWeek();
	static void SetFirstDayOfWeek(ARBCommon::ARBDayOfWeek day);
	// Runs/points options
	static ARBViewRuns GetViewRunsStyle();
	static void SetViewRunsStyle(ARBViewRuns style);
	static bool GetNewestDatesFirst();
	static void SetNewestDatesFirst(bool bNewest);
	static void GetPointsViewSort(
		ARBPointsViewSort& outPrimary,
		ARBPointsViewSort& outSecondary,
		ARBPointsViewSort& outTertiary);
	static void SetPointsViewSort(
		ARBPointsViewSort inPrimary,
		ARBPointsViewSort inSecondary,
		ARBPointsViewSort inTertiary);
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
	static bool ShowCoSanctioning();
	static void SetShowCoSanctioning(bool bSet);
	// Font options
	static void GetPrinterFontInfo(CFontInfo& info);
	static void SetPrinterFontInfo(CFontInfo const& info);
	static bool GetUnitsAsMM();
	static void SetUnitsAsMM(bool bAsMM);
	// Get margins in .01 inches or MM (or pixels, if DC is set)
	static void GetPrinterMargins(
		bool bAsMM, // In .01in or millimeters, ignored if DC is set
		long& outLeft,
		long& outRight,
		long& outTop,
		long& outBottom,
#if wxCHECK_VERSION(3, 3, 0)
		wxReadOnlyDC const* pDC
#else
		wxDC const* pDC
#endif
	); // Used to convert to logical units
	static void SetPrinterMargins(
		bool bAsMM, // In .01 inches or millimeters
		long inLeft,
		long inRight,
		long inTop,
		long inBottom);
	static void GetRunPageSize(
		bool bAsMM, // In .01in or millimeters, ignored if DC is set
		long& outWidth,
		long& outHeight,
#if wxCHECK_VERSION(3, 3, 0)
		wxReadOnlyDC const* pDC
#else
		wxDC const* pDC
#endif
	); // Used to convert to logical units
	static void SetRunPageSize(
		bool bAsMM, // In .01 inches or millimeters
		long inWidth,
		long inHeight);
	static void GetCalendarFontInfo(CFontInfo& info);
	static void SetCalendarFontInfo(CFontInfo const& info);
	// Last entered options
	static std::wstring GetLastEnteredDivision(ARB::ARBDogPtr const& inDog, ARB::ARBConfigVenuePtr const& inVenue);
	static void SetLastEnteredDivision(
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		wchar_t const* inLast);
	static std::wstring GetLastEnteredLevel(ARB::ARBDogPtr const& inDog, ARB::ARBConfigVenuePtr const& inVenue);
	static void SetLastEnteredLevel(
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		wchar_t const* inLast);
	static std::wstring GetLastEnteredHeight(ARB::ARBDogPtr const& inDog, ARB::ARBConfigVenuePtr const& inVenue);
	static void SetLastEnteredHeight(
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		wchar_t const* inLast);
	static std::wstring GetLastEnteredRefHeight();
	static void SetLastEnteredRefHeight(wchar_t const* inLast);
	static std::wstring GetLastEnteredJudge();
	static void SetLastEnteredJudge(wchar_t const* inLast);
	static std::wstring GetLastEnteredHandler(ARB::ARBDogPtr const& inDog);
	static void SetLastEnteredHandler(ARB::ARBDogPtr const& inDog, wchar_t const* inLast);
	static void CleanLastItems(std::wstring const& callName);
	static void CleanLastItems(std::wstring const& oldCallName, std::wstring const& newCallName);
	static void CleanLastItems(ARB::ARBConfig const& inConfig);
	// Import/Export options
	static long GetImportStartRow();
	static void SetImportStartRow(long row);
	static void GetImportExportDelimiters(bool bImport, ARBImportExportDelim& delim, std::wstring& delimiter);
	static void SetImportExportDelimiters(bool bImport, ARBImportExportDelim delim, std::wstring const& delimiter);
	static void GetImportExportDateFormat(bool bImport, ARBCommon::ARBDateFormat& outFormat);
	static void SetImportExportDateFormat(bool bImport, ARBCommon::ARBDateFormat inFormat);
	enum ColumnOrder
	{
		// clang-format off
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
		// clang-format on
	};
	// General program options
	static long GetMRUFileCount();
	static void SetMRUFileCount(long nFiles);
	static bool GetAutoUpdateCheck();
	static void SetAutoUpdateCheck(bool bSet);
	static long GetAutoUpdateCheckInterval();
	static void SetAutoUpdateCheckInterval(long days);
	static long GetNumBackupFiles();
	static void SetNumBackupFiles(long nFiles);
	static wxString GetBackupDirectory();
	static void SetBackupDirectory(wxString const& dir);
	static bool AutoShowPropertiesOnNewTitle();
	static void AutoShowPropertiesOnNewTitle(bool bShow);
	static bool UseAlternateRowColor();
	static void SetUseAlternateRowColor(bool bUse);
	// Internet things
	// -username/pw for accessing URLs thru ReadHTTP.cpp
	static std::wstring GetUserName(std::wstring const& hint);
	static void SetUserName(std::wstring const& hint, std::wstring const& userName);
	static bool GetUseProxy();
	static void SetUseProxy(bool inUseProxy);
	static std::wstring GetProxyServer();
	static void SetProxyServer(std::wstring const& inProxy);
	static std::wstring GetProxy();

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

} // namespace dconSoft
