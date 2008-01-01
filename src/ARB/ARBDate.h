#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief ARBDate class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-13 DRC Added inForceOutput to GetString.
 * @li 2005-07-05 DRC Added a new formatting option.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-03-21 DRC Fixed up some operators.
 * @li 2004-01-04 DRC Added FromString().
 */

#include "ARBTypes.h"

/////////////////////////////////////////////////////////////////////////////
/**
 * Date class.
 * In XML/Element, this class is only used as an attribute, never an element.
 *
 * @todo Migrate all usage away from time_t.
 */
class ARBDate
{
public:
	/**
	 * Output date format (do not change the values!)
	 */
	typedef enum
	{
		// Leading zeros
		eDefault		= 0,	///< YYYY-MM-DD or MM/DD/YYYY (input), =2 (output)
		eDashMMDDYYYY	= 1,	///< MM-DD-YYYY
		eSlashMMDDYYYY	= 2,	///< MM/DD/YYYY
		eDashYYYYMMDD	= 3,	///< YYYY-MM-DD
		eSlashYYYYMMDD	= 4,	///< YYYY/MM/DD
		eDashDDMMYYYY	= 5,	///< DD-MM-YYYY
		eSlashDDMMYYYY	= 6,	///< DD/MM/YYYY
		// No leading zeros
		eDashMDY		= 7,	///< M-D-Y
		eSlashMDY		= 8,	///< M/D/Y
		eDashYMD		= 9,	///< Y-M-D
		eSlashYMD		= 10,	///< Y/M/D
		eDashDMY		= 11,	///< D-M-Y
		eSlashDMY		= 12,	///< D/M/Y
		// New formats...
		eYYYYMMDD		= 13,	///< YYYYMMDD (output only)
	} DateFormat;

	/**
	 * Convert a string to a date
	 * @param inDate String to convert
	 * @param inFormat Parse using this format
	 * @return Parsed date, if parse fails, date is invalid.
	 */
	static ARBDate FromString(
			tstring const& inDate,
			DateFormat inFormat);

	/**
	 * Get a string showing the valid date range (if set)
	 * @param inFrom Valid from date.
	 * @param inTo Valid to date.
	 * @param inFormat Date format.
	 * @return Date range string
	 */
	static tstring GetValidDateString(
			ARBDate const& inFrom,
			ARBDate const& inTo,
			DateFormat inFormat = eDashYMD);

	/**
	 * Get the current date.
	 */
	static ARBDate Today();

	ARBDate()
		: m_Julian(0)
	{
	}
	ARBDate(ARBDate const& rhs)
		: m_Julian(rhs.m_Julian)
	{
	}
	ARBDate(time_t inTime);
#if _WIN32 && _MSC_VER >= 1300 && _MSC_VER < 1400
	// VC6 doesn't have this and VC8's time_t is 64bit.
	ARBDate(__time64_t inTime);
#endif
	ARBDate(
			int inYr,
			int inMon,
			int inDay)
		: m_Julian(0)
	{
		SetDate(inYr, inMon, inDay);
	}

	~ARBDate()
	{
	}

	/**
	 * Is the date valid?
	 */
	bool IsValid() const
	{
		return (0 < m_Julian);
	}

	/**
	 * Set the date to invalid.
	 */
	void clear()
	{
		m_Julian = 0;
	}

	/**
	 * Set the date to today.
	 */
	void SetToday();

	/**
	 * Get the julian day (not julian date)
	 */
	long GetJulianDay() const
	{
		return m_Julian;
	}

	/**
	 * Set the julian day (not julian date)
	 */
	void SetJulianDay(long inJulian)
	{
		if (0 < inJulian)
			m_Julian = inJulian;
	}

	/**
	 * Set the date
	 * @param inYr Current year.
	 * @param inMon Current month.
	 * @param inDay Current day.
	 * @param bClearOnError If the input date is bad, clear the existing date.
	 * @return Whether date was set (invalid date fails)
	 */
	bool SetDate(
			int inYr,
			int inMon,
			int inDay,
			bool bClearOnError = true);

	ARBDate& operator=(ARBDate const& rhs)
	{
		if (this != &rhs)
		{
			m_Julian = rhs.m_Julian;
		}
		return *this;
	}
	bool operator==(ARBDate const& rhs) const
	{
		return m_Julian == rhs.m_Julian;
	}
	bool operator!=(ARBDate const& rhs) const
	{
		return m_Julian != rhs.m_Julian;
	}
	bool operator<(ARBDate const& rhs) const
	{
		return m_Julian < rhs.m_Julian;
	}
	bool operator>(ARBDate const& rhs) const
	{
		return m_Julian > rhs.m_Julian;
	}
	bool operator<=(ARBDate const& rhs) const
	{
		return m_Julian <= rhs.m_Julian;
	}
	bool operator>=(ARBDate const& rhs) const
	{
		return m_Julian >= rhs.m_Julian;
	}
	long operator-(ARBDate const& rhs) const
	{
		return m_Julian - rhs.m_Julian;
	}
	ARBDate& operator++() //prefix
	{
		++m_Julian;
		return *this;
	}
	ARBDate operator++(int) //postfix
	{
		ARBDate tmp(*this);
		++m_Julian;
		return tmp;
	}
	ARBDate& operator--() //prefix
	{
		--m_Julian;
		return *this;
	}
	ARBDate operator--(int) //postfix
	{
		ARBDate tmp(*this);
		--m_Julian;
		return tmp;
	}
	ARBDate& operator+=(int inD)
	{
		m_Julian += inD;
		return *this;
	}
	ARBDate& operator-=(int inD)
	{
		m_Julian -= inD;
		return *this;
	}
	ARBDate operator+(int inD)
	{
		ARBDate d;
		d.SetJulianDay(m_Julian + inD);
		return d;
	}
	ARBDate operator-(int inD)
	{
		ARBDate d;
		d.SetJulianDay(m_Julian - inD);
		return d;
	}

	/**
	 * Test if date is between two dates (inclusive)
	 */
	bool isBetween(
			ARBDate const& inDate1,
			ARBDate const& inDate2) const
	{
		return m_Julian >= inDate1.m_Julian && m_Julian <= inDate2.m_Julian;
	}

	bool isLeap() const ///< Is this a leap year?
	{
		int yr = GetYear();
		return (yr & 3) == 0 && yr % 100 != 0 || yr % 400 == 0;
	}

	/**
	 * Return the date as a string.
	 * @param inFormat Format of date string.
	 * @param inForceOutput Always return a string, even if the date is not valid.
	 * @return Date in the format defined by inFormat.
	 */
	tstring GetString(
			DateFormat inFormat,
			bool inForceOutput = false) const;

	/**
	 * Convert the date to a time.
	 * Note, this will return an invalid time_t (-1) if the date falls
	 * outside the range of a time_t (mktime()).
	 * @return Date converted to time_t.
	 */
	time_t GetDate() const;

	/**
	 * Get the current date.
	 * @param outYr Current year.
	 * @param outMon Current month.
	 * @param outDay Current day.
	 */
	void GetDate(
			int& outYr,
			int& outMon,
			int& outDay) const;

	int GetDay() const;		///< Get the current day.
	int GetMonth() const;	///< Get the current month.
	int GetYear() const;	///< Get the current year.

	/**
	 * Days of the week (do not change the values!)
	 */
	typedef enum
	{
		eSunday = 0,	///< Sunday
		eMonday = 1,	///< Monday
		eTuesday = 2,	///< Tuesday
		eWednesday = 3,	///< Wednesday
		eThursday = 4,	///< Thursday
		eFriday = 5,	///< Friday
		eSaturday = 6,	///< Saturday
	} DayOfWeek;

	/**
	 * Get the day of the week of the current date.
	 * @param inFirstDay Define what day of week has index 0 (1st day of week).
	 */
	int GetDayOfWeek(DayOfWeek inFirstDay = eSunday) const
	{
		// This was copied from another source, but I don't remember where...
		// I suspect it won't work properly on dates before 1752 (start of
		// Gregorian calendar).
		//return int((((m_Julian+1)%7)+6)%7); // Mon = 0
		return static_cast<int>((((m_Julian+1)%7)+(7-static_cast<int>(inFirstDay)))%7);
	}

private:
	long m_Julian;	///< Julian day, not Julian date.
};
