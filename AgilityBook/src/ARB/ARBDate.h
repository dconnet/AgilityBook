#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief Utility classes.
 * @author David Connet
 *
 * Revision History
 */

#include <string>
class CElement;

/////////////////////////////////////////////////////////////////////////////
/**
 * In XML/CElement, this class is only used as an attribute, never an element.
 */
class ARBDate
{
public:
	static ARBDate Today();

	ARBDate();
	ARBDate(const ARBDate& rhs);
	ARBDate(time_t inTime);
#if _WIN32 && _MSC_VER >= 1300
	ARBDate(__time64_t inTime);
#endif
	ARBDate(int inYr, int inMon, int inDay);
	~ARBDate();

	bool IsValid() const;
	void clear();
	void SetToday();

	long GetJulianDay() const;
	void SetJulianDay(long inJulian);
	void SetDate(int inYr, int inMon, int inDay);

	ARBDate& operator=(const ARBDate& rhs);
	bool operator==(const ARBDate& rhs) const;
	bool operator!=(const ARBDate& rhs) const;
	bool operator<(const ARBDate& rhs) const;
	bool operator>(const ARBDate& rhs) const;
	bool operator<=(const ARBDate& rhs) const;
	bool operator>=(const ARBDate& rhs) const;
	long operator-(const ARBDate& rhs) const;
	void operator++();
	void operator--();
	void operator+=(int inD);
	void operator-=(int inD);

	bool isBetween(const ARBDate& inDate1, const ARBDate& inDate2) const;
	bool isLeap() const;

	/**
	 * Return the date as a string.
	 * @param bLeadingZeros Add leading 0s to single digit months/days.
	 * @param bYearFirst Form to return date string.
	 * @return Return date as "YYYY-MM-DD" or "MM/DD/YYYY"
	 */
	std::string GetString(bool inLeadingZeros, bool inYearFirst) const;

	/**
	 * Convert the date to a time. Note, this will return an invalid time_t (-1)
	 * if the date falls outside the range of a time_t (mktime()).
	 * @return Date converted to time_t.
	 */
	time_t GetDate() const;
	void GetDate(int& outYr, int& outMon, int& outDay);
	int GetDay() const;
	int GetMonth() const;
	int GetYear() const;
	typedef enum
	{
		eSunday = 0,
		eMonday = 1,
		eTuesday = 2,
		eWednesday = 3,
		eThursday = 4,
		eFriday = 5,
		eSaturday = 6,
	} DayOfWeek;
	/**
	 * @param firstDay Define what day of week has index 0 (1st day of week).
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

inline ARBDate& ARBDate::operator=(const ARBDate& rhs)
{
	if (this != &rhs)
	{
		m_Julian = rhs.m_Julian;
	}
	return *this;
}

inline bool ARBDate::operator==(const ARBDate& rhs) const
{
	return m_Julian == rhs.m_Julian;
}

inline bool ARBDate::operator!=(const ARBDate& rhs) const
{
	return m_Julian != rhs.m_Julian;
}

inline bool ARBDate::operator<(const ARBDate& rhs) const
{
	return m_Julian < rhs.m_Julian;
}

inline bool ARBDate::operator>(const ARBDate& rhs) const
{
	return m_Julian > rhs.m_Julian;
}

inline bool ARBDate::operator<=(const ARBDate& rhs) const
{
	return m_Julian <= rhs.m_Julian;
}

inline bool ARBDate::operator>=(const ARBDate& rhs) const
{
	return m_Julian >= rhs.m_Julian;
}

inline long ARBDate::operator-(const ARBDate& rhs) const
{
	return m_Julian - rhs.m_Julian;
}

inline void ARBDate::operator++()
{
	++m_Julian;
}

inline void ARBDate::operator--()
{
	--m_Julian;
}

inline void ARBDate::operator+=(int inD)
{
	m_Julian += inD;
}

inline void ARBDate::operator-=(int inD)
{
	m_Julian -= inD;
}

inline bool ARBDate::isBetween(const ARBDate& inDate1, const ARBDate& inDate2) const
{
	return m_Julian >= inDate1.m_Julian && m_Julian <= inDate2.m_Julian;
}

inline bool ARBDate::isLeap() const
{
	int yr = GetYear();
	return (yr & 3) == 0 && yr % 100 != 0 || yr % 400 == 0;
}
