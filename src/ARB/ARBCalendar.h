#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief ARBCalendar and ARBCalendarList class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-31 DRC Added FindCalendar.
 */

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * The calendar cribsheet.
 *
 * @note A venue name in a calendar entry may be any name, including multiple
 *       venues. It does not have to be a configured venue. However, in order
 *       to create a trial entry from it, the venue name must be in the
 *       configuration. Because of this, it is currently not possible to
 *       automatically create a multiple hosted trial.
 */
class ARBCalendar : public ARBBase
{
public:
	/**
	 * Various states an entry may be in.
	 */
	typedef enum
	{
		eNot,		///< Not entered
		eEntered,	///< Entered
		ePlanning	///< Not entered, but planning on it soon
	} eEntry;

	ARBCalendar();
	ARBCalendar(ARBCalendar const& rhs);
	ARBCalendar& operator=(ARBCalendar const& rhs);
	bool operator==(ARBCalendar const& rhs) const;
	bool operator!=(ARBCalendar const& rhs) const;
	bool operator<(ARBCalendar const& rhs) const;
	bool operator>(ARBCalendar const& rhs) const;
	bool operator<(ARBDate const& rhs) const;
	bool operator>(ARBDate const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::string GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	/**
	 * Load a calendar entry
	 * @pre inTree is the actual ARBCalendar element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBCalendar element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Is this calendar entry (start and end dates) before a given date?
	 * @param inDate Date to check entry against.
	 * @retval true inDate is before both entry start and end dates.
	 * @retval false inDate is after one or both the start and end dates.
	 */
	bool IsBefore(ARBDate const& inDate) const;

	/**
	 * Is the given date in the range of the calendar entry?
	 * @param inDate Date to check.
	 * @retval true inDate is on one of the dates in the entry range.
	 * @retval false inDate is outside the calendar entry range.
	 */
	bool InRange(ARBDate const& inDate) const;

	/**
	 * Does the range of inDate1 and inDate2 overlap this calendar entry?
	 * @param inDate1 Beginning of date range.
	 * @param inDate2 End of date range.
	 * @retval true There is an overlap between the entry and the given range.
	 * @retval false There is no overlap
	 */
	bool IsRangeOverlapped(
		ARBDate const& inDate1,
		ARBDate const& inDate2) const;

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetStartDate() const;
	void SetStartDate(ARBDate const& inDate);
	ARBDate const& GetEndDate() const;
	void SetEndDate(ARBDate const& inDate);
	ARBDate const& GetOpeningDate() const;
	void SetOpeningDate(ARBDate const& inDate);
	ARBDate const& GetClosingDate() const;
	void SetClosingDate(ARBDate const& inDate);
	bool IsTentative() const;
	void SetIsTentative(bool inTentative);
	std::string const& GetLocation() const;
	void SetLocation(std::string const& inLocation);
	std::string const& GetClub() const;
	void SetClub(std::string const& inClub);
	std::string const& GetVenue() const;
	void SetVenue(std::string const& inVenue);
	eEntry GetEntered() const;
	void SetEntered(eEntry inEnter);
	std::string const& GetNote() const;
	void SetNote(std::string const& inNote);

private:
	~ARBCalendar();
	ARBDate m_DateStart;
	ARBDate m_DateEnd;
	ARBDate m_DateOpening;
	ARBDate m_DateClosing;
	bool m_bTentative;
	std::string m_Location;
	std::string m_Club;
	std::string m_Venue;
	eEntry m_eEntered;
	std::string m_Note;
};

inline bool ARBCalendar::operator<(ARBCalendar const& rhs) const
{
	return m_DateStart < rhs.GetStartDate();
}

inline bool ARBCalendar::operator>(ARBCalendar const& rhs) const
{
	return m_DateEnd > rhs.GetEndDate();
}

inline bool ARBCalendar::operator<(ARBDate const& rhs) const
{
	return m_DateStart < rhs;
}

inline bool ARBCalendar::operator>(ARBDate const& rhs) const
{
	return m_DateEnd > rhs;
}

inline ARBDate const& ARBCalendar::GetStartDate() const
{
	return m_DateStart;
}

inline void ARBCalendar::SetStartDate(ARBDate const& inDate)
{
	m_DateStart = inDate;
}

inline ARBDate const& ARBCalendar::GetEndDate() const
{
	return m_DateEnd;
}

inline void ARBCalendar::SetEndDate(ARBDate const& inDate)
{
	m_DateEnd = inDate;
}

inline ARBDate const& ARBCalendar::GetOpeningDate() const
{
	return m_DateOpening;
}

inline void ARBCalendar::SetOpeningDate(ARBDate const& inDate)
{
	m_DateOpening = inDate;
}

inline ARBDate const& ARBCalendar::GetClosingDate() const
{
	return m_DateClosing;
}

inline void ARBCalendar::SetClosingDate(ARBDate const& inDate)
{
	m_DateClosing = inDate;
}

inline bool ARBCalendar::IsTentative() const
{
	return m_bTentative;
}

inline void ARBCalendar::SetIsTentative(bool inTentative)
{
	m_bTentative = inTentative;
}

inline std::string const& ARBCalendar::GetLocation() const
{
	return m_Location;
}

inline void ARBCalendar::SetLocation(std::string const& inLocation)
{
	m_Location = inLocation;
}

inline std::string const& ARBCalendar::GetClub() const
{
	return m_Club;
}

inline void ARBCalendar::SetClub(std::string const& inClub)
{
	m_Club = inClub;
}

inline std::string const& ARBCalendar::GetVenue() const
{
	return m_Venue;
}

inline void ARBCalendar::SetVenue(std::string const& inVenue)
{
	m_Venue = inVenue;
}

inline ARBCalendar::eEntry ARBCalendar::GetEntered() const
{
	return m_eEntered;
}

inline void ARBCalendar::SetEntered(ARBCalendar::eEntry inEnter)
{
	m_eEntered = inEnter;
}

inline std::string const& ARBCalendar::GetNote() const
{
	return m_Note;
}

inline void ARBCalendar::SetNote(std::string const& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBCalendar objects.
 */
class ARBCalendarList : public ARBVectorLoad1<ARBCalendar>
{
public:
	/**
	 * Sort the list by date.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get a list of all the trials that have been entered.
	 * @param outEntered Entered trials.
	 * @return Number of entered trials.
	 */
	size_t GetAllEntered(std::vector<ARBCalendar const*>& outEntered) const;

	/**
	 * Trim all calendar entries before inDate.
	 * @param inDate Date to trim entries.
	 * @return Number of entries that were trimmed.
	 * @note No dates are trimmed if inData is not valid.
	 */
	int TrimEntries(ARBDate const& inDate);

	/**
	 * Find a calendar entry.
	 * @param inCal Object to search for.
	 * @return Identical object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 * @note Equality is tested by value, not pointer.
	 */
	ARBCalendar* FindCalendar(ARBCalendar const* inCal);

	/**
	 * Add a calendar entry to the list.
	 * @param inCal Entry to add.
	 * @return Returns the added object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBCalendar* AddCalendar(ARBCalendar* inCal);

	/**
	 * Delete a calendar entry.
	 * @param inCal Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteCalendar(ARBCalendar const* inCal);
};
