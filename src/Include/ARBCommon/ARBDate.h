#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDate class.
 * @author David Connet
 *
 * Revision History
 * 2012-10-26 Changed ARBDate::GetTime to avoid time_t when possible.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-10-30 Add support for localized dates.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-25 Make time_t ctors explicit to prevent casts from bools.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-13 Added inForceOutput to GetString.
 * 2005-07-05 Added a new formatting option.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-03-21 Fixed up some operators.
 * 2004-01-04 Added FromString().
 */

#include "ARBTypes.h"
#include "LibwxARBCommon.h"

#if defined(__WXWINDOWS__)
#include <wx/datetime.h>
#endif


/**
 * Output date format (do not change the values!)
 */
enum class ARBDateFormat
{
	Locale = 0,        ///< System (current) locale (output only)
	DashMMDDYYYY = 1,  ///< MM-DD-YYYY
	SlashMMDDYYYY = 2, ///< MM/DD/YYYY
	DashYYYYMMDD = 3,  ///< YYYY-MM-DD, ISO 8601
	ISO = 3,
	SlashYYYYMMDD = 4, ///< YYYY/MM/DD
	DashDDMMYYYY = 5,  ///< DD-MM-YYYY
	SlashDDMMYYYY = 6, ///< DD/MM/YYYY
	DashMDY = 7,       ///< M-D-Y
	SlashMDY = 8,      ///< M/D/Y
	DashYMD = 9,       ///< Y-M-D
	SlashYMD = 10,     ///< Y/M/D
	DashDMY = 11,      ///< D-M-Y
	SlashDMY = 12,     ///< D/M/Y
	YYYYMMDD = 13,     ///< YYYYMMDD (output only)
	Reserved14 = 14,   ///< was eCurrentLocale
	Verbose = 15,      ///< "%A, %B %d, %Y" (output only)
};

/**
 * Days of the week (do not change the values!)
 */
enum class ARBDayOfWeek
{
	Sunday = 0,    ///< Sunday
	Monday = 1,    ///< Monday
	Tuesday = 2,   ///< Tuesday
	Wednesday = 3, ///< Wednesday
	Thursday = 4,  ///< Thursday
	Friday = 5,    ///< Friday
	Saturday = 6,  ///< Saturday
};


/**
 * Date class.
 * In XML/Element, this class is only used as an attribute, never an element.
 */
class ARBCOMMON_API ARBDate
{
public:
	/**
	 * Convert a string to a date
	 * @param inDate String to convert
	 * @param inFormat Parse using this format
	 * @return Parsed date, if parse fails, date is invalid.
	 */
	static ARBDate FromString(std::wstring const& inDate, ARBDateFormat inFormat);

	/**
	 * Get a string showing the valid date range (if set)
	 * @param inFrom Valid from date.
	 * @param inTo Valid to date.
	 * @param inFormat Date format.
	 * @return Date range string
	 */
	static std::wstring GetValidDateString(
		ARBDate const& inFrom,
		ARBDate const& inTo,
		ARBDateFormat inFormat = ARBDateFormat::DashYMD);

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
	ARBDate(ARBDate&& rhs)
		: m_Julian(std::move(rhs.m_Julian))
	{
	}
	explicit ARBDate(time_t inTime);
#if defined(__WXWINDOWS__)
	ARBDate(wxDateTime const& inTime)
	{
		ARBDate date(inTime.GetYear(), inTime.GetMonth() + 1, inTime.GetDay());
		*this = date;
	}
#endif
	ARBDate(int inYr, int inMon, int inDay)
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
	bool SetDate(int inYr, int inMon, int inDay, bool bClearOnError = true);

	ARBDate& operator=(ARBDate const& rhs)
	{
		if (this != &rhs)
		{
			m_Julian = rhs.m_Julian;
		}
		return *this;
	}
	ARBDate& operator=(ARBDate&& rhs)
	{
		if (this != &rhs)
		{
			m_Julian = std::move(rhs.m_Julian);
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
	ARBDate& operator++() // prefix
	{
		++m_Julian;
		return *this;
	}
	ARBDate operator++(int) // postfix
	{
		ARBDate tmp(*this);
		++m_Julian;
		return tmp;
	}
	ARBDate& operator--() // prefix
	{
		--m_Julian;
		return *this;
	}
	ARBDate operator--(int) // postfix
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
	ARBDate operator+(int inD) const
	{
		ARBDate d;
		d.SetJulianDay(m_Julian + inD);
		return d;
	}
	ARBDate operator-(int inD) const
	{
		ARBDate d;
		d.SetJulianDay(m_Julian - inD);
		return d;
	}

	/**
	 * Test if date is between two dates (inclusive)
	 */
	bool isBetween(ARBDate const& inDate1, ARBDate const& inDate2) const
	{
		if (inDate1 > inDate2)
			return isBetween(inDate2, inDate1);
		return m_Julian >= inDate1.m_Julian && m_Julian <= inDate2.m_Julian;
	}

	bool isLeap() const ///< Is this a leap year?
	{
		int yr = GetYear();
		return ((yr & 3) == 0 && yr % 100 != 0) || yr % 400 == 0;
	}

	/**
	 * Return the date as a string.
	 * @param inFormat Format of date string.
	 * @param inForceOutput Always return a string, even if the date is not valid.
	 * @return Date in the format defined by inFormat.
	 */
	std::wstring GetString(ARBDateFormat inFormat = ARBDateFormat::Locale, bool inForceOutput = false) const;

	/**
	 * Get the current date.
	 * @param outYr Current year.
	 * @param outMon Current month.
	 * @param outDay Current day.
	 */
	void GetDate(int& outYr, int& outMon, int& outDay) const;

	/**
	 * Convert the date to a time.
	 * Note, this will return an invalid time_t (-1) if the date falls
	 * outside the range of a time_t (mktime()).
	 * It is best to avoid using this function.
	 * @return Date converted to time_t.
	 */
	bool GetDate(time_t& outTime) const;

#ifdef _WIN32
	/**
	 * Convert the date to a time.
	 */
	bool GetDate(SYSTEMTIME& outTime) const;
#endif

#if defined(__WXWINDOWS__)
	/**
	 * Get the current date.
	 */
	bool GetDate(wxDateTime& outDate) const;
#endif

	int GetDay() const;   ///< Get the current day.
	int GetMonth() const; ///< Get the current month.
	int GetYear() const;  ///< Get the current year.

	/**
	 * Day of year (1-366)
	 */
	int GetDayOfYear() const;

	/**
	 * Get the day of the week of the current date.
	 * @param inFirstDay Define what day of week has index 0 (1st day of week).
	 */
	int GetDayOfWeek(ARBDayOfWeek inFirstDay = ARBDayOfWeek::Sunday) const
	{
		// This was copied from another source, but I don't remember where...
		// I suspect it won't work properly on dates before 1752 (start of
		// Gregorian calendar).
		// return int((((m_Julian+1)%7)+6)%7); // Mon = 0
		return static_cast<int>((((m_Julian + 1) % 7) + (7 - static_cast<int>(inFirstDay))) % 7);
	}

private:
	long m_Julian; ///< Julian day, not Julian date.
};
