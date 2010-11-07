/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-01 DRC Ported to wxWidgets.
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
#include "RegItems.h"
#include <wx/config.h>


static void FilterVenue(
		wxString inVenue,
		std::vector<CVenueFilter>& venues)
{
	if (!inVenue.IsEmpty())
	{
		std::vector<wxString> raw;
		int pos;
		while (0 <= (pos = inVenue.Find(':')))
		{
			raw.push_back(inVenue.Left(pos));
			inVenue = inVenue.Mid(pos+1);
		}
		raw.push_back(inVenue);
		for (size_t i = 0; i < raw.size(); ++i)
		{
			inVenue = raw[i];
			std::vector<wxString> rawFilter;
			while (0 <= (pos = inVenue.Find('/')))
			{
				rawFilter.push_back(inVenue.Left(pos));
				inVenue = inVenue.Mid(pos+1);
			}
			rawFilter.push_back(inVenue);
			if (0 < rawFilter.size())
			{
				CVenueFilter filter;
				switch (rawFilter.size())
				{
				default:
				case 3:
					filter.level = rawFilter[2];
					// fallthru
				case 2:
					filter.division = rawFilter[1];
					// fallthru
				case 1:
					filter.venue = rawFilter[0];
				}
				venues.push_back(filter);
			}
		}
	}
}


static wxString FilterVenue(std::vector<CVenueFilter> const& venues)
{
	wxString venue;
	for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
		iter != venues.end();
		++iter)
	{
		if (!venue.IsEmpty())
			venue += wxT(":");
		venue += (*iter).venue;
		venue += wxT("/");
		venue += (*iter).division;
		venue += wxT("/");
		venue += (*iter).level;
	}
	return venue;
}


static void TrainingNames(wxString inNames, std::set<wxString>& outNames)
{
	if (!inNames.IsEmpty())
	{
		int pos;
		while (0 <= (pos = inNames.Find(':')))
		{
			outNames.insert(inNames.Left(pos));
			inNames = inNames.Mid(pos+1);
		}
		outNames.insert(inNames);
	}
}


static wxString TrainingNames(std::set<wxString> const& inNames)
{
	wxString names;
	for (std::set<wxString>::const_iterator iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		if (!names.IsEmpty())
			names += wxT(":");
		names += *iter;
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
	wxString val;

	m_calView.m_Filter = static_cast<CCalendarViewFilter::eViewFilter>(wxConfig::Get()->Read(CFG_CAL_FILTER, static_cast<long>(CCalendarViewFilter::eViewNormal)));
	wxConfig::Get()->Read(CFG_COMMON_VIEWALLDATES, &m_bAllDates, true);
	wxConfig::Get()->Read(CFG_COMMON_STARTFILTER, &m_bStartDate, false);
	m_dateStartDate = ARBDate::Today();
	m_dateStartDate.SetJulianDay(wxConfig::Get()->Read(CFG_COMMON_STARTFILTERJDAY, m_dateStartDate.GetJulianDay()));
	wxConfig::Get()->Read(CFG_COMMON_ENDFILTER, &m_bEndDate, false);
	m_dateEndDate = ARBDate::Today();
	m_dateEndDate.SetJulianDay(wxConfig::Get()->Read(CFG_COMMON_ENDFILTERJDAY, m_dateEndDate.GetJulianDay()));

	wxConfig::Get()->Read(CFG_COMMON_VIEWALLVENUES, &m_bViewAllVenues, true);
	val = wxConfig::Get()->Read(CFG_COMMON_FILTERVENUE, wxEmptyString);
	m_venueFilter.clear();
	FilterVenue(val, m_venueFilter);

	m_eRuns = static_cast<eViewRuns>(wxConfig::Get()->Read(CFG_COMMON_VIEWRUNS, 0L));

	wxConfig::Get()->Read(CFG_COMMON_VIEWALLNAMES, &m_bViewAllNames, true);
	val = wxConfig::Get()->Read(CFG_COMMON_FILTERTRAININGNAMES, wxEmptyString);
	m_nameFilter.clear();
	TrainingNames(val, m_nameFilter);

	m_curFilter = wxConfig::Get()->Read(CFG_COMMON_CURRENTFILTER, wxEmptyString);
	m_nFilters = wxConfig::Get()->Read(CFG_COMMON_NUMFILTERS, 0L);
	m_filters.clear();
	for (int idx = 0; idx < m_nFilters; ++idx)
	{
		CFilterOptionData data(idx);
		// Protect against bad filters. When writing, we'll fix things up since
		// we already know how many we think we have.
		if (!data.filterName.empty())
			m_filters.push_back(idx);
	}
}


void CFilterOptions::Save()
{
	wxString val;

	if (!m_curFilter.empty())
	{
		std::vector<CFilterOptionData>::iterator iFilter = FindFilter(m_curFilter);
		if (iFilter != m_filters.end())
		{
			CFilterOptionData const& filter = *iFilter;
			if (filter.calView.m_Filter != m_calView.m_Filter
			|| filter.bAllDates != m_bAllDates
			|| filter.bStartDate != m_bStartDate
			|| filter.dateStartDate != m_dateStartDate
			|| filter.bEndDate != m_bEndDate
			|| filter.dateEndDate != m_dateEndDate
			|| filter.bViewAllVenues != m_bViewAllVenues
			|| filter.venueFilter != m_venueFilter
			|| filter.eRuns != m_eRuns
			|| filter.bViewAllNames != m_bViewAllNames
			|| filter.nameFilter != m_nameFilter)
			{
				m_curFilter.clear();
			}
		}
	}

	wxConfig::Get()->Write(CFG_CAL_FILTER, static_cast<long>(m_calView.m_Filter));

	wxConfig::Get()->Write(CFG_COMMON_VIEWALLDATES, m_bAllDates);
	wxConfig::Get()->Write(CFG_COMMON_STARTFILTER, m_bStartDate);
	wxConfig::Get()->Write(CFG_COMMON_STARTFILTERJDAY, m_dateStartDate.GetJulianDay());
	wxConfig::Get()->Write(CFG_COMMON_ENDFILTER, m_bEndDate);
	wxConfig::Get()->Write(CFG_COMMON_ENDFILTERJDAY, m_dateEndDate.GetJulianDay());

	wxConfig::Get()->Write(CFG_COMMON_VIEWALLVENUES, m_bViewAllVenues);
	val = FilterVenue(m_venueFilter);
	wxConfig::Get()->Write(CFG_COMMON_FILTERVENUE, val);

	wxConfig::Get()->Write(CFG_COMMON_VIEWRUNS, static_cast<long>(m_eRuns));

	wxConfig::Get()->Write(CFG_COMMON_VIEWALLNAMES, m_bViewAllNames);
	val = TrainingNames(m_nameFilter);
	wxConfig::Get()->Write(CFG_COMMON_FILTERTRAININGNAMES, val);

	wxConfig::Get()->Write(CFG_COMMON_CURRENTFILTER, m_curFilter);

	int nFilters = static_cast<int>(m_filters.size());
	if (nFilters < m_nFilters)
	{
		for (int n = nFilters; n < m_nFilters; ++n)
		{
			wxConfig::Get()->DeleteGroup(CFG_KEY_FILTER(n, false));
		}
	}
	int index = 0;
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin();
		i != m_filters.end();
		++index, ++i)
	{
		(*i).Save(index);
	}
	m_nFilters = nFilters;
	wxConfig::Get()->Write(CFG_COMMON_NUMFILTERS, m_nFilters);
}


std::vector<CFilterOptions::CFilterOptionData>::iterator
CFilterOptions::FindFilter(wxString const& inName)
{
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin();
		i != m_filters.end();
		++i)
	{
		if ((*i).filterName == inName)
			return i;
	}
	return m_filters.end();
}


size_t CFilterOptions::GetAllFilterNames(std::vector<wxString>& outNames) const
{
	outNames.clear();
	if (0 < m_filters.size())
	{
		outNames.reserve(m_filters.size());
		for (std::vector<CFilterOptionData>::const_iterator i = m_filters.begin();
			i != m_filters.end();
			++i)
		{
			outNames.push_back((*i).filterName);
		}
	}
	return outNames.size();
}


bool CFilterOptions::SetCurrentFilter(wxString const& inName)
{
	std::vector<CFilterOptionData>::iterator iter = FindFilter(inName);
	if (iter != m_filters.end())
	{
		m_curFilter = inName;
		m_calView = (*iter).calView;
		m_bAllDates = (*iter).bAllDates;
		m_bStartDate = (*iter).bStartDate;
		m_dateStartDate = (*iter).dateStartDate;
		m_bEndDate = (*iter).bEndDate;
		m_dateEndDate = (*iter).dateEndDate;
		m_bViewAllVenues = (*iter).bViewAllVenues;
		m_venueFilter = (*iter).venueFilter;
		m_eRuns = (*iter).eRuns;
		m_bViewAllNames = (*iter).bViewAllNames;
		m_nameFilter = (*iter).nameFilter;
		return true;
	}
	else
		return false;
}


bool CFilterOptions::AddFilter(wxString const& inName)
{
	bool bAdded = false;
	std::vector<CFilterOptionData>::iterator iter = FindFilter(inName);
	if (iter == m_filters.end())
	{
		bAdded = true;
		CFilterOptionData data;
		data.filterName = inName;
		iter = m_filters.insert(m_filters.end(), data);
	}
	m_curFilter = inName;
	(*iter).calView = m_calView;
	(*iter).bAllDates = m_bAllDates;
	(*iter).bStartDate = m_bStartDate;
	(*iter).dateStartDate = m_dateStartDate;
	(*iter).bEndDate = m_bEndDate;
	(*iter).dateEndDate = m_dateEndDate;
	(*iter).bViewAllVenues = m_bViewAllVenues;
	(*iter).venueFilter = m_venueFilter;
	(*iter).eRuns = m_eRuns;
	(*iter).bViewAllNames = m_bViewAllNames;
	(*iter).nameFilter = m_nameFilter;
	return bAdded;
}


bool CFilterOptions::DeleteFilter(wxString const& inName)
{
	std::vector<CFilterOptionData>::iterator iter = FindFilter(inName);
	if (iter != m_filters.end())
	{
		m_filters.erase(iter);
		if (inName == m_curFilter)
			m_curFilter.erase();
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions

bool CFilterOptions::IsFilterEnabled()
{
	if (m_bAllDates
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
	if (!m_bAllDates)
	{
		if (m_bStartDate && startDate < m_dateStartDate)
			return false;
		if (m_bEndDate && endDate > m_dateEndDate)
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
		wxString const& venue)
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
		wxString const& venue,
		wxString const& div)
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


bool CFilterOptions::IsVenueLevelVisible(
		std::vector<CVenueFilter> const& venues,
		wxString const& venue,
		wxString const& div,
		wxString const& level)
{
	if (!m_bViewAllVenues)
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if ((*iter).venue == venue
			&& (*iter).division == div
			&& (*iter).level == level)
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


bool CFilterOptions::IsCalendarVisible(
		std::vector<CVenueFilter> const& venues,
		ARBCalendarPtr pCal)
{
	if (!m_bAllDates)
	{
		if (m_bStartDate)
		{
			if (pCal->GetEndDate() < m_dateStartDate)
				return false;
		}
		if (m_bEndDate)
		{
			if (pCal->GetStartDate() > m_dateEndDate)
				return false;
		}
	}
	bool bVisible = true;
	if (!m_bViewAllVenues)
	{
		bVisible = false;
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin();
			iter != venues.end();
			++iter)
		{
			if (pCal->GetVenue() == (*iter).venue)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}


bool CFilterOptions::IsTrainingLogVisible(
		std::set<wxString> const& names,
		ARBTrainingPtr pTraining)
{
	if (!m_bAllDates)
	{
		if (m_bStartDate)
		{
			if (pTraining->GetDate() < m_dateStartDate)
				return false;
		}
		if (m_bEndDate)
		{
			if (pTraining->GetDate() > m_dateEndDate)
				return false;
		}
	}
	bool bVisible = true;
	if (!m_bViewAllNames)
	{
		bVisible = false;
		for (std::set<wxString>::const_iterator iter = names.begin();
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


bool CFilterOptions::FilterExists(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel) const
{
	for (std::vector<CVenueFilter>::const_iterator iter = m_venueFilter.begin();
		iter != m_venueFilter.end();
		++iter)
	{
		if ((*iter).venue == inVenue
		&& (0 == inDiv.length() || (0 < inDiv.length() && (*iter).division == inDiv))
		&& (0 == inLevel.length() || (0 < inLevel.length() && (*iter).level == inLevel)))
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CFilterOptions::CFilterOptionData::CFilterOptionData()
		: filterName()
		, calView(CCalendarViewFilter::eViewNormal)
		, bAllDates(true)
		, bStartDate(false)
		, dateStartDate(ARBDate::Today())
		, bEndDate(false)
		, dateEndDate(ARBDate::Today())
		, bViewAllVenues(true)
		, venueFilter()
		, eRuns(eViewRunsAll)
		, bViewAllNames(true)
		, nameFilter()
{
}


CFilterOptions::CFilterOptionData::CFilterOptionData(int index)
		: filterName()
		, calView(CCalendarViewFilter::eViewNormal)
		, bAllDates(true)
		, bStartDate(false)
		, dateStartDate(ARBDate::Today())
		, bEndDate(false)
		, dateEndDate(ARBDate::Today())
		, bViewAllVenues(true)
		, venueFilter()
		, eRuns(eViewRunsAll)
		, bViewAllNames(true)
		, nameFilter()
{
	wxString section = CFG_KEY_FILTER(index);

	filterName = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_NAME, filterName);
	calView.m_Filter = static_cast<CCalendarViewFilter::eViewFilter>(wxConfig::Get()->Read(section + CFG_FILTER_ITEM_CAL, static_cast<long>(calView.m_Filter)));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLDATES, &bAllDates, bAllDates);
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_START, &bStartDate, bStartDate);
	dateStartDate.SetJulianDay(wxConfig::Get()->Read(section + CFG_FILTER_ITEM_STARTJDAY, dateStartDate.GetJulianDay()));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_END, &bEndDate, bEndDate);
	dateEndDate.SetJulianDay(wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ENDJDAY, dateEndDate.GetJulianDay()));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLVENUES, &bViewAllVenues, bViewAllVenues);
	wxString names = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_FILTERVENUE, wxEmptyString);
	venueFilter.clear();
	FilterVenue(names, venueFilter);
	eRuns = static_cast<CFilterOptions::eViewRuns>(wxConfig::Get()->Read(section + CFG_FILTER_ITEM_VIEWRUNS, static_cast<long>(eRuns)));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLNAMES, &bViewAllNames, bViewAllNames);
	names = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_FILTERNAMES, wxEmptyString);
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
	wxString section = CFG_KEY_FILTER(index);

	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_NAME, filterName);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_CAL, static_cast<long>(calView.m_Filter));
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_ALLDATES, bAllDates);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_START, bStartDate);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_STARTJDAY, dateStartDate.GetJulianDay());
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_END, bEndDate);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_ENDJDAY, dateEndDate.GetJulianDay());
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_ALLVENUES, bViewAllVenues);
	wxString name = FilterVenue(venueFilter);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_FILTERVENUE, name);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_VIEWRUNS, static_cast<long>(eRuns));
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_ALLNAMES, bViewAllNames);
	name = TrainingNames(nameFilter);
	wxConfig::Get()->Write(section + CFG_FILTER_ITEM_FILTERNAMES, name);
	return true;
}
