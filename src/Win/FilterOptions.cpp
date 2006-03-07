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

CFilterOptions& CFilterOptions::Options()
{
	static CFilterOptions options;
	return options;
}

CFilterOptions::CFilterOptions()
{
	Load();
}

void CFilterOptions::Load()
{
	CString val;
	CWinApp* pApp = AfxGetApp();

	m_uCalFilter = static_cast<unsigned short>(pApp->GetProfileInt(_T("Calendar"), _T("Filter"), CCalendarViewFilter::eViewNormal));

	m_bViewAllDates = pApp->GetProfileInt(_T("Common"), _T("ViewAllDates"), 1) == 1;
	m_dateStart = ARBDate::Today();
	m_dateStart.SetJulianDay(pApp->GetProfileInt(_T("Common"), _T("StartFilterJDay"), m_dateStart.GetJulianDay()));
	m_bDateStart = pApp->GetProfileInt(_T("Common"), _T("StartFilter"), 0) == 1;
	m_dateEnd = ARBDate::Today();
	m_dateEnd.SetJulianDay(pApp->GetProfileInt(_T("Common"), _T("EndFilterJDay"), m_dateEnd.GetJulianDay()));
	m_bDateEnd = pApp->GetProfileInt(_T("Common"), _T("EndFilter"), 0) == 1;

	m_bViewAllVenues = pApp->GetProfileInt(_T("Common"), _T("ViewAllVenues"), 1) == 1;
	val = pApp->GetProfileString(_T("Common"), _T("FilterVenue"), _T(""));
	m_venueFilters.clear();
	FilterVenue(val, m_venueFilters);

	m_eRuns = static_cast<eViewRuns>(pApp->GetProfileInt(_T("Common"), _T("ViewRuns"), 0));

	m_bViewAllNames = pApp->GetProfileInt(_T("Common"), _T("ViewAllNames"), 1) == 1;
	val = pApp->GetProfileString(_T("Common"), _T("FilterTrainingNames"), _T(""));
	m_nameFilters.clear();
	TrainingNames(val, m_nameFilters);

	m_curFilter = (LPCTSTR)pApp->GetProfileString(_T("Common"), _T("CurrentFilter"), _T(""));
	m_nFilters = pApp->GetProfileInt(_T("Common"), _T("numFilters"), 0);
	m_filters.clear();
	for (int idx = 0; idx < m_nFilters; ++idx)
	{
		CFilterOptionData data(idx);
		m_filters.push_back(idx);
	}
}

void CFilterOptions::Save()
{
	CString val;
	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileInt(_T("Calendar"), _T("Filter"), m_uCalFilter);

	pApp->WriteProfileInt(_T("Common"), _T("ViewAllDates"), m_bViewAllDates ? 1 : 0);
	pApp->WriteProfileInt(_T("Common"), _T("StartFilterJDay"), m_dateStart.GetJulianDay());
	pApp->WriteProfileInt(_T("Common"), _T("StartFilter"), m_bDateStart ? 1 : 0);
	pApp->WriteProfileInt(_T("Common"), _T("EndFilterJDay"), m_dateEnd.GetJulianDay());
	pApp->WriteProfileInt(_T("Common"), _T("EndFilter"), m_bDateEnd ? 1 : 0);

	pApp->WriteProfileInt(_T("Common"), _T("ViewAllVenues"), m_bViewAllVenues ? 1 : 0);
	val = FilterVenue(m_venueFilters);
	pApp->WriteProfileString(_T("Common"), _T("FilterVenue"), val);

	pApp->WriteProfileInt(_T("Common"), _T("ViewRuns"), static_cast<int>(m_eRuns));

	pApp->WriteProfileInt(_T("Common"), _T("ViewAllNames"), m_bViewAllNames ? 1 : 0);
	val = TrainingNames(m_nameFilters);
	pApp->WriteProfileString(_T("Common"), _T("FilterTrainingNames"), val);

	pApp->WriteProfileString(_T("Common"), _T("CurrentFilter"), m_curFilter.c_str());

	int nFilters = static_cast<int>(m_filters.size());
	if (nFilters < m_nFilters)
	{
		// TODO: Clean up old filters
	}
	int index = 0;
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin();
		i != m_filters.end();
		++index, ++i)
	{
		(*i).Save(index);
	}
	m_nFilters = nFilters;
	pApp->WriteProfileInt(_T("Common"), _T("numFilters"), m_nFilters);
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions

bool CFilterOptions::IsFilterEnabled()
{
	if (m_bViewAllDates
	&& m_bViewAllVenues
	&& eViewRunsAll == m_eRuns)
		return false;
	else
		return true;
}

bool CFilterOptions::IsDateVisible(
		ARBDate const& startDate,
		ARBDate const& endDate)
{
	if (!m_bViewAllDates)
	{
		if (m_bDateStart && startDate < m_dateStart)
			return false;
		if (m_bDateEnd && endDate > m_dateEnd)
			return false;
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
	if (!m_bViewAllVenues)
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
	if (!m_bViewAllVenues)
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
	if (!m_bViewAllVenues)
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
	if (!m_bViewAllVenues)
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
	&& eViewRunsAll != m_eRuns)
	{
		// Only set the full filter, not the IgnoreQ filter.
		nVisible &= ~(0x1 << ARBBase::eFilter);
		bool bQualifying = eViewRunsQs == m_eRuns;
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
	if (!m_bViewAllVenues)
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
	if (!m_bViewAllDates)
	{
		if (m_bDateStart)
		{
			if (pCal->GetEndDate() < m_dateStart)
				return false;
		}
		if (m_bDateEnd)
		{
			if (pCal->GetStartDate() > m_dateEnd)
				return false;
		}
	}
	return true;
}

bool CFilterOptions::IsTrainingLogVisible(
		std::set<ARBString> const& names,
		ARBTrainingPtr pTraining)
{
	if (!m_bViewAllDates)
	{
		if (m_bDateStart)
		{
			if (pTraining->GetDate() < m_dateStart)
				return false;
		}
		if (m_bDateEnd)
		{
			if (pTraining->GetDate() > m_dateEnd)
				return false;
		}
	}
	bool bVisible = true;
	if (!m_bViewAllNames)
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

CFilterOptions::CFilterOptionData::CFilterOptionData(int index)
{
	CString section;
	CWinApp* pApp = AfxGetApp();

	section.Format(_T("Filter%d"), index);
	filterName = (LPCTSTR)pApp->GetProfileString(section, _T("Name"), _T(""));
	calView.m_Filter = static_cast<unsigned short>(pApp->GetProfileInt(section, _T("Cal"), CCalendarViewFilter::eViewNormal));
	bAllDates = pApp->GetProfileInt(section, _T("AllDates"), 1) == 1;
	bStartDate = pApp->GetProfileInt(section, _T("Start"), 0) == 1;
	dateStartDate.SetJulianDay(pApp->GetProfileInt(section, _T("StartJDay"), dateStartDate.GetJulianDay()));
	bEndDate = pApp->GetProfileInt(section, _T("End"), 0) == 1;
	dateEndDate.SetJulianDay(pApp->GetProfileInt(section, _T("EndJDay"), dateEndDate.GetJulianDay()));
	bViewAllVenues = pApp->GetProfileInt(section, _T("AllVenues"), 1) == 1;
	CString names = pApp->GetProfileString(section, _T("FilterVenue"), _T(""));
	venueFilter.clear();
	FilterVenue(names, venueFilter);
	eRuns = static_cast<CFilterOptions::eViewRuns>(pApp->GetProfileInt(section, _T("ViewRuns"), 0));
	bViewAllNames = pApp->GetProfileInt(section, _T("AllNames"), 1) == 1;
	names = pApp->GetProfileString(section, _T("FilterNames"), _T(""));
	nameFilter.clear();
	TrainingNames(names, nameFilter);
}

CFilterOptions::CFilterOptionData::CFilterOptionData(
		CFilterOptions::CFilterOptionData const& rhs)
	: filterName(rhs.filterName)
	, calView(rhs.calView)
	, bAllDates(rhs.bAllDates)
	, bStartDate(rhs.bStartDate)
	, dateStartDate(rhs.dateStartDate)
	, bEndDate(rhs.bEndDate)
	, dateEndDate(rhs.dateEndDate)
	, bViewAllVenues(rhs.bViewAllVenues)
	, venueFilter(rhs.venueFilter)
	, eRuns(rhs.eRuns)
	, bViewAllNames(rhs.bViewAllNames)
	, nameFilter(rhs.nameFilter)
{
}

CFilterOptions::CFilterOptionData& CFilterOptions::CFilterOptionData::operator=(
		CFilterOptions::CFilterOptionData const& rhs)
{
	if (this != &rhs)
	{
		filterName = rhs.filterName;
		calView = rhs.calView;
		bAllDates = rhs.bAllDates;
		bStartDate = rhs.bStartDate;
		dateStartDate = rhs.dateStartDate;
		bEndDate = rhs.bEndDate;
		dateEndDate = rhs.dateEndDate;
		bViewAllVenues = rhs.bViewAllVenues;
		venueFilter = rhs.venueFilter;
		eRuns = rhs.eRuns;
		bViewAllNames = rhs.bViewAllNames;
		nameFilter = rhs.nameFilter;
	}
	return *this;
}

bool CFilterOptions::CFilterOptionData::Save(int index)
{
	CWinApp* pApp = AfxGetApp();
	CString section;
	section.Format(_T("Filter%d"), index);

	pApp->WriteProfileString(section, _T("Name"), filterName.c_str());
	pApp->WriteProfileInt(section, _T("Cal"), calView.m_Filter);
	pApp->WriteProfileInt(section, _T("AllDates"), bAllDates ? 1 : 0);
	pApp->WriteProfileInt(section, _T("Start"), bStartDate ? 1 : 0);
	pApp->WriteProfileInt(section, _T("StartJDay"), dateStartDate.GetJulianDay());
	pApp->WriteProfileInt(section, _T("End"), bEndDate ? 1 : 0);
	pApp->WriteProfileInt(section, _T("EndJDay"), dateEndDate.GetJulianDay());
	pApp->WriteProfileInt(section, _T("AllVenues"), bViewAllVenues ? 1 : 0);
	CString name = FilterVenue(venueFilter);
	pApp->WriteProfileString(section, _T("FilterVenue"), name);
	pApp->WriteProfileInt(section, _T("ViewRuns"), static_cast<int>(eRuns));
	pApp->WriteProfileInt(section, _T("AllNames"), bViewAllNames ? 1 : 0);
	name = TrainingNames(nameFilter);
	pApp->WriteProfileString(section, _T("FilterNames"), name);
	return true;
}
