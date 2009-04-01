/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-24 DRC Ported to wxWidgets.
 * @li 2008-01-14 DRC Make ViewLifetimeEvents default to true.
 * @li 2007-08-03 DRC Added UserNames
 * @li 2006-07-16 DRC Added PointsViewSort
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-08-18 DRC Changed how filter options are done.
 * @li 2005-05-04 DRC Added IncludeCRCDImage
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2004-08-31 DRC Added AutoShowSplashScreen
 * @li 2004-06-16 DRC Added options to remember date formats.
 * @li 2004-06-06 DRC Added additional clipboard formats.
 * @li 2004-06-01 DRC Venue filtering could allow filtered runs thru at times.
 * @li 2004-04-08 DRC Added general program options.
 * @li 2004-03-13 DRC Added GetViewHiddenTitles, updated IsTitleVisible.
 * @li 2004-01-04 DRC Added GetImportExportDateFormat.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-29 DRC Created default column orders.
 * @li 2003-12-11 DRC Added options for import/export wizard.
 * @li 2003-10-09 DRC Added *etViewRunsByTrial.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookOptions.h"

#include "ARBAgilityRecordBook.h"
#include "ARBCalendar.h"
#include "ARBDate.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "DlgAssignColumns.h"
#include "VersionNum.h"

#include <wx/config.h>


/* Registry entries (all referenced in the file, unless noted otherwise)
 * DW == dword, ST == string
Calendar
	DW AutoDelete
	DW CloseColor
	DW CloseNear
	DW CloseNearColor
	DW EnteredColor
	DW Filter (FilterOptions.cpp)
	DW FontTextBold
	DW FontTextItalic
	ST FontTextName
	DW FontTextSize
	DW HideOverlapping
	DW NotEnteredColor
	DW OpenColor
	DW OpenNear
	DW OpenNearColor
	DW PastEntry
	DW PlanningColor
	DW ViewAll
	DW ViewClose
	DW ViewOpen
	Obsolete(1.7.6.12) DW EntrySize.cx
	Obsolete(1.7.6.12) DW EntrySize.cy
	Obsolete DW FontDateBold
	Obsolete DW FontDateItalic
	Obsolete ST FontDateName
	Obsolete DW FontDateSize
	Obsolete DW PrintFontDateBold
	Obsolete DW PrintFontDateItalic
	Obsolete ST PrintFontDateName
	Obsolete DW PrintFontDateSize
	Obsolete DW PrintFontTextBold
	Obsolete DW PrintFontTextItalic
	Obsolete ST PrintFontTextName
	Obsolete DW PrintFontTextSize
CalSites
	DW (DLL names in EXE directory)
CalSites2 - used for permanently disabling a version
	ST (DLL names in EXE directory)
Columns
	ST col[n]
Common
	DW CRCDImage
	ST CurrentFilter (FilterOptions.cpp)
	DW EndFilter (FilterOptions.cpp)
	DW EndFilterJDay (FilterOptions.cpp)
	ST FilterTrainingNames (FilterOptions.cpp)
	ST FilterVenue (FilterOptions.cpp)
	DW FirstDayOfWeek
	DW Margins.B
	DW Margins.L
	DW Margins.R
	DW Margins.T
	DW numFilters (FilterOptions.cpp)
	DW PrintFontListBold
	DW PrintFontListItalic
	ST PrintFontListName
	DW PrintFontListSize
	DW sortPtVw1
	DW sortPtVw2
	DW sortPtVw3
	DW StartFilter (FilterOptions.cpp)
	DW StartFilterJDay (FilterOptions.cpp)
	DW TableInYPS
	DW ViewAllDates (FilterOptions.cpp)
	DW ViewAllNames (FilterOptions.cpp)
	DW ViewAllVenues (FilterOptions.cpp)
	DW ViewHiddenTitles
	DW ViewLifetimeEvents
	DW ViewNewestFirst
	DW ViewRuns (FilterOptions.cpp)
	DW ViewRunsByTrial
	Obsolete DW TrainingEndFilter
	Obsolete DW TrainingEndFilterJDay
	Obsolete DW TrainingStartFilter
	Obsolete DW TrainingStartFilterJDay
	Obsolete DW TrainingViewAllDates
	Obsolete DW ViewAllRuns
	Obsolete DW ViewQRuns
Export
	ST col[n]
	DW dateformat
	DW delim
	ST delimiter
ExportCal
	ST col[n]
ExportCalAppt
	ST col[n]
ExportCalTask
	ST col[n]
ExportLog
	ST col[n]
Filter[number]
	DW AllDates (FilterOptions.cpp)
	DW AllNames (FilterOptions.cpp)
	DW AllVenues (FilterOptions.cpp)
	DW Cal (FilterOptions.cpp)
	DW End (FilterOptions.cpp)
	DW EndJDay (FilterOptions.cpp)
	ST FilterNames (FilterOptions.cpp)
	ST FilterVenue (FilterOptions.cpp)
	ST Name (FilterOptions.cpp)
	DW Start (FilterOptions.cpp)
	DW StartJDay (FilterOptions.cpp)
	DW ViewRuns (FilterOptions.cpp)
Import
	ST col[n]
	DW dateformat
	DW delim
	ST delimiter
	DW row
ImportCal
	ST col[n]
ImportLog
	ST col[n]
Last
	ST Division
	ST Handler
	ST Height
	ST Judge
	ST Level
	ST RefHeight
Selection:
	DW nDogs (DlgSelectDog.cpp)
	ST <dog name> (DlgSelectDog.cpp)
Settings:
	DW autoCheck
	DW autoShowTitle
	DW BackupFiles
	DW dateFormat[n]
		{
			eRunTree	= 0,
			eRunList	= 1,
			ePoints		= 2,
			eCalList	= 3,
			eCalendar	= 4,
			eTraining	= 5,
		}
	DW Lang2 [added 2.0.0.0] (LanguageManager.cpp)
	DW lastCX (AgilityBook.cpp/MainFrm.cpp)
	DW lastCY (AgilityBook.cpp/MainFrm.cpp)
	ST LastDog (AgilityBookDoc.cpp/AgilityBookTreeView.cpp)
	ST LastFile (AgilityBook.cpp/AgilityBookDoc.cpp)
	DW lastState (AgilityBook.cpp/MainFrm.cpp)
	ST lastVerCheck (AgilityBook.cpp/UpdateInfo.cpp)
	DW lastXpos (AgilityBook.cpp/MainFrm.cpp)
	DW lastYpos (AgilityBook.cpp/MainFrm.cpp)
	DW showHtml
	DW ShowSplash
	ST Splash
	DW splitCX (AgilityBookPanels.cpp)
	DW splitCX2 (AgilityBookPanels.cpp)
	DW View (TabView.cpp)
	DW ViewOrient (TabView.cpp)
	DW ViewType (TabView.cpp)
	Obsolete(2.0.0.0) DW Lang (F.cppilterOptions)
Sorting:
	ST [key]Order (ColumnOrder.cpp)
	ST [key]Sort (ColumnOrder.cpp)
	DW Calendar (AgilityBookCalendarListView.cpp)
	DW Runs (AgilityBookRunsView.cpp)
	DW Training (AgilityBookTrainingView.cpp)
Unknown
	ST col[n]
*/


void CFontInfo::CreateFont(wxFont& font)
{
	font = wxFont();
	font.SetFaceName(name);
	font.SetPointSize(size);
	int style = wxFONTSTYLE_NORMAL;
	if (italic)
		style |= wxFONTFLAG_ITALIC;
	if (bold)
		style |= wxFONTFLAG_BOLD;
	font.SetStyle(style);
}


void CFontInfo::CreateFont(
		wxFontDialog& dlg,
		wxFont& font)
{
	font = dlg.GetFontData().GetChosenFont();
	size = font.GetPointSize();
	name = font.GetFaceName();
	italic = (font.GetStyle() & wxFONTFLAG_ITALIC) ? true : false;
	bold = (font.GetStyle() & wxFONTFLAG_BOLD) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
// Calendar options

bool CAgilityBookOptions::AutoDeleteCalendarEntries()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Calendar/AutoDelete"), &val);
	return val;
}


void CAgilityBookOptions::SetAutoDeleteCalendarEntries(bool bAuto)
{
	wxConfig::Get()->Write(wxT("Calendar/AutoDelete"), bAuto);
}


// View all or hide old entries
bool CAgilityBookOptions::ViewAllCalendarEntries()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Calendar/ViewAll"), &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	wxConfig::Get()->Write(wxT("Calendar/ViewAll"), bView);
}


bool CAgilityBookOptions::ViewAllCalendarOpening()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Calendar/ViewOpen"), &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarOpening(bool bView)
{
	wxConfig::Get()->Write(wxT("Calendar/ViewOpen"), bView);
}


bool CAgilityBookOptions::ViewAllCalendarClosing()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Calendar/ViewClose"), &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarClosing(bool bView)
{
	wxConfig::Get()->Write(wxT("Calendar/ViewClose"), bView);
}


long CAgilityBookOptions::DaysTillEntryIsPast()
{
	long val = 5;
	wxConfig::Get()->Read(wxT("Calendar/PastEntry"), &val);
	return val;
}


void CAgilityBookOptions::SetDaysTillEntryIsPast(long nDays)
{
	if (0 > nDays)
		nDays = 0;
	wxConfig::Get()->Write(wxT("Calendar/PastEntry"), nDays);
}


bool CAgilityBookOptions::HideOverlappingCalendarEntries()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Calendar/HideOverlapping"), &val);
	return val;
}


void CAgilityBookOptions::SetHideOverlappingCalendarEntries(bool bHide)
{
	wxConfig::Get()->Write(wxT("Calendar/HideOverlapping"), bHide);
}


static wxChar const* CalItemName(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		return wxT("NotEnteredColor");
	case CAgilityBookOptions::eCalColorPlanning:
		return wxT("PlanningColor");
	case CAgilityBookOptions::eCalColorOpening:
		return wxT("OpenColor");
	case CAgilityBookOptions::eCalColorClosing:
		return wxT("CloseColor");
	case CAgilityBookOptions::eCalColorEntered:
		return wxT("EnteredColor");
	}
	assert(0);
	return wxT("");
}


static wxColour CalItemColor(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		return wxColour(0,0,0); // Black
	case CAgilityBookOptions::eCalColorPlanning:
		return wxColour(255,128,0); // Orange
	case CAgilityBookOptions::eCalColorOpening:
		return wxColour(0,128,0); // Dk Green
	case CAgilityBookOptions::eCalColorClosing:
		return wxColour(255,0,0); // Red
	case CAgilityBookOptions::eCalColorEntered:
		return wxColour(0,0,255); // Blue
	}
	assert(0);
	return wxColour(0,0,0);
}


// Reading/writing is abstracted since a WXCOLORREF is vastly different
// on a Mac (uchar[6] vs ulong). Which means trying to use it directly
// with wxConfig makes for a very unhappy compiler.
static wxColour ReadColor(wxString const& key, wxColour const& defColor)
{
	unsigned long val = defColor.Red();
	val |= (defColor.Green() << 8);
	val |= (defColor.Blue() << 16);
	val = wxConfig::Get()->Read(key, val);
	return wxColour(val);
}


static void WriteColor(wxString const& key, wxColour const& inColor)
{
	unsigned long val = inColor.Red();
	val |= (inColor.Green() << 8);
	val |= (inColor.Blue() << 16);
	wxConfig::Get()->Write(key, static_cast<long>(val));
}


wxColour CAgilityBookOptions::CalendarColor(CalendarColorItem inItem)
{
	wxString key(wxT("Calendar/"));
	key += CalItemName(inItem);
	return ReadColor(key, CalItemColor(inItem));
}


void CAgilityBookOptions::SetCalendarColor(CalendarColorItem inItem, wxColour inColor)
{
	wxString key(wxT("Calendar/"));
	key += CalItemName(inItem);
	WriteColor(key, inColor);
}


long CAgilityBookOptions::CalendarOpeningNear()
{
	long val = 4;
	wxConfig::Get()->Read(wxT("Calendar/OpenNear"), &val);
	return val;
}


void CAgilityBookOptions::SetCalendarOpeningNear(long inDays)
{
	wxConfig::Get()->Write(wxT("Calendar/OpenNear"), inDays);
}


long CAgilityBookOptions::CalendarClosingNear()
{
	long val = 10;
	wxConfig::Get()->Read(wxT("Calendar/CloseNear"), &val);
	return val;
}


void CAgilityBookOptions::SetCalendarClosingNear(long inDays)
{
	wxConfig::Get()->Write(wxT("Calendar/CloseNear"), inDays);
}


wxColour CAgilityBookOptions::CalendarOpeningNearColor()
{
	return ReadColor(wxT("Calendar/OpenNearColor"), wxColour(0,0,255));
}


void CAgilityBookOptions::SetCalendarOpeningNearColor(wxColour inColor)
{
	WriteColor(wxT("Calendar/OpenNearColor"), inColor);
}


wxColour CAgilityBookOptions::CalendarClosingNearColor()
{
	return ReadColor(wxT("Calendar/CloseNearColor"), wxColour(255,0,0));
}


void CAgilityBookOptions::SetCalendarClosingNearColor(wxColour inColor)
{
	WriteColor(wxT("Calendar/CloseNearColor"), inColor);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDate::DayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	long val = static_cast<long>(ARBDate::eSunday);
	wxConfig::Get()->Read(wxT("Common/FirstDayOfWeek"), &val);
	if (val < 0 || val > 6)
		val = static_cast<long>(ARBDate::eSunday);
	return static_cast<ARBDate::DayOfWeek>(val);
}


void CAgilityBookOptions::SetFirstDayOfWeek(ARBDate::DayOfWeek day)
{
	wxConfig::Get()->Write(wxT("Common/FirstDayOfWeek"), static_cast<long>(day));
}

/////////////////////////////////////////////////////////////////////////////
// Runs/points options

bool CAgilityBookOptions::GetViewRunsByTrial()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Common/ViewRunsByTrial"), &val);
	return val;
}


void CAgilityBookOptions::SetViewRunsByTrial(bool bView)
{
	wxConfig::Get()->Write(wxT("Common/ViewRunsByTrial"),bView);
}


bool CAgilityBookOptions::GetNewestDatesFirst()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Common/ViewNewestFirst"), &val);
	return val;
}


void CAgilityBookOptions::SetNewestDatesFirst(bool bNewest)
{
	wxConfig::Get()->Write(wxT("Common/ViewNewestFirst"), bNewest);
}


void CAgilityBookOptions::GetPointsViewSort(
		PointsViewSort& outPrimary,
		PointsViewSort& outSecondary,
		PointsViewSort& outTertiary)
{
	outPrimary = static_cast<PointsViewSort>(wxConfig::Get()->Read(wxT("Common/sortPtVw1"), static_cast<long>(ePointsViewSortDivision)));
	outSecondary = static_cast<PointsViewSort>(wxConfig::Get()->Read(wxT("Common/sortPtVw2"), static_cast<long>(ePointsViewSortLevel)));
	outTertiary = static_cast<PointsViewSort>(wxConfig::Get()->Read(wxT("Common/sortPtVw3"), static_cast<long>(ePointsViewSortEvent)));
}


void CAgilityBookOptions::SetPointsViewSort(
		PointsViewSort inPrimary,
		PointsViewSort inSecondary,
		PointsViewSort inTertiary)
{
	wxConfig::Get()->Write(wxT("Common/sortPtVw1"), static_cast<long>(inPrimary));
	wxConfig::Get()->Write(wxT("Common/sortPtVw2"), static_cast<long>(inSecondary));
	wxConfig::Get()->Write(wxT("Common/sortPtVw3"), static_cast<long>(inTertiary));
}


bool CAgilityBookOptions::GetViewHiddenTitles()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Common/ViewHiddenTitles"), &val);
	return val;
}


void CAgilityBookOptions::SetViewHiddenTitles(bool bSet)
{
	wxConfig::Get()->Write(wxT("Common/ViewHiddenTitles"), bSet);
}


bool CAgilityBookOptions::GetViewLifetimePointsByEvent()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Common/ViewLifetimeEvents"), &val);
	return val;
}


void CAgilityBookOptions::SetViewLifetimePointsByEvent(bool bSet)
{
	wxConfig::Get()->Write(wxT("Common/ViewLifetimeEvents"), bSet);
}


bool CAgilityBookOptions::GetTableInYPS()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Common/TableInYPS"), &val);
	return val;
}


void CAgilityBookOptions::SetTableInYPS(bool bSet)
{
	wxConfig::Get()->Write(wxT("Common/TableInYPS"), bSet);
}


bool CAgilityBookOptions::GetIncludeCRCDImage()
{
	bool val = false;
	wxConfig::Get()->Read(wxT("Common/CRCDImage"), &val);
	return val;
}


void CAgilityBookOptions::SetIncludeCRCDImage(bool bSet)
{
	wxConfig::Get()->Write(wxT("Common/CRCDImage"), bSet);
}

/////////////////////////////////////////////////////////////////////////////
// Font options

// Note: Fonts are factored by 10 for backwards compatibility with ARBv1

void CAgilityBookOptions::GetPrinterFontInfo(CFontInfo& info)
{
	info.name = wxT("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	info.name = wxConfig::Get()->Read(wxT("Common/PrintFontListName"), info.name);
	wxConfig::Get()->Read(wxT("Common/PrintFontListSize"), &info.size, info.size);
	wxConfig::Get()->Read(wxT("Common/PrintFontListItalic"), &info.italic, info.italic);
	wxConfig::Get()->Read(wxT("Common/PrintFontListBold"), &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetPrinterFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(wxT("Common/PrintFontListName"), info.name);
	wxConfig::Get()->Write(wxT("Common/PrintFontListSize"), info.size * 10);
	wxConfig::Get()->Write(wxT("Common/PrintFontListItalic"), info.italic);
	wxConfig::Get()->Write(wxT("Common/PrintFontListBold"), info.bold);
}


void CAgilityBookOptions::GetPrinterMargins(
		long& outLeft,
		long& outRight,
		long& outTop,
		long& outBottom)
{
	outLeft = wxConfig::Get()->Read(wxT("Common/Margins.L"), 50L);
	outRight = wxConfig::Get()->Read(wxT("Common/Margins.R"), 50L);
	outTop = wxConfig::Get()->Read(wxT("Common/Margins.T"), 50L);
	outBottom = wxConfig::Get()->Read(wxT("Common/Margins.B"), 50L);
}


void CAgilityBookOptions::SetPrinterMargins(
		long inLeft,
		long inRight,
		long inTop,
		long inBottom)
{
	wxConfig::Get()->Write(wxT("Common/Margins.L"), inLeft);
	wxConfig::Get()->Write(wxT("Common/Margins.R"), inRight);
	wxConfig::Get()->Write(wxT("Common/Margins.T"), inTop);
	wxConfig::Get()->Write(wxT("Common/Margins.B"), inBottom);
}


void CAgilityBookOptions::GetCalendarFontInfo(CFontInfo& info)
{
	info.name = wxT("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	info.name = wxConfig::Get()->Read(wxT("Calendar/FontTextName"), info.name);
	wxConfig::Get()->Read(wxT("Calendar/FontTextSize"), &info.size, info.size);
	wxConfig::Get()->Read(wxT("Calendar/FontTextItalic"), &info.italic, info.italic);
	wxConfig::Get()->Read(wxT("Calendar/FontTextBold"), &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetCalendarFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(wxT("Calendar/FontTextName"), info.name);
	wxConfig::Get()->Write(wxT("Calendar/FontTextSize"), info.size * 10);
	wxConfig::Get()->Write(wxT("Calendar/FontTextItalic"), info.italic);
	wxConfig::Get()->Write(wxT("Calendar/FontTextBold"), info.bold);
}

/////////////////////////////////////////////////////////////////////////////
// Last entered options

wxString CAgilityBookOptions::GetLastEnteredDivision()
{
	return wxConfig::Get()->Read(wxT("Last/Division"), wxString());
}


void CAgilityBookOptions::SetLastEnteredDivision(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/Division"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/Division"));
}


wxString CAgilityBookOptions::GetLastEnteredLevel()
{
	return wxConfig::Get()->Read(wxT("Last/Level"), wxString());
}


void CAgilityBookOptions::SetLastEnteredLevel(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/Level"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/Level"));
}


wxString CAgilityBookOptions::GetLastEnteredHeight()
{
	return wxConfig::Get()->Read(wxT("Last/Height"), wxString());
}


void CAgilityBookOptions::SetLastEnteredHeight(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/Height"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/Height"));
}


wxString CAgilityBookOptions::GetLastEnteredRefHeight()
{
	return wxConfig::Get()->Read(wxT("Last/RefHeight"), wxString());
}


void CAgilityBookOptions::SetLastEnteredRefHeight(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/RefHeight"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/RefHeight"));
}


wxString CAgilityBookOptions::GetLastEnteredJudge()
{
	return wxConfig::Get()->Read(wxT("Last/Judge"), wxString());
}


void CAgilityBookOptions::SetLastEnteredJudge(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/Judge"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/Judge"));
}


wxString CAgilityBookOptions::GetLastEnteredHandler()
{
	return wxConfig::Get()->Read(wxT("Last/Handler"), wxString());
}


void CAgilityBookOptions::SetLastEnteredHandler(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(wxT("Last/Handler"), inLast);
	else
		wxConfig::Get()->DeleteEntry(wxT("Last/Handler"));
}

/////////////////////////////////////////////////////////////////////////////
// Import/export options

long CAgilityBookOptions::GetImportStartRow()
{
	long row = wxConfig::Get()->Read(wxT("Import/row"), 1L);
	if (0 > row)
		row = 1;
	return row;
}


void CAgilityBookOptions::SetImportStartRow(long row)
{
	wxConfig::Get()->Write(wxT("Import/row"), row);
}


void CAgilityBookOptions::GetImportExportDelimiters(
		bool bImport,
		long& delim,
		wxString& delimiter)
{
	wxString section;
	if (bImport)
		section = wxT("Import/");
	else
		section = wxT("Export/");
	delim = eDelimTab;
	delimiter.Empty();
	delim = wxConfig::Get()->Read(section + wxT("delim"), delim);
	delimiter = wxConfig::Get()->Read(section + wxT("delimiter"), delimiter);
	if (1 < delimiter.length())
		delimiter = delimiter.Left(1);
}


void CAgilityBookOptions::SetImportExportDelimiters(
		bool bImport,
		long delim,
		wxString const& delimiter)
{
	wxString section;
	if (bImport)
		section = wxT("Import/");
	else
		section = wxT("Export/");
	wxConfig::Get()->Write(section + wxT("delim"), delim);
	wxConfig::Get()->Write(section + wxT("delimiter"), delimiter);
}


void CAgilityBookOptions::GetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat& outFormat)
{
	wxString section;
	if (bImport)
		section = wxT("Import/");
	else
		section = wxT("Export/");
	outFormat = static_cast<ARBDate::DateFormat>(wxConfig::Get()->Read(section + wxT("dateformat"), static_cast<long>(ARBDate::eDashYYYYMMDD)));
}


void CAgilityBookOptions::SetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat inFormat)
{
	wxString section;
	if (bImport)
		section = wxT("Import/");
	else
		section = wxT("Export/");
	wxConfig::Get()->Write(section + wxT("dateformat"), static_cast<long>(inFormat));
}


static wxChar const* const GetColumnName(CAgilityBookOptions::ColumnOrder eOrder)
{
	switch (eOrder)
	{
	default:
		return wxT("Unknown");
	case CAgilityBookOptions::eRunsImport:
		return wxT("Import");
	case CAgilityBookOptions::eRunsExport:
		return wxT("Export");
	case CAgilityBookOptions::eCalImport:
		return wxT("ImportCal");
	case CAgilityBookOptions::eCalExport:
		return wxT("ExportCal");
	case CAgilityBookOptions::eCalExportAppt:
		return wxT("ExportCalAppt");
	case CAgilityBookOptions::eCalExportTask:
		return wxT("ExportCalTask");
	case CAgilityBookOptions::eLogImport:
		return wxT("ImportLog");
	case CAgilityBookOptions::eLogExport:
		return wxT("ExportLog");
	case CAgilityBookOptions::eView:
		return wxT("Columns");
	}
}


void CAgilityBookOptions::GetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long>& outValues,
		bool bDefaultValues)
{
	outValues.clear();
	if (!bDefaultValues)
	{
		otstringstream item;
		item << GetColumnName(eOrder)
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
			<< wxT("/col") << static_cast<unsigned int>(idxColumn);
#else
			<< wxT("/col") << idxColumn;
#endif
		wxString data = wxConfig::Get()->Read(item.str().c_str(), wxString());
		int idx = data.Find(',');
		while (0 <= idx)
		{
			int val = tstringUtil::atol(data.c_str());
			outValues.push_back(val);
			data = data.Mid(idx+1);
			idx = data.Find(',');
		}
		if (0 < data.length())
		{
			long val = tstringUtil::atol(data.c_str());
			outValues.push_back(val);
		}
	}
	if (0 == outValues.size())
	{
		switch (eOrder)
		{
		default:
			break;
		case eRunsImport:
		case eRunsExport:
			switch (idxColumn)
			{
			case IO_TYPE_RUNS_FAULTS_TIME:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YARDS);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_SCT);
				outValues.push_back(IO_RUNS_TOTAL_FAULTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_TIME_FAULTS:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YARDS);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_SCT);
				outValues.push_back(IO_RUNS_TOTAL_FAULTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_OPEN_CLOSE:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_REQ_OPENING);
				outValues.push_back(IO_RUNS_REQ_CLOSING);
				outValues.push_back(IO_RUNS_OPENING);
				outValues.push_back(IO_RUNS_CLOSING);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_POINTS:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_REQ_POINTS);
				outValues.push_back(-1);
				outValues.push_back(IO_RUNS_POINTS);
				outValues.push_back(-1);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;
			}
			break;
		case eCalImport:
		case eCalExport:
			if (IO_TYPE_CALENDAR == idxColumn)
			{
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_END_DATE);
				outValues.push_back(IO_CAL_TENTATIVE);
				outValues.push_back(IO_CAL_ENTERED);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_NOTES);
			}
			break;
		case eCalExportAppt:
			if (IO_TYPE_CALENDAR_APPT == idxColumn)
			{
				outValues.push_back(IO_CAL_APPT_SUBJECT);
				outValues.push_back(IO_CAL_APPT_START_DATE);
				outValues.push_back(IO_CAL_APPT_START_TIME);
				outValues.push_back(IO_CAL_APPT_END_DATE);
				outValues.push_back(IO_CAL_APPT_END_TIME);
				outValues.push_back(IO_CAL_APPT_ALLDAY);
				outValues.push_back(IO_CAL_APPT_REMINDER);
				outValues.push_back(IO_CAL_APPT_REMINDER_DATE);
				outValues.push_back(IO_CAL_APPT_REMINDER_TIME);
				outValues.push_back(IO_CAL_APPT_ORGANIZER);
				outValues.push_back(IO_CAL_APPT_REQ_ATTENDEES);
				outValues.push_back(IO_CAL_APPT_OPT_ATTENDEES);
				outValues.push_back(IO_CAL_APPT_RESOURCES);
				outValues.push_back(IO_CAL_APPT_BILLING);
				outValues.push_back(IO_CAL_APPT_CATEGORIES);
				outValues.push_back(IO_CAL_APPT_DESCRIPTION);
				outValues.push_back(IO_CAL_APPT_LOCATION);
				outValues.push_back(IO_CAL_APPT_MILEAGE);
				outValues.push_back(IO_CAL_APPT_PRIORITY);
				outValues.push_back(IO_CAL_APPT_PRIVATE);
				outValues.push_back(IO_CAL_APPT_SENSITIVITY);
				outValues.push_back(IO_CAL_APPT_SHOW_TIME_AS);
			}
			break;
		case eCalExportTask:
			if (IO_TYPE_CALENDAR_TASK == idxColumn)
			{
				outValues.push_back(IO_CAL_TASK_SUBJECT);
				outValues.push_back(IO_CAL_TASK_START_DATE);
				outValues.push_back(IO_CAL_TASK_DUE_DATE);
				outValues.push_back(IO_CAL_TASK_REMINDER);
				outValues.push_back(IO_CAL_TASK_REMINDER_DATE);
				outValues.push_back(IO_CAL_TASK_REMINDER_TIME);
				outValues.push_back(IO_CAL_TASK_COMPLETED_DATE);
				outValues.push_back(IO_CAL_TASK_COMPLETE);
				outValues.push_back(IO_CAL_TASK_TOTAL_WORK);
				outValues.push_back(IO_CAL_TASK_ACTUAL_WORK);
				outValues.push_back(IO_CAL_TASK_BILLING);
				outValues.push_back(IO_CAL_TASK_CATEGORIES);
				outValues.push_back(IO_CAL_TASK_COMPANIES);
				outValues.push_back(IO_CAL_TASK_CONTACTS);
				outValues.push_back(IO_CAL_TASK_MILEAGE);
				outValues.push_back(IO_CAL_TASK_NOTES);
				outValues.push_back(IO_CAL_TASK_PRIORITY);
				outValues.push_back(IO_CAL_TASK_PRIVATE);
				outValues.push_back(IO_CAL_TASK_ROLE);
				outValues.push_back(IO_CAL_TASK_SCH_PRIORITY);
				outValues.push_back(IO_CAL_TASK_SENSITIVITY);
				outValues.push_back(IO_CAL_TASK_STATUS);
			}
			break;
		case eLogImport:
		case eLogExport:
			if (IO_TYPE_TRAINING == idxColumn)
			{
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
			}
			break;
		case eView:
			switch (idxColumn)
			{
			case IO_TYPE_VIEW_TREE_DOG:
				outValues.push_back(IO_TREE_DOG_CALLNAME);
				outValues.push_back(IO_TREE_DOG_AGE);
				break;
			case IO_TYPE_VIEW_TREE_TRIAL:
				outValues.push_back(IO_TREE_TRIAL_START);
				outValues.push_back(IO_TREE_TRIAL_VENUE);
#ifndef WIN32
				outValues.push_back(IO_TREE_TRIAL_VERIFIED);
#endif
				outValues.push_back(IO_TREE_TRIAL_LOCATION);
				break;
			case IO_TYPE_VIEW_TREE_RUN:
				outValues.push_back(IO_TREE_RUN_DATE);
				outValues.push_back(IO_TREE_RUN_EVENT);
				break;
			case IO_TYPE_VIEW_RUNS_LIST:
				outValues.push_back(IO_RUNS_Q);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_HEIGHT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_COMMENTS);
				break;
			case IO_TYPE_VIEW_CALENDAR_LIST:
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_DRAWS);
				outValues.push_back(IO_CAL_NOTES);
				break;
			case IO_TYPE_VIEW_TRAINING_LIST:
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
				break;
			case IO_TYPE_VIEW_CALENDAR:
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				break;
			}
			break;
		}
	}
}


void CAgilityBookOptions::SetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long> const& inValues)
{
	otstringstream data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		if (0 < i)
			data << wxT(",");
		data << inValues[i];
	}
	otstringstream item;
	item << GetColumnName(eOrder)
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
		<< wxT("/col") << static_cast<unsigned int>(idxColumn);
#else
		<< wxT("/col") << idxColumn;
#endif
	wxConfig::Get()->Write(item.str().c_str(), data.str().c_str());
}


/////////////////////////////////////////////////////////////////////////////

long CAgilityBookOptions::GetMRUFileCount()
{
	return wxConfig::Get()->Read(wxT("Settings/MRUsize"), 4L);
}


void CAgilityBookOptions::SetMRUFileCount(long nFiles)
{
	wxConfig::Get()->Write(wxT("Settings/MRUsize"), nFiles);
}


bool CAgilityBookOptions::GetAutoUpdateCheck()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Settings/autoCheck"), &val);
	return val;
}


void CAgilityBookOptions::SetAutoUpdateCheck(bool bSet)
{
	wxConfig::Get()->Write(wxT("Settings/autoCheck"), bSet);
}


long CAgilityBookOptions::GetNumBackupFiles()
{
	return wxConfig::Get()->Read(wxT("Settings/BackupFiles"), 3L);
}


void CAgilityBookOptions::SetNumBackupFiles(long nFiles)
{
	wxConfig::Get()->Write(wxT("Settings/BackupFiles"), nFiles);
}


bool CAgilityBookOptions::AutoShowSplashScreen()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Settings/ShowSplash"), &val);
	return val;
}


void CAgilityBookOptions::AutoShowSplashScreen(bool bAutoShow)
{
	wxConfig::Get()->Write(wxT("Settings/ShowSplash"), bAutoShow);
}


wxString CAgilityBookOptions::GetSplashImage()
{
	return wxConfig::Get()->Read(wxT("Settings/Splash"), wxString());
}


void CAgilityBookOptions::SetSplashImage(wxString const& filename)
{
	wxConfig::Get()->Write(wxT("Settings/Splash"), filename);
}


bool CAgilityBookOptions::AutoShowPropertiesOnNewTitle()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Settings/autoShowTitle"), &val);
	return val;
}


void CAgilityBookOptions::AutoShowPropertiesOnNewTitle(bool bShow)
{
	wxConfig::Get()->Write(wxT("Settings/autoShowTitle"), bShow);
}


ARBDate::DateFormat CAgilityBookOptions::GetDateFormat(FormattedDate inItem)
{
	otstringstream section;
	section << wxT("Settings/dateFormat") << static_cast<int>(inItem);
	ARBDate::DateFormat def;
	switch (inItem)
	{
	default:
	case eRunTree: def = ARBDate::eDashYYYYMMDD; break;
	case eRunList: def = ARBDate::eSlashMDY; break;
	case ePoints: def = ARBDate::eSlashMDY; break;
	case eCalList: def = ARBDate::eDashYMD; break;
	case eCalendar: def = ARBDate::eDashYMD; break;
	case eTraining: def = ARBDate::eDashYMD; break;
	}
	return static_cast<ARBDate::DateFormat>(wxConfig::Get()->Read(section.str().c_str(), static_cast<long>(def)));
}


void CAgilityBookOptions::SetDateFormat(
		FormattedDate inItem,
		ARBDate::DateFormat inFormat)
{
	otstringstream section;
	section << wxT("Settings/dateFormat") << static_cast<int>(inItem);
	wxConfig::Get()->Write(section.str().c_str(), static_cast<long>(inFormat));
}


bool CAgilityBookOptions::ShowHtmlPoints()
{
	bool val = true;
	wxConfig::Get()->Read(wxT("Settings/showHtml"), &val);
	return val;
}


void CAgilityBookOptions::SetShowHtmlPoints(bool bSet)
{
	wxConfig::Get()->Write(wxT("Settings/showHtml"), bSet);
}

/////////////////////////////////////////////////////////////////////////////

wxString CAgilityBookOptions::GetUserName(wxString const& hint)
{
	wxString section(wxT("UserNames/"));
	section += hint;
	return wxConfig::Get()->Read(section, wxString());
}


void CAgilityBookOptions::SetUserName(
		wxString const& hint,
		wxString const& userName)
{
	wxString section(wxT("UserNames/"));
	section += hint;
	wxConfig::Get()->Write(section, userName);
}


bool CAgilityBookOptions::IsCalSiteVisible(
		wxString const& filename,
		CVersionNum const& inVer)
{
	assert(inVer.Valid());
	if (filename.IsEmpty())
		return true;
	bool bVisible = true;
	wxString section(wxT("CalSites/"));
	section += filename;
	bool bSuppress = true;
	wxConfig::Get()->Read(section, &bSuppress);
	if (bSuppress)
		bVisible = false;
	else
	{
		CVersionNum ver = GetCalSitePermanentStatus(filename);
		if (ver.Valid() && inVer <= ver)
			bVisible = false;
	}
	return bVisible;
}


void CAgilityBookOptions::SuppressCalSite(
		wxString const& filename,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	wxString section(wxT("CalSites/"));
	section += filename;
	wxConfig::Get()->Write(section, !bSuppress);
}


CVersionNum CAgilityBookOptions::GetCalSitePermanentStatus(wxString const& filename)
{
	CVersionNum ver;
	if (!filename.IsEmpty())
	{
		wxString section(wxT("CalSites2/"));
		section += filename;
		wxString str = wxConfig::Get()->Read(section, wxString());
		if (!str.IsEmpty())
			ver.Parse(str);
	}
	return ver;
}


void CAgilityBookOptions::SuppressCalSitePermanently(
		wxString const& filename,
		CVersionNum const& inVer,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	wxString section(wxT("CalSites2/"));
	section += filename;
	if (bSuppress)
		wxConfig::Get()->Write(section, inVer.GetVersionString());
	else
	{
		// If we're clearing one, make sure we haven't written a different version
		CVersionNum ver;
		wxString str = wxConfig::Get()->Read(section, wxString());
		if (!str.IsEmpty())
			ver.Parse(str);
		if (ver == inVer)
			wxConfig::Get()->DeleteEntry(section);
	}
}
