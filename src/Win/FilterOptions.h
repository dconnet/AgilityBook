#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * 2011-08-10 Added builtin support for an 'all' filter.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-01 Ported to wxWidgets.
 * 2006-03-02 Separated filter options from main options.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <set>
#include <vector>


enum class ARBFilterViewRuns
{
	All = 0,
	Qs = 1,
	NQs = 2,
};


struct CVenueFilter
{
	std::wstring venue;
	std::wstring division;
	std::wstring level;
	bool operator==(CVenueFilter const& rhs) const
	{
		return venue == rhs.venue
			&& division == rhs.division
			&& level == rhs.level;
	}
	bool operator!=(CVenueFilter const& rhs) const
	{
		return !operator==(rhs);
	}
};


class CCalendarViewFilter
{
	friend class CFilterOptions;
public:
	CCalendarViewFilter() : m_Filter(0) {}
	CCalendarViewFilter(unsigned short inFilter);
	bool IsFiltered() const;
	bool ViewNotEntered() const;
	bool ViewPlanning() const;
	bool ViewEntered() const;
	void AddNotEntered();
	void AddPlanning();
	void AddEntered();
	void Clear()
	{
		m_Filter = 0;
	}

private:
	unsigned short m_Filter;
};


class CFilterOptions
{
private:
	class CFilterOptionData
	{
	public:
		CFilterOptionData();
		CFilterOptionData(int index);
		CFilterOptionData(CFilterOptionData const& rhs);
		CFilterOptionData(CFilterOptionData&& rhs);
		~CFilterOptionData() {}
		CFilterOptionData& operator=(CFilterOptionData const& rhs);
		CFilterOptionData& operator=(CFilterOptionData&& rhs);
		// Equality does not look at name.
		bool operator==(CFilterOptionData const& rhs) const;
		bool operator!=(CFilterOptionData const& rhs) const
		{
			return !operator==(rhs);
		}
		bool Save(int index);

		wxString filterName;
		CCalendarViewFilter calView;
		bool bAllDates;
		bool bStartDate;
		ARBDate dateStartDate;
		bool bEndDate;
		ARBDate dateEndDate;
		bool bViewAllVenues;
		std::vector<CVenueFilter> venueFilter;
		ARBFilterViewRuns eRuns;
		bool bViewAllNames;
		std::set<std::wstring> nameFilter;
	};

	CCalendarViewFilter m_calView;
	bool m_bAllDates;
	bool m_bStartDate;
	ARBDate m_dateStartDate;
	bool m_bEndDate;
	ARBDate m_dateEndDate;
	bool m_bViewAllVenues;
	std::vector<CVenueFilter> m_venueFilter;
	ARBFilterViewRuns m_eRuns;
	bool m_bViewAllNames;
	std::set<std::wstring> m_nameFilter;

	std::wstring m_curFilter;
	long m_nFilters;
	std::vector<CFilterOptionData> m_filters;

	std::vector<CFilterOptionData>::iterator FindFilter(std::wstring const& inName);

public:
	static CFilterOptions& Options();
	CFilterOptions(); ///< Should only be used in options dialog
	bool Update(
			ARBConfig const& inConfigNew,
			short configVersionPreUpdate,
			ARBConfig const& inConfigCurrent);
	void Load();
	void Save();
	std::wstring GetCurrentFilter() const
	{
		return m_curFilter;
	}
	size_t GetAllFilterNames(
			std::vector<std::wstring>& outNames,
			bool bForEditing = false) const;
	bool SetCurrentFilter(std::wstring const& inName);
	bool AddFilter(std::wstring const& inName); ///< Returns if a new one was added
	bool DeleteFilter(std::wstring const& inName);

	// Helper functions
	bool IsFilterEnabled() const;
	bool IsDateVisible(
			ARBDate const& startDate,
			ARBDate const& endDate) const;
	bool IsTitleVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTitlePtr const& inTitle) const;
	bool IsVenueVisible(
			std::vector<CVenueFilter> const& venues,
			std::wstring const& venue) const;
	bool IsVenueDivisionVisible(
			std::vector<CVenueFilter> const& venues,
			std::wstring const& venue,
			std::wstring const& div) const;
	bool IsVenueLevelVisible(
			std::vector<CVenueFilter> const& venues,
			std::wstring const& venue,
			std::wstring const& div,
			std::wstring const& level) const;
	bool IsTrialVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr const& inTrial) const;
	unsigned short IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr const& inTrial,
			ARBDogRunPtr const& inRun) const;
	bool IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBConfigVenuePtr const& inVenue,
			ARBDogTrialPtr const& inTrial,
			ARBDogRunPtr const& inRun) const;
	bool IsCalendarVisible(
			std::vector<CVenueFilter> const& venues,
			ARBCalendarPtr const& inCal) const;
	bool IsTrainingLogVisible(
			std::set<std::wstring> const& names,
			ARBTrainingPtr const& inTraining) const;

	// Filtering: Calendar
	CCalendarViewFilter FilterCalendarView() const
	{
		return m_calView;
	}
	void SetFilterCalendarView(CCalendarViewFilter inFilter)
	{
		m_calView = inFilter;
	}

	// Filtering: Date
	bool GetViewAllDates() const
	{
		return m_bAllDates;
	}
	void SetViewAllDates(bool bViewAll)
	{
		m_bAllDates = bViewAll;
	}
	ARBDate GetStartFilterDate() const
	{
		return m_dateStartDate;
	}
	void SetStartFilterDate(ARBDate const& date)
	{
		m_dateStartDate = date;
	}
	bool GetStartFilterDateSet() const
	{
		return m_bStartDate;
	}
	void SetStartFilterDateSet(bool bSet)
	{
		m_bStartDate = bSet;
	}
	ARBDate GetEndFilterDate() const
	{
		return m_dateEndDate;
	}
	void SetEndFilterDate(ARBDate const& date)
	{
		m_dateEndDate = date;
	}
	bool GetEndFilterDateSet() const
	{
		return m_bEndDate;
	}
	void SetEndFilterDateSet(bool bSet)
	{
		m_bEndDate = bSet;
	}

	// Filtering: Runs
	bool GetViewAllVenues() const
	{
		return m_bViewAllVenues;
	}
	void SetViewAllVenues(bool bViewAll)
	{
		m_bViewAllVenues = bViewAll;
	}
	void GetFilterVenue(std::vector<CVenueFilter>& venues) const
	{
		venues = m_venueFilter;
	}
	void SetFilterVenue(std::vector<CVenueFilter> const& venues)
	{
		m_venueFilter = venues;
	}
	bool FilterExists(
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel) const;

	ARBFilterViewRuns GetViewRuns() const
	{
		return m_eRuns;
	}
	void SetViewRuns(ARBFilterViewRuns eViewAll)
	{
		m_eRuns = eViewAll;
	}

	// Filtering: Training Log
	bool GetTrainingViewAllNames() const
	{
		return m_bViewAllNames;
	}
	void SetTrainingViewAllNames(bool bViewAll)
	{
		m_bViewAllNames = bViewAll;
	}
	void GetTrainingFilterNames(std::set<std::wstring>& outNames) const
	{
		outNames = m_nameFilter;
	}
	void SetTrainingFilterNames(std::set<std::wstring> const& inNames)
	{
		m_nameFilter = inNames;
	}
};
