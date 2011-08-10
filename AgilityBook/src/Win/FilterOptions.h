#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2011-08-10 DRC Added builtin support for an 'all' filter.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2006-03-02 DRC Separated filter options from main options.
 */

#include <set>
#include <vector>
#include "ARBDate.h"
#include "ARBTypes.h"


struct CVenueFilter
{
	wxString venue;
	wxString division;
	wxString level;
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
public:
	typedef enum
	{
		eReserved1 =		0x01,	///< Obsolete, implies not+plan+enter
		eReserved2 =		0x02,	///< Obsolete, changed to ViewAllCalendarOpening()
		eReserved3 =		0x04,	///< Obsolete, changed to ViewAllCalendarClosing()
		eViewNotEntered =	0x08,	///< Type of entry to view
		eViewPlanning =		0x10,	///< Type of entry to view
		eViewEntered =		0x20,	///< Type of entry to view
		eViewNormal =		eViewNotEntered | eViewPlanning | eViewEntered
	} eViewFilter;
	CCalendarViewFilter() : m_Filter(0) {}
	CCalendarViewFilter(unsigned short inFilter) : m_Filter(inFilter)
	{
		if (m_Filter & 0x01)
		{
			// Transition existing registry entries.
			m_Filter &= ~0x01;
			m_Filter |= eViewNormal;
		}
	}
	bool IsFiltered() const
	{
		return eViewNormal != (m_Filter & eViewNormal);
	}
	bool ViewNotEntered() const
	{
		return 0 == m_Filter || eViewNotEntered == (m_Filter & eViewNotEntered);
	}
	bool ViewPlanning() const
	{
		return 0 == m_Filter || eViewPlanning == (m_Filter & eViewPlanning);
	}
	bool ViewEntered() const
	{
		return 0 == m_Filter || eViewEntered == (m_Filter & eViewEntered);
	}
	void AddNotEntered()
	{
		m_Filter |= eViewNotEntered;
	}
	void AddPlanning()
	{
		m_Filter |= eViewPlanning;
	}
	void AddEntered()
	{
		m_Filter |= eViewEntered;
	}
	void Clear()
	{
		m_Filter = 0;
	}

	unsigned short m_Filter;
};


class CFilterOptions
{
public:
	typedef enum
	{
		eViewRunsAll = 0,
		eViewRunsQs = 1,
		eViewRunsNQs = 2,
	} eViewRuns;

private:
	class CFilterOptionData
	{
	public:
		CFilterOptionData();
		CFilterOptionData(int index);
		CFilterOptionData(CFilterOptionData const& rhs);
		CFilterOptionData& operator=(CFilterOptionData const& rhs);
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
		eViewRuns eRuns;
		bool bViewAllNames;
		std::set<wxString> nameFilter;
	};

	CCalendarViewFilter m_calView;
	bool m_bAllDates;
	bool m_bStartDate;
	ARBDate m_dateStartDate;
	bool m_bEndDate;
	ARBDate m_dateEndDate;
	bool m_bViewAllVenues;
	std::vector<CVenueFilter> m_venueFilter;
	eViewRuns m_eRuns;
	bool m_bViewAllNames;
	std::set<wxString> m_nameFilter;

	wxString m_curFilter;
	long m_nFilters;
	std::vector<CFilterOptionData> m_filters;

	std::vector<CFilterOptionData>::iterator FindFilter(wxString const& inName);

public:
	static CFilterOptions& Options();
	CFilterOptions(); ///< Should only be used in options dialog
	void Load();
	void Save();
	wxString GetCurrentFilter() const
	{
		return m_curFilter;
	}
	size_t GetAllFilterNames(
			std::vector<wxString>& outNames,
			bool bForEditing = false) const;
	bool SetCurrentFilter(wxString const& inName);
	bool AddFilter(wxString const& inName); ///< Returns if a new one was added
	bool DeleteFilter(wxString const& inName);

	// Helper functions
	bool IsFilterEnabled() const;
	bool IsDateVisible(
			ARBDate const& startDate,
			ARBDate const& endDate) const;
	bool IsTitleVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTitlePtr pTitle) const;
	bool IsVenueVisible(
			std::vector<CVenueFilter> const& venues,
			wxString const& venue) const;
	bool IsVenueDivisionVisible(
			std::vector<CVenueFilter> const& venues,
			wxString const& venue,
			wxString const& div) const;
	bool IsVenueLevelVisible(
			std::vector<CVenueFilter> const& venues,
			wxString const& venue,
			wxString const& div,
			wxString const& level) const;
	bool IsTrialVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial) const;
	unsigned short IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun) const;
	bool IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBConfigVenuePtr pVenue,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun) const;
	bool IsCalendarVisible(
			std::vector<CVenueFilter> const& venues,
			ARBCalendarPtr pCal) const;
	bool IsTrainingLogVisible(
			std::set<wxString> const& names,
			ARBTrainingPtr pTraining) const;

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
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel) const;

	eViewRuns GetViewRuns() const
	{
		return m_eRuns;
	}
	void SetViewRuns(eViewRuns eViewAll)
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
	void GetTrainingFilterNames(std::set<wxString>& outNames) const
	{
		outNames = m_nameFilter;
	}
	void SetTrainingFilterNames(std::set<wxString> const& inNames)
	{
		m_nameFilter = inNames;
	}
};
