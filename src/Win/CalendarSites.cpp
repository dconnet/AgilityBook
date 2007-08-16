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
 * @brief Manage the direct download of calendar data from the web.
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "CalendarSites.h"

#include <map>
#include <vector>
#include "AgilityBookDoc.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"
#include "ICalendarSite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CCalendarSitesImpl
{
public:
	CCalendarSitesImpl();
	~CCalendarSitesImpl();

	bool hasActiveSites() const;

	void clear();
	bool Update(ARBConfig const& inConfig);

	bool FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent);

private:
	CString m_PathName;
	typedef std::pair<HINSTANCE, ICalendarSite*> AccessData;
	std::map<CString, AccessData> m_DirectAccess;
};

CCalendarSitesImpl::CCalendarSitesImpl()
	: m_PathName()
{
	CString exeName;
	TCHAR* pName = exeName.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL, pName, MAX_PATH);
	exeName.ReleaseBuffer();
	int iLastSlash = exeName.ReverseFind('\\');
	if (0 < iLastSlash)
		m_PathName = exeName.Left(iLastSlash) + _T("\\cal_*.dll");
}

CCalendarSitesImpl::~CCalendarSitesImpl()
{
	for (std::map<CString, AccessData>::iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		(*i).second.second->Release();
		FreeLibrary((*i).second.first);
	}
	m_DirectAccess.clear();
}

bool CCalendarSitesImpl::hasActiveSites() const
{
	return 0 < m_DirectAccess.size() /* || ARBConfigCalSite-stuff */;
}

void CCalendarSitesImpl::clear()
{
	// Don't clear m_DirectAccess
}

bool CCalendarSitesImpl::Update(ARBConfig const& inConfig)
{
	// Load auxilary DLLs from the path where the EXE lives.
	if (!m_PathName.IsEmpty())
	{
		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(m_PathName, &data);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				CString filename(data.cFileName);
				// Only load the library if we haven't already loaded it.
				// (Otherwise we get a memory leak because we overwrite the
				// api pointer)
				if (m_DirectAccess.end() == m_DirectAccess.find(filename))
				{
					// Load the library.
					HINSTANCE hInst = LoadLibrary(filename);
					if (hInst)
					{
						// Now get the exported interface
						GETCALENDARINTERFACE pApi = reinterpret_cast<GETCALENDARINTERFACE>(GetProcAddress(hInst, "GetCalendarInterface"));
						if (pApi)
						{
							// And call it.
							ICalendarSite* pSite = pApi();
							if (pSite)
							{
								// We now have an object that must be released later.
								m_DirectAccess[filename] = AccessData(hInst, pSite);
							}
						}
					}
				}
			}
			while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}
	return false;
}

bool CCalendarSitesImpl::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent)
{
	//TODO: Add a UI if there is more than 1 entry so user can select which sites
AfxMessageBox(_T("TODO"));
	CWaitCursor wait;
	for (std::map<CString, AccessData>::iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		ICalendarSite* pSite = (*i).second.second;
		char* data = pSite->Process();
		if (data)
		{
			ElementNodePtr tree(ElementNode::New());
			ARBString errMsg;
			if (!tree->LoadXMLBuffer(data, static_cast<unsigned int>(strlen(data)), errMsg))
			{
				if (0 < errMsg.length())
					AfxMessageBox(errMsg.c_str(), MB_ICONWARNING);
			}
			else
			{
				pDoc->ImportARBCalData(tree, pParent);
			}
			pSite->releaseBuffer(data);
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CCalendarSites::CCalendarSites()
	: m_Impl(new CCalendarSitesImpl())
{
}

CCalendarSites::~CCalendarSites()
{
	delete m_Impl;
}

bool CCalendarSites::hasActiveSites() const
{
	return m_Impl->hasActiveSites();
}

void CCalendarSites::clear()
{
	m_Impl->clear();
}

bool CCalendarSites::Update(ARBConfig const& inConfig)
{
	return m_Impl->Update(inConfig);
}

bool CCalendarSites::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent)
{
	return m_Impl->FindEntries(pDoc, inCalendar, pParent);
}
