/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Obsolete registry entries
// bool GetProfileInt("Common", "TrainingViewAllDates", 1)
// ARBDate(julian) GetProfileInt("Common", "TrainingStartFilterJDay", date.GetJulianDay()))
// bool GetProfileInt("Common", "TrainingStartFilter", 0);
// ARBDate(julian) GetProfileInt("Common", "TrainingEndFilterJDay", date.GetJulianDay()));
// bool GetProfileInt("Common", "TrainingEndFilter", 0);
//
// Calendar/FontDate+Name/Size/Italic/Bold (str,int,int,int)
// Calendar/PrintFontDate+Name/Size/Italic/Bold
// Calendar/PrintFontText+Name/Size/Italic/Bold

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
		ARBDate const& startDate,
		ARBDate const& endDate)
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
		std::vector<CVenueFilter> const& venues,
		ARBDogTitle const* pTitle)
{
	if (!GetViewHiddenTitles() && pTitle->IsHidden())
		return false;
	if (!pTitle->GetDate().IsValid()
	|| !IsDateVisible(pTitle->GetDate(), pTitle->GetDate()))
		return false;
	return IsVenueVisible(venues, pTitle->GetVenue());
}

bool CAgilityBookOptions::IsVenueVisible(
		std::vector<CVenueFilter> const& venues,
		ARBString const& venue)
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

bool CAgilityBookOptions::IsVenueDivisionVisible(
		std::vector<CVenueFilter> const& venues,
		ARBString const& venue,
		ARBString const& div)
{
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if ((*iter).venue == venue
			&& (*iter).division == div)
				return true;
		}
		return false;
	}
	return true;
}

bool CAgilityBookOptions::IsTrialVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrial const* pTrial)
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

// Return type should be the same as ARBBase::m_nFiltered
unsigned short CAgilityBookOptions::IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun)
{
	unsigned short nVisible = 0;
	if (!IsDateVisible(pRun->GetDate(), pRun->GetDate()))
		return nVisible;
	nVisible = (0x1 << ARBBase::eFilter) | (0x1 << ARBBase::eIgnoreQ);
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		nVisible = 0;
		// Don't call IsTrialVisible. We need more control over the club/venue
		// check. Otherwise (for instance), if AKC/NoviceB is disabled and
		// ASCA/NoviceB is enabled, since the division/level names match, the
		// AKC run will actually show up. So we need to make sure the venue
		// of the filter matches too.
		if (IsDateVisible(pTrial->GetRuns().GetEndDate(), pTrial->GetRuns().GetStartDate()))
		{
			for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
				iter != venues.end();
				++iter)
			{
				bool bCheck = false;
				for (ARBDogClubList::const_iterator iterClub = pTrial->GetClubs().begin();
					iterClub != pTrial->GetClubs().end();
					++iterClub)
				{
					if ((*iter).venue == (*iterClub)->GetVenue())
					{
						bCheck = true;
						break;
					}
				}
				if (bCheck
				&& pRun->GetDivision() == (*iter).division
				&& pRun->GetLevel() == (*iter).level)
				{
					nVisible = (0x1 << ARBBase::eFilter) | (0x1 << ARBBase::eIgnoreQ);
					break;
				}
			}
		}
	}
	if ((nVisible & (0x1 << ARBBase::eFilter))
	&& !CAgilityBookOptions::GetViewAllRuns())
	{
		// Only set the full filter, not the IgnoreQ filter.
		nVisible &= ~(0x1 << ARBBase::eFilter);
		bool bQualifying = CAgilityBookOptions::GetViewQRuns();
		if ((pRun->GetQ().Qualified() && bQualifying)
		|| (!pRun->GetQ().Qualified() && !bQualifying))
			nVisible |= (0x1 << ARBBase::eFilter);
	}
	return nVisible;
}

// This function is used in conjunction with the above. We only need to be
// concerned with trials with more than 1 club. This is used to filter a
// run in the points view - for instance, if you have an ASCA/NADAC trial and
// set the filtering to hide NADAC novice runs, the asca visibility caused
// the novice run to appear in the nadac points listing when it shouldn't.
bool CAgilityBookOptions::IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBConfigVenue const* pVenue,
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun)
{
	if (1 >= pTrial->GetClubs().size())
		return true;
	bool bVisible = true;
	if (!CAgilityBookOptions::GetViewAllVenues())
	{
		bVisible = false;
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if (pTrial->HasVenue(pVenue->GetName())
			&& pVenue->GetName() == (*iter).venue
			&& pRun->GetDivision() == (*iter).division
			&& pRun->GetLevel() == (*iter).level)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}

bool CAgilityBookOptions::IsCalendarVisible(ARBCalendar const* pCal)
{
	if (!CAgilityBookOptions::GetViewAllDates())
	{
		if (CAgilityBookOptions::GetStartFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetStartFilterDate();
			if (pCal->GetEndDate() < date)
				return false;
		}
		if (CAgilityBookOptions::GetEndFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetEndFilterDate();
			if (pCal->GetStartDate() > date)
				return false;
		}
	}
	return true;
}

bool CAgilityBookOptions::IsTrainingLogVisible(
		std::set<ARBString> const& names,
		ARBTraining const* pTraining)
{
	if (!CAgilityBookOptions::GetViewAllDates())
	{
		if (CAgilityBookOptions::GetStartFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetStartFilterDate();
			if (pTraining->GetDate() < date)
				return false;
		}
		if (CAgilityBookOptions::GetEndFilterDateSet())
		{
			ARBDate date = CAgilityBookOptions::GetEndFilterDate();
			if (pTraining->GetDate() > date)
				return false;
		}
	}
	bool bVisible = true;
	if (!CAgilityBookOptions::GetTrainingViewAllNames())
	{
		bVisible = false;
		for (std::set<ARBString>::const_iterator iter = names.begin();
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
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("AutoDelete"), 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetAutoDeleteCalendarEntries(bool bAuto)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("AutoDelete"), bAuto ? 1 : 0);
}

bool CAgilityBookOptions::ViewAllCalendarEntries()
{
	int val = AfxGetApp()->GetProfileInt(_T("Calendar"), _T("ViewAll"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("ViewAll"), bView ? 1 : 0);
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

CSize CAgilityBookOptions::GetCalendarEntrySize()
{
	CSize szInches(100, 70);
	if (10 > (szInches.cx = abs(static_cast<LONG>(AfxGetApp()->GetProfileInt(_T("Calendar"), _T("EntrySize.cx"), szInches.cx)))))
		szInches.cx = 10;
	if (10 > (szInches.cy = abs(static_cast<LONG>(AfxGetApp()->GetProfileInt(_T("Calendar"), _T("EntrySize.cy"), szInches.cy)))))
		szInches.cy = 10;
	return szInches;
}

void CAgilityBookOptions::SetCalendarEntrySize(CSize const& sz)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("EntrySize.cx"), sz.cx);
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("EntrySize.cy"), sz.cy);
}

CCalendarViewFilter CAgilityBookOptions::FilterCalendarView()
{
	unsigned short uVal = static_cast<unsigned short>(AfxGetApp()->GetProfileInt(_T("Calendar"), _T("Filter"), CCalendarViewFilter::eViewNormal));
	return uVal;
}

void CAgilityBookOptions::SetFilterCalendarView(CCalendarViewFilter inFilter)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("Filter"), inFilter.m_Filter);
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
	return "";
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
// Filtering: Date

bool CAgilityBookOptions::GetViewAllDates()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllDates"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllDates(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllDates"), bViewAll ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetStartFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt(_T("Common"), _T("StartFilterJDay"), date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetStartFilterDate(ARBDate const& date)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("StartFilterJDay"), date.GetJulianDay());
}

bool CAgilityBookOptions::GetStartFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("StartFilter"), 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetStartFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("StartFilter"), bSet ? 1 : 0);
}

ARBDate CAgilityBookOptions::GetEndFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt(_T("Common"), _T("EndFilterJDay"), date.GetJulianDay()));
	return date;
}

void CAgilityBookOptions::SetEndFilterDate(ARBDate const& date)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("EndFilterJDay"), date.GetJulianDay());
}

bool CAgilityBookOptions::GetEndFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("EndFilter"), 0);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetEndFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("EndFilter"), bSet ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Runs

bool CAgilityBookOptions::GetViewAllVenues()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllVenues"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllVenues(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllVenues"), bViewAll ? 1 : 0);
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
		venue = AfxGetApp()->GetProfileString(_T("Common"), _T("FilterVenue"), _T(""));
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
						filter.level = (LPCTSTR)rawFilter[2];
						// fallthru
					case 2:
						filter.division = (LPCTSTR)rawFilter[1];
						// fallthru
					case 1:
						filter.venue = (LPCTSTR)rawFilter[0];
					}
					venues.push_back(filter);
				}
			}
		}
		s_venueCacheInit = true;
		s_venueCache = venues;
	}
}

void CAgilityBookOptions::SetFilterVenue(std::vector<CVenueFilter> const& venues)
{
	CString venue;
	for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
		iter != venues.end();
		++iter)
	{
		if (!venue.IsEmpty())
			venue += ':';
		venue += (*iter).venue.c_str();
		venue += _T("/");
		venue += (*iter).division.c_str();
		venue += _T("/");
		venue += (*iter).level.c_str();
	}
	AfxGetApp()->WriteProfileString(_T("Common"), _T("FilterVenue"), venue);
	s_venueCacheInit = true;
	s_venueCache = venues;
}

bool CAgilityBookOptions::GetViewAllRuns()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllRuns"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewAllRuns(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllRuns"), bViewAll ? 1 : 0);
}

// Subset of AllRuns
bool CAgilityBookOptions::GetViewQRuns()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewQRuns"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetViewQRuns(bool bViewQs)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewQRuns"), bViewQs ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Training Log

bool CAgilityBookOptions::GetTrainingViewAllNames()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllNames"), 1);
	return val == 1 ? true : false;
}

void CAgilityBookOptions::SetTrainingViewAllNames(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllNames"), bViewAll ? 1 : 0);
}

void CAgilityBookOptions::GetTrainingFilterNames(std::set<ARBString>& outNames)
{
	outNames.clear();
	CString names;
	names = AfxGetApp()->GetProfileString(_T("Common"), _T("FilterTrainingNames"), _T(""));
	if (!names.IsEmpty())
	{
		int pos;
		while (0 <= (pos = names.Find(':')))
		{
			outNames.insert((LPCTSTR)names.Left(pos));
			names = names.Mid(pos+1);
		}
		outNames.insert((LPCTSTR)names);
	}
}

void CAgilityBookOptions::SetTrainingFilterNames(std::set<ARBString> const& inNames)
{
	CString names;
	for (std::set<ARBString>::const_iterator iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		if (!names.IsEmpty())
			names += ':';
		names += (*iter).c_str();
	}
	AfxGetApp()->WriteProfileString(_T("Common"), _T("FilterTrainingNames"), names);
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
	case CAgilityBookOptions::eViewCal:
	case CAgilityBookOptions::eViewLog:
		return _T("Columns");
	}
}

void CAgilityBookOptions::GetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<int>& outValues)
{
	outValues.clear();
	CString item;
	item.Format(_T("col%d"), idxColumn);
	CString data = AfxGetApp()->GetProfileString(GetColumnName(eOrder), item, _T(""));
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
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
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
	CString item;
	CString data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		item.Format(_T("%d"), inValues[i]);
		if (0 < i)
			data += _T(",");
		data += item;
	}
	item.Format(_T("col%d"), idxColumn);
	AfxGetApp()->WriteProfileString(GetColumnName(eOrder), item, data);
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
	CString section;
	section.Format(_T("dateFormat%d"), static_cast<int>(inItem));
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
	int val = AfxGetApp()->GetProfileInt(_T("Settings"), section, static_cast<int>(def));
	return static_cast<ARBDate::DateFormat>(val);
}

void CAgilityBookOptions::SetDateFormat(
		FormattedDate inItem,
		ARBDate::DateFormat inFormat)
{
	CString section;
	section.Format(_T("dateFormat%d"), static_cast<int>(inItem));
	AfxGetApp()->WriteProfileInt(_T("Settings"), section, static_cast<int>(inFormat));
}

/////////////////////////////////////////////////////////////////////////////
// Special clipboard formats

UINT CAgilityBookOptions::GetClipboardFormat(eClipFormat fmt)
{
	static bool bInitialized = false;
	static UINT uDog = 0;
	static UINT uTrial = 0;
	static UINT uRun = 0;
	static UINT uCal = 0;
	static UINT uiCal = 0;
	static UINT uLog = 0;
	if (!bInitialized)
	{
		bInitialized = true;
		uDog = RegisterClipboardFormat(_T("ARB-Dog"));
		uTrial = RegisterClipboardFormat(_T("ARB-Trial"));
		uRun = RegisterClipboardFormat(_T("ARB-Run"));
		uCal = RegisterClipboardFormat(_T("ARB-Cal"));
		uiCal = RegisterClipboardFormat(_T("+//ISBN 1-887687-00-9::versit::PDI//vCalendar"));
		uLog = RegisterClipboardFormat(_T("ARB-Log"));
	}
	switch (fmt)
	{
	default:				return 0;
	case eFormatDog:		return uDog;
	case eFormatTrial:		return uTrial;
	case eFormatRun:		return uRun;
	case eFormatCalendar:	return uCal;
	case eFormatiCalendar:	return uiCal;
	case eFormatLog:		return uLog;
	}
}
