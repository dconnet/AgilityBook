/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Implementation of ICalendarSite
 * @author David Connet
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2010-11-05 DRC Bump to 2.2.0: USDAA changed the detail page layout.
 * @li 2009-11-01 DRC Change how initialization is done.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-25 DRC Ported to wxWidgets.
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"
#include "CalendarSiteUSDAA.h"

#include "../tidy/include/tidy.h"
#include "ARBStructure.h"
#include "BreakLine.h"
#include "Element.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "ReadHttp.h"
#include "VersionNum.h"
#include <errno.h>
#include <wx/ffile.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define CAL_VER_MAJOR		2
#define CAL_VER_MINOR		2
#define CAL_VER_DOT			0

#define GENERATE_TESTDATA	0
#define USE_TESTDATA 		0
#define TESTDATANAME		"c:\\AgilityBook\\testdata\\usdaa-raw"
#if GENERATE_TESTDATA && USE_TESTDATA
#error "Choose one!"
#endif

/////////////////////////////////////////////////////////////////////////////

static wxString mdy2ymd(wxString const& inDate)
{
	wxString date;
	wxString::size_type pos = 0;
	while (pos < inDate.length() && inDate[pos] == ' ')
		++pos;
	if (pos + 10 <= inDate.length())
	{
		if (inDate[pos + 2] == '/' && inDate[pos + 5] == '/')
		{
			date = inDate.substr(pos + 6, 4);
			date += '-';
			date += inDate.substr(pos + 0, 2);
			date += '-';
			date += inDate.substr(pos + 3, 2);
		}
	}
	return date;
}


static void StripNewlines(wxString& inStr)
{
	wxString::size_type pos = inStr.find_first_of(wxT("\n"));
	while (wxString::npos != pos)
	{
#if wxCHECK_VERSION(2, 9, 3)
		inStr.replace(pos, 1, 1, wxUniChar(' '));
#else
		inStr.replace(pos, 1, 1, ' ');
#endif
		pos = inStr.find_first_of(wxT("\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////

CCalendarSiteUSDAA::CCalendarSiteUSDAA()
{
}


CCalendarSiteUSDAA::~CCalendarSiteUSDAA()
{
}


void CCalendarSiteUSDAA::Release()
{
	delete this;
}


wxString CCalendarSiteUSDAA::GetID() const
{
	return wxT("cal_usdaa.dll"); // Backwards compatibility
}


bool CCalendarSiteUSDAA::GetVersion(CVersionNum& outVer) const
{
	outVer.Assign(CAL_VER_MAJOR, CAL_VER_MINOR, CAL_VER_DOT, 0);
	return true;
}


wxString CCalendarSiteUSDAA::GetName() const
{
	return wxT("USDAA");
}


wxString CCalendarSiteUSDAA::GetDescription() const
{
	return _("Get all the events from USDAA. This will parse the HTML returned from http://www.usdaa.com/events.cfm.");
}


size_t CCalendarSiteUSDAA::GetLocationCodes(
			std::map<wxString, wxString>& locCodes) const
{
	locCodes.clear();
	return locCodes.size();
}


size_t CCalendarSiteUSDAA::GetVenueCodes(
			std::map<wxString, wxString>& venueCodes) const
{
	venueCodes.clear();
	venueCodes[wxT("USDAA")] = wxT("USDAA");
	return venueCodes.size();
}


static ElementNodePtr ReadData(
		std::string const& inAddress
#if GENERATE_TESTDATA
		, std::string const& outTestData
#endif
		)
{
	ElementNodePtr tree;

	std::string data;
#if USE_TESTDATA
	FILE* fp = fopen(inAddress.c_str(), "r");
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
	wxString url(tstringUtil::WXString(inAddress));
	CReadHttp http(url, &data);
#endif

	wxString username, errMsg;
#if !USE_TESTDATA
	if (!http.ReadHttpFile(username, errMsg))
	{
		data.erase();
	}
#endif

	if (!data.empty())
	{
#if GENERATE_TESTDATA
{
wxFFile raw(tstringUtil::WXString(outTestData), wxT("wb"));
raw.Write(data.c_str(), data.length());
}
#endif
		TidyDoc tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
		tidyOptSetBool(tdoc, TidyNumEntities, yes);
		if (0 > tidyParseString(tdoc, data.c_str()))
		{
			tidyRelease(tdoc);
			return tree;
		}
		if (0 > tidyCleanAndRepair(tdoc))
		{
			tidyRelease(tdoc);
			return tree;
		}
		tidyRunDiagnostics(tdoc);
		uint len = 0;
		if (tidySaveString(tdoc, NULL, &len) != -ENOMEM)
		{
			tidyRelease(tdoc);
			return tree;
		}
		char* pData = new char[len+1];
		strncpy(pData, data.c_str(), len);
		data.erase();
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			tidyRelease(tdoc);
			delete [] pData;
			return tree;
		}
		tidyRelease(tdoc);
		// Note, Tidy does not null terminate the buffer.
		pData[len] = 0;
#ifdef _DEBUG
//Test code to look at 'tidy'd data
//{
//std::string out(inAddress);
//out += ".out";
//wxFFile raw(tstringUtil::WXString(out), wxT("wb"));
//raw.Write(pData, strlen(pData));
//}
#endif
		wxString err;
		tree = ElementNode::New();
		if (!tree->LoadXML(pData, len, err))
		{
			tree.reset();
			wxMessageBox(err, wxMessageBoxCaptionStr, wxOK | wxCENTRE);
		}
		else
		{
#ifdef _DEBUG
//Test code to look at xml data
//{
//std::string out(inAddress);
//out += ".tree";
//wxFFileOutputStream raw(tstringUtil::WXString(out), wxT("wb"));
//tree->SaveXML(raw);
//}
#endif
		}
		delete [] pData;
		pData = NULL;
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
		progress->SetMessage("Reading http://www.usdaa.com/events.cfm");

#if USE_TESTDATA || GENERATE_TESTDATA
	std::string testData(TESTDATANAME);
	testData += ".xml";
#if USE_TESTDATA
	ElementNodePtr tree = ReadData(testData);
#else
	ElementNodePtr tree = ReadData("http://www.usdaa.com/events.cfm", testData);
#endif
#else
	ElementNodePtr tree = ReadData("http://www.usdaa.com/events.cfm");
#endif
	if (!tree)
		return std::string();

	if (progress)
	{
		if (progress->HasCanceled())
			return std::string();
		std::string msg("Finding entries...");
		progress->SetMessage(msg.c_str());
	}

	bool bOk = false;
	ElementNodePtr calTree(ElementNode::New(TREE_BOOK));
	calTree->AddAttrib(ATTRIB_BOOK_VERSION, wxT("12.6"));

	// Ok, now we get to parse the raw html from USDAA. As of Aug 2007,
	// we want to look for a Level4 Header containing "Event Calendar"
	// The data we want is then contained in the 'table' tag that follows.
	int nEntries = 0;
	ElementNodePtr parentElement;
	int idxEventCalH4tag = -1;
	static const wxString tag(wxT("h4"));
	static const wxString name(wxT("Event Calendar"));
	if (tree->FindElementDeep(parentElement, idxEventCalH4tag, tag, &name))
	{
		int idxTable = parentElement->FindElement(wxT("table"), idxEventCalH4tag+1);
		if (0 <= idxTable)
		{
			ElementNodePtr table = parentElement->GetElementNode(idxTable);
			if (!table)
				return std::string();
			for (int i = 0; i < table->GetElementCount(); ++i)
			{
				if (Element::Element_Node != table->GetElement(i)->GetType())
					continue;
				ElementNodePtr tr = table->GetElementNode(i);
				if (!tr)
					continue;
				if (4 <= tr->GetNodeCount(Element::Element_Node))
				{
					wxString dates, club, detail, location;
					int idx = 0;
					for (int td = 0; td < tr->GetElementCount(); ++td)
					{
						if (Element::Element_Node != tr->GetElement(td)->GetType())
							continue;
						switch (idx)
						{
						case 0:
							dates = tr->GetElement(td)->GetValue();
							break;
						case 1:
							{
								int idxA = tr->GetElementNode(td)->FindElement(wxT("a"));
								if (0 <= idxA)
								{
									club = tr->GetElementNode(td)->GetElement(idxA)->GetValue();
									tr->GetElementNode(td)->GetElementNode(idxA)->GetAttrib(wxT("href"), detail);
								}
								else
									club = tr->GetElement(td)->GetValue();
								StripNewlines(club);
							}
							break;
						case 2:
							location = tstringUtil::Trim(tr->GetElement(td)->GetValue());
							StripNewlines(location);
							// Cleanup location
							{
								std::vector<wxString> fields;
								size_t n = BreakLine(wxT(','), location, fields);
								if (0 < n)
								{
									location.clear();
									for (size_t iFld = 0; iFld < n; ++iFld)
									{
										if (0 < iFld)
											location << wxT(", ");
										location << tstringUtil::Trim(fields[iFld]);
									}
								}
							}
							break;
						case 3:
							// <td><a>type</a></td>
//Text[0]=["Event Type","1 - Titling Only Event&lt;BR&gt;2 - Titling Event with Tournament Classes&lt;BR&gt;3 - Titling Event with Junior Handler Classes&lt;BR&gt;4 - Titling Event with Tournament &amp; Junior Handler Classes&lt;BR&gt;5 - Tournament Only Event&lt;BR&gt;6 - Tournament Event with Junior Handler Classes&lt;BR&gt;7 - Junior Handler Only Event&lt;BR&gt;8 - Seminar&lt;BR&gt;9 - Seminar with Non-titling Match&lt;BR&gt;0 - Non-titling Match"];
							break;
						}
						++idx;
					}
					wxString::size_type pos = dates.find(wxT("-"));
					if (wxString::npos == pos)
						continue;
					bOk = true;
					ElementNodePtr cal = calTree->AddElementNode(TREE_CALENDAR);
					cal->AddAttrib(ATTRIB_CAL_START, mdy2ymd(dates.substr(0, pos)));
					cal->AddAttrib(ATTRIB_CAL_END, mdy2ymd(dates.substr(pos+1)));
					cal->AddAttrib(ATTRIB_CAL_VENUE, wxT("USDAA"));
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
	while (0 <= (idxCal = calTree->FindElement(TREE_CALENDAR, idxCal+1)))
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
		cal->SetValue(wxT(""));
		std::string address("http://www.usdaa.com/");
		address += detail.ToUTF8();
		if (progress)
		{
			if (progress->HasCanceled())
				return std::string();
			std::string msg("Getting detail from ");
			msg += address;
			progress->SetMessage(msg.c_str());
		}
#if GENERATE_TESTDATA || USE_TESTDATA
		testData = TESTDATANAME;
		int idxAddr = address.find('=');
		testData += address.substr(idxAddr+1);
		testData += ".xml";
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
			static const wxString tag2(wxT("h3"));
			static const wxString name2(wxT("General Event Information"));
			ElementNodePtr parent;
			int idxEventCalH3tag = -1;
			if (treeDetail->FindElementDeep(parent, idxEventCalH3tag, tag2, &name2))
			{
				int idxFieldset;
				ElementNodePtr parentFieldset;
				if (parent->FindElementDeep(parentFieldset, idxFieldset, wxT("fieldset")))
				{
					ElementNodePtr fieldset = parentFieldset->GetElementNode(idxFieldset);
					// Newlines have been compressed, along with some attributes
					// Not all entries have all fields - I've seen 4 and 6
					// <div><p>Event Dates:</p><span>11/20/2010 - 11/21/2010</span></div>
					// <div><p>Club Name:</p><span>Y Agility</span></div>
					// <div><p>Location:</p><div>The Pines Farm<br/>272 Dug Road<br/>South Glastonbury, CT United States</div></div>
					// <div><p>Closing date:</p><span>11/01/2010</span></div>
					// <div><p>2nd Closing Date:</p><span>11/15/2010</span></div>
					// <div><p>Event Secretary:</p><div><a href="mailto:yagility@nycap.rr.com">Donna Young</a><br/>4 Haber Way<br/>Castleton, NY 12033 United States</div></div>
					if (4 <= fieldset->GetNodeCount(Element::Element_Node))
					{
						int idx = 0;
						for (int i = 0; i < fieldset->GetElementCount(); ++i)
						{
							if (Element::Element_Node != fieldset->GetElement(i)->GetType())
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
									ElementNodePtr parentCloseDate;
									if (div->FindElementDeep(parentCloseDate, idxDate, wxT("span")))
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
									ElementNodePtr parentTagA;
									if (div->FindElementDeep(parentTagA, idxHref, wxT("a")))
									{
										ElementNodePtr tagA = parentTagA->GetElementNode(idxHref);
										wxString email;
										tagA->GetAttrib(wxT("href"), email);
										cal->AddAttrib(ATTRIB_CAL_SECEMAIL, email);
									}
								}
								break;
							}
							++idx;
						}
					}
				}
#if OLD_USDAA_FORMAT // Changed Late-Oct 2010
				int idxTable = parent->FindElement(wxT("table"), idxEventCalH3tag+1);
				if (0 <= idxTable)
				{
					ElementNodePtr table = parent->GetElementNode(idxTable);
					if (6 <= table->GetNodeCount(Element::Element_Node))
					{
						int idx = 0;
						for (int i = 0; i < table->GetElementCount(); ++i)
						{
							if (Element::Element_Node != table->GetElement(i)->GetType())
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
									int iTD = table->GetElementNode(i)->FindElement(wxT("td"), 0);
									iTD = table->GetElementNode(i)->FindElement(wxT("td"), iTD+1);
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
									int iTD = table->GetElementNode(i)->FindElement(wxT("td"), 0);
									iTD = table->GetElementNode(i)->FindElement(wxT("td"), iTD+1);
									ElementNodePtr td = table->GetElementNode(i)->GetElementNode(iTD);
									if (td)
									{
										iTD = td->FindElement(wxT("a"));
										if (0 <= iTD)
										{
											wxString email;
											td->GetElementNode(iTD)->GetAttrib(wxT("href"), email);
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
//Test code to look at generated calendar data
//{
//wxFFileOutputStream raw(wxT("c:\\events-caltree.xml"), wxT("wb"));
//calTree->SaveXML(raw);
//}
#endif
		wxMemoryOutputStream s;
		calTree->SaveXML(s);
		return tstringUtil::tstringA(s);
	}
	return std::string();
}
