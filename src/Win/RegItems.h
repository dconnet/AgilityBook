#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Definitions of all configuration (registry) entries.
 * @author David Connet
 *
 * These are used almost exclusively in AgilityBookOptions, except as noted.
 * DW == dword, ST == string
 *
 * Revision History
 * @li 2010-04-11 DRC Separated from AgilityBookOptions.cpp.
 */


//Calendar
#define CFG_KEY_CALENDAR				wxT("Calendar/")
//	DW CloseColor
#define CFG_CAL_ITEM_CLOSECOLOR				wxT("CloseColor")
//	DW EnteredColor
#define CFG_CAL_ITEM_ENTEREDCOLOR			wxT("EnteredColor")
//	DW NotEnteredColor
#define CFG_CAL_ITEM_NOTENTEREDCOLOR		wxT("NotEnteredColor")
//	DW OpenColor
#define CFG_CAL_ITEM_OPENCOLOR				wxT("OpenColor")
//	DW PendingColor
#define CFG_CAL_ITEM_PENDINGCOLOR			wxT("PendingColor")
//	DW PlanningColor
#define CFG_CAL_ITEM_PLANNINGCOLOR			wxT("PlanningColor")
//	DW AutoDelete
#define CFG_CAL_AUTODELETE				wxT("Calendar/AutoDelete")
//	DW CloseNear
#define CFG_CAL_CLOSENEAR				wxT("Calendar/CloseNear")
//	DW CloseNearColor
#define CFG_CAL_CLOSENEARCOLOR			wxT("Calendar/CloseNearColor")
//	DW Filter (FilterOptions.cpp)
#define CFG_CAL_FILTER					wxT("Calendar/Filter")
//	DW FontTextBold
#define CFG_CAL_FONTTEXTBOLD			wxT("Calendar/FontTextBold")
//	DW FontTextItalic
#define CFG_CAL_FONTTEXTITALIC			wxT("Calendar/FontTextItalic")
//	ST FontTextName
#define CFG_CAL_FONTTEXTNAME			wxT("Calendar/FontTextName")
//	DW FontTextSize
#define CFG_CAL_FONTTEXTSIZE			wxT("Calendar/FontTextSize")
//	DW HideOverlapping
#define CFG_CAL_HIDEOVERLAPPING			wxT("Calendar/HideOverlapping")
//	DW OpenNear
#define CFG_CAL_OPENNEAR				wxT("Calendar/OpenNear")
//	DW OpenNearColor
#define CFG_CAL_OPENNEARCOLOR			wxT("Calendar/OpenNearColor")
//	DW PastEntry
#define CFG_CAL_PASTENTRY				wxT("Calendar/PastEntry")
//	DW ViewAll
#define CFG_CAL_VIEWALL					wxT("Calendar/ViewAll")
//	DW ViewClose
#define CFG_CAL_VIEWCLOSE				wxT("Calendar/ViewClose")
//	DW ViewOpen
#define CFG_CAL_VIEWOPEN				wxT("Calendar/ViewOpen")
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
#define CFG_KEY_CALSITES				wxT("CalSites/")
//	DW (DLL names in EXE directory)

//CalSites2 - used for permanently disabling a version
#define CFG_KEY_CALSITES2				wxT("CalSites2/")
//	ST (DLL names in EXE directory)

//Columns
#define CFG_KEY_COLUMNS					wxT("Columns/")
//	ST col[n]
inline wxString CFG_COL_BASENAME(size_t n)
{
	return wxString::Format(wxT("col%d"), static_cast<int>(n));
}

//ColumnInfo (NamedColumns.cpp)
//	ST CurrentConfig (NamedColumns.cpp)
#define CFG_CI_CURRENTCONFIG			wxT("ColumnInfo/CurrentConfig")
//	DW numConfigs (NamedColumns.cpp)
#define CFG_CI_NUMCONFIGS				wxT("ColumnInfo/numConfigs")
//	Config[n] (NamedColumns.cpp)
inline wxString CFG_CI_KEY_CONFIG(int n, bool bIncSep = true)
{
	return bIncSep ? wxString::Format(wxT("ColumnInfo/Config%d/"), n) : wxString::Format(wxT("ColumnInfo/Config%d"), n);
}
//		ST name (NamedColumns.cpp)
#define CFG_CI_CONFIG_NAME					wxT("name")
//		[key] (NamedColumns.cpp)
//			ST col[n] (via CAgilityBookOptions::*etColumnOrder)

//Common
//	DW CRCDImage
#define CFG_COMMON_CRCDIMAGE			wxT("Common/CRCDImage")
//	ST CurrentFilter (FilterOptions.cpp)
#define CFG_COMMON_CURRENTFILTER		wxT("Common/CurrentFilter")
//	DW EndFilter (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTER			wxT("Common/EndFilter")
//	DW EndFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_ENDFILTERJDAY		wxT("Common/EndFilterJDay")
//	ST FilterTrainingNames (FilterOptions.cpp)
#define CFG_COMMON_FILTERTRAININGNAMES	wxT("Common/FilterTrainingNames")
//	ST FilterVenue (FilterOptions.cpp)
#define CFG_COMMON_FILTERVENUE			wxT("Common/FilterVenue")
//	DW FirstDayOfWeek
#define CFG_COMMON_FIRSTDAYOFWEEK		wxT("Common/FirstDayOfWeek")
//	DW UnitsAsMM
#define CFG_COMMON_UNITSASMM			wxT("Common/UnitsAsMM")
//	DW Margins.MM
#define CFG_COMMON_MARGINS_MM			wxT("Common/Margins.MM")
//	DW Margins.B
#define CFG_COMMON_MARGINS_B			wxT("Common/Margins.B")
//	DW Margins.L
#define CFG_COMMON_MARGINS_L			wxT("Common/Margins.L")
//	DW Margins.R
#define CFG_COMMON_MARGINS_R			wxT("Common/Margins.R")
//	DW Margins.T
#define CFG_COMMON_MARGINS_T			wxT("Common/Margins.T")
//	DW RunPage.MM
#define CFG_COMMON_RUNPAGE_MM			wxT("Common/RunPage.MM")
//	DW RunPage.W
#define CFG_COMMON_RUNPAGE_W			wxT("Common/RunPage.W")
//	DW RunPage.H
#define CFG_COMMON_RUNPAGE_H			wxT("Common/RunPage.H")
//	DW numFilters (FilterOptions.cpp)
#define CFG_COMMON_NUMFILTERS			wxT("Common/numFilters")
//	DW PrintFontListBold
#define CFG_COMMON_PRINTFONTLISTBOLD	wxT("Common/PrintFontListBold")
//	DW PrintFontListItalic
#define CFG_COMMON_PRINTFONTLISTITALIC	wxT("Common/PrintFontListItalic")
//	ST PrintFontListName
#define CFG_COMMON_PRINTFONTLISTNAME	wxT("Common/PrintFontListName")
//	DW PrintFontListSize
#define CFG_COMMON_PRINTFONTLISTSIZE	wxT("Common/PrintFontListSize")
//	DW sortPtVw1
#define CFG_COMMON_SORTPTVW1			wxT("Common/sortPtVw1")
//	DW sortPtVw2
#define CFG_COMMON_SORTPTVW2			wxT("Common/sortPtVw2")
//	DW sortPtVw3
#define CFG_COMMON_SORTPTVW3			wxT("Common/sortPtVw3")
//	DW StartFilter (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTER			wxT("Common/StartFilter")
//	DW StartFilterJDay (FilterOptions.cpp)
#define CFG_COMMON_STARTFILTERJDAY		wxT("Common/StartFilterJDay")
//	DW TableInYPS
#define CFG_COMMON_TABLEINYPS			wxT("Common/TableInYPS")
//	DW ViewAllDates (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLDATES			wxT("Common/ViewAllDates")
//	DW ViewAllNames (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLNAMES			wxT("Common/ViewAllNames")
//	DW ViewAllVenues (FilterOptions.cpp)
#define CFG_COMMON_VIEWALLVENUES		wxT("Common/ViewAllVenues")
//	DW ViewHiddenTitles
#define CFG_COMMON_VIEWHIDDENTITLES		wxT("Common/ViewHiddenTitles")
//	DW ViewLifetimeEvents
#define CFG_COMMON_VIEWLIFETIMEEVENTS	wxT("Common/ViewLifetimeEvents")
//	DW ViewNewestFirst
#define CFG_COMMON_VIEWNEWESTFIRST		wxT("Common/ViewNewestFirst")
//	DW ViewRuns (FilterOptions.cpp)
#define CFG_COMMON_VIEWRUNS				wxT("Common/ViewRuns")
//	DW ViewRunsByTrial
#define CFG_COMMON_VIEWRUNSBYTRIAL		wxT("Common/ViewRunsByTrial")
//	Obsolete DW TrainingEndFilter
//	Obsolete DW TrainingEndFilterJDay
//	Obsolete DW TrainingStartFilter
//	Obsolete DW TrainingStartFilterJDay
//	Obsolete DW TrainingViewAllDates
//	Obsolete DW ViewAllRuns
//	Obsolete DW ViewQRuns

//Export
#define CFG_KEY_EXPORT					wxT("Export/")
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat
#define CFG_IMPORT_EXPORT_DATEFORMAT		wxT("dateformat")
//	DW delim
#define CFG_IMPORT_EXPORT_DELIM				wxT("delim")
//	ST delimiter
#define CFG_IMPORT_EXPORT_DELIMITER			wxT("delimiter")

//ExportCal
#define CFG_KEY_EXPORTCAL				wxT("ExportCal/")
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalAppt
#define CFG_KEY_EXPORTCALAPPT			wxT("ExportCalAppt/")
//	ST col[n] (CFG_COL_BASENAME)

//ExportCalTask
#define CFG_KEY_EXPORTCALTASK			wxT("ExportCalTask/")
//	ST col[n] (CFG_COL_BASENAME)

//ExportLog
#define CFG_KEY_EXPORTLOG				wxT("ExportLog/")
//	ST col[n] (CFG_COL_BASENAME)

//Filter[number]
inline wxString CFG_KEY_FILTER(int n, bool bIncSep = true)
{
	return bIncSep ? wxString::Format(wxT("Filter%d/"), n) : wxString::Format(wxT("Filter%d"), n);
}
//	DW AllDates (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLDATES			wxT("AllDates")
//	DW AllNames (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLNAMES			wxT("AllNames")
//	DW AllVenues (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ALLVENUES			wxT("AllVenues")
//	DW Cal (FilterOptions.cpp)
#define CFG_FILTER_ITEM_CAL					wxT("Cal")
//	DW End (FilterOptions.cpp)
#define CFG_FILTER_ITEM_END					wxT("End")
//	DW EndJDay (FilterOptions.cpp)
#define CFG_FILTER_ITEM_ENDJDAY				wxT("EndJDay")
//	ST FilterNames (FilterOptions.cpp)
#define CFG_FILTER_ITEM_FILTERNAMES			wxT("FilterNames")
//	ST FilterVenue (FilterOptions.cpp)
#define CFG_FILTER_ITEM_FILTERVENUE			wxT("FilterVenue")
//	ST Name (FilterOptions.cpp)
#define CFG_FILTER_ITEM_NAME				wxT("Name")
//	DW Start (FilterOptions.cpp)
#define CFG_FILTER_ITEM_START				wxT("Start")
//	DW StartJDay (FilterOptions.cpp)
#define CFG_FILTER_ITEM_STARTJDAY			wxT("StartJDay")
//	DW ViewRuns (FilterOptions.cpp)
#define CFG_FILTER_ITEM_VIEWRUNS			wxT("ViewRuns")

//Import
#define CFG_KEY_IMPORT					wxT("Import/")
//	ST col[n] (CFG_COL_BASENAME)
//	DW dateformat (CFG_IMPORT_EXPORT_DATEFORMAT)
//	DW delim (CFG_IMPORT_EXPORT_DELIM)
//	ST delimiter (CFG_IMPORT_EXPORT_DELIMITER)
//	DW row
#define CFG_IMPORT_ROW					wxT("Import/row")

//ImportCal
#define CFG_KEY_IMPORTCAL				wxT("ImportCal/")
//	ST col[n] (CFG_COL_BASENAME)

//ImportLog
#define CFG_KEY_IMPORTLOG				wxT("ImportLog/")
//	ST col[n] (CFG_COL_BASENAME)

//Last
//	ST Division
#define CFG_LAST_DIVISION				wxT("Last/Division")
//	ST Handler
#define CFG_LAST_HANDLER				wxT("Last/Handler")
//	ST Height
#define CFG_LAST_HEIGHT					wxT("Last/Height")
//	ST Judge
#define CFG_LAST_JUDGE					wxT("Last/Judge")
//	ST Level
#define CFG_LAST_LEVEL					wxT("Last/Level")
//	ST RefHeight
#define CFG_LAST_REFHEIGHT				wxT("Last/RefHeight")

//Selection:
//	DW nDogs (DlgSelectDog.cpp)
#define CFG_SELECTION_NDOGS				wxT("Selection/nDogs")
//	ST <dog name> (DlgSelectDog.cpp)
inline wxString CFG_SELECTION_DOG(int n)
{
	return wxString::Format(wxT("Selection/Dog%d"), n);
}

//Settings:
//	DW autoCheck
#define CFG_SETTINGS_AUTOCHECK			wxT("Settings/autoCheck")
//	DW autoShowTitle
#define CFG_SETTINGS_AUTOSHOWTITLE		wxT("Settings/autoShowTitle")
//	DW BackupFiles
#define CFG_SETTINGS_BACKUPFILES		wxT("Settings/BackupFiles")
//	Obsolete(2.0.0.0) DW Lang (LanguageManager.cpp)
#define CFG_SETTINGS_LANG				wxT("Settings/Lang")
//	Obsolete(2.1.0.0) DW Lang2 [added 2.0.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG2				wxT("Settings/Lang2")
//	ST Lang3 [added 2.1.0.0] (LanguageManager.cpp)
#define CFG_SETTINGS_LANG3				wxT("Settings/Lang3")
//	DW lastCX (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCX				wxT("Settings/lastCX")
//	DW lastCY (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTCY				wxT("Settings/lastCY")
//	ST LastDog (AgilityBookDoc.cpp/AgilityBookTreeView.cpp)
#define CFG_SETTINGS_LASTDOG			wxT("Settings/LastDog")
//	ST LastFile (AgilityBook.cpp/AgilityBookDoc.cpp)
#define CFG_SETTINGS_LASTFILE			wxT("Settings/LastFile")
//	DW lastState (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTSTATE			wxT("Settings/lastState")
//	ST lastVerCheck (AgilityBook.cpp/UpdateInfo.cpp)
#define CFG_SETTINGS_LASTVERCHECK		wxT("Settings/lastVerCheck")
//	DW lastXpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTXPOS			wxT("Settings/lastXpos")
//	DW lastYpos (AgilityBook.cpp/MainFrm.cpp)
#define CFG_SETTINGS_LASTYPOS			wxT("Settings/lastYpos")
//	DW printLand (AgilityBook.cpp)
#define CFG_SETTINGS_PRINTLAND			wxT("Settings/printLand")
//	ST proxy
#define CFG_SETTINGS_PROXY				wxT("Settings/proxy")
//	DW showHtml
#define CFG_SETTINGS_SHOWHTML			wxT("Settings/showHtml")
//	DW splitCX (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX			wxT("Settings/splitCX")
//	DW splitCX2 (AgilityBookPanels.cpp)
#define CFG_SETTINGS_SPLITCX2			wxT("Settings/splitCX2")
//	DW useproxy
#define CFG_SETTINGS_USEPROXY			wxT("Settings/useproxy")
//	DW View (TabView.cpp)
#define CFG_SETTINGS_VIEW				wxT("Settings/View")
//	DW ViewOrient (TabView.cpp)
#define CFG_SETTINGS_VIEWORIENT			wxT("Settings/ViewOrient")
//	DW ViewType (TabView.cpp)
#define CFG_SETTINGS_VIEWTYPE			wxT("Settings/ViewType")
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
	return wxT("Sorting/") + item + wxT("Order");
}
//	ST [key]Sort (ColumnOrder.cpp)
inline wxString CFG_SORTING_SORT(wxString const& item)
{
	return wxT("Sorting/") + item + wxT("Sort");
}
//	DW Calendar (AgilityBookCalendarListView.cpp)
#define CFG_SORTING_CALENDAR			wxT("Sorting/Calendar")
//	DW Runs (AgilityBookRunsView.cpp)
#define CFG_SORTING_RUNS				wxT("Sorting/Runs")
//	DW Training (AgilityBookTrainingView.cpp)
#define CFG_SORTING_TRAINING			wxT("Sorting/Training")

//UserNames:
#define CFG_KEY_USERNAMES				wxT("UserNames/")
//	ST <hint>

//Unknown
#define CFG_KEY_UNKNOWN					wxT("Unknown/")
//	ST col[n] (CFG_COL_BASENAME)
