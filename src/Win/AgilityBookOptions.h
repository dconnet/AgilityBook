#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-06-16 DRC Added options to remember date formats.
 * @li 2004-06-06 DRC Added additional clipboard formats.
 * @li 2004-05-16 DRC Added IsLevelVisible.
 * @li 2004-04-08 DRC Added general program options.
 * @li 2004-03-13 DRC Added GetViewHiddenTitles.
 * @li 2004-01-04 DRC Added GetImportExportDateFormat.
 * @li 2003-12-11 DRC Added options for import/export wizard.
 */

#include <set>
#include <string>
#include <vector>
#include "ARBDate.h"
class ARBConfigVenue;
class ARBDogRun;
class ARBDogTitle;
class ARBDogTrial;
class ARBTraining;

struct CFontInfo
{
	CString name;
	int size;
	bool italic;
	bool bold;
	void CreateFont(CFont& font, CDC* pDC = NULL);
	void CreateFont(CFontDialog const& dlg, CFont& font, CDC* pDC = NULL);
};

struct CVenueFilter
{
	std::string venue;
	std::string division;
	std::string level;
};

class CAgilityBookOptions
{
public:
	// Helper functions
	static bool IsFilterEnabled();
	static bool IsDateVisible(ARBDate const& startDate, ARBDate const& endDate);
	static bool IsTitleVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTitle const* pTitle);
	static bool IsVenueVisible(
		std::vector<CVenueFilter> const& venues,
		std::string const& venue);
	static bool IsTrialVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrial const* pTrial);
	static unsigned short IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun);
	static bool IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBConfigVenue const* pVenue,
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun);
	static bool IsTrainingLogVisible(
		std::set<std::string> const& names,
		ARBTraining const* pTraining);
	// Calendar options
	static bool AutoDeleteCalendarEntries();
	static void SetAutoDeleteCalendarEntries(bool bAuto);
	static bool ViewAllCalendarEntries();
	static void SetViewAllCalendarEntries(bool bView);
	static int DaysTillEntryIsPast();
	static void SetDaysTillEntryIsPast(int nDays);
	static bool HideOverlappingCalendarEntries();
	static void SetHideOverlappingCalendarEntries(bool bHide);
	static bool ViewCalendarAsList();
	static CSize GetCalendarEntrySize();
	static void SetCalendarEntrySize(CSize const& sz);
	// Common options
	static ARBDate::DayOfWeek GetFirstDayOfWeek();
	static void SetFirstDayOfWeek(ARBDate::DayOfWeek day);
	// Runs/points options
	static bool GetViewRunsByTrial();
	static void SetViewRunsByTrial(bool bView);
	static bool GetNewestDatesFirst();
	static void SetNewestDatesFirst(bool bNewest);
	static bool GetViewAllDates();
	static void SetViewAllDates(bool bViewAll);
	static ARBDate GetStartFilterDate();
	static void SetStartFilterDate(ARBDate const& date);
	static bool GetStartFilterDateSet();
	static void SetStartFilterDateSet(bool bSet);
	static ARBDate GetEndFilterDate();
	static void SetEndFilterDate(ARBDate const& date);
	static bool GetEndFilterDateSet();
	static void SetEndFilterDateSet(bool bSet);
	static bool GetViewAllVenues();
	static void SetViewAllVenues(bool bViewAll);
	static void GetFilterVenue(std::vector<CVenueFilter>& venues);
	static void SetFilterVenue(std::vector<CVenueFilter> const& venues);
	static bool GetViewAllRuns();
	static void SetViewAllRuns(bool bViewAll);
	static bool GetViewQRuns(); // Subset of AllRuns
	static void SetViewQRuns(bool bViewQs);
	static bool GetViewHiddenTitles();
	static void SetViewHiddenTitles(bool bSet);
	static bool GetTableInYPS();
	static void SetTableInYPS(bool bSet);
	// Training Log options
	static bool GetTrainingViewAllDates();
	static void SetTrainingViewAllDates(bool bViewAll);
	static ARBDate GetTrainingStartFilterDate();
	static void SetTrainingStartFilterDate(ARBDate const& date);
	static bool GetTrainingStartFilterDateSet();
	static void SetTrainingStartFilterDateSet(bool bSet);
	static ARBDate GetTrainingEndFilterDate();
	static void SetTrainingEndFilterDate(ARBDate const& date);
	static bool GetTrainingEndFilterDateSet();
	static void SetTrainingEndFilterDateSet(bool bSet);
	static bool GetTrainingViewAllNames();
	static void SetTrainingViewAllNames(bool bViewAll);
	static void GetTrainingFilterNames(std::set<std::string>& outNames);
	static void SetTrainingFilterNames(std::set<std::string> const& inNames);
	// Font options
	static void GetPrinterFontInfo(CFontInfo& info);
	static void SetPrinterFontInfo(CFontInfo const& info);
	static void GetCalendarDateFontInfo(CFontInfo& info, BOOL bPrinting);
	static void SetCalendarDateFontInfo(CFontInfo const& info, BOOL bPrinting);
	static void GetCalendarTextFontInfo(CFontInfo& info, BOOL bPrinting);
	static void SetCalendarTextFontInfo(CFontInfo const& info, BOOL bPrinting);
	// Last entered options
	static CString GetLastEnteredDivision();
	static void SetLastEnteredDivision(char const* inLast);
	static CString GetLastEnteredLevel();
	static void SetLastEnteredLevel(char const* inLast);
	static CString GetLastEnteredHeight();
	static void SetLastEnteredHeight(char const* inLast);
	static CString GetLastEnteredJudge();
	static void SetLastEnteredJudge(char const* inLast);
	static CString GetLastEnteredHandler();
	static void SetLastEnteredHandler(char const* inLast);
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
	static void GetImportExportDelimiters(bool bImport, int& delim, CString& delimiter);
	static void SetImportExportDelimiters(bool bImport, int delim, CString const& delimiter);
	static void GetImportExportDateFormat(bool bImport, ARBDate::DateFormat& outFormat);
	static void SetImportExportDateFormat(bool bImport, ARBDate::DateFormat inFormat);
	typedef enum
	{
		eUnknown	= 0x0000,
		eRunsImport	= 0x0001,
		eRunsExport	= 0x0002,
		eCalImport	= 0x0004,
		eCalExport	= 0x0008,
		eLogImport	= 0x0010,
		eLogExport	= 0x0020,
		eViewTree	= 0x0040,
		eViewRuns	= 0x0080,
		eViewCal	= 0x0100,
		eViewLog	= 0x0200,
	} ColumnOrder;
	// General program options
	static bool GetAutoUpdateCheck();
	static void SetAutoUpdateCheck(bool bSet);
	static bool GetAutoUpdateCheckDoc();
	static void SetAutoUpdateCheckDoc(bool bSet);
	static int GetNumBackupFiles();
	static void SetNumBackupFiles(int nFiles);
	static CString GetSplashImage();
	static void SetSplashImage(const CString& filename);
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
	static void SetDateFormat(FormattedDate inItem, ARBDate::DateFormat inFormat);

protected:
	friend class CDlgAssignColumns;
	// CDlgAssignColumns has some structs for used to ensure validity.
	static void GetColumnOrder(ColumnOrder eOrder, size_t idxColumn, std::vector<int>& outValues);
	static void SetColumnOrder(ColumnOrder eOrder, size_t idxColumn, std::vector<int> const& inValues);
public:
	// Special clipboard formats
	typedef enum
	{
		eFormatDog,
		eFormatTrial,
		eFormatRun,
		eFormatCalendar,
		eFormatLog,
	} eClipFormat;
	static UINT GetClipboardFormat(eClipFormat fmt);
};
