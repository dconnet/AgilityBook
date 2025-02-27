#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBCalendar and ARBCalendarList class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-05 Added iCalendar support.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-31 Added FindCalendar.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Various states an entry may be in.
 */
enum class ARBCalendarEntry
{
	Not,     ///< Not entered
	Entered, ///< Entered
	Pending, ///< Entered, not confirmed
	Planning ///< Not entered, but planning on it soon
};

enum class ARBAccommodations
{
	None,     ///< No accommodations
	Todo,     ///< Need a place to stay
	Confirmed ///< Reservations have been made
};


/**
 * iCalendar interface
 */
class ARB_API ICalendar
{
protected:
	ICalendar(ICalendar const&) = delete;
	ICalendar(ICalendar&&) = delete;
	ICalendar& operator=(ICalendar const&) = delete;
	ICalendar& operator=(ICalendar&&) = delete;

public:
	ICalendar();
	virtual ~ICalendar();

	/**
	 * Create an iCalendar interface and generate the beginning lines.
	 * @param ioStream Stream to write data to.
	 * @param inVersion Either 1 (vCal) or 2 (iCal).
	 */
	static ICalendarPtr iCalendarBegin(std::ostream& ioStream, int inVersion);
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
class ARB_API ARBCalendar : public ARBBase
{
public:
	/**
	 *
	 */
	enum class UidType
	{
		vEvent,
		vTodo
	};

protected:
	ARBCalendar();
	ARBCalendar(ARBCalendar const& rhs);
	ARBCalendar(ARBCalendar&& rhs);

public:
	~ARBCalendar();
	static ARBCalendarPtr New();
	ARBCalendarPtr Clone() const;

	ARBCalendar& operator=(ARBCalendar const& rhs);
	ARBCalendar& operator=(ARBCalendar&& rhs);

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
	bool operator<(ARBCommon::ARBDate const& rhs) const
	{
		return m_DateStart < rhs;
	}
	bool operator>(ARBCommon::ARBDate const& rhs) const
	{
		return m_DateEnd > rhs;
	}

	/**
	 * Get a UID. Used when generating iCalendar entries.
	 * @param inType UID type to generate.
	 */
	wxString GetUID(UidType inType) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override
	{
		return m_Venue + L" " + m_Club + L" " + m_Location;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override;

	/**
	 * Load a calendar entry
	 * @pre inTree is the actual ARBCalendar element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBCalendar element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/**
	 * Generate vEvent/vTask entries for iCalendar.
	 * @param inIoStream Stream to place data.
	 * @param inAlarm Set an alaram inAlarm days before trial start if entered
	 *                or before trial opening if planning to enter.
	 */
	void iCalendar(ICalendarPtr inIoStream, int inAlarm) const;

	/**
	 * Test for equality.
	 * @param inCal Object to test for equality.
	 * @param inMatchExact Match the entire entry, or just start end dates, venue, clubname.
	 */
	bool IsMatch(ARBCalendarPtr const& inCal, bool inMatchExact) const;

	/**
	 * Update the existing entry.
	 * @param inCal Entry to update from.
	 * @return Entry was updated
	 * @note This will not update the entry or confirmation status.
	 */
	bool Update(ARBCalendarPtr const& inCal);

	/**
	 * Is this calendar entry (start and end dates) before a given date?
	 * @param inDate Date to check entry against.
	 * @retval true inDate is before both entry start and end dates.
	 * @retval false inDate is after one or both the start and end dates.
	 */
	bool IsBefore(ARBCommon::ARBDate const& inDate) const
	{
		return (m_DateStart < inDate && m_DateEnd < inDate);
	}

	/**
	 * Is the given date in the range of the calendar entry?
	 * @param inDate Date to check.
	 * @retval true inDate is on one of the dates in the entry range.
	 * @retval false inDate is outside the calendar entry range.
	 */
	bool InRange(ARBCommon::ARBDate const& inDate) const
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
	bool IsRangeOverlapped(ARBCommon::ARBDate const& inDate1, ARBCommon::ARBDate const& inDate2) const
	{
		if (m_DateStart.isBetween(inDate1, inDate2) || m_DateEnd.isBetween(inDate1, inDate2))
			return true;
		else
			return false;
	}

	/*
	 * Getters/setters.
	 */
	ARBCommon::ARBDate const& GetStartDate() const
	{
		return m_DateStart;
	}
	void SetStartDate(ARBCommon::ARBDate const& inDate)
	{
		m_DateStart = inDate;
	}
	ARBCommon::ARBDate const& GetEndDate() const
	{
		return m_DateEnd;
	}
	void SetEndDate(ARBCommon::ARBDate const& inDate)
	{
		m_DateEnd = inDate;
	}
	ARBCommon::ARBDate const& GetOpeningDate() const
	{
		return m_DateOpening;
	}
	void SetOpeningDate(ARBCommon::ARBDate const& inDate)
	{
		m_DateOpening = inDate;
	}
	ARBCommon::ARBDate const& GetDrawDate() const
	{
		return m_DateDraw;
	}
	void SetDrawDate(ARBCommon::ARBDate const& inDate)
	{
		m_DateDraw = inDate;
	}
	ARBCommon::ARBDate const& GetClosingDate() const
	{
		return m_DateClosing;
	}
	void SetClosingDate(ARBCommon::ARBDate const& inDate)
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
	wxString const& GetLocation() const
	{
		return m_Location;
	}
	void SetLocation(wxString const& inLocation)
	{
		m_Location = inLocation;
	}
	wxString const& GetClub() const
	{
		return m_Club;
	}
	void SetClub(wxString const& inClub)
	{
		m_Club = inClub;
	}
	wxString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(wxString const& inVenue)
	{
		m_Venue = inVenue;
	}
	ARBCalendarEntry GetEntered() const
	{
		return m_eEntered;
	}
	void SetEntered(ARBCalendarEntry inEnter)
	{
		m_eEntered = inEnter;
	}
	ARBAccommodations GetAccommodation()
	{
		return m_eAccommodations;
	}
	void SetAccommodation(ARBAccommodations inAccom)
	{
		m_eAccommodations = inAccom;
	}
	wxString const& GetConfirmation() const
	{
		return m_Confirmation;
	}
	void SetConfirmation(wxString const& inConfirmation)
	{
		m_Confirmation = inConfirmation;
	}
	wxString const& GetSecEmail() const
	{
		return m_SecEmail;
	}
	void SetSecEmail(wxString const& inAddr)
	{
		m_SecEmail = inAddr;
	}
	wxString const& GetPremiumURL() const
	{
		return m_PremiumURL;
	}
	void SetPremiumURL(wxString const& inURL)
	{
		m_PremiumURL = inURL;
	}
	wxString const& GetOnlineURL() const
	{
		return m_OnlineURL;
	}
	void SetOnlineURL(wxString const& inURL)
	{
		m_OnlineURL = inURL;
	}
	wxString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(wxString const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBCommon::ARBDate m_DateStart;
	ARBCommon::ARBDate m_DateEnd;
	ARBCommon::ARBDate m_DateOpening;
	ARBCommon::ARBDate m_DateDraw;
	ARBCommon::ARBDate m_DateClosing;
	bool m_bTentative;
	wxString m_Location;
	wxString m_Club;
	wxString m_Venue;
	ARBCalendarEntry m_eEntered;
	ARBAccommodations m_eAccommodations;
	wxString m_Confirmation;
	wxString m_SecEmail;
	wxString m_PremiumURL;
	wxString m_OnlineURL;
	wxString m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBCalendar objects.
 */
class ARB_API ARBCalendarList : public ARBVector<ARBCalendarPtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
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
	int TrimEntries(ARBCommon::ARBDate const& inDate);

	/**
	 * Find a calendar entry.
	 * @param inCal Object to search for.
	 * @param inMatchExact Match the entire entry, or just start end dates, venue, clubname.
	 * @param outCal The object that was found.
	 * @return Whether the object was found.
	 * @note Equality is tested by value, not pointer.
	 */
	bool FindCalendar(ARBCalendarPtr const& inCal, bool inMatchExact, ARBCalendarPtr* outCal = nullptr) const;

	/**
	 * Add a calendar entry to the list.
	 * @param inCal Entry to add.
	 * @return Whether the object was added.
	 */
	bool AddCalendar(ARBCalendarPtr const& inCal);

	/**
	 * Delete a calendar entry.
	 * @param inCal Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteCalendar(ARBCalendarPtr const& inCal);
};

} // namespace ARB
} // namespace dconSoft
