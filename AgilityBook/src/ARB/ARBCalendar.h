#pragma once

/*
 * Copyright � 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-05 DRC Added iCalendar support.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-31 DRC Added FindCalendar.
 */

#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"

/**
 * iCalendar interface
 */
class ICalendar
{
protected:
	ICalendar();
	virtual ~ICalendar();

public:
	/**
	 * Create an iCalendar interface and generate the beginning lines.
	 * @param ioStream Stream to write data to.
	 * @param inVersion Either 1 (vCal) or 2 (iCal).
	 */
	static ICalendar* iCalendarBegin(
			otstream& ioStream,
			int inVersion);

	/**
	 * Write the final iCal lines and delete the iCalendar object.
	 */
	virtual void Release() = 0;
};

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

	typedef enum
	{
		eAccomNone,		///< No accommodations
		eAccomTodo,		///< Need a place to stay
		eAccomConfirmed	///< Reservations have been made
	} eAccommodations;

	/**
	 *
	 */
	typedef enum
	{
		eUIDvEvent,
		eUIDvTodo
	} eUidType;

protected:
	ARBCalendar();
	ARBCalendar(ARBCalendar const& rhs);

public:
	~ARBCalendar();
	static ARBCalendarPtr New();
	ARBCalendarPtr Clone() const;

	ARBCalendar& operator=(ARBCalendar const& rhs);

	bool operator==(ARBCalendar const& rhs) const;
	bool operator!=(ARBCalendar const& rhs) const
	{
		return !operator==(rhs);
	}
	bool operator<(ARBCalendar const& rhs) const
	{
		return m_DateStart < rhs.GetStartDate();
	}
	bool operator>(ARBCalendar const& rhs) const
	{
		return m_DateEnd > rhs.GetEndDate();
	}
	bool operator<(ARBDate const& rhs) const
	{
		return m_DateStart < rhs;
	}
	bool operator>(ARBDate const& rhs) const
	{
		return m_DateEnd > rhs;
	}

	/**
	 * Get a UID. Used when generating iCalendar entries.
	 * @param inType UID type to generate.
	 */
	tstring GetUID(eUidType inType) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual tstring GetGenericName() const
	{
		return m_Venue + _T(" ") + m_Club + _T(" ") + m_Location;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;

	/**
	 * Load a calendar entry
	 * @pre inTree is the actual ARBCalendar element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBCalendar element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Generate vEvent/vTask entries for iCalendar.
	 * @param inIoStream Stream to place data.
	 * @param inAlarm Set an alaram inAlarm days before trial start if entered
	 *                or before trial opening if planning to enter.
	 */
	void iCalendar(
			ICalendar* inIoStream,
			int inAlarm) const;

	/**
	 * Test for equality.
	 * @param inCal Object to test for equality.
	 * @param inMatchExact Match the entire entry, or just start end dates, venue, clubname.
	 */
	bool IsMatch(ARBCalendarPtr inCal, bool inMatchExact) const;

	/**
	 * Update the existing entry.
	 * @param inCal Entry to update from.
	 * @return Entry was updated
	 * @note This will not update the entry or confirmation status.
	 */
	bool Update(ARBCalendarPtr inCal);

	/**
	 * Is this calendar entry (start and end dates) before a given date?
	 * @param inDate Date to check entry against.
	 * @retval true inDate is before both entry start and end dates.
	 * @retval false inDate is after one or both the start and end dates.
	 */
	bool IsBefore(ARBDate const& inDate) const
	{
		return (m_DateStart < inDate && m_DateEnd < inDate);
	}

	/**
	 * Is the given date in the range of the calendar entry?
	 * @param inDate Date to check.
	 * @retval true inDate is on one of the dates in the entry range.
	 * @retval false inDate is outside the calendar entry range.
	 */
	bool InRange(ARBDate const& inDate) const
	{
		return inDate.isBetween(m_DateStart, m_DateEnd);
	}

	/**
	 * Does the range of inDate1 and inDate2 overlap this calendar entry?
	 * @param inDate1 Beginning of date range.
	 * @param inDate2 End of date range.
	 * @retval true There is an overlap between the entry and the given range.
	 * @retval false There is no overlap
	 */
	bool IsRangeOverlapped(
			ARBDate const& inDate1,
			ARBDate const& inDate2) const
	{
		if (m_DateStart.isBetween(inDate1, inDate2)
		|| m_DateEnd.isBetween(inDate1, inDate2))
			return true;
		else
			return false;
	}

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetStartDate() const
	{
		return m_DateStart;
	}
	void SetStartDate(ARBDate const& inDate)
	{
		m_DateStart = inDate;
	}
	ARBDate const& GetEndDate() const
	{
		return m_DateEnd;
	}
	void SetEndDate(ARBDate const& inDate)
	{
		m_DateEnd = inDate;
	}
	ARBDate const& GetOpeningDate() const
	{
		return m_DateOpening;
	}
	void SetOpeningDate(ARBDate const& inDate)
	{
		m_DateOpening = inDate;
	}
	ARBDate const& GetDrawDate() const
	{
		return m_DateDraw;
	}
	void SetDrawDate(ARBDate const& inDate)
	{
		m_DateDraw = inDate;
	}
	ARBDate const& GetClosingDate() const
	{
		return m_DateClosing;
	}
	void SetClosingDate(ARBDate const& inDate)
	{
		m_DateClosing = inDate;
	}
	bool IsTentative() const
	{
		return m_bTentative;
	}
	void SetIsTentative(bool inTentative)
	{
		m_bTentative = inTentative;
	}
	tstring const& GetLocation() const
	{
		return m_Location;
	}
	void SetLocation(tstring const& inLocation)
	{
		m_Location = inLocation;
	}
	tstring const& GetClub() const
	{
		return m_Club;
	}
	void SetClub(tstring const& inClub)
	{
		m_Club = inClub;
	}
	tstring const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(tstring const& inVenue)
	{
		m_Venue = inVenue;
	}
	eEntry GetEntered() const
	{
		return m_eEntered;
	}
	void SetEntered(eEntry inEnter)
	{
		m_eEntered = inEnter;
	}
	eAccommodations GetAccommodation()
	{
		return m_eAccommodations;
	}
	void SetAccommodation(eAccommodations inAccom)
	{
		m_eAccommodations = inAccom;
	}
	tstring const& GetConfirmation() const
	{
		return m_Confirmation;
	}
	void SetConfirmation(tstring const& inConfirmation)
	{
		m_Confirmation = inConfirmation;
	}
	tstring const& GetSecEmail() const
	{
		return m_SecEmail;
	}
	void SetSecEmail(tstring const& inAddr)
	{
		m_SecEmail = inAddr;
	}
	tstring const& GetPremiumURL() const
	{
		return m_PremiumURL;
	}
	void SetPremiumURL(tstring const& inURL)
	{
		m_PremiumURL = inURL;
	}
	tstring const& GetOnlineURL() const
	{
		return m_OnlineURL;
	}
	void SetOnlineURL(tstring const& inURL)
	{
		m_OnlineURL = inURL;
	}
	tstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(tstring const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDate m_DateStart;
	ARBDate m_DateEnd;
	ARBDate m_DateOpening;
	ARBDate m_DateDraw;
	ARBDate m_DateClosing;
	bool m_bTentative;
	tstring m_Location;
	tstring m_Club;
	tstring m_Venue;
	eEntry m_eEntered;
	eAccommodations m_eAccommodations;
	tstring m_Confirmation;
	tstring m_SecEmail;
	tstring m_PremiumURL;
	tstring m_OnlineURL;
	tstring m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBCalendar objects.
 */
class ARBCalendarList : public ARBVector<ARBCalendarPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by date.
	 */
	void sort();

	/**
	 * Get a list of all the trials that have been entered.
	 * @param outEntered Entered trials.
	 * @return Number of entered trials.
	 */
	size_t GetAllEntered(std::vector<ARBCalendarPtr>& outEntered) const;

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
	 * @param inMatchExact Match the entire entry, or just start end dates, venue, clubname.
	 * @param outCal The object that was found.
	 * @return Whether the object was found.
	 * @note Equality is tested by value, not pointer.
	 */
	bool FindCalendar(
			ARBCalendarPtr inCal,
			bool inMatchExact,
			ARBCalendarPtr* outCal = NULL) const;

	/**
	 * Add a calendar entry to the list.
	 * @param inCal Entry to add.
	 * @return Whether the object was added.
	 */
	bool AddCalendar(ARBCalendarPtr inCal);

	/**
	 * Delete a calendar entry.
	 * @param inCal Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteCalendar(ARBCalendarPtr inCal);
};
