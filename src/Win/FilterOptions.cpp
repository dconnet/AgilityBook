/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * 2013-01-11 Fix filters on configuration import.
 * 2011-10-12 Added better filter change detection.
 * 2011-08-10 Added builtin support for an 'all' filter.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-01 Ported to wxWidgets.
 * 2006-03-02 Separated filter options from main options.
 */

#include "stdafx.h"
#include "FilterOptions.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "RegItems.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBCalendar.h"
#include "ARB/ARBDogTitle.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/StringUtil.h"
#include <wx/config.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

void FilterVenue(wxString inVenue, std::vector<CVenueFilter>& venues)
{
	if (!inVenue.IsEmpty())
	{
		std::vector<wxString> raw;
		int pos;
		while (0 <= (pos = inVenue.Find(':')))
		{
			raw.push_back(inVenue.Left(pos));
			inVenue = inVenue.Mid(pos + 1);
		}
		raw.push_back(inVenue);
		for (size_t i = 0; i < raw.size(); ++i)
		{
			inVenue = raw[i];
			std::vector<wxString> rawFilter;
			while (0 <= (pos = inVenue.Find('/')))
			{
				rawFilter.push_back(inVenue.Left(pos));
				inVenue = inVenue.Mid(pos + 1);
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
					[[fallthrough]];
				case 2:
					filter.division = rawFilter[1];
					[[fallthrough]];
				case 1:
					filter.venue = rawFilter[0];
				}
				venues.push_back(filter);
			}
		}
	}
}


wxString FilterVenue(std::vector<CVenueFilter> const& venues)
{
	wxString venue;
	for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
	{
		if (!venue.IsEmpty())
			venue += L":";
		venue += (*iter).venue;
		venue += L"/";
		venue += (*iter).division;
		venue += L"/";
		venue += (*iter).level;
	}
	return venue;
}


void TrainingNames(wxString inNames, std::set<wxString>& outNames)
{
	if (!inNames.empty())
	{
		wxString::size_type pos;
		while (wxString::npos != (pos = inNames.find(':')))
		{
			outNames.insert(inNames.substr(0, pos));
			inNames = inNames.substr(pos + 1);
		}
		outNames.insert(inNames);
	}
}


wxString TrainingNames(std::set<wxString> const& inNames)
{
	wxString names;
	for (std::set<wxString>::const_iterator iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		if (0 < names.size())
			names << L":";
		names << *iter;
	}
	return names;
}

/////////////////////////////////////////////////////////////////////////////

// These values are written to the registry.
enum ViewFilter
{
	eReserved1 = 0x01,      ///< Obsolete, implies not+plan+enter
	eReserved2 = 0x02,      ///< Obsolete, changed to ViewAllCalendarOpening()
	eReserved3 = 0x04,      ///< Obsolete, changed to ViewAllCalendarClosing()
	eViewNotEntered = 0x08, ///< Type of entry to view
	eViewPlanning = 0x10,   ///< Type of entry to view
	eViewEntered = 0x20,    ///< Type of entry to view
	eViewNormal = eViewNotEntered | eViewPlanning | eViewEntered
};


CCalendarViewFilter::CCalendarViewFilter(unsigned short inFilter)
	: m_Filter(inFilter)
{
	if (m_Filter & 0x01)
	{
		// Transition existing registry entries.
		m_Filter &= ~0x01;
		m_Filter |= eViewNormal;
	}
}

bool CCalendarViewFilter::IsFiltered() const
{
	return eViewNormal != (m_Filter & eViewNormal);
}
bool CCalendarViewFilter::ViewNotEntered() const
{
	return 0 == m_Filter || eViewNotEntered == (m_Filter & eViewNotEntered);
}
bool CCalendarViewFilter::ViewPlanning() const
{
	return 0 == m_Filter || eViewPlanning == (m_Filter & eViewPlanning);
}
bool CCalendarViewFilter::ViewEntered() const
{
	return 0 == m_Filter || eViewEntered == (m_Filter & eViewEntered);
}
void CCalendarViewFilter::AddNotEntered()
{
	m_Filter |= eViewNotEntered;
}
void CCalendarViewFilter::AddPlanning()
{
	m_Filter |= eViewPlanning;
}
void CCalendarViewFilter::AddEntered()
{
	m_Filter |= eViewEntered;
}

/////////////////////////////////////////////////////////////////////////////

CFilterOptions& CFilterOptions::Options()
{
	static CFilterOptions options;
	return options;
}


CFilterOptions::CFilterOptions()
	: m_calView()
	, m_bAllDates(false)
	, m_bStartDate(false)
	, m_dateStartDate()
	, m_bEndDate(false)
	, m_dateEndDate()
	, m_bViewAllVenues(false)
	, m_venueFilter()
	, m_eRuns(ARBFilterViewRuns::All)
	, m_bViewAllNames(false)
	, m_nameFilter()
	, m_curFilter()
	, m_nFilters(0)
	, m_filters()
{
	Load();
}


bool CFilterOptions::Update(
	ARBConfig const& inConfigNew,
	short configVersionPreUpdate,
	ARBConfig const& inConfigCurrent)
{
	bool bRefresh = false;

	if (!m_bViewAllVenues)
	{
		for (std::vector<CVenueFilter>::iterator i = m_venueFilter.begin(); i != m_venueFilter.end(); ++i)
		{
			CVenueFilter filter = *i;
			if (inConfigNew.GetActions()
					.Update(configVersionPreUpdate, inConfigCurrent, filter.venue, filter.division, filter.level))
			{
				bRefresh = true;
				(*i).venue = filter.venue;
				(*i).division = filter.division;
				(*i).level = filter.level;
			}
		}
	}

	for (std::vector<CFilterOptionData>::iterator iFilter = m_filters.begin(); iFilter != m_filters.end(); ++iFilter)
	{
		if (!(*iFilter).bViewAllVenues)
		{
			for (std::vector<CVenueFilter>::iterator i = (*iFilter).venueFilter.begin();
				 i != (*iFilter).venueFilter.end();
				 ++i)
			{
				CVenueFilter filter = *i;
				if (inConfigNew.GetActions()
						.Update(configVersionPreUpdate, inConfigCurrent, filter.venue, filter.division, filter.level))
				{
					bRefresh = true;
					(*i).venue = filter.venue;
					(*i).division = filter.division;
					(*i).level = filter.level;
				}
			}
		}
	}

	if (bRefresh)
	{
		Save();
		CFilterOptions::Options().Load();
	}
	return bRefresh;
}


void CFilterOptions::Load()
{
	wxString val;
	wxString all(wxGetTranslation(arbT("IDS_ALL")));

	m_calView.m_Filter
		= static_cast<unsigned short>(wxConfig::Get()->Read(CFG_CAL_FILTER, static_cast<long>(eViewNormal)));
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

	m_eRuns = static_cast<ARBFilterViewRuns>(wxConfig::Get()->Read(CFG_COMMON_VIEWRUNS, 0L));

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
		// As of v2.3, check for the special name "<All>". We now automatically
		// create this.
		if (data.filterName == all)
		{
			// And since this may have been any kind of filter before, make
			// sure we don't leave it as current.
			if (m_curFilter == all)
				m_curFilter.clear();
			continue;
		}
		// Protect against bad filters. When writing, we'll fix things up since
		// we already know how many we think we have.
		if (!data.filterName.empty())
			m_filters.push_back(idx);
	}
}


void CFilterOptions::Save()
{
	wxString val;

	CFilterOptionData data;
	data.calView = m_calView;
	data.bAllDates = m_bAllDates;
	data.bStartDate = m_bStartDate;
	data.dateStartDate = m_dateStartDate;
	data.bEndDate = m_bEndDate;
	data.dateEndDate = m_dateEndDate;
	data.bViewAllVenues = m_bViewAllVenues;
	if (m_bViewAllVenues)
		m_venueFilter.clear();
	data.venueFilter = m_venueFilter;
	data.eRuns = m_eRuns;
	data.bViewAllNames = m_bViewAllNames;
	if (m_bViewAllNames)
		m_nameFilter.clear();
	data.nameFilter = m_nameFilter;

	bool bFound = false;
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin(); i != m_filters.end(); ++i)
	{
		if (*i == data)
		{
			bFound = true;
			m_curFilter = (*i).filterName;
		}
	}
	if (!bFound)
	{
		if (IsFilterEnabled())
			m_curFilter.clear();
		else
			m_curFilter = wxGetTranslation(arbT("IDS_ALL"));
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
			wxConfig::Get()->DeleteGroup(CFG_KEY_FILTER_N(n, false));
		}
	}
	int index = 0;
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin(); i != m_filters.end(); ++index, ++i)
	{
		(*i).Save(index);
	}
	m_nFilters = nFilters;
	wxConfig::Get()->Write(CFG_COMMON_NUMFILTERS, m_nFilters);
}


std::vector<CFilterOptions::CFilterOptionData>::iterator CFilterOptions::FindFilter(wxString const& inName)
{
	wxString wxName(inName);
	for (std::vector<CFilterOptionData>::iterator i = m_filters.begin(); i != m_filters.end(); ++i)
	{
		if ((*i).filterName == wxName)
			return i;
	}
	return m_filters.end();
}

size_t CFilterOptions::GetAllFilterNames(std::vector<wxString>& outNames, bool bForEditing) const
{
	outNames.clear();
	if (0 < m_filters.size())
	{
		outNames.reserve(m_filters.size());
		for (std::vector<CFilterOptionData>::const_iterator i = m_filters.begin(); i != m_filters.end(); ++i)
		{
			outNames.push_back((*i).filterName);
		}
	}
	std::stable_sort(outNames.begin(), outNames.end(), [](wxString const& one, wxString const& two) {
		return StringUtil::CompareNoCase(one, two) < 0;
	});
	if (!bForEditing)
	{
		if (0 < outNames.size() || IsFilterEnabled())
			outNames.insert(outNames.begin(), wxGetTranslation(arbT("IDS_ALL")));
	}
	return outNames.size();
}


bool CFilterOptions::SetCurrentFilter(wxString const& inName)
{
	wxString all(wxGetTranslation(arbT("IDS_ALL")));
	CFilterOptions::CFilterOptionData data;
	std::vector<CFilterOptionData>::iterator iter = FindFilter(inName);
	if (inName.empty() || inName == all)
	{
		m_curFilter = all;
	}
	else if (iter != m_filters.end())
	{
		m_curFilter = inName;
		data = *iter;
	}
	else
		return false;
	m_calView = data.calView;
	m_bAllDates = data.bAllDates;
	m_bStartDate = data.bStartDate;
	m_dateStartDate = data.dateStartDate;
	m_bEndDate = data.bEndDate;
	m_dateEndDate = data.dateEndDate;
	m_bViewAllVenues = data.bViewAllVenues;
	m_venueFilter = data.venueFilter;
	m_eRuns = data.eRuns;
	m_bViewAllNames = data.bViewAllNames;
	m_nameFilter = data.nameFilter;
	return true;
}


bool CFilterOptions::AddFilter(wxString const& inName)
{
	if (inName == wxGetTranslation(arbT("IDS_ALL")))
		return false;
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
			m_curFilter.clear();
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions

bool CFilterOptions::IsFilterEnabled() const
{
	if (m_bAllDates && m_bViewAllVenues && ARBFilterViewRuns::All == m_eRuns)
		return false;
	else
		return true;
}


bool CFilterOptions::IsDateVisible(ARBDate const& startDate, ARBDate const& endDate) const
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


bool CFilterOptions::IsTitleVisible(std::vector<CVenueFilter> const& venues, ARBDogTitlePtr const& inTitle) const
{
	if (!CAgilityBookOptions::GetViewHiddenTitles() && inTitle->IsHidden())
		return false;
	if (!inTitle->GetDate().IsValid() || !IsDateVisible(inTitle->GetDate(), inTitle->GetDate()))
		return false;
	return IsVenueVisible(venues, inTitle->GetVenue());
}


bool CFilterOptions::IsVenueVisible(std::vector<CVenueFilter> const& venues, wxString const& venue) const
{
	if (!m_bViewAllVenues)
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
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
	wxString const& div) const
{
	if (!m_bViewAllVenues)
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
		{
			if ((*iter).venue == venue && (*iter).division == div)
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
	wxString const& level) const
{
	if (!m_bViewAllVenues)
	{
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
		{
			if ((*iter).venue == venue && (*iter).division == div && (*iter).level == level)
				return true;
		}
		return false;
	}
	return true;
}


bool CFilterOptions::IsTrialVisible(std::vector<CVenueFilter> const& venues, ARBDogTrialPtr const& inTrial) const
{
	// Yes, it seems backwards, but it is correct.
	if (!IsDateVisible(inTrial->GetEndDate(), inTrial->GetStartDate()))
		return false;
	if (!m_bViewAllVenues)
	{
		for (ARBDogClubList::const_iterator iterClub = inTrial->GetClubs().begin();
			 iterClub != inTrial->GetClubs().end();
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
	ARB::ARBDogTrialPtr const& inTrial,
	ARB::ARBDogRunPtr const& inRun) const
{
	unsigned short nVisible = 0;
	if (!IsDateVisible(inRun->GetDate(), inRun->GetDate()))
		return nVisible;
	nVisible = GetFilterMask(ARBFilterType::Full) | GetFilterMask(ARBFilterType::IgnoreQ);
	if (!m_bViewAllVenues)
	{
		nVisible = 0;
		// Don't call IsTrialVisible. We need more control over the club/venue
		// check. Otherwise (for instance), if AKC/NoviceB is disabled and
		// ASCA/NoviceB is enabled, since the division/level names match, the
		// AKC run will actually show up. So we need to make sure the venue
		// of the filter matches too.
		if (IsDateVisible(inTrial->GetRuns().GetEndDate(), inTrial->GetRuns().GetStartDate()))
		{
			for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
			{
				bool bCheck = false;
				for (ARBDogClubList::const_iterator iterClub = inTrial->GetClubs().begin();
					 iterClub != inTrial->GetClubs().end();
					 ++iterClub)
				{
					if ((*iter).venue == (*iterClub)->GetVenue())
					{
						bCheck = true;
						break;
					}
				}
				if (bCheck && inRun->GetDivision() == (*iter).division && inRun->GetLevel() == (*iter).level)
				{
					nVisible = GetFilterMask(ARBFilterType::Full) | GetFilterMask(ARBFilterType::IgnoreQ);
					break;
				}
			}
		}
	}
	if ((nVisible & GetFilterMask(ARBFilterType::Full)) && ARBFilterViewRuns::All != m_eRuns)
	{
		// Only set the full filter, not the IgnoreQ filter.
		nVisible &= ~GetFilterMask(ARBFilterType::Full);
		bool bQualifying = ARBFilterViewRuns::Qs == m_eRuns;
		if ((inRun->GetQ().Qualified() && bQualifying) || (!inRun->GetQ().Qualified() && !bQualifying))
			nVisible |= GetFilterMask(ARBFilterType::Full);
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
	ARB::ARBConfigVenuePtr const& inVenue,
	ARB::ARBDogTrialPtr const& inTrial,
	ARB::ARBDogRunPtr const& inRun) const
{
	if (1 >= inTrial->GetClubs().size())
		return true;
	bool bVisible = true;
	if (!m_bViewAllVenues)
	{
		bVisible = false;
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
		{
			if (inTrial->HasVenue(inVenue->GetName()) && inVenue->GetName() == (*iter).venue
				&& inRun->GetDivision() == (*iter).division && inRun->GetLevel() == (*iter).level)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}


bool CFilterOptions::IsCalendarVisible(std::vector<CVenueFilter> const& venues, ARBCalendarPtr const& inCal) const
{
	if (!m_bAllDates)
	{
		if (m_bStartDate)
		{
			if (inCal->GetEndDate() < m_dateStartDate)
				return false;
		}
		if (m_bEndDate)
		{
			if (inCal->GetStartDate() > m_dateEndDate)
				return false;
		}
	}
	bool bVisible = true;
	if (!m_bViewAllVenues)
	{
		bVisible = false;
		for (std::vector<CVenueFilter>::const_iterator iter = venues.begin(); iter != venues.end(); ++iter)
		{
			if (inCal->GetVenue() == (*iter).venue)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}


bool CFilterOptions::IsTrainingLogVisible(std::set<wxString> const& names, ARBTrainingPtr const& inTraining) const
{
	if (!m_bAllDates)
	{
		if (m_bStartDate)
		{
			if (inTraining->GetDate() < m_dateStartDate)
				return false;
		}
		if (m_bEndDate)
		{
			if (inTraining->GetDate() > m_dateEndDate)
				return false;
		}
	}
	bool bVisible = true;
	if (!m_bViewAllNames)
	{
		bVisible = false;
		for (std::set<wxString>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
		{
			if (inTraining->GetName() == *iter)
			{
				bVisible = true;
				break;
			}
		}
	}
	return bVisible;
}


bool CFilterOptions::FilterExists(wxString const& inVenue, wxString const& inDiv, wxString const& inLevel) const
{
	for (std::vector<CVenueFilter>::const_iterator iter = m_venueFilter.begin(); iter != m_venueFilter.end(); ++iter)
	{
		if ((*iter).venue == inVenue && (0 == inDiv.length() || (0 < inDiv.length() && (*iter).division == inDiv))
			&& (0 == inLevel.length() || (0 < inLevel.length() && (*iter).level == inLevel)))
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CFilterOptions::CFilterOptionData::CFilterOptionData()
	: filterName()
	, calView(eViewNormal)
	, bAllDates(true)
	, bStartDate(false)
	, dateStartDate(ARBDate::Today())
	, bEndDate(false)
	, dateEndDate(ARBDate::Today())
	, bViewAllVenues(true)
	, venueFilter()
	, eRuns(ARBFilterViewRuns::All)
	, bViewAllNames(true)
	, nameFilter()
{
}


CFilterOptions::CFilterOptionData::CFilterOptionData(int index)
	: filterName()
	, calView(eViewNormal)
	, bAllDates(true)
	, bStartDate(false)
	, dateStartDate(ARBDate::Today())
	, bEndDate(false)
	, dateEndDate(ARBDate::Today())
	, bViewAllVenues(true)
	, venueFilter()
	, eRuns(ARBFilterViewRuns::All)
	, bViewAllNames(true)
	, nameFilter()
{
	wxString section = CFG_KEY_FILTER_N(index);

	filterName = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_NAME, filterName);
	calView.m_Filter = static_cast<unsigned short>(
		wxConfig::Get()->Read(section + CFG_FILTER_ITEM_CAL, static_cast<long>(calView.m_Filter)));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLDATES, &bAllDates, bAllDates);
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_START, &bStartDate, bStartDate);
	dateStartDate.SetJulianDay(
		wxConfig::Get()->Read(section + CFG_FILTER_ITEM_STARTJDAY, dateStartDate.GetJulianDay()));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_END, &bEndDate, bEndDate);
	dateEndDate.SetJulianDay(wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ENDJDAY, dateEndDate.GetJulianDay()));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLVENUES, &bViewAllVenues, bViewAllVenues);
	wxString names = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_FILTERVENUE, wxEmptyString);
	venueFilter.clear();
	FilterVenue(names, venueFilter);
	eRuns = static_cast<ARBFilterViewRuns>(
		wxConfig::Get()->Read(section + CFG_FILTER_ITEM_VIEWRUNS, static_cast<long>(eRuns)));
	wxConfig::Get()->Read(section + CFG_FILTER_ITEM_ALLNAMES, &bViewAllNames, bViewAllNames);
	names = wxConfig::Get()->Read(section + CFG_FILTER_ITEM_FILTERNAMES, wxEmptyString);
	nameFilter.clear();
	TrainingNames(names, nameFilter);
}


CFilterOptions::CFilterOptionData::CFilterOptionData(CFilterOptions::CFilterOptionData const& rhs)
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


CFilterOptions::CFilterOptionData::CFilterOptionData(CFilterOptions::CFilterOptionData&& rhs)
	: filterName(std::move(rhs.filterName))
	, calView(std::move(rhs.calView))
	, bAllDates(std::move(rhs.bAllDates))
	, bStartDate(std::move(rhs.bStartDate))
	, dateStartDate(std::move(rhs.dateStartDate))
	, bEndDate(std::move(rhs.bEndDate))
	, dateEndDate(std::move(rhs.dateEndDate))
	, bViewAllVenues(std::move(rhs.bViewAllVenues))
	, venueFilter(std::move(rhs.venueFilter))
	, eRuns(std::move(rhs.eRuns))
	, bViewAllNames(std::move(rhs.bViewAllNames))
	, nameFilter(std::move(rhs.nameFilter))
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


CFilterOptions::CFilterOptionData& CFilterOptions::CFilterOptionData::operator=(CFilterOptions::CFilterOptionData&& rhs)
{
	if (this != &rhs)
	{
		filterName = std::move(rhs.filterName);
		calView = std::move(rhs.calView);
		bAllDates = std::move(rhs.bAllDates);
		bStartDate = std::move(rhs.bStartDate);
		dateStartDate = std::move(rhs.dateStartDate);
		bEndDate = std::move(rhs.bEndDate);
		dateEndDate = std::move(rhs.dateEndDate);
		bViewAllVenues = std::move(rhs.bViewAllVenues);
		venueFilter = std::move(rhs.venueFilter);
		eRuns = std::move(rhs.eRuns);
		bViewAllNames = std::move(rhs.bViewAllNames);
		nameFilter = std::move(rhs.nameFilter);
	}
	return *this;
}


bool CFilterOptions::CFilterOptionData::operator==(CFilterOptions::CFilterOptionData const& rhs) const
{
	return !(
		calView.m_Filter != rhs.calView.m_Filter || eRuns != rhs.eRuns || bAllDates != rhs.bAllDates
		|| (!bAllDates && (bStartDate != rhs.bStartDate || bEndDate != rhs.bEndDate))
		|| (bStartDate && dateStartDate != rhs.dateStartDate) || (bEndDate && dateEndDate != rhs.dateEndDate)
		|| bViewAllVenues != rhs.bViewAllVenues || (!bViewAllVenues && venueFilter != rhs.venueFilter)
		|| bViewAllNames != rhs.bViewAllNames || (!bViewAllNames && nameFilter != rhs.nameFilter));
}


bool CFilterOptions::CFilterOptionData::Save(int index)
{
	wxString section = CFG_KEY_FILTER_N(index);

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

} // namespace dconSoft
