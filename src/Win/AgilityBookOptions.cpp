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
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-11 DRC Added GetHideRequiredTitles.
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
#include "ARBDate.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "DlgAssignColumns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

void CFontInfo::CreateFont(CFont& font, CDC* pDC)
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

void CFontInfo::CreateFont(const CFontDialog& dlg, CFont& font, CDC* pDC)
{
	size = dlg.GetSize();
	name = dlg.GetFaceName();
	italic = dlg.IsItalic() ? true : false;
	bold = dlg.IsBold() ? true : false;
	CreateFont(font, pDC);
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions

bool CAgilityBookOptions::IsFilterEnabled()
{
	if (CAgilityBookOptions::GetViewAllDates()
	&& CAgilityBookOptions::GetViewAllVenues()
	&& CAgilityBookOptions::GetViewAllRuns())
		return false;
	else
		return true;
}

bool CAgilityBookOptions::IsDateVisible(
	const ARBDate& startDate,
	const ARBDate& endDate)
{
	if (!CAgilityBookOptions::GetViewAllDates())
	{
		if (CAgilityBookOptions::GetStartFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetStartFilterDate();
			if (startDate < date)
				return false;
		}
		if (CAgilityBookOptions::GetEndFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetEndFilterDate();
			if (endDate > date)
				return false;
		}
	}
	return true;
}

bool CAgilityBookOptions::IsTitleVisible(
	const std::vector<CVenueFilter>& venues,
	const ARBDogTitle* pTitle)
{
	if (!IsDateVisible(pTitle->GetDate(), pTitle->GetDate()))
		return false;
	return IsVenueVisible(venues, pTitle->GetVenue());
}

bool CAgilityBookOptions::IsVenueVisible(
	const std::vector<CVenueFilter>& venues,
	const std::string& venue)
{
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if ((*iter).venue == venue)
				return true;
		}
		return false;
	}
	return true;
}

bool CAgilityBookOptions::IsTrialVisible(
	const std::vector<CVenueFilter>& venues,
	const ARBDogTrial* pTrial)
{
	// Yes, it seems backwards, but it is correct.
	if (!IsDateVisible(pTrial->GetRuns().GetEndDate(), pTrial->GetRuns().GetStartDate()))
		return false;
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		for (ARBDogClubList::const_iterator iterClub = pTrial->GetClubs().begin();
			iterClub != pTrial->GetClubs().end();
			++iterClub)
		{
			if (IsVenueVisible(venues, (*iterClub)->GetVenue()))
				return true;
		}
		return false;
	}
	return true;
}

bool CAgilityBookOptions::IsRunVisible(
	const std::vector<CVenueFilter>& venues,
	const ARBDogTrial* pTrial,
	const ARBDogRun* pRun)
{
	if (!IsDateVisible(pRun->GetDate(), pRun->GetDate()))
		return false;
	bool bVisible = true;
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		bVisible = false;
		if (IsTrialVisible(venues, pTrial))
		{
			for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
				iter != venues.end();
				++iter)
			{
				if (pRun->GetDivision() == (*iter).division
				&& pRun->GetLevel() == (*iter).level)
				{
					bVisible = true;
					break;
				}
			}
		}
	}
	if (bVisible && !CAgilityBookOptions::GetViewAllRuns())
	{
		bVisible = false;
		bool bQualifying = CAgilityBookOptions::GetViewQRuns();
		if ((pRun->GetQ().Qualified() && bQualifying)
		|| (!pRun->GetQ().Qualified() && !bQualifying))
			bVisible = true;
	}
	return bVisible;
}

bool CAgilityBookOptions::IsTrainingLogVisible(
	const std::set<std::string>& names,
	const ARBTraining* pTraining)
{
	if (!CAgilityBookOptions::GetTrainingViewAllDates())
	{
		if (CAgilityBookOptions::GetTrainingStartFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetTrainingStartFilterDate();
			if (pTraining->GetDate() < date)
				return false;
		}
		if (CAgilityBookOptions::GetTrainingEndFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetTrainingEndFilterDate();
			if (pTraining->GetDate() > date)
				return false;
		}
	}
	bool bVisible = true;
	if (!CAgilityBookOptions::GetTrainingViewAllNames())
	{
		bVisible = false;
		for (std::set<std::string>::const_iterator iter = names.begin();
			iter != names.end();
			++iter)
		{
			if (pTraining->GetName() == *iter)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}

/////////////////////////////////////////////////////////////////////////////
// Calendar options

bool CAgilityBookOptions::AutoDeleteCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt("Calendar", "AutoDelete", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetAutoDeleteCalendarEntries(bool bAuto)
{
	AfxGetApp()->WriteProfileInt("Calendar", "AutoDelete", bAuto ? 1 : 0);
}

bool CAgilityBookOptions::ViewAllCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt("Calendar", "ViewAll", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	AfxGetApp()->WriteProfileInt("Calendar", "ViewAll", bView ? 1 : 0);
}

int CAgilityBookOptions::DaysTillEntryIsPast()
{
	return AfxGetApp()->GetProfileInt("Calendar", "PastEntry", 5);
}

void CAgilityBookOptions::SetDaysTillEntryIsPast(int nDays)
{
	if (0 > nDays)
		nDays = 0;
	AfxGetApp()->WriteProfileInt("Calendar", "PastEntry", nDays);
}

bool CAgilityBookOptions::HideOverlappingCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt("Calendar", "HideOverlapping", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetHideOverlappingCalendarEntries(bool bHide)
{
	AfxGetApp()->WriteProfileInt("Calendar", "HideOverlapping", bHide ? 1 : 0);
}

CSize CAgilityBookOptions::GetCalendarEntrySize()
{
	CSize szInches(100, 70);
	if (10 > (szInches.cx = abs(static_cast<LONG>(AfxGetApp()->GetProfileInt("Calendar", "EntrySize.cx", szInches.cx)))))
		szInches.cx = 10;
	if (10 > (szInches.cy = abs(static_cast<LONG>(AfxGetApp()->GetProfileInt("Calendar", "EntrySize.cy", szInches.cy)))))
		szInches.cy = 10;
	return szInches;
}

void CAgilityBookOptions::SetCalendarEntrySize(const CSize& sz)
{
	AfxGetApp()->WriteProfileInt("Calendar", "EntrySize.cx", sz.cx);
	AfxGetApp()->WriteProfileInt("Calendar", "EntrySize.cy", sz.cy);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDate::DayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	int val = AfxGetApp()->GetProfileInt("Common", "FirstDayOfWeek", static_cast<int>(ARBDate::eSunday));
	if (val < 0 || val > 6)
		val = static_cast<int>(ARBDate::eSunday);
	return static_cast<ARBDate::DayOfWeek>(val);
}

void CAgilityBookOptions::SetFirstDayOfWeek(ARBDate::DayOfWeek day)
{
	AfxGetApp()->WriteProfileInt("Common", "FirstDayOfWeek", static_cast<int>(day));
}

/////////////////////////////////////////////////////////////////////////////
// Runs/points options

bool CAgilityBookOptions::GetViewRunsByTrial()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewRunsByTrial", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewRunsByTrial(bool bView)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewRunsByTrial", bView ? 1 : 0);
}

bool CAgilityBookOptions::GetNewestDatesFirst()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewNewestFirst", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetNewestDatesFirst(bool bNewest)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewNewestFirst", bNewest ? 1 : 0);
}

bool CAgilityBookOptions::GetViewAllDates()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewAllDates", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllDates(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewAllDates", bViewAll ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetStartFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt("Common", "StartFilterJDay", date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetStartFilterDate(const ARBDate& date)
{
	AfxGetApp()->WriteProfileInt("Common", "StartFilterJDay", date.GetJulianDay());
}

bool CAgilityBookOptions::GetStartFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt("Common", "StartFilter", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetStartFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt("Common", "StartFilter", bSet ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetEndFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt("Common", "EndFilterJDay", date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetEndFilterDate(const ARBDate& date)
{
	AfxGetApp()->WriteProfileInt("Common", "EndFilterJDay", date.GetJulianDay());
}

bool CAgilityBookOptions::GetEndFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt("Common", "EndFilter", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetEndFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt("Common", "EndFilter", bSet ? 1 : 0);
}

bool CAgilityBookOptions::GetViewAllVenues()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewAllVenues", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllVenues(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewAllVenues", bViewAll ? 1 : 0);
}

static bool s_venueCacheInit = false;
static std::vector<CVenueFilter> s_venueCache;

void CAgilityBookOptions::GetFilterVenue(std::vector<CVenueFilter>& venues)
{
	venues.clear();
	if (s_venueCacheInit)
	{
		venues = s_venueCache;
	}
	else
	{
		CString venue;
		venue = AfxGetApp()->GetProfileString("Common", "FilterVenue", "");
		if (!venue.IsEmpty())
		{
			CStringArray raw;
			int pos;
			while (0 <= (pos = venue.Find(':')))
			{
				raw.Add(venue.Left(pos));
				venue = venue.Mid(pos+1);
			}
			raw.Add(venue);
			for (int i = 0; i < raw.GetSize(); ++i)
			{
				venue = raw[i];
				CStringArray rawFilter;
				while (0 <= (pos = venue.Find('/')))
				{
					rawFilter.Add(venue.Left(pos));
					venue = venue.Mid(pos+1);
				}
				rawFilter.Add(venue);
				if (0 < rawFilter.GetSize())
				{
					CVenueFilter filter;
					switch (rawFilter.GetSize())
					{
					default:
					case 3:
						filter.level = (LPCSTR)rawFilter[2];
						// fallthru
					case 2:
						filter.division = (LPCSTR)rawFilter[1];
						// fallthru
					case 1:
						filter.venue = (LPCSTR)rawFilter[0];
					}
					venues.push_back(filter);
				}
			}
		}
		s_venueCacheInit = true;
		s_venueCache = venues;
	}
}

void CAgilityBookOptions::SetFilterVenue(const std::vector<CVenueFilter>& venues)
{
	CString venue;
	for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
		iter != venues.end();
		++iter)
	{
		if (!venue.IsEmpty())
			venue += ':';
		venue += (*iter).venue.c_str();
		venue += "/";
		venue += (*iter).division.c_str();
		venue += "/";
		venue += (*iter).level.c_str();
	}
	AfxGetApp()->WriteProfileString("Common", "FilterVenue", venue);
	s_venueCacheInit = true;
	s_venueCache = venues;
}

bool CAgilityBookOptions::GetViewAllRuns()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewAllRuns", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllRuns(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewAllRuns", bViewAll ? 1 : 0);
}

// Subset of AllRuns
bool CAgilityBookOptions::GetViewQRuns()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewQRuns", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewQRuns(bool bViewQs)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewQRuns", bViewQs ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Training Log options

bool CAgilityBookOptions::GetTrainingViewAllDates()
{
	int val = AfxGetApp()->GetProfileInt("Common", "TrainingViewAllDates", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetTrainingViewAllDates(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt("Common", "TrainingViewAllDates", bViewAll ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetTrainingStartFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt("Common", "TrainingStartFilterJDay", date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetTrainingStartFilterDate(const ARBDate& date)
{
	AfxGetApp()->WriteProfileInt("Common", "TrainingStartFilterJDay", date.GetJulianDay());
}

bool CAgilityBookOptions::GetTrainingStartFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt("Common", "TrainingStartFilter", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetTrainingStartFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt("Common", "TrainingStartFilter", bSet ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetTrainingEndFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt("Common", "TrainingEndFilterJDay", date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetTrainingEndFilterDate(const ARBDate& date)
{
	AfxGetApp()->WriteProfileInt("Common", "TrainingEndFilterJDay", date.GetJulianDay());
}

bool CAgilityBookOptions::GetTrainingEndFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt("Common", "TrainingEndFilter", 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetTrainingEndFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt("Common", "TrainingEndFilter", bSet ? 1 : 0);
}

bool CAgilityBookOptions::GetTrainingViewAllNames()
{
	int val = AfxGetApp()->GetProfileInt("Common", "ViewAllNames", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetTrainingViewAllNames(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt("Common", "ViewAllNames", bViewAll ? 1 : 0);
}

void CAgilityBookOptions::GetTrainingFilterNames(std::set<std::string>& outNames)
{
	outNames.clear();
	CString names;
	names = AfxGetApp()->GetProfileString("Common", "FilterTrainingNames", "");
	if (!names.IsEmpty())
	{
		int pos;
		while (0 <= (pos = names.Find(':')))
		{
			outNames.insert((LPCSTR)names.Left(pos));
			names = names.Mid(pos+1);
		}
		outNames.insert((LPCSTR)names);
	}
}

void CAgilityBookOptions::SetTrainingFilterNames(const std::set<std::string>& inNames)
{
	CString names;
	for (std::set<std::string>::const_iterator iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		if (!names.IsEmpty())
			names += ':';
		names += (*iter).c_str();
	}
	AfxGetApp()->WriteProfileString("Common", "FilterTrainingNames", names);
}

/////////////////////////////////////////////////////////////////////////////
// Font options

void CAgilityBookOptions::GetPrinterFontInfo(CFontInfo& info)
{
	info.name = "Times New Roman";
	info.size = 80;
	info.italic = false;
	info.bold = false;
	CString item("PrintFontList");
	info.name = AfxGetApp()->GetProfileString("Common", item + "Name", info.name);
	info.size = AfxGetApp()->GetProfileInt("Common", item + "Size", info.size);
	info.italic = (AfxGetApp()->GetProfileInt("Common", item + "Italic", info.italic ? 1 : 0)) == 1 ? true : false;
	info.bold = (AfxGetApp()->GetProfileInt("Common", item + "Bold", info.bold ? 1 : 0)) == 1 ? true : false;
}

void CAgilityBookOptions::SetPrinterFontInfo(const CFontInfo& info)
{
	CString item("PrintFontList");
	AfxGetApp()->WriteProfileString("Common", item + "Name", info.name);
	AfxGetApp()->WriteProfileInt("Common", item + "Size", info.size);
	AfxGetApp()->WriteProfileInt("Common", item + "Italic", info.italic ? 1 : 0);
	AfxGetApp()->WriteProfileInt("Common", item + "Bold", info.bold ? 1 : 0);
}

void CAgilityBookOptions::GetCalendarDateFontInfo(CFontInfo& info, BOOL bPrinting)
{
	info.name = "Times New Roman";
	info.size = 100;
	info.italic = false;
	info.bold = false;
	CString item("Font");
	if (bPrinting)
	{
		item = "PrintFont";
		info.size = 80;
	}
	item += "Date";
	info.name = AfxGetApp()->GetProfileString("Calendar", item + "Name", info.name);
	info.size = AfxGetApp()->GetProfileInt("Calendar", item + "Size", info.size);
	info.italic = (AfxGetApp()->GetProfileInt("Calendar", item + "Italic", info.italic ? 1 : 0)) == 1 ? true : false;
	info.bold = (AfxGetApp()->GetProfileInt("Calendar", item + "Bold", info.bold ? 1 : 0)) == 1 ? true : false;
}

void CAgilityBookOptions::SetCalendarDateFontInfo(const CFontInfo& info, BOOL bPrinting)
{
	CString item("Font");
	if (bPrinting)
		item = "PrintFont";
	item += "Date";
	AfxGetApp()->WriteProfileString("Calendar", item + "Name", info.name);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Size", info.size);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Italic", info.italic ? 1 : 0);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Bold", info.bold ? 1 : 0);
}

void CAgilityBookOptions::GetCalendarTextFontInfo(CFontInfo& info, BOOL bPrinting)
{
	info.name = "Times New Roman";
	info.size = 100;
	info.italic = false;
	info.bold = false;
	CString item("Font");
	if (bPrinting)
	{
		item = "PrintFont";
		info.size = 80;
	}
	item += "Text";
	info.name = AfxGetApp()->GetProfileString("Calendar", item + "Name", info.name);
	info.size = AfxGetApp()->GetProfileInt("Calendar", item + "Size", info.size);
	info.italic = (AfxGetApp()->GetProfileInt("Calendar", item + "Italic", info.italic ? 1 : 0)) == 1 ? true : false;
	info.bold = (AfxGetApp()->GetProfileInt("Calendar", item + "Bold", info.bold ? 1 : 0)) == 1 ? true : false;
}

void CAgilityBookOptions::SetCalendarTextFontInfo(const CFontInfo& info, BOOL bPrinting)
{
	CString item("Font");
	if (bPrinting)
		item = "PrintFont";
	item += "Text";
	AfxGetApp()->WriteProfileString("Calendar", item + "Name", info.name);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Size", info.size);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Italic", info.italic ? 1 : 0);
	AfxGetApp()->WriteProfileInt("Calendar", item + "Bold", info.bold ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Last entered options

CString CAgilityBookOptions::GetLastEnteredDivision()
{
	return AfxGetApp()->GetProfileString("Last", "Division", "");
}

void CAgilityBookOptions::SetLastEnteredDivision(const char* inLast)
{
	AfxGetApp()->WriteProfileString("Last", "Division", inLast);
}

CString CAgilityBookOptions::GetLastEnteredLevel()
{
	return AfxGetApp()->GetProfileString("Last", "Level", "");
}

void CAgilityBookOptions::SetLastEnteredLevel(const char* inLast)
{
	AfxGetApp()->WriteProfileString("Last", "Level", inLast);
}

CString CAgilityBookOptions::GetLastEnteredHeight()
{
	return AfxGetApp()->GetProfileString("Last", "Height", "");
}

void CAgilityBookOptions::SetLastEnteredHeight(const char* inLast)
{
	AfxGetApp()->WriteProfileString("Last", "Height", inLast);
}

CString CAgilityBookOptions::GetLastEnteredJudge()
{
	return AfxGetApp()->GetProfileString("Last", "Judge", "");
}

void CAgilityBookOptions::SetLastEnteredJudge(const char* inLast)
{
	AfxGetApp()->WriteProfileString("Last", "Judge", inLast);
}

CString CAgilityBookOptions::GetLastEnteredHandler()
{
	return AfxGetApp()->GetProfileString("Last", "Handler", "");
}

void CAgilityBookOptions::SetLastEnteredHandler(const char* inLast)
{
	AfxGetApp()->WriteProfileString("Last", "Handler", inLast);
}

/////////////////////////////////////////////////////////////////////////////
// Titles options

bool CAgilityBookOptions::GetHideRequiredTitles()
{
	int val = AfxGetApp()->GetProfileInt("Titles", "HideReq", 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetHideRequiredTitles(bool bHide)
{
	AfxGetApp()->WriteProfileInt("Titles", "HideReq", bHide ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Import/export options

int CAgilityBookOptions::GetImportStartRow()
{
	int row = AfxGetApp()->GetProfileInt("Import", "row", 1);
	if (0 > row)
		row = 1;
	return row;
}

void CAgilityBookOptions::SetImportStartRow(int row)
{
	AfxGetApp()->WriteProfileInt("Import", "row", row);
}

void CAgilityBookOptions::GetImportExportDelimiters(bool bImport, int& delim, CString& delimiter)
{
	CString section;
	if (bImport)
		section = "Import";
	else
		section = "Export";
	delim = eDelimTab;
	delimiter.Empty();
	delim = AfxGetApp()->GetProfileInt(section, "delim", delim);
	delimiter = AfxGetApp()->GetProfileString(section, "delimiter", delimiter);
	if (1 < delimiter.GetLength())
		delimiter = delimiter.Left(1);
}

void CAgilityBookOptions::SetImportExportDelimiters(bool bImport, int delim, const CString& delimiter)
{
	CString section;
	if (bImport)
		section = "Import";
	else
		section = "Export";
	AfxGetApp()->WriteProfileInt(section, "delim", delim);
	AfxGetApp()->WriteProfileString(section, "delimiter", delimiter);
}

void CAgilityBookOptions::GetImportExportDateFormat(bool bImport, ARBDate::DateFormat& outFormat)
{
	CString section;
	if (bImport)
		section = "Import";
	else
		section = "Export";
	outFormat = static_cast<ARBDate::DateFormat>(AfxGetApp()->GetProfileInt(section, "dateformat", static_cast<int>(ARBDate::eDashYYYYMMDD)));
}

void CAgilityBookOptions::SetImportExportDateFormat(bool bImport, ARBDate::DateFormat inFormat)
{
	CString section;
	if (bImport)
		section = "Import";
	else
		section = "Export";
	AfxGetApp()->WriteProfileInt(section, "dateformat", static_cast<int>(inFormat));
}

void CAgilityBookOptions::GetColumnOrder(ColumnOrder eOrder, size_t idxColumn, std::vector<int>& outValues)
{
	CString section;
	switch (eOrder)
	{
	default:			section = "Unknown"; break;
	case eRunsImport:	section = "Import"; break;
	case eRunsExport:	section = "Export"; break;
	case eCalImport:	section = "ImportCal"; break;
	case eCalExport:	section = "ExportCal"; break;
	case eLogImport:	section = "ImportLog"; break;
	case eLogExport:	section = "ExportLog"; break;
	case eViewTree:
	case eViewRuns:
	case eViewCal:
	case eViewLog:
		section = "Columns"; break;
	}
	outValues.clear();
	CString item;
	item.Format("col%d", idxColumn);
	CString data = AfxGetApp()->GetProfileString(section, item, "");
	int idx = data.Find(',');
	while (0 <= idx)
	{
		int val = atol((LPCTSTR)data);
		outValues.push_back(val);
		data = data.Mid(idx+1);
		idx = data.Find(',');
	}
	if (0 < data.GetLength())
	{
		int val = atol((LPCTSTR)data);
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
		case eLogImport:
		case eLogExport:
			if (IO_TYPE_TRAINING == idxColumn)
			{
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_NOTES);
			}
			break;
		case eViewTree:
			switch (idxColumn)
			{
			case IO_TYPE_VIEW_TREE_DOG:
				outValues.push_back(IO_TREE_DOG_CALLNAME);
				break;
			case IO_TYPE_VIEW_TREE_TRIAL:
				outValues.push_back(IO_TREE_TRIAL_START);
				outValues.push_back(IO_TREE_TRIAL_END);
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
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_COMMENTS);
			}
			break;
		case eViewCal:
			if (IO_TYPE_VIEW_CALENDAR_LIST == idxColumn)
			{
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_END_DATE);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_NOTES);
			}
			break;
		case eViewLog:
			if (IO_TYPE_VIEW_TRAINING_LIST == idxColumn)
			{
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_NOTES);
			}
			break;
		}
	}
}

void CAgilityBookOptions::SetColumnOrder(ColumnOrder eOrder, size_t idxColumn, const std::vector<int>& inValues)
{
	CString section;
	switch (eOrder)
	{
	default:			section = "Unknown"; break;
	case eRunsImport:	section = "Import"; break;
	case eRunsExport:	section = "Export"; break;
	case eCalImport:	section = "ImportCal"; break;
	case eCalExport:	section = "ExportCal"; break;
	case eLogImport:	section = "ImportLog"; break;
	case eLogExport:	section = "ExportLog"; break;
	case eViewTree:
	case eViewRuns:
	case eViewCal:
	case eViewLog:
		section = "Columns"; break;
	}
	CString item;
	CString data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		item.Format("%d", inValues[i]);
		if (0 < i)
			data += ",";
		data += item;
	}
	item.Format("col%d", idxColumn);
	AfxGetApp()->WriteProfileString(section, item, data);
}

/////////////////////////////////////////////////////////////////////////////
// Special clipboard formats

UINT CAgilityBookOptions::GetClipboardFormat(eClipFormat fmt)
{
	static bool bInitialized = false;
	static UINT uTrial = 0;
	static UINT uRun = 0;
	if (!bInitialized)
	{
		bInitialized = true;
		uTrial = RegisterClipboardFormat("ARB-Trial");
		uRun = RegisterClipboardFormat("ARB-Run");
	}
	switch (fmt)
	{
	default:			return 0;
	case eFormatTrial:	return uTrial;
	case eFormatRun:	return uRun;
	}
}
