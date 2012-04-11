/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Date class
 * @author David Connet
 *
 * Revision History
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2011-12-30 DRC Added eVerbose to GetString.
 * @li 2010-09-06 DRC Fix a bug when adjusting for DST.
 * @li 2010-07-17 DRC When returning time_t, adjust for DST.
 * @li 2009-10-30 DRC Add support for localized dates.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-13 DRC Added inForceOutput to GetString.
 * @li 2005-07-05 DRC Added a new formatting option.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Added FromString().
 */

#include "stdafx.h"
#include "ARBDate.h"
#include <sstream>
#include <time.h>

#include "ARBString.h"
#include <wx/datetime.h>
#include <wx/intl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
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

static void SdnToGregorian(
		long int sdn,
		int* pYear,
		int* pMonth,
		int* pDay)
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


static long int GregorianToSdn(
		int inputYear,
		int inputMonth,
		int inputDay)
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


static int ParseFields(
		std::wstring inDate,
		char sep,
		unsigned short& val1,
		unsigned short& val2,
		unsigned short& val3)
{
	int nVals = 0;
	std::wstring::size_type pos = inDate.find(sep);
	if (std::wstring::npos != pos)
	{
		val1 = static_cast<unsigned short>(StringUtil::ToCLong(inDate));
		++nVals;
		inDate = inDate.substr(pos+1);
		pos = inDate.find(sep);
		if (std::wstring::npos != pos)
		{
			val2 = static_cast<unsigned short>(StringUtil::ToCLong(inDate));
			++nVals;
			inDate = inDate.substr(pos+1);
			val3 = static_cast<unsigned short>(StringUtil::ToCLong(inDate));
			++nVals;
		}
	}
	return nVals;
}


// static
ARBDate ARBDate::FromString(
		std::wstring const& inDate,
		ARBDate::DateFormat inFormat)
{
	ARBDate date;
	if (eLocale == inFormat || eCurrentLocale == inFormat)
	{
		wxLocale* locale = NULL;
		if (eLocale == inFormat)
			locale = new wxLocale(wxLANGUAGE_DEFAULT, 0);
		wxDateTime dt;
		if (dt.ParseDate(inDate.c_str()))
			date.SetDate(dt.GetYear(), static_cast<int>(dt.GetMonth())+1, dt.GetDay());
		delete locale;
	}
	else if (0 < inDate.length())
	{
		unsigned short val1 = 0, val2 = 0, val3 = 0;
		int nDash = ParseFields(inDate, '-', val1, val2, val3);
		int nSlash = ParseFields(inDate, '/', val1, val2, val3);
		int yr = 0, mon = 0, day = 0;
		if ((3 == nDash &&
		(eDashMMDDYYYY == inFormat || eDashYYYYMMDD == inFormat || eDashDDMMYYYY == inFormat
			|| eDashMDY == inFormat || eDashYMD == inFormat || eDashDMY == inFormat))
		|| (3 == nSlash &&
		(eSlashMMDDYYYY == inFormat || eSlashYYYYMMDD == inFormat || eSlashDDMMYYYY == inFormat
			|| eSlashMDY == inFormat || eSlashYMD == inFormat || eSlashDMY == inFormat)))
		{
			switch (inFormat)
			{
			case eDashMMDDYYYY:
			case eDashMDY:
			case eSlashMMDDYYYY:
			case eSlashMDY:
				mon = val1;
				day = val2;
				yr = val3;
				break;
			case eDashYYYYMMDD: // eISO
			case eDashYMD:
			case eSlashYYYYMMDD:
			case eSlashYMD:
				yr = val1;
				mon = val2;
				day = val3;
				break;
			case eDashDDMMYYYY:
			case eDashDMY:
			case eSlashDDMMYYYY:
			case eSlashDMY:
				day = val1;
				mon = val2;
				yr = val3;
				break;
			}
			date.SetDate(yr, mon, day);
		}
		if (date.IsValid())
		{
			int yr2, mon2, day2;
			date.GetDate(yr2, mon2, day2);
			if (yr != yr2 || mon != mon2 || day != day2)
				date.clear();
		}
	}
	return date;
}


// static
std::wstring ARBDate::GetValidDateString(
		ARBDate const& inFrom,
		ARBDate const& inTo,
		DateFormat inFormat)
{
	std::wstring str;
	if (inFrom.IsValid() || inTo.IsValid())
	{
		str += wxT("[");
		if (inFrom.IsValid())
			str += inFrom.GetString(inFormat);
		else
			str += wxT("*");
		str += wxT("-");
		if (inTo.IsValid())
			str += inTo.GetString(inFormat);
		else
			str += wxT("*");
		str += wxT("]");
	}
	return str;
}


ARBDate::ARBDate(time_t inTime)
	: m_Julian(0)
{
	if (0 != inTime)
	{
#if defined(ARB_HAS_SECURE_LOCALTIME)
		struct tm l;
		_localtime64_s(&l, &inTime);
		struct tm* pTime = &l;
#else
		struct tm* pTime = localtime(&inTime);
#endif
		m_Julian = GregorianToSdn(
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday);
	}
}


#if defined(ARB_HAS_32_AND_64_BIT_TIMET)
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


void ARBDate::SetToday()
{
	time_t t;
	time(&t);
#if defined(ARB_HAS_SECURE_LOCALTIME)
	struct tm l;
	_localtime64_s(&l, &t);
	struct tm* pTime = &l;
#else
	struct tm* pTime = localtime(&t);
#endif
	m_Julian = GregorianToSdn(
		pTime->tm_year + 1900,
		pTime->tm_mon + 1,
		pTime->tm_mday);
}


bool ARBDate::SetDate(
		int inYr,
		int inMon,
		int inDay,
		bool bClearOnError)
{
	bool bOk = true;
	long julian = GregorianToSdn(inYr, inMon, inDay);
	int yr, mon, day;
	SdnToGregorian(julian, &yr, &mon, &day);
	if (yr != inYr || mon != inMon || day != inDay)
	{
		bOk = false;
		if (bClearOnError)
			m_Julian = 0;
	}
	else
		m_Julian = julian;
	return bOk;
}


std::wstring ARBDate::GetString(
		DateFormat inFormat,
		bool inForceOutput) const
{
	if (!inForceOutput && !IsValid())
		return wxT("");
	std::wstring date;
	int yr = 0;
	int mon = 0;
	int day = 0;
	if (IsValid())
		SdnToGregorian(m_Julian, &yr, &mon, &day);
	switch (inFormat)
	{
	case eLocale:
		{
			wxLocale locale(wxLANGUAGE_DEFAULT, 0);
			wxDateTime dt(static_cast<wxDateTime::wxDateTime_t>(day), static_cast<wxDateTime::Month>(mon-1), yr);
			date = dt.FormatDate();
		}
		break;
	case eCurrentLocale:
		{
			wxDateTime dt(static_cast<wxDateTime::wxDateTime_t>(day), static_cast<wxDateTime::Month>(mon-1), yr);
			date = dt.FormatDate();
		}
		break;
	case eDashMMDDYYYY:		///< MM-DD-YYYY
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(2);
			buf << mon << L'-';
			buf.width(2);
			buf << day << L'-';
			buf.width(4);
			buf << yr;
			date = buf.str();
		}
		break;
	case eYYYYMMDD:
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(4);
			buf << yr;
			buf.width(2);
			buf << mon;
			buf.width(2);
			buf << day;
			date = buf.str();
		}
		break;
	case eVerbose:
		{
			wxDateTime dt(static_cast<wxDateTime::wxDateTime_t>(day), static_cast<wxDateTime::Month>(mon-1), yr);
			date = dt.Format(wxT("%A, %B %d, %Y")).wx_str();
		}
		break;
	default:				///< YYYY-MM-DD or MM/DD/YYYY
	case eSlashMMDDYYYY:	///< MM/DD/YYYY
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(2);
			buf << mon << L'/';
			buf.width(2);
			buf << day << L'/';
			buf.width(4);
			buf << yr;
			date = buf.str();
		}
		break;
	case eDashYYYYMMDD:		///< YYYY-MM-DD
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(4);
			buf << yr << L'-';
			buf.width(2);
			buf << mon << L'-';
			buf.width(2);
			buf << day;
			date = buf.str();
		}
		break;
	case eSlashYYYYMMDD:	///< YYYY/MM/DD
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(4);
			buf << yr << L'/';
			buf.width(2);
			buf << mon << L'/';
			buf.width(2);
			buf << day;
			date = buf.str();
		}
		break;
	case eDashDDMMYYYY:		///< DD-MM-YYYY
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(2);
			buf << day << L'-';
			buf.width(2);
			buf << mon << L'-';
			buf.width(4);
			buf << yr;
			date = buf.str();
		}
		break;
	case eSlashDDMMYYYY:	///< DD/MM/YYYY
		{
			std::wostringstream buf;
			buf.fill(L'0');
			buf.width(2);
			buf << day << L'/';
			buf.width(2);
			buf << mon << L'/';
			buf.width(4);
			buf << yr;
			date = buf.str();
		}
		break;
	case eDashMDY:	///< M-D-Y
		{
			std::wostringstream buf;
			buf << mon << L'-' << day << L'-' << yr;
			date = buf.str();
		}
		break;
	case eSlashMDY:	///< M/D/Y
		{
			std::wostringstream buf;
			buf << mon << L'/' << day << L'/' << yr;
			date = buf.str();
		}
		break;
	case eDashYMD:	///< Y-M-D
		{
			std::wostringstream buf;
			buf << yr << L'-' << mon << L'-' << day;
			date = buf.str();
		}
		break;
	case eSlashYMD:	///< Y/M/D
		{
			std::wostringstream buf;
			buf << yr << L'/' << mon << L'/' << day;
			date = buf.str();
		}
		break;
	case eDashDMY:	///< D-M-Y
		{
			std::wostringstream buf;
			buf << day << L'-' << mon << L'-' << yr;
			date = buf.str();
		}
		break;
	case eSlashDMY:	///< D/M/Y
		{
			std::wostringstream buf;
			buf << day << L'/' << mon << L'/' << yr;
			date = buf.str();
		}
		break;
	}
	return date;
}


time_t ARBDate::GetDate() const
{
	time_t t = 0;
	if (0 < m_Julian)
	{
		int yr, mon, day;
		SdnToGregorian(m_Julian, &yr, &mon, &day);
		struct tm tim;
		// This initializes tm_isdst properly.
		time_t inTime = time(NULL);
#if defined(ARB_HAS_SECURE_LOCALTIME)
		_localtime64_s(&tim, &inTime);
#else
		tim = *localtime(&inTime);
#endif
		int dst = tim.tm_isdst;
		tim.tm_sec = 0;
		tim.tm_min = 0;
		tim.tm_hour = 0;
		tim.tm_mday = day;
		tim.tm_mon = mon - 1;
		tim.tm_year = yr - 1900;
		tim.tm_wday = 0;
		tim.tm_yday = 0;
		t = mktime(&tim);
		// If DST changes, recompute - otherwise we can end up 1 day early.
		if (dst != tim.tm_isdst)
		{
			tim.tm_sec = 0;
			tim.tm_min = 0;
			tim.tm_hour = 0;
			tim.tm_mday = day;
			tim.tm_mon = mon - 1;
			tim.tm_year = yr - 1900;
			tim.tm_wday = 0;
			tim.tm_yday = 0;
			t = mktime(&tim);
		}
	}
	return t;
}


#ifdef _WIN32
bool ARBDate::GetDate(SYSTEMTIME& outTime) const
{
	if (0 < m_Julian)
	{
		int yr, mon, day;
		SdnToGregorian(m_Julian, &yr, &mon, &day);
		outTime.wYear = static_cast<WORD>(yr);
		outTime.wMonth = static_cast<WORD>(mon);
		outTime.wDayOfWeek = static_cast<WORD>(GetDayOfWeek());
		outTime.wDay = static_cast<WORD>(day);
		outTime.wHour = 0;
		outTime.wMinute = 0;
		outTime.wSecond = 0;
		outTime.wMilliseconds = 0;
		return true;
	}
	return false;
}
#endif


void ARBDate::GetDate(
		int& outYr,
		int& outMon,
		int& outDay) const
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


int ARBDate::GetDayOfYear() const
{
	ARBDate d(GetYear(), 1, 1);
	return *this - d + 1;
}
