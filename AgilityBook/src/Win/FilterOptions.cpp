/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2006-03-02 DRC Separated filter options from main options.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "FilterOptions.h"

#include "ARBAgilityRecordBook.h"
#include "ARBCalendar.h"
#include "ARBDate.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "AgilityBookOptions.h"
#include "DlgAssignColumns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Helper functions

bool CFilterOptions::IsFilterEnabled()
{
	if (CFilterOptions::GetViewAllDates()
	&& CFilterOptions::GetViewAllVenues()
	&& eViewRunsAll == CFilterOptions::GetViewRuns())
		return false;
	else
		return true;
}

bool CFilterOptions::IsDateVisible(
		ARBDate const& startDate,
		ARBDate const& endDate)
{
	if (!CFilterOptions::GetViewAllDates())
	{
		if (CFilterOptions::GetStartFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetStartFilterDate();
			if (startDate < date)
				return false;
		}
		if (CFilterOptions::GetEndFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetEndFilterDate();
			if (endDate > date)
				return false;
		}
	}
	return true;
}

bool CFilterOptions::IsTitleVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTitlePtr pTitle)
{
	if (!CAgilityBookOptions::GetViewHiddenTitles() && pTitle->IsHidden())
		return false;
	if (!pTitle->GetDate().IsValid()
	|| !IsDateVisible(pTitle->GetDate(), pTitle->GetDate()))
		return false;
	return IsVenueVisible(venues, pTitle->GetVenue());
}

bool CFilterOptions::IsVenueVisible(
		std::vector<CVenueFilter> const& venues,
		ARBString const& venue)
{
	if (!CFilterOptions::GetViewAllVenues())
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

bool CFilterOptions::IsVenueDivisionVisible(
		std::vector<CVenueFilter> const& venues,
		ARBString const& venue,
		ARBString const& div)
{
	if (!CFilterOptions::GetViewAllVenues())
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

bool CFilterOptions::IsTrialVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrialPtr pTrial)
{
	// Yes, it seems backwards, but it is correct.
	if (!IsDateVisible(pTrial->GetRuns().GetEndDate(), pTrial->GetRuns().GetStartDate()))
		return false;
	if (!CFilterOptions::GetViewAllVenues())
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
unsigned short CFilterOptions::IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun)
{
	unsigned short nVisible = 0;
	if (!IsDateVisible(pRun->GetDate(), pRun->GetDate()))
		return nVisible;
	nVisible = (0x1 << ARBBase::eFilter) | (0x1 << ARBBase::eIgnoreQ);
	if (!CFilterOptions::GetViewAllVenues())
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
	&& eViewRunsAll != CFilterOptions::GetViewRuns())
	{
		// Only set the full filter, not the IgnoreQ filter.
		nVisible &= ~(0x1 << ARBBase::eFilter);
		bool bQualifying = eViewRunsQs == CFilterOptions::GetViewRuns();
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
bool CFilterOptions::IsRunVisible(
		std::vector<CVenueFilter> const& venues,
		ARBConfigVenuePtr pVenue,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun)
{
	if (1 >= pTrial->GetClubs().size())
		return true;
	bool bVisible = true;
	if (!CFilterOptions::GetViewAllVenues())
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

bool CFilterOptions::IsCalendarVisible(ARBCalendarPtr pCal)
{
	if (!CFilterOptions::GetViewAllDates())
	{
		if (CFilterOptions::GetStartFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetStartFilterDate();
			if (pCal->GetEndDate() < date)
				return false;
		}
		if (CFilterOptions::GetEndFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetEndFilterDate();
			if (pCal->GetStartDate() > date)
				return false;
		}
	}
	return true;
}

bool CFilterOptions::IsTrainingLogVisible(
		std::set<ARBString> const& names,
		ARBTrainingPtr pTraining)
{
	if (!CFilterOptions::GetViewAllDates())
	{
		if (CFilterOptions::GetStartFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetStartFilterDate();
			if (pTraining->GetDate() < date)
				return false;
		}
		if (CFilterOptions::GetEndFilterDateSet())
		{
			ARBDate date = CFilterOptions::GetEndFilterDate();
			if (pTraining->GetDate() > date)
				return false;
		}
	}
	bool bVisible = true;
	if (!CFilterOptions::GetTrainingViewAllNames())
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

static void FilterVenue(
		CString inVenue,
		std::vector<CVenueFilter>& venues)
{
	if (!inVenue.IsEmpty())
	{
		CStringArray raw;
		int pos;
		while (0 <= (pos = inVenue.Find(':')))
		{
			raw.Add(inVenue.Left(pos));
			inVenue = inVenue.Mid(pos+1);
		}
		raw.Add(inVenue);
		for (int i = 0; i < raw.GetSize(); ++i)
		{
			inVenue = raw[i];
			CStringArray rawFilter;
			while (0 <= (pos = inVenue.Find('/')))
			{
				rawFilter.Add(inVenue.Left(pos));
				inVenue = inVenue.Mid(pos+1);
			}
			rawFilter.Add(inVenue);
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
}

static CString FilterVenue(std::vector<CVenueFilter> const& venues)
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
	return venue;
}

static void TrainingNames(CString inNames, std::set<ARBString>& outNames)
{
	if (!inNames.IsEmpty())
	{
		int pos;
		while (0 <= (pos = inNames.Find(':')))
		{
			outNames.insert((LPCTSTR)inNames.Left(pos));
			inNames = inNames.Mid(pos+1);
		}
		outNames.insert((LPCTSTR)inNames);
	}
}

static CString TrainingNames(std::set<ARBString> const& inNames)
{
	CString names;
	for (std::set<ARBString>::const_iterator iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		if (!names.IsEmpty())
			names += ':';
		names += (*iter).c_str();
	}
	return names;
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Calendar

CCalendarViewFilter CFilterOptions::FilterCalendarView()
{
	unsigned short uVal = static_cast<unsigned short>(AfxGetApp()->GetProfileInt(_T("Calendar"), _T("Filter"), CCalendarViewFilter::eViewNormal));
	return uVal;
}

void CFilterOptions::SetFilterCalendarView(CCalendarViewFilter inFilter)
{
	AfxGetApp()->WriteProfileInt(_T("Calendar"), _T("Filter"), inFilter.m_Filter);
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Date

bool CFilterOptions::GetViewAllDates()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllDates"), 1);
	return val == 1 ? true : false;
}

void CFilterOptions::SetViewAllDates(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllDates"), bViewAll ? 1 : 0);
}

ARBDate CFilterOptions::GetStartFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt(_T("Common"), _T("StartFilterJDay"), date.GetJulianDay()));
	return date;
}

void CFilterOptions::SetStartFilterDate(ARBDate const& date)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("StartFilterJDay"), date.GetJulianDay());
}

bool CFilterOptions::GetStartFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("StartFilter"), 0);
	return val == 1 ? true : false;
}

void CFilterOptions::SetStartFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("StartFilter"), bSet ? 1 : 0);
}

ARBDate CFilterOptions::GetEndFilterDate()
{
	ARBDate date(ARBDate::Today());
	date.SetJulianDay(AfxGetApp()->GetProfileInt(_T("Common"), _T("EndFilterJDay"), date.GetJulianDay()));
	return date;
}

void CFilterOptions::SetEndFilterDate(ARBDate const& date)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("EndFilterJDay"), date.GetJulianDay());
}

bool CFilterOptions::GetEndFilterDateSet()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("EndFilter"), 0);
	return val == 1 ? true : false;
}

void CFilterOptions::SetEndFilterDateSet(bool bSet)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("EndFilter"), bSet ? 1 : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Runs

bool CFilterOptions::GetViewAllVenues()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllVenues"), 1);
	return val == 1 ? true : false;
}

void CFilterOptions::SetViewAllVenues(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllVenues"), bViewAll ? 1 : 0);
}

static bool s_venueCacheInit = false;
static std::vector<CVenueFilter> s_venueCache;

void CFilterOptions::GetFilterVenue(std::vector<CVenueFilter>& venues)
{
	venues.clear();
	if (s_venueCacheInit)
	{
		venues = s_venueCache;
	}
	else
	{
		CString venue = AfxGetApp()->GetProfileString(_T("Common"), _T("FilterVenue"), _T(""));
		FilterVenue(venue, venues);
		s_venueCacheInit = true;
		s_venueCache = venues;
	}
}

void CFilterOptions::SetFilterVenue(std::vector<CVenueFilter> const& venues)
{
	CString venue = FilterVenue(venues);
	AfxGetApp()->WriteProfileString(_T("Common"), _T("FilterVenue"), venue);
	s_venueCacheInit = true;
	s_venueCache = venues;
}

CFilterOptions::eViewRuns CFilterOptions::GetViewRuns()
{
	return static_cast<eViewRuns>(AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewRuns"), 0));
}

void CFilterOptions::SetViewRuns(eViewRuns eViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewRuns"), static_cast<int>(eViewAll));
}

/////////////////////////////////////////////////////////////////////////////
// Filtering: Training Log

bool CFilterOptions::GetTrainingViewAllNames()
{
	int val = AfxGetApp()->GetProfileInt(_T("Common"), _T("ViewAllNames"), 1);
	return val == 1 ? true : false;
}

void CFilterOptions::SetTrainingViewAllNames(bool bViewAll)
{
	AfxGetApp()->WriteProfileInt(_T("Common"), _T("ViewAllNames"), bViewAll ? 1 : 0);
}

void CFilterOptions::GetTrainingFilterNames(std::set<ARBString>& outNames)
{
	outNames.clear();
	CString names = AfxGetApp()->GetProfileString(_T("Common"), _T("FilterTrainingNames"), _T(""));
	outNames.clear();
	TrainingNames(names, outNames);
}

void CFilterOptions::SetTrainingFilterNames(std::set<ARBString> const& inNames)
{
	CString names = TrainingNames(inNames);
	AfxGetApp()->WriteProfileString(_T("Common"), _T("FilterTrainingNames"), names);
}

/////////////////////////////////////////////////////////////////////////////

ARBString CFilterOptions::GetCurrentFilter()
{
	return (LPCTSTR)AfxGetApp()->GetProfileString(_T("Settings"), _T("Filter"), "");
}

size_t CFilterOptions::GetAllNamedFilters(std::set<ARBString>& outNames)
{
	outNames.clear();
	CString names = AfxGetApp()->GetProfileString(_T("Settings"), _T("Filters"), "");
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
	return outNames.size();
}

static void SetAllNamedFilters(std::set<ARBString> const& inNames)
{
	CString names;
	for (std::set<ARBString>::const_iterator i = inNames.begin();
		i != inNames.end();
		++i)
	{
		if (!names.IsEmpty())
			names += ':';
		names += (*i).c_str();
	}
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Filters"), names);
}

void CFilterOptions::DeleteNamedFilter(ARBString const& inName)
{
	if (inName.empty())
		return;
	std::set<ARBString> names;
	if (0 < GetAllNamedFilters(names))
	{
		std::set<ARBString>::iterator i = names.find(inName);
		if (names.end() != i)
		{
			names.erase(i);
			SetAllNamedFilters(names);
			CString section(_T("Filter"));
			section += inName.c_str();
			AfxGetApp()->WriteProfileString(section, NULL, NULL);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CFilterOptionData::CFilterOptionData()
	: filterName(_T(""))
{
	filterName = CFilterOptions::GetCurrentFilter();
	calView = CFilterOptions::FilterCalendarView();
	bAllDates = CFilterOptions::GetViewAllDates();
	bStartDate = CFilterOptions::GetStartFilterDateSet();
	dateStartDate = CFilterOptions::GetStartFilterDate();
	bEndDate = CFilterOptions::GetEndFilterDateSet();
	dateEndDate = CFilterOptions::GetEndFilterDate();
	bViewAllVenues = CFilterOptions::GetViewAllVenues();
	CFilterOptions::GetFilterVenue(venueFilter);
	eRuns = CFilterOptions::GetViewRuns();
	bViewAllNames = CFilterOptions::GetTrainingViewAllNames();
	CFilterOptions::GetTrainingFilterNames(nameFilter);
}

CFilterOptionData::CFilterOptionData(ARBString const& inName)
	: filterName(inName)
{
	if (filterName.empty())
	{
		calView = CFilterOptions::FilterCalendarView();
		bAllDates = CFilterOptions::GetViewAllDates();
		bStartDate = CFilterOptions::GetStartFilterDateSet();
		dateStartDate = CFilterOptions::GetStartFilterDate();
		bEndDate = CFilterOptions::GetEndFilterDateSet();
		dateEndDate = CFilterOptions::GetEndFilterDate();
		bViewAllVenues = CFilterOptions::GetViewAllVenues();
		CFilterOptions::GetFilterVenue(venueFilter);
		eRuns = CFilterOptions::GetViewRuns();
		bViewAllNames = CFilterOptions::GetTrainingViewAllNames();
		CFilterOptions::GetTrainingFilterNames(nameFilter);
	}
	else
	{
		CString section(_T("Filter"));
		section += inName.c_str();
		calView.m_Filter = static_cast<unsigned short>(AfxGetApp()->GetProfileInt(section, _T("Cal"), CCalendarViewFilter::eViewNormal));
		bAllDates = AfxGetApp()->GetProfileInt(section, _T("AllDates"), 1) == 1;
		bStartDate = AfxGetApp()->GetProfileInt(section, _T("Start"), 0) == 1;
		AfxGetApp()->GetProfileInt(section, _T("StartJDay"), dateStartDate.GetJulianDay());
		bEndDate = AfxGetApp()->GetProfileInt(section, _T("End"), 0) == 1;
		AfxGetApp()->GetProfileInt(section, _T("EndJDay"), dateEndDate.GetJulianDay());
		bViewAllVenues = AfxGetApp()->GetProfileInt(section, _T("AllVenues"), 1) == 1;
		CString names = AfxGetApp()->GetProfileString(section, _T("FilterVenue"), _T(""));
		venueFilter.clear();
		FilterVenue(names, venueFilter);
		eRuns = static_cast<CFilterOptions::eViewRuns>(AfxGetApp()->GetProfileInt(section, _T("ViewRuns"), 0));
		bViewAllNames = AfxGetApp()->GetProfileInt(section, _T("AllNames"), 1) == 1;
		names = AfxGetApp()->GetProfileString(section, _T("FilterNames"), _T(""));
		nameFilter.clear();
		TrainingNames(names, nameFilter);
	}
}

bool CFilterOptionData::SaveName()
{
	if (filterName.empty())
		return false;

	std::set<ARBString> names;
	CFilterOptions::GetAllNamedFilters(names);
	names.insert(filterName);
	SetAllNamedFilters(names);

	CString section(_T("Filter"));
	section += filterName.c_str();
	AfxGetApp()->WriteProfileInt(section, _T("Cal"), calView.m_Filter);
	AfxGetApp()->WriteProfileInt(section, _T("AllDates"), bAllDates ? 1 : 0);
	AfxGetApp()->WriteProfileInt(section, _T("Start"), bStartDate ? 1 : 0);
	AfxGetApp()->WriteProfileInt(section, _T("StartJDay"), dateStartDate.GetJulianDay());
	AfxGetApp()->WriteProfileInt(section, _T("End"), bEndDate ? 1 : 0);
	AfxGetApp()->WriteProfileInt(section, _T("EndJDay"), dateEndDate.GetJulianDay());
	AfxGetApp()->WriteProfileInt(section, _T("AllVenues"), bViewAllVenues ? 1 : 0);
	CString name = FilterVenue(venueFilter);
	AfxGetApp()->WriteProfileString(section, _T("FilterVenue"), name);
	AfxGetApp()->WriteProfileInt(section, _T("ViewRuns"), static_cast<int>(eRuns));
	AfxGetApp()->WriteProfileInt(section, _T("AllNames"), bViewAllNames ? 1 : 0);
	name = TrainingNames(nameFilter);
	AfxGetApp()->WriteProfileString(section, _T("FilterNames"), name);
	return true;
}

void CFilterOptionData::SaveDefault()
{
	CFilterOptions::SetFilterCalendarView(calView);
	CFilterOptions::SetViewAllDates(bAllDates);
	CFilterOptions::SetStartFilterDateSet(bStartDate);
	CFilterOptions::SetStartFilterDate(dateStartDate);
	CFilterOptions::SetEndFilterDateSet(bEndDate);
	CFilterOptions::SetEndFilterDate(dateEndDate);
	CFilterOptions::SetViewAllVenues(bViewAllVenues);
	CFilterOptions::SetFilterVenue(venueFilter);
	CFilterOptions::SetViewRuns(eRuns);
	CFilterOptions::SetTrainingViewAllNames(bViewAllNames);
	CFilterOptions::SetTrainingFilterNames(nameFilter);
}
