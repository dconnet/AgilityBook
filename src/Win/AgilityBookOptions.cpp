/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @li 2003-19-05 DRC Removed registry entry: "Calendar"/"List" (boolean)
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookOptions.h"

#include "ARBAgilityRecordBook.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"

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
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		if (!IsTrialVisible(venues, pTrial))
			return false;
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if (pRun->GetDivision() == (*iter).division
			&& pRun->GetLevel() == (*iter).level)
				return true;
		}
		return false;
	}
	return true;
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
// Runs options

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
