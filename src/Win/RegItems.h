#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Definitions of all configuration (registry) entries.
 * @author David Connet
 *
 * These are used almost exclusively in AgilityBookOptions, except as noted.
 * DW == dword, ST == string
 *
 * Revision History
 * 2010-04-11 Separated from AgilityBookOptions.cpp.
 * 2018-10-11 Separated common items into LibARBWin
 */

#include "LibARBWin/RegItemsBase.h"


namespace dconSoft
{

// clang-format off
// Appended to color keys when in darkmode
#define CFG_DARKMODE					L"Dark"
//Calendar
#define CFG_KEY_CALENDAR				L"Calendar"
//	DW CloseColor
#define CFG_CAL_ITEM_CLOSECOLOR				L"CloseColor"
//	DW EnteredColor
#define CFG_CAL_ITEM_ENTEREDCOLOR			L"EnteredColor"
//	DW NotEnteredColor
#define CFG_CAL_ITEM_NOTENTEREDCOLOR		L"NotEnteredColor"
//	DW OpenColor
#define CFG_CAL_ITEM_OPENCOLOR				L"OpenColor"
//	DW PastColor
#define CFG_CAL_ITEM_PASTCOLOR				L"PastColor"
//	DW PendingColor
#define CFG_CAL_ITEM_PENDINGCOLOR			L"PendingColor"
//	DW PlanningColor
#define CFG_CAL_ITEM_PLANNINGCOLOR			L"PlanningColor"
//	DW AutoDelete
#define CFG_CAL_AUTODELETE				CFG_KEY_CALENDAR L"/AutoDelete"
//	DW CloseNear
#define CFG_CAL_CLOSENEAR				CFG_KEY_CALENDAR L"/CloseNear"
//	DW CloseNearColor
#define CFG_CAL_CLOSENEARCOLOR			CFG_KEY_CALENDAR L"/CloseNearColor"
//	DW Filter (FilterOptions.cpp)
#define CFG_CAL_FILTER					CFG_KEY_CALENDAR L"/Filter"
//	DW FontTextBold
#define CFG_CAL_FONTTEXTBOLD			CFG_KEY_CALENDAR L"/FontTextBold"
//	DW FontTextItalic
#define CFG_CAL_FONTTEXTITALIC			CFG_KEY_CALENDAR L"/FontTextItalic"
//	ST FontTextName
#define CFG_CAL_FONTTEXTNAME			CFG_KEY_CALENDAR L"/FontTextName"
//	DW FontTextSize
#define CFG_CAL_FONTTEXTSIZE			CFG_KEY_CALENDAR L"/FontTextSize"
//	DW HideOverlapping
#define CFG_CAL_HIDEOVERLAPPING			CFG_KEY_CALENDAR L"/HideOverlapping"
//	DW OpenNear
#define CFG_CAL_OPENNEAR				CFG_KEY_CALENDAR L"/OpenNear"
//	DW OpenNearColor
#define CFG_CAL_OPENNEARCOLOR			CFG_KEY_CALENDAR L"/OpenNearColor"
//	DW PastEntry
#define CFG_CAL_PASTENTRY				CFG_KEY_CALENDAR L"/PastEntry"
//	DW ViewAll
#define CFG_CAL_VIEWALL					CFG_KEY_CALENDAR L"/ViewAll"
//	DW ViewClose
#define CFG_CAL_VIEWCLOSE				CFG_KEY_CALENDAR L"/ViewClose"
//	DW ViewOpen
#define CFG_CAL_VIEWOPEN				CFG_KEY_CALENDAR L"/ViewOpen"
//	Obsolete(1.7.6.12) DW EntrySize.cx
//	Obsolete(1.7.6.12) DW EntrySize.cy
//	Obsolete DW FontDateBold
//	Obsolete DW FontDateItalic
//	Obsolete ST FontDateName
//	Obsolete DW FontDateSize
//	Obsolete DW PrintFontDateBold
//	Obsolete DW PrintFontDateItalic
//	Obsolete ST PrintFontDateName
//	Obsolete DW PrintFontDateSize
//	Obsolete DW PrintFontTextBold
//	Obsolete DW PrintFontTextItalic
//	Obsolete ST PrintFontTextName
//	Obsolete DW PrintFontTextSize

//Columns
#define CFG_KEY_COLUMNS					L"Columns"
//	ST col[n]
inline wxString CFG_COL_BASENAME(size_t n)
{
	return wxString::Format(L"col%zu", n);
}

//ColumnInfo (NamedColumns.cpp)
#define CFG_KEY_COLUMNINFO				L"ColumnInfo"
//	ST CurrentConfig (NamedColumns.cpp)
#define CFG_CI_CURRENTCONFIG			CFG_KEY_COLUMNINFO L"/CurrentConfig"
//	DW numConfigs (NamedColumns.cpp)
#define CFG_CI_NUMCONFIGS				CFG_KEY_COLUMNINFO L"/numConfigs"
//	Config[n] (NamedColumns.cpp)
#define CFG_KEY_CONFIG					L"Config"
inline wxString CFG_CI_KEY_CONFIG(int n, bool bIncSep = true)
{
	wxString str;
	str << CFG_KEY_COLUMNINFO  << L"/" << CFG_KEY_CONFIG << n << (bIncSep ? L"/" : L"");
	return str;
}
//		ST name (NamedColumns.cpp)
#define CFG_CI_CONFIG_NAME					L"name"
//		[key] (NamedColumns.cpp)
//			ST col[n] (via CAgilityBookOptions::*etColumnOrder)

//Common
#define CFG_KEY_COMMON					L"Common"
//	DW CRCDImage
#define CFG_COMMON_CRCDIMAGE			CFG_KEY_COMMON L"/CRCDImage"
//	ST CurrentFilter (FilterOptions.cpp)
#define CFG_COMMON_CURRENTFILTER		CFG_KEY_COMMON L"/CurrentFilter"
//	DW EndFilter (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTER			CFG_KEY_COMMON L"/EndFilter"
//	DW EndFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTERJDAY		CFG_KEY_COMMON L"/EndFilterJDay"
//	ST FilterTrainingNames (FilterOptions.cpp)
#define CFG_COMMON_FILTERTRAININGNAMES	CFG_KEY_COMMON L"/FilterTrainingNames"
//	ST FilterVenue (FilterOptions.cpp)
#define CFG_COMMON_FILTERVENUE			CFG_KEY_COMMON L"/FilterVenue"
//	DW FirstDayOfWeek
#define CFG_COMMON_FIRSTDAYOFWEEK		CFG_KEY_COMMON L"/FirstDayOfWeek"
//	DW UnitsAsMM
#define CFG_COMMON_UNITSASMM			CFG_KEY_COMMON L"/UnitsAsMM"
//	DW Margins.MM
#define CFG_COMMON_MARGINS_MM			CFG_KEY_COMMON L"/Margins.MM"
//	DW Margins.B
#define CFG_COMMON_MARGINS_B			CFG_KEY_COMMON L"/Margins.B"
//	DW Margins.L
#define CFG_COMMON_MARGINS_L			CFG_KEY_COMMON L"/Margins.L"
//	DW Margins.R
#define CFG_COMMON_MARGINS_R			CFG_KEY_COMMON L"/Margins.R"
//	DW Margins.T
#define CFG_COMMON_MARGINS_T			CFG_KEY_COMMON L"/Margins.T"
//	DW RunPage.MM
#define CFG_COMMON_RUNPAGE_MM			CFG_KEY_COMMON L"/RunPage.MM"
//	DW RunPage.W
#define CFG_COMMON_RUNPAGE_W			CFG_KEY_COMMON L"/RunPage.W"
//	DW RunPage.H
#define CFG_COMMON_RUNPAGE_H			CFG_KEY_COMMON L"/RunPage.H"
//	DW numFilters (FilterOptions.cpp)
#define CFG_COMMON_NUMFILTERS			CFG_KEY_COMMON L"/numFilters"
//	DW PrintFontListBold
#define CFG_COMMON_PRINTFONTLISTBOLD	CFG_KEY_COMMON L"/PrintFontListBold"
//	DW PrintFontListItalic
#define CFG_COMMON_PRINTFONTLISTITALIC	CFG_KEY_COMMON L"/PrintFontListItalic"
//	ST PrintFontListName
#define CFG_COMMON_PRINTFONTLISTNAME	CFG_KEY_COMMON L"/PrintFontListName"
//	DW PrintFontListSize
#define CFG_COMMON_PRINTFONTLISTSIZE	CFG_KEY_COMMON L"/PrintFontListSize"
//	DW ShowCoSanction
#define CFG_COMMON_SHOWCOSANCTION		CFG_KEY_COMMON L"/ShowCoSanction"
//	DW sortPtVw1
#define CFG_COMMON_SORTPTVW1			CFG_KEY_COMMON L"/sortPtVw1"
//	DW sortPtVw2
#define CFG_COMMON_SORTPTVW2			CFG_KEY_COMMON L"/sortPtVw2"
//	DW sortPtVw3
#define CFG_COMMON_SORTPTVW3			CFG_KEY_COMMON L"/sortPtVw3"
//	DW StartFilter (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTER			CFG_KEY_COMMON L"/StartFilter"
//	DW StartFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTERJDAY		CFG_KEY_COMMON L"/StartFilterJDay"
//	DW TableInYPS
#define CFG_COMMON_TABLEINYPS			CFG_KEY_COMMON L"/TableInYPS"
//	DW RunTimeInOPS
#define CFG_COMMON_RUNTIMEINOPS			CFG_KEY_COMMON L"/RunTimeInOPS"
//	DW ViewAllDates (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLDATES			CFG_KEY_COMMON L"/ViewAllDates"
//	DW ViewAllNames (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLNAMES			CFG_KEY_COMMON L"/ViewAllNames"
//	DW ViewAllVenues (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLVENUES		CFG_KEY_COMMON L"/ViewAllVenues"
//	DW ViewHiddenTitles
#define CFG_COMMON_VIEWHIDDENTITLES		CFG_KEY_COMMON L"/ViewHiddenTitles"
//	DW ViewLifetimeEvents
#define CFG_COMMON_VIEWLIFETIMEEVENTS	CFG_KEY_COMMON L"/ViewLifetimeEvents"
//	DW ViewNewestFirst
#define CFG_COMMON_VIEWNEWESTFIRST		CFG_KEY_COMMON L"/ViewNewestFirst"
//	DW ViewRuns (FilterOptions.cpp)
#define CFG_COMMON_VIEWRUNS				CFG_KEY_COMMON L"/ViewRuns"
//	DW ViewRunsByTrial
#define CFG_COMMON_VIEWRUNSBYTRIAL		CFG_KEY_COMMON L"/ViewRunsByTrial"
//	Obsolete DW TrainingEndFilter
//	Obsolete DW TrainingEndFilterJDay
//	Obsolete DW TrainingStartFilter
//	Obsolete DW TrainingStartFilterJDay
//	Obsolete DW TrainingViewAllDates
//	Obsolete DW ViewAllRuns
//	Obsolete DW ViewQRuns

//Export
#define CFG_KEY_EXPORT					L"Export"
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat
#define CFG_IMPORT_EXPORT_DATEFORMAT		L"dateformat"
//	DW delim
#define CFG_IMPORT_EXPORT_DELIM				L"delim"
//	ST delimiter
#define CFG_IMPORT_EXPORT_DELIMITER			L"delimiter"

//ExportCal
#define CFG_KEY_EXPORTCAL				L"ExportCal"
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalAppt
#define CFG_KEY_EXPORTCALAPPT			L"ExportCalAppt"
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalTask
#define CFG_KEY_EXPORTCALTASK			L"ExportCalTask"
//	ST col[n] (CFG_COL_BASENAME)

//ExportLog
#define CFG_KEY_EXPORTLOG				L"ExportLog"
//	ST col[n] (CFG_COL_BASENAME)

//Filter[number]
#define CFG_KEY_FILTER					L"Filter"
inline wxString CFG_KEY_FILTER_N(int n, bool bIncSep = true)
{
	wxString str;
	str << CFG_KEY_FILTER << n << (bIncSep ? L"/" : L"");
	return str;
}
//	DW AllDates (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLDATES			L"AllDates"
//	DW AllNames (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLNAMES			L"AllNames"
//	DW AllVenues (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLVENUES			L"AllVenues"
//	DW Cal (FilterOptions.cpp)
#define CFG_FILTER_ITEM_CAL					L"Cal"
//	DW End (FilterOptions.cpp)
#define CFG_FILTER_ITEM_END					L"End"
//	DW EndJDay (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ENDJDAY				L"EndJDay"
//	ST FilterNames (FilterOptions.cpp)
#define CFG_FILTER_ITEM_FILTERNAMES			L"FilterNames"
//	ST FilterVenue (FilterOptions.cpp)
#define CFG_FILTER_ITEM_FILTERVENUE			L"FilterVenue"
//	ST Name (FilterOptions.cpp)
#define CFG_FILTER_ITEM_NAME				L"Name"
//	DW Start (FilterOptions.cpp)
#define CFG_FILTER_ITEM_START				L"Start"
//	DW StartJDay (FilterOptions.cpp)
#define CFG_FILTER_ITEM_STARTJDAY			L"StartJDay"
//	DW ViewRuns (FilterOptions.cpp)
#define CFG_FILTER_ITEM_VIEWRUNS			L"ViewRuns"

//Import
#define CFG_KEY_IMPORT					L"Import"
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat (CFG_IMPORT_EXPORT_DATEFORMAT)
//	DW delim (CFG_IMPORT_EXPORT_DELIM)
//	ST delimiter (CFG_IMPORT_EXPORT_DELIMITER)
//	DW row
#define CFG_IMPORT_ROW					CFG_KEY_IMPORT L"/row"

//ImportCal
#define CFG_KEY_IMPORTCAL				L"ImportCal"
//	ST col[n] (CFG_COL_BASENAME)

//ImportLog
#define CFG_KEY_IMPORTLOG				L"ImportLog"
//	ST col[n] (CFG_COL_BASENAME)

//Last
#define CFG_KEY_LAST					L"Last"
//	ST Division
#define CFG_LAST_DIVISION				CFG_KEY_LAST L"/Division"
//	ST Handler
#define CFG_LAST_HANDLER				CFG_KEY_LAST L"/Handler"
//	ST Height
#define CFG_LAST_HEIGHT					CFG_KEY_LAST L"/Height"
//	ST Judge
#define CFG_LAST_JUDGE					CFG_KEY_LAST L"/Judge"
//	ST Level
#define CFG_LAST_LEVEL					CFG_KEY_LAST L"/Level"
//	ST RefHeight
#define CFG_LAST_REFHEIGHT				CFG_KEY_LAST L"/RefHeight"

//Last: "Last/<item>/dogname/<venue>"
// See related item above
#define CFG_LAST_DIVISION_KEY			CFG_KEY_LAST L"/Divisions"
#define CFG_LAST_HEIGHT_KEY				CFG_KEY_LAST L"/Heights"
#define CFG_LAST_LEVEL_KEY				CFG_KEY_LAST L"/Levels"
//Last: "Last/<item>/dogname"
#define CFG_LAST_HANDLER_KEY			CFG_KEY_LAST L"/Handlers"

//Selection:
#define CFG_KEY_SELECTION				L"Selection"
//	DW nDogs (DlgSelectDog.cpp)
#define CFG_SELECTION_NDOGS				CFG_KEY_SELECTION L"/nDogs"
//	ST <dog name> (DlgSelectDog.cpp)
inline wxString CFG_SELECTION_DOG(int n)
{
	return wxString::Format(CFG_KEY_SELECTION L"/Dog%d", n);
}

//Settings:
#define CFG_KEY_SETTINGS				L"Settings"
//	DW autoCheck
#define CFG_SETTINGS_AUTOCHECK			CFG_KEY_SETTINGS L"/autoCheck"
//	DW autoCheck
#define CFG_SETTINGS_AUTOCHECK_INTERVAL	CFG_KEY_SETTINGS L"/autoCheckInterval"
//	DW autoShowTitle
#define CFG_SETTINGS_AUTOSHOWTITLE		CFG_KEY_SETTINGS L"/autoShowTitle"
//	DW enableDarkMode
#define CFG_SETTINGS_ENABLEDARKMODE		CFG_KEY_SETTINGS L"/enableDarkMode"
//	DW useAltRowColor
#define CFG_SETTINGS_USEALTROWCOLOR		CFG_KEY_SETTINGS L"/useAltRowColor"
//	DW BackupFiles
#define CFG_SETTINGS_BACKUPFILES		CFG_KEY_SETTINGS L"/BackupFiles"
//	ST BackupDir
#define CFG_SETTINGS_BACKUPDIR			CFG_KEY_SETTINGS L"/BackupDir"
//	DW isLocal (defined in SetupApp.cpp)
//#define CFG_SETTINGS_ISLOCAL			CFG_KEY_SETTINGS L"/isLocal"
//	Obsolete(2.0.0.0) DW Lang (LanguageManager.cpp)
#define CFG_SETTINGS_LANG				CFG_KEY_SETTINGS L"/Lang"
//	Obsolete(2.1.0.0) DW Lang2 [added 2.0.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG2				CFG_KEY_SETTINGS L"/Lang2"
//	ST Lang3 [added 2.1.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG3				CFG_KEY_SETTINGS L"/Lang3"
//	DW lastCX (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCX				CFG_KEY_SETTINGS L"/lastCX"
//	DW lastCY (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCY				CFG_KEY_SETTINGS L"/lastCY"
//	ST LastDog (AgilityBookDoc.cpp/AgilityBookTreeView.cpp)
#define CFG_SETTINGS_LASTDOG			CFG_KEY_SETTINGS L"/LastDog"
//	ST LastFile (AgilityBook.cpp/AgilityBookDoc.cpp)
#define CFG_SETTINGS_LASTFILE			CFG_KEY_SETTINGS L"/LastFile"
//	DW lastState (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTSTATE			CFG_KEY_SETTINGS L"/lastState"
//	ST lastVerCheck (AgilityBook.cpp/UpdateInfo.cpp)
#define CFG_SETTINGS_LASTVERCHECK		CFG_KEY_SETTINGS L"/lastVerCheck"
//	DW lastXpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTXPOS_OLD		CFG_KEY_SETTINGS L"/lastXpos"
#define CFG_SETTINGS_LASTXPOS			CFG_KEY_SETTINGS L"/lastXpos2"
//	DW lastYpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTYPOS_OLD		CFG_KEY_SETTINGS L"/lastYpos"
#define CFG_SETTINGS_LASTYPOS			CFG_KEY_SETTINGS L"/lastYpos2"
//  DW MRUsize (AgilityBookOptions.cpp)
#define CFG_SETTINGS_MRU				CFG_KEY_SETTINGS L"/MRUsize"
//	DW printLand (AgilityBook.cpp)
#define CFG_SETTINGS_PRINTLAND			CFG_KEY_SETTINGS L"/printLand"
//	ST proxy
#define CFG_SETTINGS_PROXY				CFG_KEY_SETTINGS L"/proxy"
//	DW splitCX (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX			CFG_KEY_SETTINGS L"/splitCX"
//	DW splitCX2 (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX2			CFG_KEY_SETTINGS L"/splitCX2"
//	DW useproxy
#define CFG_SETTINGS_USEPROXY			CFG_KEY_SETTINGS L"/useproxy"
//	DW View (TabView.cpp)
#define CFG_SETTINGS_VIEW				CFG_KEY_SETTINGS L"/View"
//	DW ViewOrient (TabView.cpp)
#define CFG_SETTINGS_VIEWORIENT			CFG_KEY_SETTINGS L"/ViewOrient"
//	DW ViewType (TabView.cpp)
#define CFG_SETTINGS_VIEWTYPE			CFG_KEY_SETTINGS L"/ViewType"
//	Obsolete(3.3.3) DW showHtml
//	Obsolete(2.1.0.0) DW dateFormat[n]
//		{
//			eRunTree	= 0,
//			eRunList	= 1,
//			ePoints		= 2,
//			eCalList	= 3,
//			eCalendar	= 4,
//			eTraining	= 5,
//		}
//	Obsolete(2.0.0.0) DW ShowSplash
//	Obsolete(2.0.0.0) ST Splash

//	DW Calendar (AgilityBookCalendarListView.cpp)
#define CFG_SORTING_CALENDAR			CFG_KEY_SORTING L"/Calendar"
//	DW Runs (AgilityBookRunsView.cpp)
#define CFG_SORTING_RUNS				CFG_KEY_SORTING L"/Runs"
//	DW RunsEnabled (AgilityBookRunsView.cpp)
#define CFG_SORTING_RUNS_ENABLED		CFG_KEY_SORTING L"/RunsEnabled"
//	DW Training (AgilityBookTrainingView.cpp)
#define CFG_SORTING_TRAINING			CFG_KEY_SORTING L"/Training"

//UserNames:
#define CFG_KEY_USERNAMES				L"UserNames"
//	ST <hint>

//Unknown
#define CFG_KEY_UNKNOWN					L"Unknown"
//	ST col[n] (CFG_COL_BASENAME)

// clang-format on

} // namespace dconSoft
