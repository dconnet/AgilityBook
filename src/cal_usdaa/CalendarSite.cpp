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
#include "../ARB/ARBStructure.h"
#include "../ARB/Element.h"
#include "../Win/ReadHttp.h"
#include "../tidy/include/tidy.h"
#include <sstream>

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

/////////////////////////////////////////////////////////////////////////////

class CCalendarSite : public ICalendarSite
{
public:
	CCalendarSite();
	~CCalendarSite();

	virtual void Release();
	virtual void releaseBuffer(char* pData) const;
	virtual char* GetDescription() const;
	virtual char* Process() const;
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

char* CCalendarSite::GetDescription() const
{
	return Allocate("TODO: This is a test");
}

static bool FindEventCalendarTag(
		ARBString const& tag,
		ARBString const& name,
		Element const& inTree,
		Element const*& outParentElement,
		int& outH4tag)
{
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& ele = inTree.GetElement(i);
		if (ele.GetName() == tag && ele.GetValue() == name)
		{
			outParentElement = &inTree;
			outH4tag = i;
			return true;
		}
		else if (FindEventCalendarTag(tag, name, ele, outParentElement, outH4tag))
			return true;
	}
	return false;
}

#include <fstream>
char* CCalendarSite::Process() const
{
	CStringA data;
	//CReadHttp http(_T("http://www.usdaa.com/events.cfm"), data);
//	CReadHttp http(_T("http://www.usdaa.com/events_detail.cfm?event_no=103312"), data);
	FILE* fp = fopen("c:\\events-raw.xml", "r");
	char buffer[1001];
	size_t sz;
	while (0 < (sz = fread(buffer, 1, 1000, fp)))
	{
		buffer[sz] = 0;
		data += buffer;
	}
	fclose(fp);

	CString username, errMsg;
//	if (http.ReadHttpFile(username, errMsg))
	{
//{
//std::ofstream raw("c:\\detail-raw.xml", std::ios::out);
//raw << (LPCSTR)data;
//raw.close();
//}
		TidyDoc tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
		tidyOptSetBool(tdoc, TidyNumEntities, yes);
		if (0 > tidyParseString(tdoc, (LPCSTR)data))
		{
			tidyRelease(tdoc);
			return NULL;
		}
		if (0 > tidyCleanAndRepair(tdoc))
		{
			tidyRelease(tdoc);
			return NULL;
		}
		tidyRunDiagnostics(tdoc);
		uint len = 0;
		if (tidySaveString(tdoc, NULL, &len) != -ENOMEM)
		{
			tidyRelease(tdoc);
			return NULL;
		}
		char* pData = data.GetBuffer(len);
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			tidyRelease(tdoc);
			data.ReleaseBuffer();
			return NULL;
		}
		tidyRelease(tdoc);
		// Note, Tidy does not null terminate the buffer. So we need to
		// truncate the buffer when releasing. CString may have allocated
		// more memory then we asked for.
		data.ReleaseBuffer(len);
//{
//std::ofstream raw("c:\\detail-rawout.xml", std::ios::out);
//raw << (LPCSTR)data;
//raw.close();
//}

		ARBString err;
		Element tree;
		if (!tree.LoadXMLBuffer((LPCSTR)data, data.GetLength(), err))
		{
			AfxMessageBox(err.c_str());
			return NULL;
		}
//{
//std::ofstream raw("c:\\detail-tree.xml", std::ios::out);
//tree.SaveXML(raw);
//raw.close();
//}

		bool bOk = false;
		Element calTree(TREE_BOOK);
		calTree.AddAttrib(ATTRIB_BOOK_VERSION, "12.6");

		// Ok, now we get to parse the raw html from USDAA. As of Aug 2007,
		// we want to look for a Level4 Header containing "Event Calendar"
		// The data we want is then contained in the 'table' tag that follows.
		Element const* parentElement = NULL;
		int idxEventCalH4tag = -1;
		static const ARBString tag(_T("h4"));
		static const ARBString name(_T("Event Calendar"));
		if (FindEventCalendarTag(tag, name, tree, parentElement, idxEventCalH4tag))
		{
			int idxTable = parentElement->FindElement(_T("table"), idxEventCalH4tag);
			if (0 > idxTable)
			{
				Element const& table = parentElement->GetElement(idxTable);
				// Start with the 2nd 'tr'
				for (int i = 1; i < table.GetElementCount(); ++i)
				{
					Element const& tr = table.GetElement(i);
					// Structure is:
					// <td>mm/dd/yyyy - mm/dd/yyyy</td>
					// <td><a href="events_detail.cfm?event_no=103312">Club</a></td>
					// <td>Location</td>
					// <td><a>type</a></td>
//Text[0]=["Event Type","1 - Titling Only Event&lt;BR&gt;2 - Titling Event with Tournament Classes&lt;BR&gt;3 - Titling Event with Junior Handler Classes&lt;BR&gt;4 - Titling Event with Tournament &amp; Junior Handler Classes&lt;BR&gt;5 - Tournament Only Event&lt;BR&gt;6 - Tournament Event with Junior Handler Classes&lt;BR&gt;7 - Junior Handler Only Event&lt;BR&gt;8 - Seminar&lt;BR&gt;9 - Seminar with Non-titling Match&lt;BR&gt;0 - Non-titling Match"];
//
					// href Detail is:
					// look for 'h3/General Event Information', data is in following table
					static const ARBString tag2(_T("h3"));
					static const ARBString name2(_T("General Event Information"));
					// <tr>
					//   <td><b>Event Date</b></td>
					//   <td>mm/dd/yyyy - mm/dd/yyyy</td>
					// <tr>
					//   <td><b>Club Name</b></td>
					//   <td>name</td>
					// <tr>
					//   <td><b>Location</b></td>
					//   <td>text<br/>test<br/>text</td>
					// <tr>
					//   <td><b>Closing Date</b></td>
					//   <td>mm/dd/yyyy</td>
					// <tr>
					//   <td><b>2<sup>nd</sup> Closing Date</b></td>
					//   <td>mm/dd/yyyy</td>
					// <tr>
					//   <td><b>Event Secretary</b></td>
					//   <td><a href="mailto...">name</a><br/>addr<br/></td>
					//note:Element will lose the text since there are subelements
				}
				return NULL;
			}
		}


		/*
		CString tmp(data);
		AfxMessageBox(tmp);
	//TODO
	s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		<< "<AgilityBook Book=\"12.6\">\n"

		<< "<Calendar"
		<< " Acc=\"C\""
		<< " Club=\"Test\""
		<< " DateStart=\"2007-9-5\""
		<< " DateEnd=\"2007-9-6\""
		<< " Entered=\"P\""
		<< " Location=\"Prunedale, CA\""
		<< " Venue=\"USDAA\"/>"

		<< "</AgilityBook>\n";
		*/

		if (bOk)
		{
			std::ostringstream s;
			calTree.SaveXML(s);
			return Allocate(s.str());
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) ICalendarSite* GetCalendarInterface()
{
	return new CCalendarSite();
}
