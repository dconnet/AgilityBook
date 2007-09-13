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

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "DlgProgress.h"
#include "Element.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CProgressMeter : public IProgressMeter
{
public:
	CProgressMeter(int nEntries, CWnd* pParent);
	~CProgressMeter();

	void SetForegroundWindow();
	void Dismiss();
	void StepMe();

	virtual void SetMessage(char const* pMessage);
	virtual void SetRange(int inLower, int inUpper);
	virtual void SetStep(int inStep);
	virtual void StepIt();
	virtual void SetPos(int pos);
	virtual int GetPos();

private:
	int m_nEntries;
	IDlgProgress* m_pProgress;
};


CProgressMeter::CProgressMeter(int nEntries, CWnd* pParent)
	: m_nEntries(nEntries)
	, m_pProgress(NULL)
{
	m_pProgress = IDlgProgress::CreateProgress(pParent);
	if (1 < m_nEntries)
	{
		m_pProgress->SetNumProgressBars(2);
		m_pProgress->SetRange(1, 0, m_nEntries);
	}
	else
		m_pProgress->SetNumProgressBars(1);
	m_pProgress->Show();
}


CProgressMeter::~CProgressMeter()
{
	if (m_pProgress)
		m_pProgress->Dismiss();
}


void CProgressMeter::SetForegroundWindow()
{
	if (m_pProgress)
		m_pProgress->SetForegroundWindow();
}


void CProgressMeter::Dismiss()
{
	if (m_pProgress)
	{
		m_pProgress->Dismiss();
		m_pProgress = NULL;
	}
}


void CProgressMeter::StepMe()
{
	if (1 < m_nEntries)
		m_pProgress->StepIt(1);
}


void CProgressMeter::SetMessage(char const* pMessage)
{
	if (m_pProgress)
	{
		CString msg;
		if (pMessage)
			msg = pMessage;
		m_pProgress->SetMessage((LPCTSTR)msg);
	}
}


void CProgressMeter::SetRange(int inLower, int inUpper)
{
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->SetRange(nBar, inLower, inUpper);
	}
}


void CProgressMeter::SetStep(int inStep)
{
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->SetStep(nBar, inStep);
	}
}


void CProgressMeter::StepIt()
{
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->StepIt(nBar);
	}
}


void CProgressMeter::SetPos(int pos)
{
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->SetPos(nBar, pos);
	}
}


int CProgressMeter::GetPos()
{
	int pos = -1;
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->GetPos(nBar, pos);
	}	
	return pos;
}

/////////////////////////////////////////////////////////////////////////////

class CalSiteData
{
private:
	// Copy semantics don't work well with our cleanup code!
	CalSiteData(CalSiteData const&);
	CalSiteData& operator=(CalSiteData const&);
public:
	CalSiteData(CString const& pathname, CString const& filename);
	~CalSiteData();

	void Connect(CString const& pathname);

	bool isValid() const					{return NULL != m_pSite;}
	void Unload(bool bPermanently = false);

	CStringA GetName();
	CStringA GetDescription();
	CStringA Process(IProgressMeter *progress);

private:
	CString m_FileName;
	HINSTANCE m_hDllInst;
	ICalendarSite* m_pSite;
};


typedef boost::shared_ptr<CalSiteData> CalSiteDataPtr;


CalSiteData::CalSiteData(CString const& pathname, CString const& filename)
	: m_FileName(filename)
	, m_hDllInst(NULL)
	, m_pSite(NULL)
{
	Connect(pathname);
}


CalSiteData::~CalSiteData()
{
	Unload();
}


void CalSiteData::Connect(CString const& pathname)
{
	if (isValid())
		return;
	// Load the library.
	if (CAgilityBookOptions::IsCalSiteVisible(m_FileName))
	{
		if (!m_hDllInst)
		{
			CString path(pathname);
			path += m_FileName;
			m_hDllInst = LoadLibrary(path);
		}
		if (m_hDllInst)
		{
			// Get the exported interface
			GETCALENDARINTERFACE pApi = reinterpret_cast<GETCALENDARINTERFACE>(GetProcAddress(m_hDllInst, "GetCalendarInterface"));
			if (pApi)
			{
				// And call it.
				m_pSite = pApi();
				// We now have an object that must be released later.
			}
		}
	}
}


void CalSiteData::Unload(bool bPermanently)
{
	if (m_pSite)
	{
		try
		{
			m_pSite->Release();
		}
		catch(...)
		{
		}
		m_pSite = NULL;
	}
	if (m_hDllInst)
	{
		FreeLibrary(m_hDllInst);
		m_hDllInst = NULL;
	}
	if (bPermanently)
	{
		CAgilityBookOptions::SuppressCalSite(m_FileName, true);
	}
}


CStringA CalSiteData::GetName()
{
	CStringA data;
	if (m_pSite)
	{
		char* pData = NULL;
		try
		{
			pData = m_pSite->GetName();
		}
		catch (...)
		{
			Unload(true);
		}
		if (pData)
			data = CStringA(pData);
		try
		{
			m_pSite->releaseBuffer(pData);
		}
		catch (...)
		{
			Unload(true);
		}
	}
	return data;
}


CStringA CalSiteData::GetDescription()
{
	CStringA data;
	if (m_pSite)
	{
		char* pData = NULL;
		try
		{
			pData = m_pSite->GetDescription();
		}
		catch (...)
		{
			Unload(true);
		}
		if (pData)
			data = CStringA(pData);
		try
		{
			m_pSite->releaseBuffer(pData);
		}
		catch (...)
		{
			Unload(true);
		}
	}
	return data;
}


CStringA CalSiteData::Process(IProgressMeter *progress)
{
	CStringA data;
	if (m_pSite)
	{
		char* pData = NULL;
		try
		{
			pData = m_pSite->Process(progress);
		}
		catch (...)
		{
			Unload(true);
		}
		if (pData)
			data = CStringA(pData);
		try
		{
			m_pSite->releaseBuffer(pData);
		}
		catch (...)
		{
			Unload(true);
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////

class CCalendarSitesImpl
{
public:
	CCalendarSitesImpl();
	~CCalendarSitesImpl();

	bool hasActiveSites() const;

	void ManageSites(ARBConfig const& inConfig);
	bool UpdateSites(ARBConfig const& inConfig);
	bool FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent);

private:
	CString m_PathName;
	std::map<CString, CalSiteDataPtr> m_DirectAccess;
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
		m_PathName = exeName.Left(iLastSlash + 1);
}


CCalendarSitesImpl::~CCalendarSitesImpl()
{
	m_DirectAccess.clear();
}


bool CCalendarSitesImpl::hasActiveSites() const
{
	for (std::map<CString, CalSiteDataPtr>::const_iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		if ((*i).second->isValid())
			return true;
	}
	return false;
}


void CCalendarSitesImpl::ManageSites(ARBConfig const& inConfig)
{
	AfxMessageBox(_T("TODO: Manage CalSite data"));
}


bool CCalendarSitesImpl::UpdateSites(ARBConfig const& inConfig)
{
	// Load auxilary DLLs from the path where the EXE lives.
	if (!m_PathName.IsEmpty())
	{
		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(m_PathName + _T("cal_*.dll"), &data);
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
					m_DirectAccess[filename] = CalSiteDataPtr(new CalSiteData(m_PathName, filename));
				}
				else
				{
					m_DirectAccess[filename]->Connect(m_PathName);
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
	int nEntries = 0;
	std::map<CString, CalSiteDataPtr>::iterator i;
	for (i = m_DirectAccess.begin(); i != m_DirectAccess.end(); ++i)
	{
		if ((*i).second->isValid())
			++nEntries;
	}
	if (0 == nEntries)
		return false;

	//TODO: Add a UI if there is more than 1 entry so user can select which sites
	CProgressMeter progress(nEntries, pParent);

	CWaitCursor wait;
	std::map<CString, std::list<ARBCalendarPtr> > newEntries;
	for (i = m_DirectAccess.begin(); i != m_DirectAccess.end(); ++i)
	{
		progress.StepMe();
		if (!(*i).second->isValid())
			continue;
		CStringA n = (*i).second->GetName();
		CStringA d = (*i).second->GetDescription();
		CStringA data = (*i).second->Process(&progress);
		ElementNodePtr tree(ElementNode::New());
		tstring errMsg;
		bool bOk = false;
		if (!data.IsEmpty())
		{
			bOk = tree->LoadXMLBuffer((LPCSTR)data, data.GetLength(), errMsg);
			data.Empty();
		}
		if (bOk)
		{
			CErrorCallback err;
			ARBAgilityRecordBook book;
			bOk = book.Load(tree, true, false, false, false, false, err);
			tree.reset();
			if (bOk)
			{
				if (0 < err.m_ErrMsg.length())
				{
					AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONINFORMATION);
					progress.SetForegroundWindow();
					wait.Restore();
				}
				for (ARBCalendarList::iterator iter = book.GetCalendar().begin(); iter != book.GetCalendar().end(); ++iter)
				{
					newEntries[(*i).first].push_back(*iter);
				}
			}
		}
		else
		{
			CString str((*i).second->GetName());
			CString err;
			err.FormatMessage(IDS_ERR_PARSING_DATA, (LPCTSTR)str);
			if (!errMsg.empty())
			{
				err += ":\n\t";
				err += errMsg.c_str();
			}
			err += "\n\n";
			str.LoadString(IDS_USE_PLUGIN);
			err += str;
			if (IDNO == AfxMessageBox(err, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
				(*i).second->Unload(true);
			progress.SetForegroundWindow();
			wait.Restore();
		}
	}
	progress.Dismiss();

	if (0 < newEntries.size())
	{
		// TODO: include UI that will allow me to select which entries to add
		int nAdded = 0;
		int nUpdated = 0;
		for (std::map<CString, std::list<ARBCalendarPtr> >::iterator iEntries = newEntries.begin();
			iEntries != newEntries.end();
			++iEntries)
		{
			for (std::list<ARBCalendarPtr>::iterator iCal = (*iEntries).second.begin();
				iCal != (*iEntries).second.end();
				++iCal)
			{
				ARBCalendarPtr cal = *iCal;
				ARBCalendarPtr calFound;
				if (!pDoc->GetCalendar().FindCalendar(cal, false, &calFound))
				{
					if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
					{
						pDoc->GetCalendar().AddCalendar(cal);
						++nAdded;
					}
				}
				else
				{
					if (calFound->Update(cal))
						++nUpdated;
				}
			}
		}
		if (0 < nAdded + nUpdated)
		{
			pDoc->GetCalendar().sort();
			pDoc->UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
			pDoc->SetModifiedFlag();
		}
		CString str;
		str.FormatMessage(IDS_UPDATED_CAL_ITEMS, nAdded, nUpdated);
		AfxMessageBox(str, MB_ICONINFORMATION);
	}
	return true;
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


void CCalendarSites::ManageSites(ARBConfig const& inConfig)
{
	m_Impl->ManageSites(inConfig);
}


bool CCalendarSites::UpdateSites(ARBConfig const& inConfig)
{
	return m_Impl->UpdateSites(inConfig);
}


bool CCalendarSites::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent)
{
	return m_Impl->FindEntries(pDoc, inCalendar, pParent);
}
