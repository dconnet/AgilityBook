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
	friend class CFilterOptions;
	friend class CFilterOptionData;
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
private:
	unsigned short m_Filter;
};

class CFilterOptions
{
public:
	// Helper functions
	static bool IsFilterEnabled();
	static bool IsDateVisible(
			ARBDate const& startDate,
			ARBDate const& endDate);
	static bool IsTitleVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTitlePtr pTitle);
	static bool IsVenueVisible(
			std::vector<CVenueFilter> const& venues,
			ARBString const& venue);
	static bool IsVenueDivisionVisible(
			std::vector<CVenueFilter> const& venues,
			ARBString const& venue,
			ARBString const& div);
	static bool IsTrialVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial);
	static unsigned short IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	static bool IsRunVisible(
			std::vector<CVenueFilter> const& venues,
			ARBConfigVenuePtr pVenue,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	static bool IsCalendarVisible(ARBCalendarPtr pCal);
	static bool IsTrainingLogVisible(
			std::set<ARBString> const& names,
			ARBTrainingPtr pTraining);

	// Filtering: Calendar
	static CCalendarViewFilter FilterCalendarView();
	static void SetFilterCalendarView(CCalendarViewFilter inFilter);

	// Filtering: Date
	static bool GetViewAllDates();
	static void SetViewAllDates(bool bViewAll);
	static ARBDate GetStartFilterDate();
	static void SetStartFilterDate(ARBDate const& date);
	static bool GetStartFilterDateSet();
	static void SetStartFilterDateSet(bool bSet);
	static ARBDate GetEndFilterDate();
	static void SetEndFilterDate(ARBDate const& date);
	static bool GetEndFilterDateSet();
	static void SetEndFilterDateSet(bool bSet);

	// Filtering: Runs
	static bool GetViewAllVenues();
	static void SetViewAllVenues(bool bViewAll);
	static void GetFilterVenue(std::vector<CVenueFilter>& venues);
	static void SetFilterVenue(std::vector<CVenueFilter> const& venues);
	typedef enum
	{
		eViewRunsAll = 0,
		eViewRunsQs = 1,
		eViewRunsNQs = 2,
	} eViewRuns;
	static eViewRuns GetViewRuns();
	static void SetViewRuns(eViewRuns eViewAll);

	// Filtering: Training Log
	static bool GetTrainingViewAllNames();
	static void SetTrainingViewAllNames(bool bViewAll);
	static void GetTrainingFilterNames(std::set<ARBString>& outNames);
	static void SetTrainingFilterNames(std::set<ARBString> const& inNames);

	static ARBString GetCurrentFilter();
	static void SetCurrentFilter(ARBString const& inName);
	static size_t GetAllNamedFilters(std::set<ARBString>& outNames);
	static void DeleteNamedFilter(ARBString const& inName);
};

/**
 * Used only for getting/setting options in the options dialog.
 * The current named set of options is always written into the current
 * registry settings.
 */
class CFilterOptionData
{
public:
	CFilterOptionData();
	CFilterOptionData(ARBString const& inName);
	CFilterOptionData(CFilterOptionData const& rhs);
	CFilterOptionData& operator=(CFilterOptionData const& rhs);
	bool SaveName();
	void SaveDefault();

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
