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
 * @brief Date class
 * @author David Connet
 *
 * Revision History
 */

#include "StdAfx.h"
#include "ARBDate.h"
#include <time.h>

#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The following public domain code was found on the internet (calendar-1.11)
// SDN stands for serial day number. The author notes it is called this to
// differentiate the julian day (SDN) from a Julian calendar date.
// It has been modified only slightly.

// SdnToGregorian and GregorianToSdn
/* $selId: gregor.c,v 2.0 1995/10/24 01:13:06 lees Exp $
 * Copyright 1993-1995, Scott E. Lee, all rights reserved.
 * Permission granted to use, copy, modify, distribute and sell so long as
 * the above copyright and this permission statement are retained in all
 * copies.  THERE IS NO WARRANTY - USE AT YOUR OWN RISK.
 */

// VALID RANGE
//     4714 B.C. to at least 10000 A.D.
// REFERENCES
//     Conversions Between Calendar Date and Julian Day Number by Robert J.
//     Tantzen, Communications of the Association for Computing Machinery
//     August 1963.  (Also published in Collected Algorithms from CACM,
//     algorithm number 199).

#define SDN_OFFSET			32045
#define DAYS_PER_5_MONTHS	153
#define DAYS_PER_4_YEARS	1461
#define DAYS_PER_400_YEARS	146097

static void SdnToGregorian(long int sdn, int *pYear, int *pMonth, int *pDay)
{
	if (sdn <= 0)
	{
		*pYear = 0;
		*pMonth = 0;
		*pDay = 0;
		return;
	}

	long int temp = (sdn + SDN_OFFSET) * 4 - 1;

	/* Calculate the century (year/100). */
	int century = temp / DAYS_PER_400_YEARS;

	/* Calculate the year and day of year (1 <= dayOfYear <= 366). */
	temp = ((temp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
	int year = (century * 100) + (temp / DAYS_PER_4_YEARS);
	int dayOfYear = (temp % DAYS_PER_4_YEARS) / 4 + 1;

	/* Calculate the month and day of month. */
	temp = dayOfYear * 5 - 3;
	int month = temp / DAYS_PER_5_MONTHS;
	int day = (temp % DAYS_PER_5_MONTHS) / 5 + 1;

	/* Convert to the normal beginning of the year. */
	if (month < 10)
		month += 3;
	else
	{
		year += 1;
		month -= 9;
	}

	/* Adjust to the B.C./A.D. type numbering. */
	year -= 4800;
	if (year <= 0)
		year--;

	*pYear = year;
	*pMonth = month;
	*pDay = day;
}

static long int GregorianToSdn(int inputYear, int inputMonth, int inputDay)
{
	int year;
	int month;

	/* check for invalid dates */
	if (inputYear == 0 || inputYear < -4714
	|| inputMonth <= 0 || inputMonth > 12
	|| inputDay <= 0 || inputDay > 31)
	{
		return 0;
	}

	/* check for dates before SDN 1 (Nov 25, 4714 B.C.) */
	if (inputYear == -4714)
	{
		if (inputMonth < 11)
			return 0;
		if (inputMonth == 11 && inputDay < 25)
			return 0;
	}

	/* Make year always a positive number. */
	if (inputYear < 0)
		year = inputYear + 4801;
	else
		year = inputYear + 4800;

	/* Adjust the start of the year. */
	if (inputMonth > 2)
		month = inputMonth - 3;
	else
	{
		month = inputMonth + 9;
		year--;
	}

	return ( ((year / 100) * DAYS_PER_400_YEARS) / 4
		+ ((year % 100) * DAYS_PER_4_YEARS) / 4
		+ (month * DAYS_PER_5_MONTHS + 2) / 5
		+ inputDay
		- SDN_OFFSET );
}

/////////////////////////////////////////////////////////////////////////////

// static
ARBDate ARBDate::Today()
{
	ARBDate today;
	today.SetToday();
	return today;
}

ARBDate::ARBDate()
	: m_Julian(0)
{
}

ARBDate::ARBDate(const ARBDate& rhs)
	: m_Julian(rhs.m_Julian)
{
}

ARBDate::ARBDate(time_t inTime)
	: m_Julian(0)
{
	if (0 != inTime)
	{
		struct tm* pTime = localtime(&inTime);
		m_Julian = GregorianToSdn(
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday);
	}
}

#if _WIN32 && _MSC_VER >= 1300
ARBDate::ARBDate(__time64_t inTime)
	: m_Julian(0)
{
	if (0 != inTime)
	{
		struct tm* pTime = _localtime64(&inTime);
		m_Julian = GregorianToSdn(
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday);
	}
}
#endif

ARBDate::ARBDate(int inYr, int inMon, int inDay)
	: m_Julian(0)
{
	SetDate(inYr, inMon, inDay);
}

void ARBDate::SetJulianDay(long inJulian)
{
	if (0 < inJulian)
		m_Julian = inJulian;
}

void ARBDate::SetDate(int inYr, int inMon, int inDay)
{
	m_Julian = GregorianToSdn(inYr, inMon, inDay);
	int yr, mon, day;
	SdnToGregorian(m_Julian, &yr, &mon, &day);
	if (yr != inYr || mon != inMon || day != inDay)
		m_Julian = 0;
}

void ARBDate::SetToday()
{
	time_t t;
	time(&t);
	struct tm* pTime = localtime(&t);
	m_Julian = GregorianToSdn(
		pTime->tm_year + 1900,
		pTime->tm_mon + 1,
		pTime->tm_mday);
}

std::string ARBDate::GetString(bool inLeadingZeros, bool inYearFirst) const
{
	char buffer[100];
	if (IsValid())
	{
		int yr, mon, day;
		SdnToGregorian(m_Julian, &yr, &mon, &day);
		if (inYearFirst)
		{
			if (inLeadingZeros)
				sprintf(buffer, "%04d-%02d-%02d", yr, mon, day);
			else
				sprintf(buffer, "%d-%d-%d", yr, mon, day);
		}
		else
		{
			if (inLeadingZeros)
				sprintf(buffer, "%02d/%02d/%04d", mon, day, yr);
			else
				sprintf(buffer, "%d/%d/%d", mon, day, yr);
		}
	}
	else
		buffer[0] = NULL;
	return std::string(buffer);
}

time_t ARBDate::GetDate() const
{
	time_t t = 0;
	if (0 < m_Julian)
	{
		int yr, mon, day;
		SdnToGregorian(m_Julian, &yr, &mon, &day);
		struct tm tim;
		tim.tm_sec = 0;
		tim.tm_min = 0;
		tim.tm_hour = 0;
		tim.tm_mday = day;
		tim.tm_mon = mon - 1;
		tim.tm_year = yr - 1900;
		tim.tm_wday = 0;
		tim.tm_yday = 0;
		tim.tm_isdst = 0;
		t = mktime(&tim);
	}
	return t;
}

void ARBDate::GetDate(int& outYr, int& outMon, int& outDay)
{
	SdnToGregorian(m_Julian, &outYr, &outMon, &outDay);
}

int ARBDate::GetDay() const
{
	int yr, mon, day;
	SdnToGregorian(m_Julian, &yr, &mon, &day);
	return day;
}

int ARBDate::GetMonth() const
{
	int yr, mon, day;
	SdnToGregorian(m_Julian, &yr, &mon, &day);
	return mon;
}

int ARBDate::GetYear() const
{
	int yr, mon, day;
	SdnToGregorian(m_Julian, &yr, &mon, &day);
	return yr;
}

int ARBDate::GetDayOfWeek(DayOfWeek inFirstDay) const
{
	// This was copied from another source, but I don't remember where...
	// I suspect it won't work properly on dates before 1752 (start of
	// Gregorian calendar).
    //return int((((m_Julian+1)%7)+6)%7); // Mon = 0
    return static_cast<int>((((m_Julian+1)%7)+(7-static_cast<int>(inFirstDay)))%7);
}
