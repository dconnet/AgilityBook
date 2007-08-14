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
	ARBString err;
	Element::Initialize(err);
}

CCalendarSite::~CCalendarSite()
{
	Element::Terminate();
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

char* CCalendarSite::Process() const
{
	CStringA data;
	CReadHttp http(_T("http://www.usdaa.com/events.cfm"), data);

	CString username, errMsg;
	std::ostringstream s;
	if (http.ReadHttpFile(username, errMsg))
	{
	    TidyDoc tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
		if (0 > tidyParseString(tdoc, (LPCSTR)data))
			return NULL;
		if (0 > tidyCleanAndRepair(tdoc))
			return NULL;
		uint len = 0;
		tidySaveString(tdoc, NULL, &len);
		char* pData = data.GetBuffer(len+1);
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			data.ReleaseBuffer();
			return NULL;
		}
		data.ReleaseBuffer();

		ARBString err;
		Element tree;
		if (!tree.LoadXMLBuffer((LPCSTR)data, data.GetLength(), err))
		{
			AfxMessageBox(err.c_str());
			tree.SaveXML(s);
			return NULL;
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
	}
	return Allocate(s.str());
}

/////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) ICalendarSite* GetCalendarInterface()
{
	return new CCalendarSite();
}
