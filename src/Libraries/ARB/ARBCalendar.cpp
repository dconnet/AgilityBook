/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The calendar cribsheet.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-08-11 Changed usage of FindCalendar.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-13 Added vCalendar support, finished iCalendar.
 * 2005-07-05 Added iCalendar support.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-31 Added FindCalendar.
 * 2003-07-09 IsRangeOverlapped test was wrong. It was actually a 'is
 *            range contained in' test.
 * 2003-01-23 DocVer 2: Replaced 'PlanOn' attribute and with 'Entered'.
 */

#include "stdafx.h"
#include "ARB/ARBCalendar.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
constexpr wchar_t ENTRY_NOT[] = L"N";
constexpr wchar_t ENTRY_ENTERED[] = L"E";
constexpr wchar_t ENTRY_PENDING[] = L"O";
constexpr wchar_t ENTRY_PLANNING[] = L"P";

constexpr wchar_t ACCOM_NONE[] = L"N";
constexpr wchar_t ACCOM_TODO[] = L"T";
constexpr wchar_t ACCOM_CONFIRMED[] = L"C";
} // namespace

/////////////////////////////////////////////////////////////////////////////
// Static functions (for iCalendar/vCalender)
// See RFC2445. (http://tools.ietf.org/html/rfc2445)
// Note: EOL sequence is defined as "\r\n"

class ARBiCal : public ICalendar
{
public:
	ARBiCal(std::ostream& ioStream, int inVersion);
	~ARBiCal()
	{
		Write("END:VCALENDAR\r\n");
	}

	void BeginEvent()
	{
		Write("BEGIN:VEVENT\r\n");
	}
	void DoUID(std::wstring const& inUID)
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
	void DoSUMMARY(std::wstring const& inStr)
	{
		WriteText("SUMMARY", inStr, true);
	}
	void DoLOCATION(std::wstring const& inStr)
	{
		WriteText("LOCATION", inStr, true);
	}
	void DoDESCRIPTION(std::wstring const& inStr)
	{
		WriteText("DESCRIPTION", inStr, true);
	}
	void DoAlarm(int inDaysBefore)
	{
		if (1 < m_Version)
		{
			Write("BEGIN:VALARM\r\nACTION:DISPLAY\r\nTRIGGER:-PT");
			Write(fmt::format("{}", inDaysBefore * 24 * 60));
			Write("M\r\nDESCRIPTION:Reminder\r\nEND:VALARM\r\n");
		}
	}
	void EndEvent()
	{
		Write("END:VEVENT\r\n");
	}

private:
	void Write(char inVal);
	void Write(char const* const inVal);
	void Write(std::string const& inVal);
	void Write(char const* const inVal, ARBDate inDate, bool inStartOfDay);
	void WriteSafeChar(std::string const& inText);
	void WriteText(char const* const inToken, std::wstring const& inText, bool bQuotedPrint);

	std::ostream& m_ioStream;
	int m_Version;
	DECLARE_NO_COPY_IMPLEMENTED(ARBiCal);
};

typedef std::shared_ptr<ARBiCal> ARBiCalPtr;


ARBiCal::ARBiCal(std::ostream& ioStream, int inVersion)
	: m_ioStream(ioStream)
	, m_Version(inVersion)
{
	// All V1.0 syntax was figured out by exporting an entry from Outlook2003.
	Write("BEGIN:VCALENDAR\r\n");
	Write("PRODID:-//dcon Software//Agility Record Book//EN\r\n");
	switch (m_Version)
	{
	case 1:
		Write("VERSION:1.0\r\n");
		break;
	case 2:
		Write("VERSION:2.0\r\n");
		// Figured this out thru trial/error with Outlook2003.
		Write("METHOD:PUBLISH\r\n");
		break;
	default:
		break;
	}
}


void ARBiCal::Write(char inVal)
{
	m_ioStream.write(&inVal, 1);
}


void ARBiCal::Write(char const* const inVal)
{
	if (inVal && *inVal)
	{
		size_t len = strlen(inVal);
		m_ioStream.write(inVal, len);
	}
}


void ARBiCal::Write(std::string const& inVal)
{
	if (!inVal.empty())
		m_ioStream.write(inVal.c_str(), inVal.length());
}


void ARBiCal::Write(char const* const inVal, ARBDate inDate, bool inStartOfDay)
{
	if (inVal)
	{
		Write(inVal);
		if (1 < m_Version)
			Write(";VALUE=DATE");
		Write(':');
	}
	Write(StringUtil::stringA(inDate.GetString(ARBDateFormat::YYYYMMDD)));
	if (1 == m_Version)
	{
		if (inStartOfDay)
			Write("T070000");
		else
			Write("T180000");
	}
	if (inVal)
		Write("\r\n");
}


void ARBiCal::WriteSafeChar(std::string const& inText)
{
	for (std::string::const_iterator iter = inText.begin(); iter != inText.end(); ++iter)
	{
		unsigned char c = *iter;
		// See the RFC...
		if ((0x21 <= c && c <= 0x7e)    // ASCII
			|| (0x80 <= c && c <= 0xf8) // NON-US-ASCII
			|| 0x20 == c || 0x09 == c)  // WSP
		{
			if (1 == m_Version && '=' == c)
				Write("=3D");
			else if (1 < m_Version && (';' == c || ',' == c || '\\' == c))
				Write('\\');
			else
				Write(c);
		}
		else if ('\n' == c)
		{
			if (1 == m_Version)
				Write("=0A");
			else
				Write("\\n");
		}
		else
			Write('?');
	}
}


void ARBiCal::WriteText(char const* const inToken, std::wstring const& inText, bool bQuotedPrint)
{
	if (0 < inText.length())
	{
		size_t nLineLength = 75 - strlen(inToken);
		Write(inToken);
		if (1 == m_Version && bQuotedPrint)
		{
			Write(";ENCODING=QUOTED-PRINTABLE");
			nLineLength -= 26;
		}
		Write(':');
		// "Fold" a long line. RFC 2445, section 4.1
		std::string tmp(StringUtil::stringA(inText));
		while (nLineLength < tmp.length())
		{
			// Version 1 stuff is a best-guess.
			WriteSafeChar(tmp.substr(0, nLineLength));
			if (1 == m_Version)
				Write("=\r\n");
			else
				Write("\r\n\t");
			tmp = tmp.substr(nLineLength);
			nLineLength = 75;
		}
		if (0 < tmp.length())
			WriteSafeChar(tmp);
		Write("\r\n");
	}
}


void ARBiCal::DoDTSTAMP()
{
	if (1 < m_Version)
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
		std::string buffer = fmt::format(
			"DTSTAMP:{:04}{:02}{:02}T{:02}{:02}{:02}\r\n",
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec);
		Write(buffer);
	}
}


ICalendar::ICalendar()
{
}


ICalendar::~ICalendar()
{
}


ICalendarPtr ICalendar::iCalendarBegin(std::ostream& ioStream, int inVersion)
{
	ICalendarPtr pCal;
	if (1 == inVersion || 2 == inVersion)
		pCal = std::make_shared<ARBiCal>(ioStream, inVersion);
	return pCal;
}

/////////////////////////////////////////////////////////////////////////////

class ARBCalendar_concrete : public ARBCalendar
{
public:
	ARBCalendar_concrete()
	{
	}
	ARBCalendar_concrete(ARBCalendar const& rhs)
		: ARBCalendar(rhs)
	{
	}
};


ARBCalendarPtr ARBCalendar::New()
{
	return std::make_shared<ARBCalendar_concrete>();
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
	, m_eEntered(ARBCalendarEntry::Not)
	, m_eAccommodations(ARBAccommodations::None)
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


ARBCalendar::ARBCalendar(ARBCalendar&& rhs)
	: m_DateStart(std::move(rhs.m_DateStart))
	, m_DateEnd(std::move(rhs.m_DateEnd))
	, m_DateOpening(std::move(rhs.m_DateOpening))
	, m_DateDraw(std::move(rhs.m_DateDraw))
	, m_DateClosing(std::move(rhs.m_DateClosing))
	, m_bTentative(std::move(rhs.m_bTentative))
	, m_Location(std::move(rhs.m_Location))
	, m_Club(std::move(rhs.m_Club))
	, m_Venue(std::move(rhs.m_Venue))
	, m_eEntered(std::move(rhs.m_eEntered))
	, m_eAccommodations(std::move(rhs.m_eAccommodations))
	, m_Confirmation(std::move(rhs.m_Confirmation))
	, m_SecEmail(std::move(rhs.m_SecEmail))
	, m_PremiumURL(std::move(rhs.m_PremiumURL))
	, m_OnlineURL(std::move(rhs.m_OnlineURL))
	, m_Note(std::move(rhs.m_Note))
{
}


ARBCalendar::~ARBCalendar()
{
}


ARBCalendarPtr ARBCalendar::Clone() const
{
	return std::make_shared<ARBCalendar_concrete>(*this);
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


ARBCalendar& ARBCalendar::operator=(ARBCalendar&& rhs)
{
	if (this != &rhs)
	{
		m_DateStart = std::move(rhs.m_DateStart);
		m_DateEnd = std::move(rhs.m_DateEnd);
		m_DateOpening = std::move(rhs.m_DateOpening);
		m_DateDraw = std::move(rhs.m_DateDraw);
		m_DateClosing = std::move(rhs.m_DateClosing);
		m_bTentative = std::move(rhs.m_bTentative);
		m_Location = std::move(rhs.m_Location);
		m_Club = std::move(rhs.m_Club);
		m_Venue = std::move(rhs.m_Venue);
		m_eEntered = std::move(rhs.m_eEntered);
		m_eAccommodations = std::move(rhs.m_eAccommodations);
		m_Confirmation = std::move(rhs.m_Confirmation);
		m_SecEmail = std::move(rhs.m_SecEmail);
		m_PremiumURL = std::move(rhs.m_PremiumURL);
		m_OnlineURL = std::move(rhs.m_OnlineURL);
		m_Note = std::move(rhs.m_Note);
	}
	return *this;
}


bool ARBCalendar::operator==(ARBCalendar const& rhs) const
{
	// clang-format off
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
	// clang-format on
}


std::wstring ARBCalendar::GetUID(UidType inType) const
{
	fmt::wmemory_buffer str;
	switch (inType)
	{
	case UidType::vEvent:
		fmt::format_to(std::back_inserter(str), L"{}", L"e");
		break;
	case UidType::vTodo:
		fmt::format_to(std::back_inserter(str), L"{}", L"t");
		break;
	}
	fmt::format_to(std::back_inserter(str), L"{}", m_DateStart.GetString(ARBDateFormat::YYYYMMDD));
	fmt::format_to(std::back_inserter(str), L"{}", m_DateEnd.GetString(ARBDateFormat::YYYYMMDD));
	fmt::format_to(std::back_inserter(str), L"{}", m_DateOpening.GetString(ARBDateFormat::YYYYMMDD, true));
	fmt::format_to(std::back_inserter(str), L"{}", m_DateDraw.GetString(ARBDateFormat::YYYYMMDD, true));
	fmt::format_to(std::back_inserter(str), L"{}", m_DateClosing.GetString(ARBDateFormat::YYYYMMDD, true));
	return fmt::to_string(str);
}


size_t ARBCalendar::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_DateStart.GetString(ARBDateFormat::DashYMD));
	++nItems;

	ioStrings.insert(m_DateEnd.GetString(ARBDateFormat::DashYMD));
	++nItems;

	if (m_DateOpening.IsValid())
	{
		ioStrings.insert(m_DateOpening.GetString(ARBDateFormat::DashYMD));
		++nItems;
	}

	if (m_DateDraw.IsValid())
	{
		ioStrings.insert(m_DateDraw.GetString(ARBDateFormat::DashYMD));
		++nItems;
	}

	if (m_DateClosing.IsValid())
	{
		ioStrings.insert(m_DateClosing.GetString(ARBDateFormat::DashYMD));
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


bool ARBCalendar::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_CALENDAR)
		return false;
	switch (inTree->GetAttrib(ATTRIB_CAL_START, m_DateStart))
	{
	case ARBAttribLookup::Found:
		break;
	case ARBAttribLookup::NotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_START));
		return false;
	case ARBAttribLookup::Invalid:
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_START, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_START, msg.c_str()));
	}
		return false;
	}

	switch (inTree->GetAttrib(ATTRIB_CAL_END, m_DateEnd))
	{
	case ARBAttribLookup::Found:
		break;
	case ARBAttribLookup::NotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALENDAR, ATTRIB_CAL_END));
		return false;
	case ARBAttribLookup::Invalid:
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_END, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_END, msg.c_str()));
	}
		return false;
	}

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_CAL_OPENING, m_DateOpening))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_OPENING, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_OPENING, msg.c_str()));
		return false;
	}

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_CAL_DRAW, m_DateDraw))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_DRAW, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_DRAW, msg.c_str()));
		return false;
	}

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_CAL_CLOSING, m_DateClosing))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_CAL_CLOSING, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_CLOSING, msg.c_str()));
		return false;
	}

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_CAL_MAYBE, m_bTentative))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
			TREE_CALENDAR,
			ATTRIB_CAL_MAYBE,
			Localization()->ValidValuesBool().c_str()));
		return false;
	}

	inTree->GetAttrib(ATTRIB_CAL_LOCATION, m_Location);
	inTree->GetAttrib(ATTRIB_CAL_CLUB, m_Club);
	inTree->GetAttrib(ATTRIB_CAL_VENUE, m_Venue);

	if (inVersion == ARBVersion(1, 0))
	{
		std::wstring attrib;
		if (ARBAttribLookup::Found == inTree->GetAttrib(L"PlanOn", attrib))
		{
			if (attrib == L"y")
				m_eEntered = ARBCalendarEntry::Planning;
			else
				m_eEntered = ARBCalendarEntry::Not;
		}
	}
	else if (inVersion >= ARBVersion(2, 0))
	{
		std::wstring attrib;
		if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_CAL_ENTERED, attrib))
		{
			if (attrib == ENTRY_ENTERED)
				m_eEntered = ARBCalendarEntry::Entered;
			else if (attrib == ENTRY_PENDING)
				m_eEntered = ARBCalendarEntry::Pending;
			else if (attrib == ENTRY_PLANNING)
				m_eEntered = ARBCalendarEntry::Planning;
			else if (attrib == ENTRY_NOT)
				m_eEntered = ARBCalendarEntry::Not;
			else
			{
				std::wstring msg(Localization()->ValidValues());
				msg += ENTRY_ENTERED;
				msg += L", ";
				msg += ENTRY_PLANNING;
				msg += L", ";
				msg += ENTRY_NOT;
				ioCallback.LogMessage(
					Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ENTERED, msg.c_str()));
				return false;
			}
		}

		if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_CAL_ACCOMMODATION, attrib))
		{
			if (attrib == ACCOM_NONE)
				m_eAccommodations = ARBAccommodations::None;
			else if (attrib == ACCOM_TODO)
				m_eAccommodations = ARBAccommodations::Todo;
			else if (attrib == ACCOM_CONFIRMED)
				m_eAccommodations = ARBAccommodations::Confirmed;
			else
			{
				std::wstring msg(Localization()->ValidValues());
				msg += ACCOM_NONE;
				msg += L", ";
				msg += ACCOM_TODO;
				msg += L", ";
				msg += ACCOM_CONFIRMED;
				ioCallback.LogMessage(
					Localization()->ErrorInvalidAttributeValue(TREE_CALENDAR, ATTRIB_CAL_ACCOMMODATION, msg.c_str()));
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


bool ARBCalendar::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
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
	case ARBCalendarEntry::Not:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_NOT);
		break;
	case ARBCalendarEntry::Entered:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_ENTERED);
		break;
	case ARBCalendarEntry::Pending:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_PENDING);
		break;
	case ARBCalendarEntry::Planning:
		cal->AddAttrib(ATTRIB_CAL_ENTERED, ENTRY_PLANNING);
		break;
	}
	switch (m_eAccommodations)
	{
	case ARBAccommodations::None:
		cal->AddAttrib(ATTRIB_CAL_ACCOMMODATION, ACCOM_NONE);
		break;
	case ARBAccommodations::Todo:
		cal->AddAttrib(ATTRIB_CAL_ACCOMMODATION, ACCOM_TODO);
		break;
	case ARBAccommodations::Confirmed:
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


void ARBCalendar::iCalendar(ICalendarPtr inIoStream, int inAlarm) const
{
	ARBiCalPtr ioStream = std::dynamic_pointer_cast<ARBiCal, ICalendar>(inIoStream);
	ioStream->BeginEvent();
	ioStream->DoUID(GetUID(UidType::vEvent));
	ioStream->DoDTSTAMP();
	ioStream->DoDTSTART(m_DateStart);
	ioStream->DoDTEND(m_DateEnd);
	ioStream->DoSUMMARY(GetGenericName());
	ioStream->DoLOCATION(m_Location);
	{
		fmt::wmemory_buffer str;
		if (IsTentative())
			fmt::format_to(std::back_inserter(str), L"{} ", Localization()->CalendarTentative());
		switch (GetEntered())
		{
		case ARBCalendarEntry::Not:
			fmt::format_to(std::back_inserter(str), L"{} ", Localization()->CalendarStatusN());
			break;
		case ARBCalendarEntry::Entered:
			fmt::format_to(std::back_inserter(str), L"{} ", Localization()->CalendarStatusE());
			break;
		case ARBCalendarEntry::Pending:
			fmt::format_to(std::back_inserter(str), L"{} ", Localization()->CalendarStatusO());
			break;
		case ARBCalendarEntry::Planning:
			fmt::format_to(std::back_inserter(str), L"{} ", Localization()->CalendarStatusP());
			break;
		}
		if (m_DateOpening.IsValid())
		{
			fmt::format_to(
				std::back_inserter(str),
				L"{}{} ",
				Localization()->CalendarOpens(),
				m_DateOpening.GetString(ARBDateFormat::ISO));
		}
		if (m_DateDraw.IsValid())
		{
			fmt::format_to(
				std::back_inserter(str),
				L"{}{} ",
				Localization()->CalendarDraw(),
				m_DateDraw.GetString(ARBDateFormat::ISO));
		}
		if (m_DateClosing.IsValid())
		{
			fmt::format_to(
				std::back_inserter(str),
				L"{}{} ",
				Localization()->CalendarCloses(),
				m_DateClosing.GetString(ARBDateFormat::ISO));
		}
		fmt::format_to(std::back_inserter(str), L"{}", GetNote());
		ioStream->DoDESCRIPTION(fmt::to_string(str));
	}
	if (ARBCalendarEntry::Planning == m_eEntered && m_DateOpening.IsValid())
	{
		inAlarm += m_DateStart - m_DateOpening;
	}
	if (ARBCalendarEntry::Planning == m_eEntered || ARBCalendarEntry::Pending == m_eEntered
		|| ARBCalendarEntry::Entered == m_eEntered)
	{
		ioStream->DoAlarm(inAlarm);
	}
	ioStream->EndEvent();
}


bool ARBCalendar::IsMatch(ARBCalendarPtr const& inCal, bool inMatchExact) const
{
	if (!inCal)
		return false;
	if (inMatchExact)
		return *inCal == *this;
	else
	{
		// clang-format off
		return m_DateStart == inCal->GetStartDate()
			&& m_DateEnd == inCal->GetEndDate()
			&& m_Venue == inCal->GetVenue()
			&& m_Club == inCal->GetClub();
		// clang-format on
	}
}


bool ARBCalendar::Update(ARBCalendarPtr const& inCal)
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
	if (!inCal->GetSecEmail().empty() && m_SecEmail != inCal->GetSecEmail())
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

bool ARBCalendarList::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	ARBCalendarPtr thing(ARBCalendar::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBCalendarList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBCalendarPtr const& one, ARBCalendarPtr const& two) {
		return one->GetStartDate() < two->GetStartDate();
	});
}


size_t ARBCalendarList::GetAllEntered(std::vector<ARBCalendarPtr>& outEntered) const
{
	outEntered.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (ARBCalendarEntry::Pending == pCal->GetEntered() || ARBCalendarEntry::Entered == pCal->GetEntered())
			outEntered.push_back(pCal);
	}
	return outEntered.size();
}


int ARBCalendarList::TrimEntries(ARBDate const& inDate)
{
	int trimmed = 0;
	if (inDate.IsValid())
	{
		for (iterator iter = begin(); iter != end();)
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


bool ARBCalendarList::FindCalendar(ARBCalendarPtr const& inCal, bool inMatchExact, ARBCalendarPtr* outCal) const
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


bool ARBCalendarList::AddCalendar(ARBCalendarPtr const& inCal)
{
	bool bAdded = false;
	if (inCal)
	{
		if (!inCal->GetStartDate().IsValid() || !inCal->GetEndDate().IsValid())
			return false;
		bAdded = true;
		push_back(inCal);
	}
	return bAdded;
}


bool ARBCalendarList::DeleteCalendar(ARBCalendarPtr const& inCal)
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

} // namespace ARB
} // namespace dconSoft
