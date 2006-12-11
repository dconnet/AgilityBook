/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-13 DRC Added vCalendar support, finished iCalendar.
 * @li 2005-07-05 DRC Added iCalendar support.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
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
#include "ARBTypes.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Static functions (for iCalendar/vCalender)
// See RFC2445.
// Note: EOL sequence is defined as "\r\n"

class ARBiCal : public ICalendar
{
public:
	ARBiCal(
			ARBostream& ioStream,
			int inVersion);
	virtual ~ARBiCal()
	{
		m_ioStream << "END:VCALENDAR\r\n";
	}

	virtual void Release()
	{
		delete this;
	}

	void BeginEvent()
	{
		m_ioStream << "BEGIN:VEVENT\r\n";
	}
#ifdef UNICODE
	void DoUID(std::wstring const& inUID)
	{
		CStringA convert(inUID.c_str());
		DoUID(std::string(convert));
	}
#endif
	void DoUID(std::string const& inUID)
	{
		WriteText("UID", inUID, false);
	}
	void DoDTSTAMP();
	void DoDTSTART(ARBDate inDate)
	{
		Write("DTSTART", inDate, true);
	}
	void DoDTEND(ARBDate inDate)
	{
		// Note, DTEND is the non-inclusive end
		if (1 < m_Version)
			inDate += 1;
		Write("DTEND", inDate, false);
	}
#ifdef UNICODE
	void DoSUMMARY(std::wstring const& inStr)
	{
		CStringA convert(inStr.c_str());
		DoSUMMARY(std::string(convert));
	}
	void DoLOCATION(std::wstring const& inStr)
	{
		CStringA convert(inStr.c_str());
		DoLOCATION(std::string(convert));
	}
	void DoDESCRIPTION(std::wstring const& inStr)
	{
		CStringA convert(inStr.c_str());
		DoDESCRIPTION(std::string(convert));
	}
#endif
	void DoSUMMARY(std::string const& inStr)
	{
		WriteText("SUMMARY", inStr, true);
	}
	void DoLOCATION(std::string const& inStr)
	{
		WriteText("LOCATION", inStr, true);
	}
	void DoDESCRIPTION(std::string const& inStr)
	{
		WriteText("DESCRIPTION", inStr, true);
	}
	void DoAlarm(int inDaysBefore)
	{
		if (1 < m_Version)
		{
			m_ioStream << "BEGIN:VALARM\r\n";
			m_ioStream << "ACTION:DISPLAY\r\n";
			m_ioStream << "TRIGGER:-PT" << inDaysBefore * 24 * 60 << "M\r\n";
			m_ioStream << "DESCRIPTION:Reminder\r\n";
			m_ioStream << "END:VALARM\r\n";
		}
	}
	void EndEvent()
	{
		m_ioStream << "END:VEVENT" << "\r\n";
	}

private:
	void Write(
			char const* const inVal,
			ARBDate inDate,
			bool inStartOfDay);
	void WriteSafeChar(std::string const& inText);
	void WriteText(
			char const* const inToken,
			std::string const& inText,
			bool bQuotedPrint);

	ARBostream& m_ioStream;
	int m_Version;
};

ARBiCal::ARBiCal(
		ARBostream& ioStream,
		int inVersion)
	: m_ioStream(ioStream)
	, m_Version(inVersion)
{
	// All V1.0 syntax was figured out by exporting an entry from Outlook2003.
	m_ioStream << "BEGIN:VCALENDAR\r\n";
	m_ioStream << "PRODID:-//dcon Software//Agility Record Book//EN\r\n";
	switch (m_Version)
	{
	case 1:
		m_ioStream << "VERSION:1.0\r\n";
		break;
	case 2:
		m_ioStream << "VERSION:2.0\r\n";
		// Figured this out thru trial/error with Outlook2003.
		m_ioStream << "METHOD:PUBLISH\r\n";
		break;
	}
}

void ARBiCal::Write(
			char const* const inVal,
			ARBDate inDate,
			bool inStartOfDay)
{
	if (inVal)
	{
		m_ioStream << inVal;
		if (1 < m_Version)
			m_ioStream << ";VALUE=DATE";
		m_ioStream << ':';
	}
#ifdef UNICODE
	CStringA tmp(inDate.GetString(ARBDate::eYYYYMMDD).c_str());
	m_ioStream << tmp;
#else
	m_ioStream << inDate.GetString(ARBDate::eYYYYMMDD);
#endif
	if (1 == m_Version)
	{
		if (inStartOfDay)
			m_ioStream << "T070000";
		else
			m_ioStream << "T180000";
	}
	if (inVal)
		m_ioStream << "\r\n";
}

void ARBiCal::WriteSafeChar(std::string const& inText)
{
	for (std::string::const_iterator iter = inText.begin();
		iter != inText.end();
		++iter)
	{
		char c = *iter;
		// See the RFC...
		if ((0x21 <= c && c <= 0x7e) // ASCII
		|| (0x80 <= c && c <= 0xf8) // NON-US-ASCII
		|| 0x20 == c || 0x09 == c) // WSP
		{
			if (1 == m_Version && '=' == c)
				m_ioStream << "=3D";
			else if (1 < m_Version
			&& (';' == c || ',' == c || '\\' == c))
				m_ioStream << '\\';
			else
				m_ioStream << c;
		}
		else if ('\n' == c)
		{
			if (1 == m_Version)
				m_ioStream << "=0A";
			else
				m_ioStream << "\\n";
		}
		else
			m_ioStream << '?';
	}
}

void ARBiCal::WriteText(
		char const* const inToken,
		std::string const& inText,
		bool bQuotedPrint)
{
	if (0 < inText.length())
	{
		size_t nLineLength = 75 - strlen(inToken);
		m_ioStream << inToken;
		if (1 == m_Version && bQuotedPrint)
		{
			m_ioStream << ";ENCODING=QUOTED-PRINTABLE";
			nLineLength -= 26;
		}
		m_ioStream << ':';
		// "Fold" a long line. RFC 2445, section 4.1
		std::string tmp(inText);
		while (nLineLength < tmp.length())
		{
			// Version 1 stuff is a best-guess.
			WriteSafeChar(tmp.substr(0, nLineLength));
			if (1 == m_Version)
				m_ioStream << "=\r\n";
			else
				m_ioStream << "\r\n\t";
			tmp = tmp.substr(nLineLength);
			nLineLength = 75;
		}
		if (0 < tmp.length())
			WriteSafeChar(tmp);
		m_ioStream << "\r\n";
	}
}

void ARBiCal::DoDTSTAMP()
{
	if (1 < m_Version)
	{
		time_t t;
		time(&t);
#if _MSC_VER < 1400
		struct tm* pTime = localtime(&t);
#else
		struct tm l;
		_localtime64_s(&l, &t);
		struct tm* pTime = &l;
#endif
		ARBostringstream str;
		str.fill(_T('0'));
		str.width(4);
		str << pTime->tm_year + 1900;
		str.width(2);
		str << pTime->tm_mon + 1;
		str.width(2);
		str << pTime->tm_mday << _T('T');
		str.width(2);
		str << pTime->tm_hour;
		str.width(2);
		str << pTime->tm_min;
		str.width(2);
		str << pTime->tm_sec;
		m_ioStream << _T("DTSTAMP:") << str.str() << _T("\r\n");
	}
}

ICalendar::ICalendar()
{
}

ICalendar::~ICalendar()
{
}

ICalendar* ICalendar::iCalendarBegin(
		ARBostream& ioStream,
		int inVersion)
{
	ICalendar* pCal = NULL;
	if (1 == inVersion || 2 == inVersion)
		pCal = new ARBiCal(ioStream, inVersion);
	return pCal;
}

/////////////////////////////////////////////////////////////////////////////

ARBCalendarPtr ARBCalendar::New()
{
	return ARBCalendarPtr(new ARBCalendar());
}

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

ARBCalendar::ARBCalendar(ARBCalendar const& rhs)
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

ARBCalendarPtr ARBCalendar::Clone() const
{
	return ARBCalendarPtr(new ARBCalendar(*this));
}

ARBCalendar& ARBCalendar::operator=(ARBCalendar const& rhs)
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

bool ARBCalendar::operator==(ARBCalendar const& rhs) const
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

ARBString ARBCalendar::GetUID(eUidType inType) const
{
	ARBostringstream str;
	switch (inType)
	{
	default:
		ASSERT(0);
		str << _T("u");
		break;
	case eUIDvEvent:
		str << _T("e");
		break;
	case eUIDvTodo:
		str << _T("t");
		break;
	}
	str << m_DateStart.GetString(ARBDate::eYYYYMMDD);
	str << m_DateEnd.GetString(ARBDate::eYYYYMMDD);
	str << m_DateOpening.GetString(ARBDate::eYYYYMMDD, true);
	str << m_DateClosing.GetString(ARBDate::eYYYYMMDD, true);
	return str.str();
}

size_t ARBCalendar::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_DateStart.GetString(ARBDate::eDashYMD));
	++nItems;

	ioStrings.insert(m_DateEnd.GetString(ARBDate::eDashYMD));
	++nItems;

	if (m_DateOpening.IsValid())
	{
		ioStrings.insert(m_DateOpening.GetString(ARBDate::eDashYMD));
		++nItems;
	}

	if (m_DateClosing.IsValid())
	{
		ioStrings.insert(m_DateClosing.GetString(ARBDate::eDashYMD));
		++nItems;
	}

	if (0 < m_Location.length())
	{
		ioStrings.insert(m_Location);
		++nItems;
	}

	if (0 < m_Club.length())
	{
		ioStrings.insert(m_Club);
		++nItems;
	}

	if (0 < m_Venue.length())
	{
		ioStrings.insert(m_Venue);
		++nItems;
	}

	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}

	return nItems;
}

bool ARBCalendar::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	switch (inTree.GetAttrib(ATTRIB_CAL_START, m_DateStart))
	{
	case Element::eNotFound:
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_START));
		return false;
	case Element::eInvalidValue:
		{
			ARBString attrib;
			inTree.GetAttrib(ATTRIB_CAL_START, attrib);
			ARBString msg(INVALID_DATE);
			msg += attrib;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_START, msg.c_str()));
		}
		return false;
	}

	switch (inTree.GetAttrib(ATTRIB_CAL_END, m_DateEnd))
	{
	case Element::eNotFound:
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_END));
		return false;
	case Element::eInvalidValue:
		{
			ARBString attrib;
			inTree.GetAttrib(ATTRIB_CAL_END, attrib);
			ARBString msg(INVALID_DATE);
			msg += attrib;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_END, msg.c_str()));
			return false;
		}
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_OPENING, m_DateOpening))
	{
		ARBString attrib;
		inTree.GetAttrib(ATTRIB_CAL_OPENING, attrib);
		ARBString msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_OPENING, msg.c_str()));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_CLOSING, m_DateClosing))
	{
		ARBString attrib;
		inTree.GetAttrib(ATTRIB_CAL_CLOSING, attrib);
		ARBString msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_CLOSING, msg.c_str()));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_CAL_MAYBE, m_bTentative))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_MAYBE, VALID_VALUES_BOOL));
		return false;
	}

	inTree.GetAttrib(ATTRIB_CAL_LOCATION, m_Location);
	inTree.GetAttrib(ATTRIB_CAL_CLUB, m_Club);
	inTree.GetAttrib(ATTRIB_CAL_VENUE, m_Venue);

	if (inVersion == ARBVersion(1,0))
	{
		ARBString attrib;
		if (Element::eFound == inTree.GetAttrib(_T("PlanOn"), attrib))
		{
			if (attrib == _T("y"))
				m_eEntered = ePlanning;
			else
				m_eEntered = eNot;
		}
	}
	else if (inVersion >= ARBVersion(2,0))
	{
		ARBString attrib;
		if (Element::eFound == inTree.GetAttrib(ATTRIB_CAL_ENTERED, attrib))
		{
			if (attrib == _T("E"))
				m_eEntered = eEntered;
			else if (attrib == _T("P"))
				m_eEntered = ePlanning;
			else if (attrib == _T("N"))
				m_eEntered = eNot;
			else
			{
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ENTERED, VALID_VALUES_ENTRY));
				return false;
			}
		}
	}

	m_Note = inTree.GetValue();
	return true;
}

bool ARBCalendar::Save(Element& ioTree) const
{
	Element& cal = ioTree.AddElement(TREE_CALENDAR);
	cal.AddAttrib(ATTRIB_CAL_START, m_DateStart);
	cal.AddAttrib(ATTRIB_CAL_END, m_DateEnd);
	cal.AddAttrib(ATTRIB_CAL_OPENING, m_DateOpening);
	cal.AddAttrib(ATTRIB_CAL_CLOSING, m_DateClosing);
	if (m_bTentative) // Default is no
		cal.AddAttrib(ATTRIB_CAL_MAYBE, m_bTentative);
	cal.AddAttrib(ATTRIB_CAL_LOCATION, m_Location);
	cal.AddAttrib(ATTRIB_CAL_CLUB, m_Club);
	cal.AddAttrib(ATTRIB_CAL_VENUE, m_Venue);
	ARBString entered(_T("N"));
	switch (m_eEntered)
	{
	default:
		break;
	case eEntered:
		entered = _T("E");
		break;
	case ePlanning:
		entered = _T("P");
		break;
	}
	cal.AddAttrib(ATTRIB_CAL_ENTERED, entered);
	if (0 < m_Note.length())
		cal.SetValue(m_Note);
	return true;
}

void ARBCalendar::iCalendar(ICalendar* inIoStream, int inAlarm) const
{
	ARBiCal* ioStream = dynamic_cast<ARBiCal*>(inIoStream);
	ioStream->BeginEvent();
	ioStream->DoUID(GetUID(eUIDvEvent));
	ioStream->DoDTSTAMP();
	ioStream->DoDTSTART(m_DateStart);
	ioStream->DoDTEND(m_DateEnd);
	ioStream->DoSUMMARY(GetGenericName());
	ioStream->DoLOCATION(m_Location);
	{
		ARBostringstream str;
		if (IsTentative())
			str << CALENDAR_TENTATIVE << _T(" ");
		switch (GetEntered())
		{
		default:
		case ARBCalendar::eNot:
			str << CALENDAR_STATUS_N << _T(" ");
			break;
		case ARBCalendar::eEntered:
			str << CALENDAR_STATUS_E << _T(" ");
			break;
		case ARBCalendar::ePlanning:
			str << CALENDAR_STATUS_P << _T(" ");
			break;
		}
		if (m_DateOpening.IsValid())
		{
			str << CALENDAR_OPENS
				<< m_DateOpening.GetString(ARBDate::eDefault)
				<< _T(" ");
		}
		if (m_DateClosing.IsValid())
		{
			str << CALENDAR_CLOSES
				<< m_DateClosing.GetString(ARBDate::eDefault)
				<< _T(" ");
		}
		str << GetNote();
		ioStream->DoDESCRIPTION(str.str());
	}
	if (ePlanning == m_eEntered && m_DateOpening.IsValid())
		inAlarm += m_DateStart - m_DateOpening;
	if (ePlanning == m_eEntered || eEntered == m_eEntered)
		ioStream->DoAlarm(inAlarm);
	ioStream->EndEvent();
}

/////////////////////////////////////////////////////////////////////////////

bool ARBCalendarList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBCalendarPtr thing(ARBCalendar::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

class SortCalendar
{
public:
	SortCalendar() {}
	bool operator()(ARBCalendarPtr one, ARBCalendarPtr two) const
	{
		return one->GetStartDate() < two->GetStartDate();
	}
};

void ARBCalendarList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortCalendar());
}

size_t ARBCalendarList::GetAllEntered(std::vector<ARBCalendarPtr>& outEntered) const
{
	outEntered.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (ARBCalendar::eEntered == pCal->GetEntered())
			outEntered.push_back(pCal);
	}
	return outEntered.size();
}

int ARBCalendarList::TrimEntries(ARBDate const& inDate)
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

bool ARBCalendarList::FindCalendar(
		ARBCalendarPtr inCal,
		ARBCalendarPtr* outCal) const
{
	if (outCal)
		outCal->reset();
	if (inCal)
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (*(*iter) == *inCal)
			{
				if (outCal)
					*outCal = *iter;
				return true;
			}
		}
	}
	return false;
}

bool ARBCalendarList::AddCalendar(ARBCalendarPtr inCal)
{
	bool bAdded = false;
	if (inCal)
	{
		bAdded = true;
		push_back(inCal);
	}
	return bAdded;
}

bool ARBCalendarList::DeleteCalendar(ARBCalendarPtr inCal)
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
