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
 * @li 2003-07-09 DRC IsRangeOverlapped test was wrong. It was actually a 'is
 *                    range contained in' test.
 * @li 2003-01-23 DRC DocVer 2: Replaced 'PlanOn' attribute and with 'Entered'.
 */

#include "StdAfx.h"
#include "ARBCalendar.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBCalendar::ARBCalendar()
	: m_DateStart()
	, m_DateEnd()
	, m_DateOpening()
	, m_DateClosing()
	, m_bTentative(false)
	, m_Location()
	, m_Club()
	, m_Venue()
	, m_eEntered(eNot)
	, m_Note()
{
}

ARBCalendar::ARBCalendar(const ARBCalendar& rhs)
	: m_DateStart(rhs.m_DateStart)
	, m_DateEnd(rhs.m_DateEnd)
	, m_DateOpening(rhs.m_DateOpening)
	, m_DateClosing(rhs.m_DateClosing)
	, m_bTentative(rhs.m_bTentative)
	, m_Location(rhs.m_Location)
	, m_Club(rhs.m_Club)
	, m_Venue(rhs.m_Venue)
	, m_eEntered(rhs.m_eEntered)
	, m_Note(rhs.m_Note)
{
}

ARBCalendar::~ARBCalendar()
{
}

ARBCalendar& ARBCalendar::operator=(const ARBCalendar& rhs)
{
	if (this != &rhs)
	{
		m_DateStart = rhs.m_DateStart;
		m_DateEnd = rhs.m_DateEnd;
		m_DateOpening = rhs.m_DateOpening;
		m_DateClosing = rhs.m_DateClosing;
		m_bTentative = rhs.m_bTentative;
		m_Location = rhs.m_Location;
		m_Club = rhs.m_Club;
		m_Venue = rhs.m_Venue;
		m_eEntered = rhs.m_eEntered;
		m_Note = rhs.m_Note;
	}
	return *this;
}

bool ARBCalendar::operator==(const ARBCalendar& rhs) const
{
	return m_DateStart == rhs.m_DateStart
		&& m_DateEnd == rhs.m_DateEnd
		&& m_DateOpening == rhs.m_DateOpening
		&& m_DateClosing == rhs.m_DateClosing
		&& m_bTentative == rhs.m_bTentative
		&& m_Location == rhs.m_Location
		&& m_Club == rhs.m_Club
		&& m_Venue == rhs.m_Venue
		&& m_eEntered == rhs.m_eEntered
		&& m_Note == rhs.m_Note;
}

bool ARBCalendar::operator!=(const ARBCalendar& rhs) const
{
	return !operator==(rhs);
}

std::string ARBCalendar::GetGenericName() const
{
	std::string name = m_Venue;
	name += " ";
	name += m_Club;
	name += " ";
	name += m_Location;
	return name;
}

bool ARBCalendar::IsBefore(const ARBDate& inDate) const
{
	return (m_DateStart < inDate && m_DateEnd < inDate);
}

bool ARBCalendar::InRange(const ARBDate& inDate) const
{
	return inDate.isBetween(m_DateStart, m_DateEnd);
}

bool ARBCalendar::IsRangeOverlapped(const ARBDate& inDate1, const ARBDate& inDate2) const
{
	if (m_DateStart.isBetween(inDate1, inDate2)
	|| m_DateEnd.isBetween(inDate1, inDate2))
		return true;
	else
		return false;
}

bool ARBCalendar::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	switch (inTree.GetAttrib(ATTRIB_CAL_START, m_DateStart))
	{
	case CElement::eNotFound:
		ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_START);
		return false;
	case CElement::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_CAL_START, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_START, msg.c_str());
		}
		return false;
	}

	switch (inTree.GetAttrib(ATTRIB_CAL_END, m_DateEnd))
	{
	case CElement::eNotFound:
		ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_END);
		return false;
	case CElement::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_CAL_END, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_END, msg.c_str());
			return false;
		}
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_OPENING, m_DateOpening))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_CAL_OPENING, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_OPENING, msg.c_str());
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_CLOSING, m_DateClosing))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_CAL_CLOSING, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_CLOSING, msg.c_str());
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_MAYBE, m_bTentative))
	{
		ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_MAYBE, VALID_VALUES_BOOL);
		return false;
	}

	inTree.GetAttrib(ATTRIB_CAL_LOCATION, m_Location);
	inTree.GetAttrib(ATTRIB_CAL_CLUB, m_Club);
	inTree.GetAttrib(ATTRIB_CAL_VENUE, m_Venue);

	if (inVersion == ARBVersion(1,0))
	{
		std::string attrib;
		if (CElement::eFound == inTree.GetAttrib("PlanOn", attrib))
		{
			if (attrib == "y")
				m_eEntered = ePlanning;
			else
				m_eEntered = eNot;
		}
	}
	else if (inVersion >= ARBVersion(2,0))
	{
		std::string attrib;
		if (CElement::eFound == inTree.GetAttrib(ATTRIB_CAL_ENTERED, attrib))
		{
			if (attrib == "E")
				m_eEntered = eEntered;
			else if (attrib == "P")
				m_eEntered = ePlanning;
			else if (attrib == "N")
				m_eEntered = eNot;
			else
			{
				ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ENTERED, VALID_VALUES_ENTRY);
				return false;
			}
		}
	}

	m_Note = inTree.GetValue();
	return true;
}

bool ARBCalendar::Save(CElement& ioTree) const
{
	CElement& cal = ioTree.AddElement(TREE_CALENDAR);
	cal.AddAttrib(ATTRIB_CAL_START, m_DateStart);
	cal.AddAttrib(ATTRIB_CAL_END, m_DateEnd);
	cal.AddAttrib(ATTRIB_CAL_OPENING, m_DateOpening);
	cal.AddAttrib(ATTRIB_CAL_CLOSING, m_DateClosing);
	cal.AddAttrib(ATTRIB_CAL_MAYBE, m_bTentative);
	cal.AddAttrib(ATTRIB_CAL_LOCATION, m_Location);
	cal.AddAttrib(ATTRIB_CAL_CLUB, m_Club);
	cal.AddAttrib(ATTRIB_CAL_VENUE, m_Venue);
	std::string entered("N");
	switch (m_eEntered)
	{
	default:
		break;
	case eEntered:
		entered = "E";
		break;
	case ePlanning:
		entered = "P";
		break;
	}
	cal.AddAttrib(ATTRIB_CAL_ENTERED, entered);
	if (0 < m_Note.length())
		cal.SetValue(m_Note);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortCalendar
{
public:
	SortCalendar(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBCalendar* one, ARBCalendar* two) const
	{
		if (one->GetStartDate() < two->GetStartDate())
			return m_bDescending;
		else if (one->GetStartDate() > two->GetStartDate())
			return !m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBCalendarList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortCalendar(inDescending));
}

size_t ARBCalendarList::GetAllEntered(std::vector<const ARBCalendar*>& ioEntered) const
{
	ioEntered.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		const ARBCalendar* pCal = (*iter);
		if (ARBCalendar::eEntered == pCal->GetEntered())
			ioEntered.push_back(pCal);
	}
	return ioEntered.size();
}

int ARBCalendarList::TrimEntries(const ARBDate& inDate)
{
	int trimmed = 0;
	if (inDate.IsValid())
	{
		for (iterator iter = begin(); iter != end(); )
		{
			if ((*iter)->IsBefore(inDate))
			{
				++trimmed;
				erase(iter);
				iter = begin();
			}
			else
				++iter;
		}
	}
	return trimmed;
}

ARBCalendar* ARBCalendarList::FindCalendar(ARBCalendar* inCal)
{
	ARBCalendar* pCal = NULL;
	if (inCal)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if (*(*iter) == *inCal)
			{
				pCal = *iter;
				break;
			}
		}
	}
	return pCal;
}

ARBCalendar* ARBCalendarList::AddCalendar(ARBCalendar* inCal)
{
	if (inCal)
	{
		inCal->AddRef();
		push_back(inCal);
	}
	return inCal;
}

bool ARBCalendarList::DeleteCalendar(const ARBCalendar* inCal)
{
	if (inCal)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inCal)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
