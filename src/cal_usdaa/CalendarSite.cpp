/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"

#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "../ARB/ARBStructure.h"
#include "../ARB/Element.h"
#include "../Win/ReadHttp.h"
#include "../tidy/include/tidy.h"
#include <sstream>
#ifdef _DEBUG
#include <fstream>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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


static ARBString mdy2ymd(ARBString const& inDate)
{
	ARBString date;
	ARBString::size_type pos = 0;
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


static void StripNewlines(ARBString& inStr)
{
	ARBString::size_type pos = inStr.find_first_of(_T("\n"));
	while (ARBString::npos != pos)
	{
		inStr.replace(pos, 1, 1, ' ');
		pos = inStr.find_first_of(_T("\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////

class CCalendarSite : public ICalendarSite
{
public:
	CCalendarSite();
	~CCalendarSite();

	virtual void Release();
	virtual void releaseBuffer(char* pData) const;
	virtual char* GetName() const;
	virtual char* GetDescription() const;
	virtual char* Process(IProgressMeter* progress) const;
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
	static const std::string desc("Get all the events from USDAA. This will parse the HTML returned from http://www.usdaa.com/events.cfm.");
	return Allocate(desc);
}


static ElementNodePtr ReadData(CString const& inAddress)
{
	ElementNodePtr tree;

	CStringA data;
	CReadHttp http(inAddress, data);
//	CStringA address(inAddress);
//	FILE* fp = fopen(inAddress, "r");
//	char buffer[1001];
//	size_t sz;
//	while (0 < (sz = fread(buffer, 1, 1000, fp)))
//	{
//		buffer[sz] = 0;
//		data += buffer;
//	}
//	fclose(fp);

	CString username, errMsg;
	if (http.ReadHttpFile(username, errMsg))
	{
#ifdef _DEBUG
//{
//std::ofstream raw("c:\\events-raw.xml", std::ios::out);
//raw << (LPCSTR)data;
//raw.close();
//}
#endif
		TidyDoc tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
		tidyOptSetBool(tdoc, TidyNumEntities, yes);
		if (0 > tidyParseString(tdoc, (LPCSTR)data))
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
		char* pData = data.GetBuffer(len);
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			tidyRelease(tdoc);
			data.ReleaseBuffer();
			return tree;
		}
		tidyRelease(tdoc);
		// Note, Tidy does not null terminate the buffer. So we need to
		// truncate the buffer when releasing. CString may have allocated
		// more memory then we asked for.
		data.ReleaseBuffer(len);
#ifdef _DEBUG
{
//std::string out(address);
//out += ".out";
//std::ofstream raw(out.c_str(), std::ios::out);
//raw << (LPCSTR)data;
//raw.close();
}
#endif

		ARBString err;
		tree = ElementNode::New();
		if (!tree->LoadXMLBuffer((LPCSTR)data, data.GetLength(), err))
		{
			tree.reset();
			AfxMessageBox(err.c_str());
		}
		else
		{
#ifdef _DEBUG
{
//std::string out(address);
//out += ".tree";
//std::ofstream raw(out.c_str(), std::ios::out);
//tree->SaveXML(raw);
//raw.close();
}
#endif
		}
	}
	return tree;
}


char* CCalendarSite::Process(IProgressMeter* progress) const
{
	if (progress)
		progress->SetMessage("Reading http://www.usdaa.com/events.cfm");

	ElementNodePtr tree = ReadData(_T("http://www.usdaa.com/events.cfm"));
//	ElementNodePtr tree = ReadData("c:\\events-raw.xml");
	if (!tree)
		return NULL;

	if (progress)
		progress->SetMessage("Finding entries...");

	bool bOk = false;
	ElementNodePtr calTree(ElementNode::New(TREE_BOOK));
	calTree->AddAttrib(ATTRIB_BOOK_VERSION, _T("12.6"));

	// Ok, now we get to parse the raw html from USDAA. As of Aug 2007,
	// we want to look for a Level4 Header containing "Event Calendar"
	// The data we want is then contained in the 'table' tag that follows.
	int nEntries = 0;
	ElementNodePtr parentElement;
	int idxEventCalH4tag = -1;
	static const ARBString tag(_T("h4"));
	static const ARBString name(_T("Event Calendar"));
	if (tree->FindElementDeep(parentElement, idxEventCalH4tag, tag, &name))
	{
		int idxTable = parentElement->FindElement(_T("table"), idxEventCalH4tag+1);
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
					ARBString dates, club, detail, location;
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
								int idxA = tr->GetElementNode(td)->FindElement(_T("a"));
								if (0 <= idxA)
								{
									club = tr->GetElementNode(td)->GetElement(idxA)->GetValue();
									tr->GetElementNode(td)->GetElementNode(idxA)->GetAttrib(_T("href"), detail);
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
					ARBString::size_type pos = dates.find(_T("-"));
					if (ARBString::npos == pos)
						continue;
					bOk = true;
					ElementNodePtr cal = calTree->AddElementNode(TREE_CALENDAR);
					cal->AddAttrib(ATTRIB_CAL_START, mdy2ymd(dates.substr(0, pos)));
					cal->AddAttrib(ATTRIB_CAL_END, mdy2ymd(dates.substr(pos+1)));
					cal->AddAttrib(ATTRIB_CAL_VENUE, _T("USDAA"));
					cal->AddAttrib(ATTRIB_CAL_CLUB, club);
					cal->AddAttrib(ATTRIB_CAL_LOCATION, location);
					cal->SetValue(detail);
					++nEntries;
				}
			}
		}
	}

	if (progress)
		progress->SetRange(0, nEntries);

	int idxCal = -1;
	while (0 <= (idxCal = calTree->FindElement(TREE_CALENDAR, idxCal+1)))
	{
		if (progress)
			progress->StepIt();
		ElementNodePtr cal = calTree->GetElementNode(idxCal);
		ARBString detail = cal->GetValue();
		if (detail.empty())
			continue;
		cal->SetValue(_T(""));
		CString address(_T("http://www.usdaa.com/"));
		address += detail.c_str();
		if (progress)
		{
			CStringA msg("Getting detail from ");
			msg += address;
			progress->SetMessage((LPCSTR)msg);
		}
		ElementNodePtr treeDetail = ReadData(address);
//		ElementNodePtr treeDetail = ReadData("c:\\detail-raw.xml");
		if (treeDetail)
		{
			static const ARBString tag2(_T("h3"));
			static const ARBString name2(_T("General Event Information"));
			ElementNodePtr parent;
			int idxEventCalH3tag = -1;
			if (treeDetail->FindElementDeep(parent, idxEventCalH3tag, tag2, &name2))
			{
				int idxTable = parent->FindElement(_T("table"), idxEventCalH3tag+1);
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
									int iTD = table->GetElementNode(i)->FindElement(_T("td"), 0);
									iTD = table->GetElementNode(i)->FindElement(_T("td"), iTD+1);
									if (0 <= iTD)
									{
										ARBString date = table->GetElementNode(i)->GetElement(iTD)->GetValue();
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
									int iTD = table->GetElementNode(i)->FindElement(_T("td"), 0);
									iTD = table->GetElementNode(i)->FindElement(_T("td"), iTD+1);
									ElementNodePtr td = table->GetElementNode(i)->GetElementNode(iTD);
									if (td)
									{
										iTD = td->FindElement(_T("a"));
										if (0 <= iTD)
										{
											ARBString email;
											td->GetElementNode(iTD)->GetAttrib(_T("href"), email);
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
{
std::ofstream raw("c:\\events-caltree.xml", std::ios::out);
calTree->SaveXML(raw);
raw.close();
}
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
