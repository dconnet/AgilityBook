#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief The calendar cribsheet.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-31 DRC Added FindCalendar.
 */

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBCalendar : public ARBBase
{
public:
	typedef enum
	{
		eNot,
		eEntered,
		ePlanning
	} eEntry;

	ARBCalendar();
	ARBCalendar(const ARBCalendar& rhs);
	ARBCalendar& operator=(const ARBCalendar& rhs);
	bool operator==(const ARBCalendar& rhs) const;
	bool operator!=(const ARBCalendar& rhs) const;
	bool operator<(const ARBCalendar& rhs) const;
	bool operator>(const ARBCalendar& rhs) const;
	bool operator<(const ARBDate& rhs) const;
	bool operator>(const ARBDate& rhs) const;

	virtual std::string GetGenericName() const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	/**
	 * Is this calendar entry (start and end dates) before a given date?
	 * @param inDate Date to check entry against.
	 * @retval true inDate is before both entry start and end dates.
	 * @retval false inDate is after one or both the start and end dates.
	 */
	bool IsBefore(const ARBDate& inDate) const;
	/**
	 * Is the given date in the range of the calendar entry?
	 * @param inDate Date to check.
	 * @retval true inDate is on one of the dates in the entry range.
	 * @retval false inDate is outside the calendar entry range.
	 */
	bool InRange(const ARBDate& inDate) const;
	/**
	 * Does the range of inDate1 and inDate2 overlap this calendar entry?
	 * @param inDate1 Beginning of date range.
	 * @param inDate2 End of date range.
	 * @retval true There is an overlap between the entry and the given range.
	 * @retval false There is no overlap
	 */
	bool IsRangeOverlapped(
		const ARBDate& inDate1,
		const ARBDate& inDate2) const;

	/**
	 * Trial start date
	 */
	const ARBDate& GetStartDate() const;
	void SetStartDate(const ARBDate& inDate);
	/**
	 * Trial end date
	 */
	const ARBDate& GetEndDate() const;
	void SetEndDate(const ARBDate& inDate);
	/**
	 * Date trial entries are accepted.
	 */
	const ARBDate& GetOpeningDate() const;
	void SetOpeningDate(const ARBDate& inDate);
	/**
	 * Date trial entries are no longer accepted.
	 */
	const ARBDate& GetClosingDate() const;
	void SetClosingDate(const ARBDate& inDate);
	/**
	 * Is the trial date tentative?
	 */
	bool IsTentative() const;
	void SetIsTentative(bool inTentative);
	/**
	 * Location of trial.
	 */
	const std::string& GetLocation() const;
	void SetLocation(const std::string& inLocation);
	/**
	 * Name of club hosting trial.
	 */
	const std::string& GetClub() const;
	void SetClub(const std::string& inClub);
	/**
	 * Trial venue. Note, this does not have to be a configured venue.
	 * However, in order to create a trial entry from it, the venue name must
	 * be in the configuration.
	 */
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);
	/**
	 * Am I planning on entering the trial?
	 */
	eEntry GetEntered() const;
	void SetEntered(eEntry inEnter);
	/**
	 * A note...
	 */
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);

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

inline bool ARBCalendar::operator<(const ARBCalendar& rhs) const
{
	return m_DateStart < rhs.GetStartDate();
}

inline bool ARBCalendar::operator>(const ARBCalendar& rhs) const
{
	return m_DateEnd > rhs.GetEndDate();
}

inline bool ARBCalendar::operator<(const ARBDate& rhs) const
{
	return m_DateStart < rhs;
}

inline bool ARBCalendar::operator>(const ARBDate& rhs) const
{
	return m_DateEnd > rhs;
}

inline const ARBDate& ARBCalendar::GetStartDate() const
{
	return m_DateStart;
}

inline void ARBCalendar::SetStartDate(const ARBDate& inDate)
{
	m_DateStart = inDate;
}

inline const ARBDate& ARBCalendar::GetEndDate() const
{
	return m_DateEnd;
}

inline void ARBCalendar::SetEndDate(const ARBDate& inDate)
{
	m_DateEnd = inDate;
}

inline const ARBDate& ARBCalendar::GetOpeningDate() const
{
	return m_DateOpening;
}

inline void ARBCalendar::SetOpeningDate(const ARBDate& inDate)
{
	m_DateOpening = inDate;
}

inline const ARBDate& ARBCalendar::GetClosingDate() const
{
	return m_DateClosing;
}

inline void ARBCalendar::SetClosingDate(const ARBDate& inDate)
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

inline const std::string& ARBCalendar::GetLocation() const
{
	return m_Location;
}

inline void ARBCalendar::SetLocation(const std::string& inLocation)
{
	m_Location = inLocation;
}

inline const std::string& ARBCalendar::GetClub() const
{
	return m_Club;
}

inline void ARBCalendar::SetClub(const std::string& inClub)
{
	m_Club = inClub;
}

inline const std::string& ARBCalendar::GetVenue() const
{
	return m_Venue;
}

inline void ARBCalendar::SetVenue(const std::string& inVenue)
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

inline const std::string& ARBCalendar::GetNote() const
{
	return m_Note;
}

inline void ARBCalendar::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

class ARBCalendarList : public ARBVectorLoad1<ARBCalendar>
{
public:
	bool operator==(const ARBCalendarList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBCalendarList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending = true);

	size_t GetAllEntered(std::vector<const ARBCalendar*>& ioEntered) const;
	int TrimEntries(const ARBDate& inDate);

	ARBCalendar* FindCalendar(ARBCalendar* inCal);

	ARBCalendar* AddCalendar(ARBCalendar* inCal);
	bool DeleteCalendar(const ARBCalendar* inCal);
};
