/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

/* Registry entries
Calendar
	DW AutoDelete
	DW ViewAll
	DW ViewOpen
	DW ViewClose
	DW PastEntry
	DW HideOverlapping
	DW Filter
	DW NotEnteredColor
	DW PlanningColor
	DW OpenColor
	DW CloseColor
	DW EnteredColor
	DW OpenNear
	DW CloseNear
	DW OpenNearColor
	DW CloseNearColor
	ST FontTextName
	DW FontTextSize
	DW FontTextItalic
	DW FontTextBold
	Obsolete ST FontDateName
	Obsolete DW FontDateSize
	Obsolete DW FontDateItalic
	Obsolete DW FontDateBold
	Obsolete ST PrintFontDateName
	Obsolete DW PrintFontDateSize
	Obsolete DW PrintFontDateItalic
	Obsolete DW PrintFontDateBold
	Obsolete ST PrintFontTextName
	Obsolete DW PrintFontTextSize
	Obsolete DW PrintFontTextItalic
	Obsolete DW PrintFontTextBold
	Obsolete(1.7.6.12) DW EntrySize.cx
	Obsolete(1.7.6.12) DW EntrySize.cy
CalSites
	DW (DLL names in EXE directory)
CalSites2 - used for permanently disabling a version
	ST (DLL names in EXE directory)
Columns
	ST col[n]
Common
	DW FirstDayOfWeek
	DW ViewAllDates
	DW StartFilterJDay
	DW StartFilter
	DW EndFilterJDay
	DW EndFilter
	DW ViewAllVenues
	ST FilterVenue
	DW ViewRuns
	Obsolete DW ViewAllRuns
	Obsolete DW ViewQRuns
	DW ViewAllNames
	ST FilterTrainingNames
	DW ViewRunsByTrial
	DW ViewNewestFirst
	DW ViewHiddenTitles
	DW TableInYPS
	DW CRCDImage
	ST PrintFontListName
	DW PrintFontListSize
	DW PrintFontListItalic
	DW PrintFontListBold
	DW Margins.L
	DW Margins.R
	DW Margins.T
	DW Margins.B
	DW numFilters
	ST CurrentFilter
	DW sortPtVw1
	DW sortPtVw2
	DW sortPtVw3
	Obsolete DW TrainingViewAllDates
	Obsolete DW TrainingStartFilterJDay
	Obsolete DW TrainingStartFilter
	Obsolete DW TrainingEndFilterJDay
	Obsolete DW TrainingEndFilter
Export
	DW delim
	ST delimiter
	DW dateformat
	ST col[n]
ExportCal
	ST col[n]
ExportCalAppt
	ST col[n]
ExportCalTask
	ST col[n]
ExportLog
	ST col[n]
Filter[number]
	DW Cal
	DW AllDates
	DW Start
	DW StartJDay
	DW End
	DW EndJDay
	DW AllVenues
	ST FilterVenues
	DW ViewRuns
	DW AllNames
	ST FilterNames
Import
	DW row
	DW delim
	ST delimiter
	DW dateformat
	ST col[n]
ImportCal
	ST col[n]
ImportLog
	ST col[n]
Last
	ST Division
	ST Level
	ST Height
	ST RefHeight
	ST Judge
	ST Handler
Settings:
	DW autoCheck
	DW BackupFiles
	DW Lang
	DW ShowSplash
	ST Splash
	DW autoShowTitle
	DW showHtml
	DW dateFormat[n]
		{
			eRunTree	= 0,
			eRunList	= 1,
			ePoints		= 2,
			eCalList	= 3,
			eCalendar	= 4,
			eTraining	= 5,
		}
Unknown
	ST col[n]
*/

/////////////////////////////////////////////////////////////////////////////

void CFontInfo::CreateFont(
		CFont& font,
		CDC* pDC)
{
	font.DeleteObject();
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	if (pDC && pDC->IsPrinting())
	{
		int logPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
		logFont.lfHeight = -MulDiv(size/10, logPixelsY, 72);
	}
	else
		logFont.lfHeight = size;
	lstrcpy(logFont.lfFaceName, (LPCTSTR)name);
	if (italic)
		logFont.lfItalic = TRUE;
	else
		logFont.lfItalic = FALSE;
	if (bold)
		logFont.lfWeight = FW_BOLD;
	else
		logFont.lfWeight = FW_NORMAL;
	if (pDC && pDC->IsPrinting())
		font.CreateFontIndirect(&logFont);
	else
		font.CreatePointFontIndirect(&logFont);
}


void CFontInfo::CreateFont(
		CFontDialog const& dlg,
		CFont& font,
		CDC* pDC)
{
	size = dlg.GetSize();
	name = dlg.GetFaceName();
	italic = dlg.IsItalic() ? true : false;
	bold = dlg.IsBold() ? true : false;
	CreateFont(font, pDC);
}

/////////////////////////////////////////////////////////////////////////////
// Calendar options

bool CAgilityBookOptions::AutoDeleteCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("AutoDelete"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetAutoDeleteCalendarEntries(bool bAuto)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("AutoDelete"), bAuto ? 1 : 0);
}


// View all or hide old entries
bool CAgilityBookOptions::ViewAllCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("ViewAll"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("ViewAll"), bView ? 1 : 0);
}


bool CAgilityBookOptions::ViewAllCalendarOpening()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("ViewOpen"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetViewAllCalendarOpening(bool bView)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("ViewOpen"), bView ? 1 : 0);
}


bool CAgilityBookOptions::ViewAllCalendarClosing()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("ViewClose"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetViewAllCalendarClosing(bool bView)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("ViewClose"), bView ? 1 : 0);
}


int CAgilityBookOptions::DaysTillEntryIsPast()
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), _T("PastEntry"), 5);
}


void CAgilityBookOptions::SetDaysTillEntryIsPast(int nDays)
{
	if (0 > nDays)
		nDays = 0;
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("PastEntry"), nDays);
}


bool CAgilityBookOptions::HideOverlappingCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("HideOverlapping"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetHideOverlappingCalendarEntries(bool bHide)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("HideOverlapping"), bHide ? 1 : 0);
}


static LPCTSTR CalItemName(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		return _T("NotEnteredColor");
	case CAgilityBookOptions::eCalColorPlanning:
		return _T("PlanningColor");
	case CAgilityBookOptions::eCalColorOpening:
		return _T("OpenColor");
	case CAgilityBookOptions::eCalColorClosing:
		return _T("CloseColor");
	case CAgilityBookOptions::eCalColorEntered:
		return _T("EnteredColor");
	}
	ASSERT(0);
	return _T("");
}


static COLORREF CalItemColor(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		return RGB(0,0,0); // Black
	case CAgilityBookOptions::eCalColorPlanning:
		return RGB(255,128,0); // Orange
	case CAgilityBookOptions::eCalColorOpening:
		return RGB(0,128,0); // Dk Green
	case CAgilityBookOptions::eCalColorClosing:
		return RGB(255,0,0); // Red
	case CAgilityBookOptions::eCalColorEntered:
		return RGB(0,0,255); // Blue
	}
	ASSERT(0);
	return 0;
}


COLORREF CAgilityBookOptions::CalendarColor(CalendarColorItem inItem)
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), CalItemName(inItem), CalItemColor(inItem));
}


void CAgilityBookOptions::SetCalendarColor(CalendarColorItem inItem, COLORREF inColor)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), CalItemName(inItem), inColor);
}


int CAgilityBookOptions::CalendarOpeningNear()
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), _T("OpenNear"), 5);
}


void CAgilityBookOptions::SetCalendarOpeningNear(int inDays)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("OpenNear"), inDays);
}


int CAgilityBookOptions::CalendarClosingNear()
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), _T("CloseNear"), 10);
}


void CAgilityBookOptions::SetCalendarClosingNear(int inDays)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("CloseNear"), inDays);
}


COLORREF CAgilityBookOptions::CalendarOpeningNearColor()
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), _T("OpenNearColor"), RGB(0,0,255));
}


void CAgilityBookOptions::SetCalendarOpeningNearColor(COLORREF inColor)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("OpenNearColor"), inColor);
}


COLORREF CAgilityBookOptions::CalendarClosingNearColor()
{
	return AfxGetApp()->GetProfileInt(_T("Calendar"), _T("CloseNearColor"), RGB(255,0,0));
}


void CAgilityBookOptions::SetCalendarClosingNearColor(COLORREF inColor)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("CloseNearColor"), inColor);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDate::DayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("FirstDayOfWeek"), static_cast<int>(ARBDate::eSunday));
	if (val < 0 || val > 6)
		val = static_cast<int>(ARBDate::eSunday);
	return static_cast<ARBDate::DayOfWeek>(val);
}


void CAgilityBookOptions::SetFirstDayOfWeek(ARBDate::DayOfWeek day)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("FirstDayOfWeek"), static_cast<int>(day));
}

/////////////////////////////////////////////////////////////////////////////
// Runs/points options

bool CAgilityBookOptions::GetViewRunsByTrial()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewRunsByTrial"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetViewRunsByTrial(bool bView)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewRunsByTrial"), bView ? 1 : 0);
}


bool CAgilityBookOptions::GetNewestDatesFirst()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewNewestFirst"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetNewestDatesFirst(bool bNewest)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewNewestFirst"), bNewest ? 1 : 0);
}


void CAgilityBookOptions::GetPointsViewSort(
		PointsViewSort& outPrimary,
		PointsViewSort& outSecondary,
		PointsViewSort& outTertiary)
{
	outPrimary = (PointsViewSort)AfxGetApp()->GetProfileInt(_T("Common"), _T("sortPtVw1"), (int)ePointsViewSortDivision);
	outSecondary = (PointsViewSort)AfxGetApp()->GetProfileInt(_T("Common"), _T("sortPtVw2"), (int)ePointsViewSortLevel);
	outTertiary = (PointsViewSort)AfxGetApp()->GetProfileInt(_T("Common"), _T("sortPtVw3"), (int)ePointsViewSortEvent);
}


void CAgilityBookOptions::SetPointsViewSort(
		PointsViewSort inPrimary,
		PointsViewSort inSecondary,
		PointsViewSort inTertiary)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("sortPtVw1"), (int)inPrimary);
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("sortPtVw2"), (int)inSecondary);
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("sortPtVw3"), (int)inTertiary);
}


bool CAgilityBookOptions::GetViewHiddenTitles()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewHiddenTitles"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetViewHiddenTitles(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewHiddenTitles"), bSet ? 1 : 0);
}


bool CAgilityBookOptions::GetTableInYPS()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("TableInYPS"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetTableInYPS(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("TableInYPS"), bSet ? 1 : 0);
}


bool CAgilityBookOptions::GetIncludeCRCDImage()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("CRCDImage"), 0);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetIncludeCRCDImage(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("CRCDImage"), bSet ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Font options

void CAgilityBookOptions::GetPrinterFontInfo(CFontInfo& info)
{
	info.name = _T("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	CString item(_T("PrintFontList"));
	info.name = AfxGetApp()->GetProfileString(_T("Common"), item + _T("Name"), info.name);
	info.size = AfxGetApp()->GetProfileInt(_T("Common"), item + _T("Size"), info.size);
	info.italic = (AfxGetApp()->GetProfileInt(_T("Common"), item + _T("Italic"), info.italic ? 1 : 0)) == 1 ? true : false;
	info.bold = (AfxGetApp()->GetProfileInt(_T("Common"), item + _T("Bold"), info.bold ? 1 : 0)) == 1 ? true : false;
}


void CAgilityBookOptions::SetPrinterFontInfo(CFontInfo const& info)
{
	CString item(_T("PrintFontList"));
	AfxGetApp()->WriteProfileString(_T("Common"), item + _T("Name"), info.name);
	AfxGetApp()->WriteProfileInt(_T("Common"), item + _T("Size"), info.size);
	AfxGetApp()->WriteProfileInt(_T("Common"), item + _T("Italic"), info.italic ? 1 : 0);
	AfxGetApp()->WriteProfileInt(_T("Common"), item + _T("Bold"), info.bold ? 1 : 0);
}


void CAgilityBookOptions::GetPrinterMargins(CRect& outMargins)
{
	outMargins.left = AfxGetApp()->GetProfileInt(_T("Common"), _T("Margins.L"), 50);
	outMargins.top = AfxGetApp()->GetProfileInt(_T("Common"), _T("Margins.T"), 50);
	outMargins.right = AfxGetApp()->GetProfileInt(_T("Common"), _T("Margins.R"), 50);
	outMargins.bottom = AfxGetApp()->GetProfileInt(_T("Common"), _T("Margins.B"), 50);
}


void CAgilityBookOptions::SetPrinterMargins(CRect const& inMargins)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("Margins.L"), inMargins.left);
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("Margins.T"), inMargins.top);
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("Margins.R"), inMargins.right);
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("Margins.B"), inMargins.bottom);
}


void CAgilityBookOptions::GetCalendarFontInfo(CFontInfo& info)
{
	info.name = _T("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	info.name = AfxGetApp()->GetProfileString(_T("Calendar"), _T("FontTextName"), info.name);
	info.size = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("FontTextSize"), info.size);
	info.italic = (AfxGetApp()->GetProfileInt(_T("Calendar"), _T("FontTextItalic"), info.italic ? 1 : 0)) == 1 ? true : false;
	info.bold = (AfxGetApp()->GetProfileInt(_T("Calendar"), _T("FontTextBold"), info.bold ? 1 : 0)) == 1 ? true : false;
}


void CAgilityBookOptions::SetCalendarFontInfo(CFontInfo const& info)
{
	AfxGetApp()->WriteProfileString(_T("Calendar"), _T("FontTextName"), info.name);
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("FontTextSize"), info.size);
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("FontTextItalic"), info.italic ? 1 : 0);
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("FontTextBold"), info.bold ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Last entered options

CString CAgilityBookOptions::GetLastEnteredDivision()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("Division"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredDivision(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("Division"), inLast);
}


CString CAgilityBookOptions::GetLastEnteredLevel()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("Level"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredLevel(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("Level"), inLast);
}


CString CAgilityBookOptions::GetLastEnteredHeight()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("Height"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredHeight(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("Height"), inLast);
}


CString CAgilityBookOptions::GetLastEnteredRefHeight()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("RefHeight"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredRefHeight(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("RefHeight"), inLast);
}


CString CAgilityBookOptions::GetLastEnteredJudge()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("Judge"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredJudge(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("Judge"), inLast);
}


CString CAgilityBookOptions::GetLastEnteredHandler()
{
	return AfxGetApp()->GetProfileString(_T("Last"), _T("Handler"), _T(""));
}


void CAgilityBookOptions::SetLastEnteredHandler(TCHAR const* inLast)
{
	AfxGetApp()->WriteProfileString(_T("Last"), _T("Handler"), inLast);
}

/////////////////////////////////////////////////////////////////////////////
// Import/export options

int CAgilityBookOptions::GetImportStartRow()
{
	int row = AfxGetApp()->GetProfileInt(_T("Import"), _T("row"), 1);
	if (0 > row)
		row = 1;
	return row;
}


void CAgilityBookOptions::SetImportStartRow(int row)
{
	AfxGetApp()->WriteProfileInt(_T("Import"), _T("row"), row);
}


void CAgilityBookOptions::GetImportExportDelimiters(
		bool bImport,
		int& delim,
		CString& delimiter)
{
	CString section;
	if (bImport)
		section = _T("Import");
	else
		section = _T("Export");
	delim = eDelimTab;
	delimiter.Empty();
	delim = AfxGetApp()->GetProfileInt(section, _T("delim"), delim);
	delimiter = AfxGetApp()->GetProfileString(section, _T("delimiter"), delimiter);
	if (1 < delimiter.GetLength())
		delimiter = delimiter.Left(1);
}


void CAgilityBookOptions::SetImportExportDelimiters(
		bool bImport,
		int delim,
		CString const& delimiter)
{
	CString section;
	if (bImport)
		section = _T("Import");
	else
		section = _T("Export");
	AfxGetApp()->WriteProfileInt(section, _T("delim"), delim);
	AfxGetApp()->WriteProfileString(section, _T("delimiter"), delimiter);
}


void CAgilityBookOptions::GetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat& outFormat)
{
	CString section;
	if (bImport)
		section = _T("Import");
	else
		section = _T("Export");
	outFormat = static_cast<ARBDate::DateFormat>(AfxGetApp()->GetProfileInt(section, _T("dateformat"), static_cast<int>(ARBDate::eDashYYYYMMDD)));
}


void CAgilityBookOptions::SetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat inFormat)
{
	CString section;
	if (bImport)
		section = _T("Import");
	else
		section = _T("Export");
	AfxGetApp()->WriteProfileInt(section, _T("dateformat"), static_cast<int>(inFormat));
}


static TCHAR const* const GetColumnName(CAgilityBookOptions::ColumnOrder eOrder)
{
	switch (eOrder)
	{
	default:
		return _T("Unknown");
	case CAgilityBookOptions::eRunsImport:
		return _T("Import");
	case CAgilityBookOptions::eRunsExport:
		return _T("Export");
	case CAgilityBookOptions::eCalImport:
		return _T("ImportCal");
	case CAgilityBookOptions::eCalExport:
		return _T("ExportCal");
	case CAgilityBookOptions::eCalExportAppt:
		return _T("ExportCalAppt");
	case CAgilityBookOptions::eCalExportTask:
		return _T("ExportCalTask");
	case CAgilityBookOptions::eLogImport:
		return _T("ImportLog");
	case CAgilityBookOptions::eLogExport:
		return _T("ExportLog");
	case CAgilityBookOptions::eViewTree:
	case CAgilityBookOptions::eViewRuns:
	case CAgilityBookOptions::eViewCalList:
	case CAgilityBookOptions::eViewLog:
	case CAgilityBookOptions::eViewCal:
		return _T("Columns");
	}
}


void CAgilityBookOptions::GetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<int>& outValues)
{
	outValues.clear();
	otstringstream item;
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
	item << _T("col") << static_cast<UINT>(idxColumn);
#else
	item << _T("col") << idxColumn;
#endif
	CString data = AfxGetApp()->GetProfileString(GetColumnName(eOrder), item.str().c_str(), _T(""));
	int idx = data.Find(',');
	while (0 <= idx)
	{
		int val = _tstol((LPCTSTR)data);
		outValues.push_back(val);
		data = data.Mid(idx+1);
		idx = data.Find(',');
	}
	if (0 < data.GetLength())
	{
		int val = _tstol((LPCTSTR)data);
		outValues.push_back(val);
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
		case eViewTree:
			switch (idxColumn)
			{
			case IO_TYPE_VIEW_TREE_DOG:
				outValues.push_back(IO_TREE_DOG_CALLNAME);
				outValues.push_back(IO_TREE_DOG_AGE);
				break;
			case IO_TYPE_VIEW_TREE_TRIAL:
				outValues.push_back(IO_TREE_TRIAL_START);
				outValues.push_back(IO_TREE_TRIAL_VENUE);
				outValues.push_back(IO_TREE_TRIAL_LOCATION);
				break;
			case IO_TYPE_VIEW_TREE_RUN:
				outValues.push_back(IO_TREE_RUN_DATE);
				outValues.push_back(IO_TREE_RUN_EVENT);
				break;
			}
			break;
		case eViewRuns:
			if (IO_TYPE_VIEW_RUNS_LIST == idxColumn)
			{
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
			}
			break;
		case eViewCalList:
			if (IO_TYPE_VIEW_CALENDAR_LIST == idxColumn)
			{
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_DRAWS);
				outValues.push_back(IO_CAL_NOTES);
			}
			break;
		case eViewLog:
			if (IO_TYPE_VIEW_TRAINING_LIST == idxColumn)
			{
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
			}
			break;
		case eViewCal:
			if (IO_TYPE_VIEW_CALENDAR == idxColumn)
			{
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
			}
			break;
		}
	}
}


void CAgilityBookOptions::SetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<int> const& inValues)
{
	otstringstream data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		if (0 < i)
			data << _T(",");
		data << inValues[i];
	}
	otstringstream item;
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
	item << _T("col") << static_cast<UINT>(idxColumn);
#else
	item << _T("col") << idxColumn;
#endif
	AfxGetApp()->WriteProfileString(GetColumnName(eOrder), item.str().c_str(), data.str().c_str());
}


/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookOptions::GetAutoUpdateCheck()
{
	int val = AfxGetApp()->GetProfileInt(_T("Settings"), _T("autoCheck"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::SetAutoUpdateCheck(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("autoCheck"), bSet ? 1 : 0);
}


int CAgilityBookOptions::GetNumBackupFiles()
{
	return AfxGetApp()->GetProfileInt(_T("Settings"), _T("BackupFiles"), 3);
}


void CAgilityBookOptions::SetNumBackupFiles(int nFiles)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("BackupFiles"), nFiles);
}


bool CAgilityBookOptions::AutoShowSplashScreen()
{
	int val = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ShowSplash"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::AutoShowSplashScreen(bool bAutoShow)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ShowSplash"), bAutoShow ? 1 : 0);
}


CString CAgilityBookOptions::GetSplashImage()
{
	return AfxGetApp()->GetProfileString(_T("Settings"), _T("Splash"), _T(""));
}


void CAgilityBookOptions::SetSplashImage(CString const& filename)
{
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Splash"), filename);
}


bool CAgilityBookOptions::AutoShowPropertiesOnNewTitle()
{
	int val = AfxGetApp()->GetProfileInt(_T("Settings"), _T("autoShowTitle"), 1);
	return val == 1 ? true : false;
}


void CAgilityBookOptions::AutoShowPropertiesOnNewTitle(bool bShow)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("autoShowTitle"), bShow ? 1 : 0);
}


ARBDate::DateFormat CAgilityBookOptions::GetDateFormat(FormattedDate inItem)
{
	otstringstream section;
	section << _T("dateFormat") << static_cast<int>(inItem);
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
	int val = AfxGetApp()->GetProfileInt(_T("Settings"), section.str().c_str(), static_cast<int>(def));
	return static_cast<ARBDate::DateFormat>(val);
}


void CAgilityBookOptions::SetDateFormat(
		FormattedDate inItem,
		ARBDate::DateFormat inFormat)
{
	otstringstream section;
	section << _T("dateFormat") << static_cast<int>(inItem);
	AfxGetApp()->WriteProfileInt(_T("Settings"), section.str().c_str(), static_cast<int>(inFormat));
}


bool CAgilityBookOptions::ShowHtmlPoints(bool* outIEInstalled)
{
	static bool s_bChecked = false;
	static bool s_bIEInstalled = false;

	if (!s_bChecked)
	{
		s_bChecked = true;
		HINSTANCE hShellDoc = LoadLibrary(_T("shdocvw.dll"));
		if (hShellDoc)
		{
			CVersionNum ver(hShellDoc);
			CVersionNum::VERSION_NUMBER verno;
			ver.GetVersion(verno);
			// See MSDN q164539: "How to Determine Which Version of Internet Explorer Is Installed"
			// Version 5.0.3314.2100: ie5.01sp2 (win95/98/nt4)
			// Version 5.0.3315.2879: ie5.01sp2 (w2k)
			if (5 < verno.part1
			|| (5 == verno.part1 && 0 < verno.part2)
			|| (5 == verno.part1 && 0 == verno.part2 && 3314 <= verno.part3))
				s_bIEInstalled = true;
			// No need to unload, we'll use it shortly.
		}
	}
	if (outIEInstalled)
		*outIEInstalled = s_bIEInstalled;
	if (s_bIEInstalled)
	{
		int val = AfxGetApp()->GetProfileInt(_T("Settings"), _T("showHtml"), 1);
		return val == 1 ? true : false;
	}
	else
		return false;
}


void CAgilityBookOptions::SetShowHtmlPoints(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("showHtml"), bSet ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////

CString CAgilityBookOptions::GetUserName(CString const& hint)
{
	return AfxGetApp()->GetProfileString(_T("UserNames"), hint, _T(""));
}


void CAgilityBookOptions::SetUserName(
		CString const& hint,
		CString const& userName)
{
	AfxGetApp()->WriteProfileString(_T("UserNames"), hint, userName);
}


bool CAgilityBookOptions::IsCalSiteVisible(
		CString const& filename,
		CVersionNum const& inVer)
{
	ASSERT(inVer.Valid());
	if (filename.IsEmpty())
		return true;
	bool bVisible = true;
	if (1 == AfxGetApp()->GetProfileInt(_T("CalSites"), filename, 1))
	{
		CVersionNum ver = GetCalSitePermanentStatus(filename);
		if (ver.Valid() && inVer <= ver)
			bVisible = false;
	}
	else
		bVisible = false;
	return bVisible;
}


void CAgilityBookOptions::SuppressCalSite(
		CString const& filename,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	AfxGetApp()->WriteProfileInt(_T("CalSites"), filename, bSuppress ? 0 : 1);
}


CVersionNum CAgilityBookOptions::GetCalSitePermanentStatus(CString const& filename)
{
	CVersionNum ver;
	if (!filename.IsEmpty())
	{
		CString str = AfxGetApp()->GetProfileString(_T("CalSites2"), filename, NULL);
		if (!str.IsEmpty())
			ver.Parse(filename, str);
	}
	return ver;
}


void CAgilityBookOptions::SuppressCalSitePermanently(
		CString const& filename,
		CVersionNum const& inVer,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	if (bSuppress)
		AfxGetApp()->WriteProfileString(_T("CalSites2"), filename, inVer.GetVersionString());
	else
	{
		// If we're clearing one, make sure we haven't written a different version
		CVersionNum ver;
		CString str = AfxGetApp()->GetProfileString(_T("CalSites2"), filename, NULL);
		if (!str.IsEmpty())
			ver.Parse(filename, str);
		if (ver == inVer)
			AfxGetApp()->WriteProfileString(_T("CalSites2"), filename, NULL);
	}
}
