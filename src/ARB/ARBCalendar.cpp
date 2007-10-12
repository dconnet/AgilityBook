/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2007-08-11 DRC Changed usage of FindCalendar.
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
#include "ARBLocalization.h"
#include "ARBTypes.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define ENTRY_NOT		_T("N")
#define ENTRY_ENTERED	_T("E")
#define ENTRY_PLANNING	_T("P")

#define ACCOM_NONE		_T("N")
#define ACCOM_TODO		_T("T")
#define ACCOM_CONFIRMED	_T("C")

/////////////////////////////////////////////////////////////////////////////
// Static functions (for iCalendar/vCalender)
// See RFC2445.
// Note: EOL sequence is defined as "\r\n"

class ARBiCal : public ICalendar
{
public:
	ARBiCal(
			otstream& ioStream,
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

	otstream& m_ioStream;
	int m_Version;
};


ARBiCal::ARBiCal(
		otstream& ioStream,
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
		otstringstream str;
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
		otstream& ioStream,
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
	, m_DateDraw()
	, m_DateClosing()
	, m_bTentative(false)
	, m_Location()
	, m_Club()
	, m_Venue()
	, m_eEntered(eNot)
	, m_eAccommodations(eAccomNone)
	, m_Confirmation()
	, m_SecEmail()
	, m_PremiumURL()
	, m_OnlineURL()
	, m_Note()
{
}


ARBCalendar::ARBCalendar(ARBCalendar const& rhs)
	: m_DateStart(rhs.m_DateStart)
	, m_DateEnd(rhs.m_DateEnd)
	, m_DateOpening(rhs.m_DateOpening)
	, m_DateDraw(rhs.m_DateDraw)
	, m_DateClosing(rhs.m_DateClosing)
	, m_bTentative(rhs.m_bTentative)
	, m_Location(rhs.m_Location)
	, m_Club(rhs.m_Club)
	, m_Venue(rhs.m_Venue)
	, m_eEntered(rhs.m_eEntered)
	, m_eAccommodations(rhs.m_eAccommodations)
	, m_Confirmation(rhs.m_Confirmation)
	, m_SecEmail(rhs.m_SecEmail)
	, m_PremiumURL(rhs.m_PremiumURL)
	, m_OnlineURL(rhs.m_OnlineURL)
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
		m_DateDraw = rhs.m_DateDraw;
		m_DateClosing = rhs.m_DateClosing;
		m_bTentative = rhs.m_bTentative;
		m_Location = rhs.m_Location;
		m_Club = rhs.m_Club;
		m_Venue = rhs.m_Venue;
		m_eEntered = rhs.m_eEntered;
		m_eAccommodations = rhs.m_eAccommodations;
		m_Confirmation = rhs.m_Confirmation;
		m_SecEmail = rhs.m_SecEmail;
		m_PremiumURL = rhs.m_PremiumURL;
		m_OnlineURL = rhs.m_OnlineURL;
		m_Note = rhs.m_Note;
	}
	return *this;
}


bool ARBCalendar::operator==(ARBCalendar const& rhs) const
{
	return m_DateStart == rhs.m_DateStart
		&& m_DateEnd == rhs.m_DateEnd
		&& m_DateOpening == rhs.m_DateOpening
		&& m_DateDraw == rhs.m_DateDraw
		&& m_DateClosing == rhs.m_DateClosing
		&& m_bTentative == rhs.m_bTentative
		&& m_Location == rhs.m_Location
		&& m_Club == rhs.m_Club
		&& m_Venue == rhs.m_Venue
		&& m_eEntered == rhs.m_eEntered
		&& m_eAccommodations == rhs.m_eAccommodations
		&& m_Confirmation == rhs.m_Confirmation
		&& m_SecEmail == rhs.m_SecEmail
		&& m_PremiumURL == rhs.m_PremiumURL
		&& m_OnlineURL == rhs.m_OnlineURL
		&& m_Note == rhs.m_Note;
}


tstring ARBCalendar::GetUID(eUidType inType) const
{
	otstringstream str;
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
	str << m_DateDraw.GetString(ARBDate::eYYYYMMDD, true);
	str << m_DateClosing.GetString(ARBDate::eYYYYMMDD, true);
	return str.str();
}


size_t ARBCalendar::GetSearchStrings(std::set<tstring>& ioStrings) const
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

	if (m_DateDraw.IsValid())
	{
		ioStrings.insert(m_DateDraw.GetString(ARBDate::eDashYMD));
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

	if (0 < m_Confirmation.length())
	{
		ioStrings.insert(m_Confirmation);
		++nItems;
	}

	if (0 < m_SecEmail.length())
	{
		ioStrings.insert(m_SecEmail);
		++nItems;
	}

	if (0 < m_PremiumURL.length())
	{
		ioStrings.insert(m_PremiumURL);
		++nItems;
	}

	if (0 < m_OnlineURL.length())
	{
		ioStrings.insert(m_OnlineURL);
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	switch (inTree->GetAttrib(ATTRIB_CAL_START, m_DateStart))
	{
	case ElementNode::eNotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_START));
		return false;
	case ElementNode::eInvalidValue:
		{
			tstring attrib;
			inTree->GetAttrib(ATTRIB_CAL_START, attrib);
			tstring msg(Localization()->InvalidDate());
			msg += attrib;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_START, msg.c_str()));
		}
		return false;
	}

	switch (inTree->GetAttrib(ATTRIB_CAL_END, m_DateEnd))
	{
	case ElementNode::eNotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_END));
		return false;
	case ElementNode::eInvalidValue:
		{
			tstring attrib;
			inTree->GetAttrib(ATTRIB_CAL_END, attrib);
			tstring msg(Localization()->InvalidDate());
			msg += attrib;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_END, msg.c_str()));
			return false;
		}
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_CAL_OPENING, m_DateOpening))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_OPENING, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_OPENING, msg.c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_CAL_DRAW, m_DateDraw))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_DRAW, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_DRAW, msg.c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_CAL_CLOSING, m_DateClosing))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_CLOSING, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_CLOSING, msg.c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_CAL_MAYBE, m_bTentative))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_MAYBE, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	inTree->GetAttrib(ATTRIB_CAL_LOCATION, m_Location);
	inTree->GetAttrib(ATTRIB_CAL_CLUB, m_Club);
	inTree->GetAttrib(ATTRIB_CAL_VENUE, m_Venue);

	if (inVersion == ARBVersion(1,0))
	{
		tstring attrib;
		if (ElementNode::eFound == inTree->GetAttrib(_T("PlanOn"), attrib))
		{
			if (attrib == _T("y"))
				m_eEntered = ePlanning;
			else
				m_eEntered = eNot;
		}
	}
	else if (inVersion >= ARBVersion(2,0))
	{
		tstring attrib;
		if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_CAL_ENTERED, attrib))
		{
			if (attrib == ENTRY_ENTERED)
				m_eEntered = eEntered;
			else if (attrib == ENTRY_PLANNING)
				m_eEntered = ePlanning;
			else if (attrib == ENTRY_NOT)
				m_eEntered = eNot;
			else
			{
				tstring msg(Localization()->ValidValues());
				msg += ENTRY_ENTERED;
				msg += _T(", ");
				msg += ENTRY_PLANNING;
				msg += _T(", ");
				msg += ENTRY_NOT;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ENTERED, msg.c_str()));
				return false;
			}
		}

		if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_CAL_ACCOMMODATION, attrib))
		{
			if (attrib == ACCOM_NONE)
				m_eAccommodations = eAccomNone;
			else if (attrib == ACCOM_TODO)
				m_eAccommodations = eAccomTodo;
			else if (attrib == ACCOM_CONFIRMED)
				m_eAccommodations = eAccomConfirmed;
			else
			{
				tstring msg(Localization()->ValidValues());
				msg += ACCOM_NONE;
				msg += _T(", ");
				msg += ACCOM_TODO;
				msg += _T(", ");
				msg += ACCOM_CONFIRMED;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ACCOMMODATION, msg.c_str()));
				return false;
			}
		}
		inTree->GetAttrib(ATTRIB_CAL_CONFIRMATION, m_Confirmation);
	}

	inTree->GetAttrib(ATTRIB_CAL_SECEMAIL, m_SecEmail);
	inTree->GetAttrib(ATTRIB_CAL_PREMIUMURL, m_PremiumURL);
	inTree->GetAttrib(ATTRIB_CAL_ONLINEURL, m_OnlineURL);

	m_Note = inTree->GetValue();
	return true;
}


bool ARBCalendar::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr cal = ioTree->AddElementNode(TREE_CALENDAR);
	cal->AddAttrib(ATTRIB_CAL_START, m_DateStart);
	cal->AddAttrib(ATTRIB_CAL_END, m_DateEnd);
	cal->AddAttrib(ATTRIB_CAL_OPENING, m_DateOpening);
	cal->AddAttrib(ATTRIB_CAL_DRAW, m_DateDraw);
	cal->AddAttrib(ATTRIB_CAL_CLOSING, m_DateClosing);
	if (m_bTentative) // Default is no
		cal->AddAttrib(ATTRIB_CAL_MAYBE, m_bTentative);
	cal->AddAttrib(ATTRIB_CAL_LOCATION, m_Location);
	cal->AddAttrib(ATTRIB_CAL_CLUB, m_Club);
	cal->AddAttrib(ATTRIB_CAL_VENUE, m_Venue);
	switch (m_eEntered)
	{
	default:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_NOT);
		break;
	case eEntered:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_ENTERED);
		break;
	case ePlanning:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_PLANNING);
		break;
	}
	switch (m_eAccommodations)
	{
	default:
		cal->AddAttrib(ATTRIB_CAL_ACCOMMODATION, ACCOM_NONE);
		break;
	case eAccomTodo:
		cal->AddAttrib(ATTRIB_CAL_ACCOMMODATION, ACCOM_TODO);
		break;
	case eAccomConfirmed:
		cal->AddAttrib(ATTRIB_CAL_ACCOMMODATION, ACCOM_CONFIRMED);
		break;
	}
	if (0 < m_Confirmation.length())
		cal->AddAttrib(ATTRIB_CAL_CONFIRMATION, m_Confirmation);
	if (0 < m_SecEmail.length())
		cal->AddAttrib(ATTRIB_CAL_SECEMAIL, m_SecEmail);
	if (0 < m_PremiumURL.length())
		cal->AddAttrib(ATTRIB_CAL_PREMIUMURL, m_PremiumURL);
	if (0 < m_OnlineURL.length())
		cal->AddAttrib(ATTRIB_CAL_ONLINEURL, m_OnlineURL);
	if (0 < m_Note.length())
		cal->SetValue(m_Note);
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
		otstringstream str;
		if (IsTentative())
			str << Localization()->CalendarTentative() << _T(" ");
		switch (GetEntered())
		{
		default:
		case ARBCalendar::eNot:
			str << Localization()->CalendarStatusN() << _T(" ");
			break;
		case ARBCalendar::eEntered:
			str << Localization()->CalendarStatusE() << _T(" ");
			break;
		case ARBCalendar::ePlanning:
			str << Localization()->CalendarStatusP() << _T(" ");
			break;
		}
		if (m_DateOpening.IsValid())
		{
			str << Localization()->CalendarOpens()
				<< m_DateOpening.GetString(ARBDate::eDefault)
				<< _T(" ");
		}
		if (m_DateDraw.IsValid())
		{
			str << Localization()->CalendarDraw()
				<< m_DateDraw.GetString(ARBDate::eDefault)
				<< _T(" ");
		}
		if (m_DateClosing.IsValid())
		{
			str << Localization()->CalendarCloses()
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


bool ARBCalendar::IsMatch(ARBCalendarPtr inCal, bool inMatchExact) const
{
	if (!inCal)
		return false;
	if (inMatchExact)
		return *inCal == *this;
	else
	{
		return m_DateStart == inCal->GetStartDate()
			&& m_DateEnd == inCal->GetEndDate()
			&& m_Venue == inCal->GetVenue()
			&& m_Club == inCal->GetClub();
	}
}


bool ARBCalendar::Update(ARBCalendarPtr inCal)
{
	if (!inCal)
		return false;
	bool bChanged = false;
	if (inCal->GetStartDate().IsValid() && m_DateStart != inCal->GetStartDate())
	{
		bChanged = true;
		m_DateStart = inCal->GetStartDate();
	}
	if (inCal->GetEndDate().IsValid() && m_DateEnd != inCal->GetEndDate())
	{
		bChanged = true;
		m_DateEnd = inCal->GetEndDate();
	}
	if (inCal->GetOpeningDate().IsValid() && m_DateOpening != inCal->GetOpeningDate())
	{
		bChanged = true;
		m_DateOpening = inCal->GetOpeningDate();
	}
	if (inCal->GetDrawDate().IsValid() && m_DateDraw != inCal->GetDrawDate())
	{
		bChanged = true;
		m_DateDraw = inCal->GetDrawDate();
	}
	if (inCal->GetClosingDate().IsValid() && m_DateClosing != inCal->GetClosingDate())
	{
		bChanged = true;
		m_DateClosing = inCal->GetClosingDate();
	}
	if (m_bTentative != inCal->IsTentative())
	{
		bChanged = true;
		m_bTentative = inCal->IsTentative();
	}
	if (!inCal->GetLocation().empty() && m_Location != inCal->GetLocation())
	{
		bChanged = true;
		m_Location = inCal->GetLocation();
	}
	if (!inCal->GetClub().empty() && m_Club != inCal->GetClub())
	{
		bChanged = true;
		m_Club = inCal->GetClub();
	}
	if (!inCal->GetVenue().empty() && m_Venue != inCal->GetVenue())
	{
		bChanged = true;
		m_Venue = inCal->GetVenue();
	}
	if  (!inCal->GetSecEmail().empty() && m_SecEmail != inCal->GetSecEmail())
	{
		bChanged = true;
		m_SecEmail = inCal->GetSecEmail();
	}
	if (!inCal->GetPremiumURL().empty() && m_PremiumURL != inCal->GetPremiumURL())
	{
		bChanged = true;
		m_PremiumURL = inCal->GetPremiumURL();
	}
	if (!inCal->GetOnlineURL().empty() && m_OnlineURL != inCal->GetOnlineURL())
	{
		bChanged = true;
		m_OnlineURL = inCal->GetOnlineURL();
	}
	if (!inCal->GetNote().empty() && m_Note != inCal->GetNote())
	{
		bChanged = true;
		m_Note = inCal->GetNote();
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBCalendarList::Load(
		ElementNodePtr inTree,
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
		bool inMatchExact,
		ARBCalendarPtr* outCal) const
{
	if (outCal)
		outCal->reset();
	if (inCal)
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter)->IsMatch(inCal, inMatchExact))
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
