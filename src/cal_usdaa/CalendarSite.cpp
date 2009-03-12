/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Implementation of ICalendarSite
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-25 DRC Ported to wxWidgets.
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"

#include "CalVerNum.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "../ARB/ARBStructure.h"
#include "../ARB/Element.h"
#include "../Win/ReadHttp.h"
#include "../tidy/include/tidy.h"
#include <errno.h>
#include <sstream>

#define GENERATE_TESTDATA	0
#define USE_TESTDATA 		0
#define TESTDATANAME		"c:\\AgilityBook\\testdata\\usdaa-raw"
#if GENERATE_TESTDATA && USE_TESTDATA
#error "Choose one!"
#endif

#if GENERATE_TESTDATA || USE_TESTDATA
#include <fstream>
#endif


/////////////////////////////////////////////////////////////////////////////

static char* Allocate(std::string const& inStr)
{
	if (inStr.empty())
		return NULL;
	char* pData = new char[inStr.length()+1];
	memcpy(pData, inStr.c_str(), inStr.length());
	pData[inStr.length()] = 0;
	return pData;
}


static tstring mdy2ymd(tstring const& inDate)
{
	tstring date;
	tstring::size_type pos = 0;
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


static void StripNewlines(tstring& inStr)
{
	tstring::size_type pos = inStr.find_first_of(wxT("\n"));
	while (tstring::npos != pos)
	{
		inStr.replace(pos, 1, 1, ' ');
		pos = inStr.find_first_of(wxT("\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////

class CCalendarSite : public ICalendarSite
{
public:
	CCalendarSite();
	~CCalendarSite();

	virtual void Release();
	virtual bool GetVersion(
			short* pMajor,
			short* pMinor,
			short* pRelease,
			short* pBuild) const;
	virtual void releaseBuffer(char* pData) const;
	virtual char* GetName() const;
	virtual char* GetDescription() const;
	virtual char* GetLocationCodes() const;
	virtual char* GetVenueCodes() const;
	virtual char* Process(
			char const* inLocCodes,
			char const* inVenueCodes,
			IProgressMeter* progress) const;
};


CCalendarSite::CCalendarSite()
{
}


CCalendarSite::~CCalendarSite()
{
}


void CCalendarSite::Release()
{
	delete this;
}


bool CCalendarSite::GetVersion(
		short* pMajor,
		short* pMinor,
		short* pDot,
		short* pBuild) const
{
	if (pMajor)
		*pMajor = CAL_VER_MAJOR;
	if (pMinor)
		*pMinor = CAL_VER_MINOR;
	if (pDot)
		*pDot = CAL_VER_DOT;
	if (pBuild)
		*pBuild = CAL_VER_BUILD;
	return true;
}


void CCalendarSite::releaseBuffer(char* pData) const
{
	delete [] pData;
}


char* CCalendarSite::GetName() const
{
	static const std::string name("USDAA");
	return Allocate(name);
}


char* CCalendarSite::GetDescription() const
{
	std::string desc("Get all the events from USDAA. This will parse the HTML returned from http://www.usdaa.com/events.cfm.");
	return Allocate(desc);
}


char* CCalendarSite::GetLocationCodes() const
{
	return NULL;
}


char* CCalendarSite::GetVenueCodes() const
{
	return Allocate("USDAA:USDAA");
}


static ElementNodePtr ReadData(
#if USE_TESTDATA
		std::string const& inAddress
#else
		wxString const& inAddress
#endif
#if GENERATE_TESTDATA
		, std::string const& outTestData
#endif
		)
{
	ElementNodePtr tree;

	std::string data;
#if USE_TESTDATA
	FILE* fp = fopen(inAddress, "r");
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
	CReadHttp http(inAddress, &data);
#endif

	wxString username, errMsg;
#if !USE_TESTDATA
	if (http.ReadHttpFile(username, errMsg))
#endif
	{
#if GENERATE_TESTDATA
{
std::ofstream raw(outTestData, std::ios::out);
raw << data.c_str();
raw.close();
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
		strcpy(pData, data.c_str());
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			tidyRelease(tdoc);
			delete [] pData;
			return tree;
		}
		tidyRelease(tdoc);
		// Note, Tidy does not null terminate the buffer.
		pData[len] = 0;
		data = pData;
		delete [] pData;
		pData = NULL;
#ifdef _DEBUG
//Test code to look at 'tidy'd data
//{
//std::string out(inAddress);
//out += ".out";
//std::ofstream raw(out, std::ios::out);
//raw << data.c_str();
//raw.close();
//}
#endif

		tstring err;
		tree = ElementNode::New();
		if (!tree->LoadXMLBuffer(data.c_str(), data.length(), err))
		{
			tree.reset();
			wxMessageBox(err.c_str(), wxMessageBoxCaptionStr, wxCENTRE);
		}
		else
		{
#ifdef _DEBUG
//Test code to look at xml data
//{
//std::string out(inAddress);
//out += ".tree";
//std::ofstream raw(out, std::ios::out);
//tree->SaveXML(raw);
//raw.close();
//}
#endif
		}
	}
	return tree;
}


// The only filtering of calendar events the website has is by type
// (titling/tourny/etc). So just ignore any supplied codes.
char* CCalendarSite::Process(
		char const* /*inLocCodes*/,
		char const* /*inVenueCodes*/,
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
	ElementNodePtr tree = ReadData(wxT("http://www.usdaa.com/events.cfm"), testData);
#endif
#else
	ElementNodePtr tree = ReadData(wxT("http://www.usdaa.com/events.cfm"));
#endif
	if (!tree)
		return NULL;

	if (progress)
	{
		if (progress->HasCanceled())
			return NULL;
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
	static const tstring tag(wxT("h4"));
	static const tstring name(wxT("Event Calendar"));
	if (tree->FindElementDeep(parentElement, idxEventCalH4tag, tag, &name))
	{
		int idxTable = parentElement->FindElement(wxT("table"), idxEventCalH4tag+1);
		if (0 <= idxTable)
		{
			ElementNodePtr table = parentElement->GetElementNode(idxTable);
			if (!table)
				return NULL;
			for (int i = 0; i < table->GetElementCount(); ++i)
			{
				if (Element::Element_Node != table->GetElement(i)->GetType())
					continue;
				ElementNodePtr tr = table->GetElementNode(i);
				if (!tr)
					continue;
				if (4 <= tr->GetNodeCount(Element::Element_Node))
				{
					tstring dates, club, detail, location;
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
							location = tr->GetElement(td)->GetValue();
							StripNewlines(location);
							break;
						case 3:
							// <td><a>type</a></td>
//Text[0]=["Event Type","1 - Titling Only Event&lt;BR&gt;2 - Titling Event with Tournament Classes&lt;BR&gt;3 - Titling Event with Junior Handler Classes&lt;BR&gt;4 - Titling Event with Tournament &amp; Junior Handler Classes&lt;BR&gt;5 - Tournament Only Event&lt;BR&gt;6 - Tournament Event with Junior Handler Classes&lt;BR&gt;7 - Junior Handler Only Event&lt;BR&gt;8 - Seminar&lt;BR&gt;9 - Seminar with Non-titling Match&lt;BR&gt;0 - Non-titling Match"];
							break;
						}
						++idx;
					}
					tstring::size_type pos = dates.find(wxT("-"));
					if (tstring::npos == pos)
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
			return NULL;
		progress->SetRange(0, nEntries);
	}

	int idxCal = -1;
	while (0 <= (idxCal = calTree->FindElement(TREE_CALENDAR, idxCal+1)))
	{
		if (progress)
		{
			if (progress->HasCanceled())
				return NULL;
			progress->StepIt();
		}
		ElementNodePtr cal = calTree->GetElementNode(idxCal);
		tstring detail = cal->GetValue();
		if (detail.empty())
			continue;
		cal->SetValue(wxT(""));
		wxString address(wxT("http://www.usdaa.com/"));
		address += detail.c_str();
		if (progress)
		{
			if (progress->HasCanceled())
				return NULL;
			std::string msg("Getting detail from ");
#ifdef UNICODE
			msg += tstringUtil::Convert(address.c_str());
#endif
			progress->SetMessage(msg.c_str());
		}
#if GENERATE_TESTDATA || USE_TESTDATA
		testData = TESTDATANAME;
		int idxAddr = address.Find('=');
		testData += address.Mid(idxAddr+1);
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
			static const tstring tag2(wxT("h3"));
			static const tstring name2(wxT("General Event Information"));
			ElementNodePtr parent;
			int idxEventCalH3tag = -1;
			if (treeDetail->FindElementDeep(parent, idxEventCalH3tag, tag2, &name2))
			{
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
										tstring date = table->GetElementNode(i)->GetElement(iTD)->GetValue();
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
											tstring email;
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
			}
		}
	}
	if (bOk)
	{
#ifdef _DEBUG
//Test code to look at generated calendar data
//{
//std::ofstream raw("c:\\events-caltree.xml", std::ios::out);
//calTree->SaveXML(raw);
//raw.close();
//}
#endif
		std::ostringstream s;
		calTree->SaveXML(s);
		return Allocate(s.str());
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) ICalendarSite* GetCalendarInterface()
{
	return new CCalendarSite();
}
