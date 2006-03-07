#pragma once

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
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2006-03-02 DRC Separated filter options from main options.
 */

#include <set>
#include <vector>
#include "ARBDate.h"
#include "ARBTypes.h"

struct CVenueFilter
{
	ARBString venue;
	ARBString division;
	ARBString level;
};

class CCalendarViewFilter
{
public:
	typedef enum
	{
		eReserved =			0x01, // No longer used - implies not+plan+enter
		eViewOpening =		0x02,
		eViewClosing =		0x04,
		eViewNotEntered =	0x08,
		eViewPlanning =		0x10,
		eViewEntered =		0x20,
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
	bool ViewOpening() const
	{
		return eViewOpening == (m_Filter & eViewOpening);
	}
	bool ViewClosing() const
	{
		return eViewClosing == (m_Filter & eViewClosing);
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
	void AddOpening()
	{
		m_Filter |= eViewOpening;
	}
	void AddClosing()
	{
		m_Filter |= eViewClosing;
	}
	void clear()
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

	class CFilterOptionData
	{
	public:
		CFilterOptionData(int index);
		CFilterOptionData(CFilterOptionData const& rhs);
		CFilterOptionData& operator=(CFilterOptionData const& rhs);
		bool Save(int index);

		ARBString filterName;
		CCalendarViewFilter calView;
		bool bAllDates;
		bool bStartDate;
		ARBDate dateStartDate;
		bool bEndDate;
		ARBDate dateEndDate;
		bool bViewAllVenues;
		std::vector<CVenueFilter> venueFilter;
		CFilterOptions::eViewRuns eRuns;
		bool bViewAllNames;
		std::set<ARBString> nameFilter;
	};

private:
	unsigned short m_uCalFilter;
	bool m_bViewAllDates;
	ARBDate m_dateStart;
	bool m_bDateStart;
	ARBDate m_dateEnd;
	bool m_bDateEnd;
	bool m_bViewAllVenues;
	std::vector<CVenueFilter> m_venueFilters;
	eViewRuns m_eRuns;
	bool m_bViewAllNames;
	std::set<ARBString> m_nameFilters;
	ARBString m_curFilter;
	int m_nFilters;
	std::vector<CFilterOptionData> m_filters;

public:
	static CFilterOptions& Options();
	CFilterOptions(); ///< Should only be used in options dialog
	void Load();
	void Save();

	// Helper functions
	bool IsFilterEnabled();
	bool IsDateVisible(
			ARBDate const& startDate,
			ARBDate const& endDate);
	bool IsTitleVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTitlePtr pTitle);
	bool IsVenueVisible(
			std::vector<CVenueFilter> const& venues,
			ARBString const& venue);
	bool IsVenueDivisionVisible(
			std::vector<CVenueFilter> const& venues,
			ARBString const& venue,
			ARBString const& div);
	bool IsTrialVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial);
	unsigned short IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	bool IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBConfigVenuePtr pVenue,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	bool IsCalendarVisible(ARBCalendarPtr pCal);
	bool IsTrainingLogVisible(
			std::set<ARBString> const& names,
			ARBTrainingPtr pTraining);

	// Filtering: Calendar
	CCalendarViewFilter FilterCalendarView() const
	{
		return m_uCalFilter;
	}
	void SetFilterCalendarView(CCalendarViewFilter inFilter)
	{
		m_uCalFilter = inFilter.m_Filter;
	}

	// Filtering: Date
	bool GetViewAllDates() const
	{
		return m_bViewAllDates;
	}
	void SetViewAllDates(bool bViewAll)
	{
		m_bViewAllDates = bViewAll;
	}
	ARBDate GetStartFilterDate() const
	{
		return m_dateStart;
	}
	void SetStartFilterDate(ARBDate const& date)
	{
		m_dateStart = date;
	}
	bool GetStartFilterDateSet() const
	{
		return m_bDateStart;
	}
	void SetStartFilterDateSet(bool bSet)
	{
		m_bDateStart = bSet;
	}
	ARBDate GetEndFilterDate() const
	{
		return m_dateEnd;
	}
	void SetEndFilterDate(ARBDate const& date)
	{
		m_dateEnd = date;
	}
	bool GetEndFilterDateSet() const
	{
		return m_bDateEnd;
	}
	void SetEndFilterDateSet(bool bSet)
	{
		m_bDateEnd = bSet;
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
		venues = m_venueFilters;
	}
	void SetFilterVenue(std::vector<CVenueFilter> const& venues)
	{
		m_venueFilters = venues;
	}

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
	void GetTrainingFilterNames(std::set<ARBString>& outNames) const
	{
		outNames = m_nameFilters;
	}
	void SetTrainingFilterNames(std::set<ARBString> const& inNames)
	{
		m_nameFilters = inNames;
	}

	ARBString GetCurrentFilter() const
	{
		return m_curFilter;
	}
	void SetCurrentFilter(ARBString const& inName)
	{
		m_curFilter = inName;
	}
};
