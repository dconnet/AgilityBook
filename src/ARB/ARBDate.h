#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-03-21 DRC Fixed up some operators.
 * @li 2004-01-04 DRC Added FromString().
 */

#include <string>
class Element;

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
	 * Output date format
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
	} DateFormat;

	/**
	 * Convert a string to a date
	 * @param inDate String to convert
	 * @param inFormat Parse using this format
	 * @return Parsed date, if parse fails, date is invalid.
	 */
	static ARBDate FromString(std::string const& inDate, DateFormat inFormat);

	/**
	 * Get the current date.
	 */
	static ARBDate Today();

	ARBDate();
	ARBDate(ARBDate const& rhs);
	ARBDate(time_t inTime);
#if _WIN32 && _MSC_VER >= 1300 && _MSC_VER < 1400
	// VC6 doesn't have this and VC8's time_t is 64bit.
	ARBDate(__time64_t inTime);
#endif
	ARBDate(int inYr, int inMon, int inDay);
	~ARBDate();

	/**
	 * Is the date valid?
	 */
	bool IsValid() const;

	/**
	 * Set the date to invalid.
	 */
	void clear();

	/**
	 * Set the date to today.
	 */
	void SetToday();

	/**
	 * Get the julian day (not julian date)
	 */
	long GetJulianDay() const;

	/**
	 * Set the julian day (not julian date)
	 */
	void SetJulianDay(long inJulian);

	/**
	 * Set the date
	 * @param inYr Current year.
	 * @param inMon Current month.
	 * @param inDay Current day.
	 */
	void SetDate(int inYr, int inMon, int inDay);

	ARBDate& operator=(ARBDate const& rhs);
	bool operator==(ARBDate const& rhs) const;
	bool operator!=(ARBDate const& rhs) const;
	bool operator<(ARBDate const& rhs) const;
	bool operator>(ARBDate const& rhs) const;
	bool operator<=(ARBDate const& rhs) const;
	bool operator>=(ARBDate const& rhs) const;
	long operator-(ARBDate const& rhs) const;
	ARBDate& operator++(); //prefix
	ARBDate operator++(int); //postfix
	ARBDate& operator--(); //prefix
	ARBDate operator--(int); //postfix
	ARBDate& operator+=(int inD);
	ARBDate& operator-=(int inD);

	/**
	 * Test if date is between two dates (inclusive)
	 */
	bool isBetween(ARBDate const& inDate1, ARBDate const& inDate2) const;

	bool isLeap() const; ///< Is this a leap year?

	/**
	 * Return the date as a string.
	 * @param inFormat Format of date string.
	 * @return Date in the format defined by inFormat.
	 */
	std::string GetString(DateFormat inFormat) const;

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
	void GetDate(int& outYr, int& outMon, int& outDay) const;

	int GetDay() const;		///< Get the current day.
	int GetMonth() const;	///< Get the current month.
	int GetYear() const;	///< Get the current year.

	/**
	 * Days of the week
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
	int GetDayOfWeek(DayOfWeek inFirstDay = eSunday) const;

private:
	long m_Julian;	///< Julian day, not Julian date.
};

inline ARBDate::~ARBDate()
{
}

inline bool ARBDate::IsValid() const
{
	return (0 < m_Julian);
}

inline void ARBDate::clear()
{
	m_Julian = 0;
}

inline long ARBDate::GetJulianDay() const
{
	return m_Julian;
}

inline ARBDate& ARBDate::operator=(ARBDate const& rhs)
{
	if (this != &rhs)
	{
		m_Julian = rhs.m_Julian;
	}
	return *this;
}

inline bool ARBDate::operator==(ARBDate const& rhs) const
{
	return m_Julian == rhs.m_Julian;
}

inline bool ARBDate::operator!=(ARBDate const& rhs) const
{
	return m_Julian != rhs.m_Julian;
}

inline bool ARBDate::operator<(ARBDate const& rhs) const
{
	return m_Julian < rhs.m_Julian;
}

inline bool ARBDate::operator>(ARBDate const& rhs) const
{
	return m_Julian > rhs.m_Julian;
}

inline bool ARBDate::operator<=(ARBDate const& rhs) const
{
	return m_Julian <= rhs.m_Julian;
}

inline bool ARBDate::operator>=(ARBDate const& rhs) const
{
	return m_Julian >= rhs.m_Julian;
}

inline long ARBDate::operator-(ARBDate const& rhs) const
{
	return m_Julian - rhs.m_Julian;
}

inline ARBDate& ARBDate::operator++()
{
	++m_Julian;
	return *this;
}

inline ARBDate ARBDate::operator++(int)
{
	ARBDate tmp(*this);
	++m_Julian;
	return tmp;
}

inline ARBDate& ARBDate::operator--()
{
	--m_Julian;
	return *this;
}

inline ARBDate ARBDate::operator--(int)
{
	ARBDate tmp(*this);
	--m_Julian;
	return tmp;
}

inline ARBDate& ARBDate::operator+=(int inD)
{
	m_Julian += inD;
	return *this;
}

inline ARBDate& ARBDate::operator-=(int inD)
{
	m_Julian -= inD;
	return *this;
}

inline bool ARBDate::isBetween(ARBDate const& inDate1, ARBDate const& inDate2) const
{
	return m_Julian >= inDate1.m_Julian && m_Julian <= inDate2.m_Julian;
}

inline bool ARBDate::isLeap() const
{
	int yr = GetYear();
	return (yr & 3) == 0 && yr % 100 != 0 || yr % 400 == 0;
}
