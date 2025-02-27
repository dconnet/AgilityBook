/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Implementation of ICalendarSite
 * @author David Connet
 *
 * Revision History
 * 2013-08-23 Add error message if tidy error is empty.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2010-11-05 Bump to 2.2.0: USDAA changed the detail page layout.
 * 2009-11-01 Change how initialization is done.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-25 Ported to wxWidgets.
 * 2007-08-12 Created
 */

#include "stdafx.h"
#include "CalendarSiteUSDAA.h"

#include "ICalendarSite.h"
#include "IProgressMeter.h"

#include "ARB/ARBStructure.h"
#include "ARBCommon/BreakLine.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ReadHttp.h"
#include "LibTidyHtml/LibTidyHtml.h"
#include <wx/ffile.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <errno.h>
#include <sstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define CAL_VER_MAJOR 2
#define CAL_VER_MINOR 2
#define CAL_VER_DOT   0

#define GENERATE_TESTDATA 0
#define USE_TESTDATA      0
#define TESTDATANAME      L"c:\\AgilityBook\\testdata\\usdaa-raw"
#define DEBUG_TIDY        1
#if GENERATE_TESTDATA && USE_TESTDATA
#error "Choose one!"
#endif

namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

namespace
{
constexpr wchar_t k_eventURL[] = L"https://usdaa.com/events/event-calendar.cfm";

wxString mdy2ymd(wxString const& inDate)
{
	wxString date;
	wxString::size_type pos = 0;
	while (pos < inDate.length() && inDate[pos] == L' ')
		++pos;
	if (pos + 10 <= inDate.length())
	{
		if (inDate[pos + 2] == L'/' && inDate[pos + 5] == L'/')
		{
			date = inDate.substr(pos + 6, 4);
			date += L'-';
			date += inDate.substr(pos + 0, 2);
			date += L'-';
			date += inDate.substr(pos + 3, 2);
		}
	}
	return date;
}

} // namespace

/////////////////////////////////////////////////////////////////////////////

ICalendarSitePtr CCalendarSiteUSDAA::Create()
{
	return std::make_shared<CCalendarSiteUSDAA>();
}


CCalendarSiteUSDAA::CCalendarSiteUSDAA()
{
}


CCalendarSiteUSDAA::~CCalendarSiteUSDAA()
{
}


wxString CCalendarSiteUSDAA::GetID() const
{
	return L"cal_usdaa.dll"; // Backwards compatibility
}


bool CCalendarSiteUSDAA::GetVersion(CVersionNum& outVer) const
{
	outVer.Assign(CAL_VER_MAJOR, CAL_VER_MINOR, CAL_VER_DOT, 0);
	return true;
}


wxString CCalendarSiteUSDAA::GetName() const
{
	return L"USDAA";
}


wxString CCalendarSiteUSDAA::GetDescription() const
{
	return wxString::Format(_("Get all the events from USDAA. This will parse the HTML returned from %s"), k_eventURL);
}


size_t CCalendarSiteUSDAA::GetLocationCodes(std::map<wxString, wxString>& locCodes) const
{
	locCodes.clear();
	return locCodes.size();
}


size_t CCalendarSiteUSDAA::GetVenueCodes(std::map<wxString, wxString>& venueCodes) const
{
	venueCodes.clear();
	venueCodes[L"USDAA"] = L"USDAA";
	return venueCodes.size();
}


static ElementNodePtr ReadData(
	wxString const& inAddress
#if GENERATE_TESTDATA
	,
	wxString const& outTestData
#endif
)
{
	ElementNodePtr tree;

	std::string data;
#if USE_TESTDATA
	FILE* fp = fopen(inAddress.utf8_string().c_str(), "r");
	if (fp)
	{
		char buffer[1001];
		size_t sz;
		while (0 < (sz = fread(buffer, 1, 1000, fp)))
		{
			buffer[sz] = 0;
			data += buffer;
		}
		fclose(fp);
	}
#else
	{
		wxString errMsg;
#pragma PRAGMA_TODO(use wxWebRequest to read url asynchronously)
		// if (!ReadHttp::ReadHttpFileSync(errMsg, inAddress, data))
		data.clear();
	}
#endif

	if (!data.empty())
	{
		std::string debug;
#if GENERATE_TESTDATA
#if DEBUG_TIDY
		debug = StringUtil::stringA(outTestData);
#endif
		{
			wxFFile raw(outTestData, L"wb");
			raw.Write(data.c_str(), data.length());
		}
#endif
		std::string errTidy;
		auto treeData = TidyHtml::TidyHtmlData(data, errTidy, &debug);

		wxString err;
		if (!errTidy.empty())
			err << errTidy;

		tree = ElementNode::New();
		if (!tree->LoadXML(treeData.data(), treeData.length(), err))
		{
			tree.reset();
		}
		if (!tree)
		{
			if (err.empty())
				err = wxString::Format(_("IDS_ERR_PARSING_DATA"), inAddress);
			wxMessageBox(err, _("Agility Record Book"), wxOK | wxCENTRE);
		}
	}
	return tree;
}


// The only filtering of calendar events the website has is by type
// (titling/tourny/etc). So just ignore any supplied codes.
std::string CCalendarSiteUSDAA::Process(
	std::vector<wxString> const& /*inLocCodes*/,
	std::vector<wxString> const& /*inVenueCodes*/,
	IProgressMeter* progress) const
{
	if (progress)
		progress->SetMessage(wxString::Format(L"Reading %s", k_eventURL));

#if USE_TESTDATA || GENERATE_TESTDATA
	wxString testData(TESTDATANAME);
	testData += L".xml";
#if USE_TESTDATA
	ElementNodePtr tree = ReadData(testData);
#else
	ElementNodePtr tree = ReadData(k_eventURL, testData);
#endif
#else
	ElementNodePtr tree = ReadData(k_eventURL);
#endif
	if (!tree)
		return std::string();

	if (progress)
	{
		if (progress->HasCanceled())
			return std::string();
		progress->SetMessage(L"Finding entries...");
	}

	bool bOk = false;
	ElementNodePtr calTree(ElementNode::New(TREE_BOOK));
	calTree->AddAttrib(ATTRIB_BOOK_VERSION, L"12.6");

	// Ok, now we get to parse the raw html from USDAA. As of Aug 2007,
	// we want to look for a Level4 Header containing "Event Calendar"
	// The data we want is then contained in the 'table' tag that follows.
	// TODO: Note, the website was redesigned in early 2020. This is broken.
	int nEntries = 0;
	ElementNode const* parentElement = nullptr;
	int idxEventCalH4tag = -1;
	static const wxString tag(L"h4");
	static const wxString name(L"Event Calendar");
	if (tree->FindElementDeep(parentElement, idxEventCalH4tag, tag, &name))
	{
		int idxTable = parentElement->FindElement(L"table", idxEventCalH4tag + 1);
		if (0 <= idxTable)
		{
			ElementNodePtr table = parentElement->GetElementNode(idxTable);
			if (!table)
				return std::string();
			for (int i = 0; i < table->GetElementCount(); ++i)
			{
				if (ARBElementType::Node != table->GetElement(i)->GetType())
					continue;
				ElementNodePtr tr = table->GetElementNode(i);
				if (!tr)
					continue;
				if (4 <= tr->GetNodeCount(ARBElementType::Node))
				{
					wxString dates, club, detail, location;
					int idx = 0;
					for (int td = 0; td < tr->GetElementCount(); ++td)
					{
						if (ARBElementType::Node != tr->GetElement(td)->GetType())
							continue;
						switch (idx)
						{
						case 0:
							dates = tr->GetElement(td)->GetValue();
							break;
						case 1:
						{
							int idxA = tr->GetElementNode(td)->FindElement(L"a");
							if (0 <= idxA)
							{
								club = tr->GetElementNode(td)->GetElement(idxA)->GetValue();
								tr->GetElementNode(td)->GetElementNode(idxA)->GetAttrib(L"href", detail);
							}
							else
								club = tr->GetElement(td)->GetValue();
							club.Replace(L"\n", L"");
						}
						break;
						case 2:
							location = StringUtil::Trim(tr->GetElement(td)->GetValue());
							location.Replace(L"\n", L"");
							// Cleanup location
							{
								std::vector<wxString> fields;
								size_t n = BreakLine(L',', location, fields);
								if (0 < n)
								{
									location.clear();
									for (size_t iFld = 0; iFld < n; ++iFld)
									{
										if (0 < iFld)
											location += L", ";
										location += StringUtil::Trim(fields[iFld]);
									}
								}
							}
							break;
						case 3:
							// clang-format off
							// <td><a>type</a></td>
							//Text[0]=["Event Type","1 - Titling Only Event&lt;BR&gt;2 - Titling Event with Tournament Classes&lt;BR&gt;3 - Titling Event with Junior Handler Classes&lt;BR&gt;4 - Titling Event with Tournament &amp; Junior Handler Classes&lt;BR&gt;5 - Tournament Only Event&lt;BR&gt;6 - Tournament Event with Junior Handler Classes&lt;BR&gt;7 - Junior Handler Only Event&lt;BR&gt;8 - Seminar&lt;BR&gt;9 - Seminar with Non-titling Match&lt;BR&gt;0 - Non-titling Match"];
							// clang-format on
							break;
						default:
							break;
						}
						++idx;
					}
					wxString::size_type pos = dates.find(L"-");
					if (wxString::npos == pos)
						continue;
					bOk = true;
					ElementNodePtr cal = calTree->AddElementNode(TREE_CALENDAR);
					cal->AddAttrib(ATTRIB_CAL_START, mdy2ymd(dates.substr(0, pos)));
					cal->AddAttrib(ATTRIB_CAL_END, mdy2ymd(dates.substr(pos + 1)));
					cal->AddAttrib(ATTRIB_CAL_VENUE, L"USDAA");
					cal->AddAttrib(ATTRIB_CAL_CLUB, club);
					cal->AddAttrib(ATTRIB_CAL_LOCATION, location);
					cal->SetValue(detail);
					++nEntries;
				}
			}
		}
	}

	if (progress)
	{
		if (progress->HasCanceled())
			return std::string();
		progress->SetRange(nEntries);
	}

	int idxCal = -1;
	while (0 <= (idxCal = calTree->FindElement(TREE_CALENDAR, idxCal + 1)))
	{
		if (progress)
		{
			if (progress->HasCanceled())
				return std::string();
			progress->StepIt();
		}
		ElementNodePtr cal = calTree->GetElementNode(idxCal);
		wxString detail = cal->GetValue();
		if (detail.empty())
			continue;
		cal->SetValue(L"");
		wxString address(L"https://www.usdaa.com/");
		address += detail;
		if (progress)
		{
			if (progress->HasCanceled())
				return std::string();
			wxString msg(L"Getting detail from ");
			msg += address;
			progress->SetMessage(msg);
		}
#if GENERATE_TESTDATA || USE_TESTDATA
		testData = TESTDATANAME;
		size_t idxAddr = address.find('=');
		testData += address.substr(idxAddr + 1);
		testData += L".xml";
#if USE_TESTDATA
		ElementNodePtr treeDetail = ReadData(testData);
#else
		ElementNodePtr treeDetail = ReadData(address, testData);
#endif
#else
		ElementNodePtr treeDetail = ReadData(address);
#endif
		if (treeDetail)
		{
			static const wxString tag2(L"h3");
			static const wxString name2(L"General Event Information");
			ElementNode const* parent = nullptr;
			int idxEventCalH3tag = -1;
			if (treeDetail->FindElementDeep(parent, idxEventCalH3tag, tag2, &name2))
			{
				int idxFieldset;
				ElementNode const* parentFieldset;
				if (parent->FindElementDeep(parentFieldset, idxFieldset, L"fieldset"))
				{
					ElementNodePtr fieldset = parentFieldset->GetElementNode(idxFieldset);
					// Newlines have been compressed, along with some attributes
					// Not all entries have all fields - I've seen 4 and 6
					// clang-format off
					// <div><p>Event Dates:</p><span>11/20/2010 - 11/21/2010</span></div>
					// <div><p>Club Name:</p><span>Y Agility</span></div>
					// <div><p>Location:</p><div>The Pines Farm<br/>272 Dug Road<br/>South Glastonbury, CT United States</div></div>
					// <div><p>Closing date:</p><span>11/01/2010</span></div>
					// <div><p>2nd Closing Date:</p><span>11/15/2010</span></div>
					// <div><p>Event Secretary:</p><div><a href="mailto:yagility@nycap.rr.com">Donna Young</a><br/>4 Haber Way<br/>Castleton, NY 12033 United States</div></div>
					// clang-format on
					if (4 <= fieldset->GetNodeCount(ARBElementType::Node))
					{
						int idx = 0;
						for (int i = 0; i < fieldset->GetElementCount(); ++i)
						{
							if (ARBElementType::Node != fieldset->GetElement(i)->GetType())
								continue;
							ElementNodePtr div = fieldset->GetElementNode(i);
							switch (idx)
							{
							case 0: // Dates
								break;
							case 1: // Club name
								break;
							case 2: // Location
								break;
							case 3: // Closing date
							{
								int idxDate;
								ElementNode const* parentCloseDate;
								if (div->FindElementDeep(parentCloseDate, idxDate, L"span"))
								{
									ElementNodePtr closeDate = parentCloseDate->GetElementNode(idxDate);
									wxString date = closeDate->GetValue();
									cal->AddAttrib(ATTRIB_CAL_CLOSING, mdy2ymd(date));
								}
							}
							break;
							case 4: // 2nd closing
								break;
							case 5: // Secretary
							{
								int idxHref;
								ElementNode const* parentTagA;
								if (div->FindElementDeep(parentTagA, idxHref, L"a"))
								{
									ElementNodePtr tagA = parentTagA->GetElementNode(idxHref);
									wxString email;
									tagA->GetAttrib(L"href", email);
									cal->AddAttrib(ATTRIB_CAL_SECEMAIL, email);
								}
							}
							break;
							default:
								break;
							}
							++idx;
						}
					}
				}
#if OLD_USDAA_FORMAT // Changed Late-Oct 2010
				int idxTable = parent->FindElement(L"table", idxEventCalH3tag + 1);
				if (0 <= idxTable)
				{
					ElementNodePtr table = parent->GetElementNode(idxTable);
					if (6 <= table->GetNodeCount(ARBElementType::Node))
					{
						int idx = 0;
						for (int i = 0; i < table->GetElementCount(); ++i)
						{
							if (ARBElementType::Node != table->GetElement(i)->GetType())
								continue;
							switch (idx)
							{
							case 0:
								// <tr>
								//   <td><b>Event Date</b></td>
								//   <td>mm/dd/yyyy - mm/dd/yyyy</td>
								break;
							case 1:
								// <tr>
								//   <td><b>Club Name</b></td>
								//   <td>name</td>
								break;
							case 2:
								// <tr>
								//   <td><b>Location</b></td>
								//   <td>text<br/>test<br/>text</td>
								break;
							case 3:
								// <tr>
								//   <td><b>Closing Date</b></td>
								//   <td>mm/dd/yyyy</td>
								{
									int iTD = table->GetElementNode(i)->FindElement(L"td", 0);
									iTD = table->GetElementNode(i)->FindElement(L"td", iTD + 1);
									if (0 <= iTD)
									{
										wxString date = table->GetElementNode(i)->GetElement(iTD)->GetValue();
										cal->AddAttrib(ATTRIB_CAL_CLOSING, mdy2ymd(date));
									}
								}
								break;
							case 4:
								// <tr>
								//   <td><b>2<sup>nd</sup> Closing Date</b></td>
								//   <td>mm/dd/yyyy</td>
								break;
							case 5:
								// <tr>
								//   <td><b>Event Secretary</b></td>
								//   <td><a href="mailto...">name</a><br/>addr<br/></td>
								{
									int iTD = table->GetElementNode(i)->FindElement(L"td", 0);
									iTD = table->GetElementNode(i)->FindElement(L"td", iTD + 1);
									ElementNodePtr td = table->GetElementNode(i)->GetElementNode(iTD);
									if (td)
									{
										iTD = td->FindElement(L"a");
										if (0 <= iTD)
										{
											wxString email;
											td->GetElementNode(iTD)->GetAttrib(L"href", email);
											cal->AddAttrib(ATTRIB_CAL_SECEMAIL, email);
										}
									}
								}
								break;
							}
							++idx;
						}
					}
				}
#endif
			}
		}
	}
	if (bOk)
	{
#ifdef _DEBUG
		// Test code to look at generated calendar data
		//{
		//	wxFFileOutputStream raw(L"c:\\events-caltree.xml", L"wb");
		//	calTree->SaveXML(raw);
		//}
#endif
		std::stringstream s;
		calTree->SaveXML(s);
		return s.str();
	}
	return std::string();
}

} // namespace dconSoft
