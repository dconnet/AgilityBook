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
 */


//Calendar
#define CFG_KEY_CALENDAR				L"Calendar/"
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
#define CFG_CAL_AUTODELETE				L"Calendar/AutoDelete"
//	DW CloseNear
#define CFG_CAL_CLOSENEAR				L"Calendar/CloseNear"
//	DW CloseNearColor
#define CFG_CAL_CLOSENEARCOLOR			L"Calendar/CloseNearColor"
//	DW Filter (FilterOptions.cpp)
#define CFG_CAL_FILTER					L"Calendar/Filter"
//	DW FontTextBold
#define CFG_CAL_FONTTEXTBOLD			L"Calendar/FontTextBold"
//	DW FontTextItalic
#define CFG_CAL_FONTTEXTITALIC			L"Calendar/FontTextItalic"
//	ST FontTextName
#define CFG_CAL_FONTTEXTNAME			L"Calendar/FontTextName"
//	DW FontTextSize
#define CFG_CAL_FONTTEXTSIZE			L"Calendar/FontTextSize"
//	DW HideOverlapping
#define CFG_CAL_HIDEOVERLAPPING			L"Calendar/HideOverlapping"
//	DW OpenNear
#define CFG_CAL_OPENNEAR				L"Calendar/OpenNear"
//	DW OpenNearColor
#define CFG_CAL_OPENNEARCOLOR			L"Calendar/OpenNearColor"
//	DW PastEntry
#define CFG_CAL_PASTENTRY				L"Calendar/PastEntry"
//	DW ViewAll
#define CFG_CAL_VIEWALL					L"Calendar/ViewAll"
//	DW ViewClose
#define CFG_CAL_VIEWCLOSE				L"Calendar/ViewClose"
//	DW ViewOpen
#define CFG_CAL_VIEWOPEN				L"Calendar/ViewOpen"
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

//CalSites
#define CFG_KEY_CALSITES				L"CalSites/"
//	DW (DLL names in EXE directory)

//CalSites2 - used for permanently disabling a version
#define CFG_KEY_CALSITES2				L"CalSites2/"
//	ST (DLL names in EXE directory)

//Columns
#define CFG_KEY_COLUMNS					L"Columns/"
//	ST col[n]
inline wxString CFG_COL_BASENAME(size_t n)
{
	return wxString::Format(L"col%d", static_cast<int>(n));
}

//ColumnInfo (NamedColumns.cpp)
//	ST CurrentConfig (NamedColumns.cpp)
#define CFG_CI_CURRENTCONFIG			L"ColumnInfo/CurrentConfig"
//	DW numConfigs (NamedColumns.cpp)
#define CFG_CI_NUMCONFIGS				L"ColumnInfo/numConfigs"
//	Config[n] (NamedColumns.cpp)
inline wxString CFG_CI_KEY_CONFIG(int n, bool bIncSep = true)
{
	return bIncSep ? wxString::Format(L"ColumnInfo/Config%d/", n) : wxString::Format(L"ColumnInfo/Config%d", n);
}
//		ST name (NamedColumns.cpp)
#define CFG_CI_CONFIG_NAME					L"name"
//		[key] (NamedColumns.cpp)
//			ST col[n] (via CAgilityBookOptions::*etColumnOrder)

//Common
//	DW CRCDImage
#define CFG_COMMON_CRCDIMAGE			L"Common/CRCDImage"
//	ST CurrentFilter (FilterOptions.cpp)
#define CFG_COMMON_CURRENTFILTER		L"Common/CurrentFilter"
//	DW EndFilter (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTER			L"Common/EndFilter"
//	DW EndFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTERJDAY		L"Common/EndFilterJDay"
//	ST FilterTrainingNames (FilterOptions.cpp)
#define CFG_COMMON_FILTERTRAININGNAMES	L"Common/FilterTrainingNames"
//	ST FilterVenue (FilterOptions.cpp)
#define CFG_COMMON_FILTERVENUE			L"Common/FilterVenue"
//	DW FirstDayOfWeek
#define CFG_COMMON_FIRSTDAYOFWEEK		L"Common/FirstDayOfWeek"
//	DW UnitsAsMM
#define CFG_COMMON_UNITSASMM			L"Common/UnitsAsMM"
//	DW Margins.MM
#define CFG_COMMON_MARGINS_MM			L"Common/Margins.MM"
//	DW Margins.B
#define CFG_COMMON_MARGINS_B			L"Common/Margins.B"
//	DW Margins.L
#define CFG_COMMON_MARGINS_L			L"Common/Margins.L"
//	DW Margins.R
#define CFG_COMMON_MARGINS_R			L"Common/Margins.R"
//	DW Margins.T
#define CFG_COMMON_MARGINS_T			L"Common/Margins.T"
//	DW RunPage.MM
#define CFG_COMMON_RUNPAGE_MM			L"Common/RunPage.MM"
//	DW RunPage.W
#define CFG_COMMON_RUNPAGE_W			L"Common/RunPage.W"
//	DW RunPage.H
#define CFG_COMMON_RUNPAGE_H			L"Common/RunPage.H"
//	DW numFilters (FilterOptions.cpp)
#define CFG_COMMON_NUMFILTERS			L"Common/numFilters"
//	DW PrintFontListBold
#define CFG_COMMON_PRINTFONTLISTBOLD	L"Common/PrintFontListBold"
//	DW PrintFontListItalic
#define CFG_COMMON_PRINTFONTLISTITALIC	L"Common/PrintFontListItalic"
//	ST PrintFontListName
#define CFG_COMMON_PRINTFONTLISTNAME	L"Common/PrintFontListName"
//	DW PrintFontListSize
#define CFG_COMMON_PRINTFONTLISTSIZE	L"Common/PrintFontListSize"
//	DW sortPtVw1
#define CFG_COMMON_SORTPTVW1			L"Common/sortPtVw1"
//	DW sortPtVw2
#define CFG_COMMON_SORTPTVW2			L"Common/sortPtVw2"
//	DW sortPtVw3
#define CFG_COMMON_SORTPTVW3			L"Common/sortPtVw3"
//	DW StartFilter (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTER			L"Common/StartFilter"
//	DW StartFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTERJDAY		L"Common/StartFilterJDay"
//	DW TableInYPS
#define CFG_COMMON_TABLEINYPS			L"Common/TableInYPS"
//	DW RunTimeInOPS
#define CFG_COMMON_RUNTIMEINOPS			L"Common/RunTimeInOPS"
//	DW ViewAllDates (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLDATES			L"Common/ViewAllDates"
//	DW ViewAllNames (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLNAMES			L"Common/ViewAllNames"
//	DW ViewAllVenues (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLVENUES		L"Common/ViewAllVenues"
//	DW ViewHiddenTitles
#define CFG_COMMON_VIEWHIDDENTITLES		L"Common/ViewHiddenTitles"
//	DW ViewLifetimeEvents
#define CFG_COMMON_VIEWLIFETIMEEVENTS	L"Common/ViewLifetimeEvents"
//	DW ViewNewestFirst
#define CFG_COMMON_VIEWNEWESTFIRST		L"Common/ViewNewestFirst"
//	DW ViewRuns (FilterOptions.cpp)
#define CFG_COMMON_VIEWRUNS				L"Common/ViewRuns"
//	DW ViewRunsByTrial
#define CFG_COMMON_VIEWRUNSBYTRIAL		L"Common/ViewRunsByTrial"
//	Obsolete DW TrainingEndFilter
//	Obsolete DW TrainingEndFilterJDay
//	Obsolete DW TrainingStartFilter
//	Obsolete DW TrainingStartFilterJDay
//	Obsolete DW TrainingViewAllDates
//	Obsolete DW ViewAllRuns
//	Obsolete DW ViewQRuns

//Export
#define CFG_KEY_EXPORT					L"Export/"
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat
#define CFG_IMPORT_EXPORT_DATEFORMAT		L"dateformat"
//	DW delim
#define CFG_IMPORT_EXPORT_DELIM				L"delim"
//	ST delimiter
#define CFG_IMPORT_EXPORT_DELIMITER			L"delimiter"

//ExportCal
#define CFG_KEY_EXPORTCAL				L"ExportCal/"
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalAppt
#define CFG_KEY_EXPORTCALAPPT			L"ExportCalAppt/"
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalTask
#define CFG_KEY_EXPORTCALTASK			L"ExportCalTask/"
//	ST col[n] (CFG_COL_BASENAME)

//ExportLog
#define CFG_KEY_EXPORTLOG				L"ExportLog/"
//	ST col[n] (CFG_COL_BASENAME)

//Filter[number]
inline wxString CFG_KEY_FILTER(int n, bool bIncSep = true)
{
	return bIncSep ? wxString::Format(L"Filter%d/", n) : wxString::Format(L"Filter%d", n);
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
#define CFG_KEY_IMPORT					L"Import/"
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat (CFG_IMPORT_EXPORT_DATEFORMAT)
//	DW delim (CFG_IMPORT_EXPORT_DELIM)
//	ST delimiter (CFG_IMPORT_EXPORT_DELIMITER)
//	DW row
#define CFG_IMPORT_ROW					L"Import/row"

//ImportCal
#define CFG_KEY_IMPORTCAL				L"ImportCal/"
//	ST col[n] (CFG_COL_BASENAME)

//ImportLog
#define CFG_KEY_IMPORTLOG				L"ImportLog/"
//	ST col[n] (CFG_COL_BASENAME)

//Last
//	ST Division
#define CFG_LAST_DIVISION				L"Last/Division"
//	ST Handler
#define CFG_LAST_HANDLER				L"Last/Handler"
//	ST Height
#define CFG_LAST_HEIGHT					L"Last/Height"
//	ST Judge
#define CFG_LAST_JUDGE					L"Last/Judge"
//	ST Level
#define CFG_LAST_LEVEL					L"Last/Level"
//	ST RefHeight
#define CFG_LAST_REFHEIGHT				L"Last/RefHeight"

//Last: "Last/<item>/dogname/<venue>"
// See related item above
#define CFG_LAST_DIVISION_KEY			L"Last/Divisions"
#define CFG_LAST_HEIGHT_KEY				L"Last/Heights"
#define CFG_LAST_LEVEL_KEY				L"Last/Levels"
//Last: "Last/<item>/dogname"
#define CFG_LAST_HANDLER_KEY			L"Last/Handlers"

//Selection:
//	DW nDogs (DlgSelectDog.cpp)
#define CFG_SELECTION_NDOGS				L"Selection/nDogs"
//	ST <dog name> (DlgSelectDog.cpp)
inline wxString CFG_SELECTION_DOG(int n)
{
	return wxString::Format(L"Selection/Dog%d", n);
}

//Settings:
//	DW autoCheck
#define CFG_SETTINGS_AUTOCHECK			L"Settings/autoCheck"
//	DW autoShowTitle
#define CFG_SETTINGS_AUTOSHOWTITLE		L"Settings/autoShowTitle"
//	DW BackupFiles
#define CFG_SETTINGS_BACKUPFILES		L"Settings/BackupFiles"
//	DW isLocal (defined in SetupApp.cpp)
//#define CFG_SETTINGS_ISLOCAL			L"Settings/isLocal"
//	Obsolete(2.0.0.0) DW Lang (LanguageManager.cpp)
#define CFG_SETTINGS_LANG				L"Settings/Lang"
//	Obsolete(2.1.0.0) DW Lang2 [added 2.0.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG2				L"Settings/Lang2"
//	ST Lang3 [added 2.1.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG3				L"Settings/Lang3"
//	DW lastCX (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCX				L"Settings/lastCX"
//	DW lastCY (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCY				L"Settings/lastCY"
//	ST LastDog (AgilityBookDoc.cpp/AgilityBookTreeView.cpp)
#define CFG_SETTINGS_LASTDOG			L"Settings/LastDog"
//	ST LastFile (AgilityBook.cpp/AgilityBookDoc.cpp)
#define CFG_SETTINGS_LASTFILE			L"Settings/LastFile"
//	DW lastState (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTSTATE			L"Settings/lastState"
//	ST lastVerCheck (AgilityBook.cpp/UpdateInfo.cpp)
#define CFG_SETTINGS_LASTVERCHECK		L"Settings/lastVerCheck"
//	DW lastXpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTXPOS			L"Settings/lastXpos"
//	DW lastYpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTYPOS			L"Settings/lastYpos"
//	DW printLand (AgilityBook.cpp)
#define CFG_SETTINGS_PRINTLAND			L"Settings/printLand"
//	ST proxy
#define CFG_SETTINGS_PROXY				L"Settings/proxy"
//	DW showHtml
#define CFG_SETTINGS_SHOWHTML			L"Settings/showHtml"
//	DW splitCX (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX			L"Settings/splitCX"
//	DW splitCX2 (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX2			L"Settings/splitCX2"
//	DW useproxy
#define CFG_SETTINGS_USEPROXY			L"Settings/useproxy"
//	DW View (TabView.cpp)
#define CFG_SETTINGS_VIEW				L"Settings/View"
//	DW ViewOrient (TabView.cpp)
#define CFG_SETTINGS_VIEWORIENT			L"Settings/ViewOrient"
//	DW ViewType (TabView.cpp)
#define CFG_SETTINGS_VIEWTYPE			L"Settings/ViewType"
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

//Sorting:
//	ST [key]Order (ColumnOrder.cpp)
inline wxString CFG_SORTING_ORDER(wxString const& item)
{
	return L"Sorting/" + item + L"Order";
}
//	ST [key]Sort (ColumnOrder.cpp)
inline wxString CFG_SORTING_SORT(wxString const& item)
{
	return L"Sorting/" + item + L"Sort";
}
//	DW Calendar (AgilityBookCalendarListView.cpp)
#define CFG_SORTING_CALENDAR			L"Sorting/Calendar"
//	DW Runs (AgilityBookRunsView.cpp)
#define CFG_SORTING_RUNS				L"Sorting/Runs"
//	DW RunsEnabled (AgilityBookRunsView.cpp)
#define CFG_SORTING_RUNS_ENABLED		L"Sorting/RunsEnabled"
//	DW Training (AgilityBookTrainingView.cpp)
#define CFG_SORTING_TRAINING			L"Sorting/Training"

//UserNames:
#define CFG_KEY_USERNAMES				L"UserNames/"
//	ST <hint>

//Unknown
#define CFG_KEY_UNKNOWN					L"Unknown/"
//	ST col[n] (CFG_COL_BASENAME)
