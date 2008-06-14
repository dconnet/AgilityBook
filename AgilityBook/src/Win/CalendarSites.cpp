/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "CheckTreeCtrl.h"
#include "DlgAssignColumns.h"
#include "DlgBaseDialog.h"
#include "DlgCalendarQueryDetail.h"
#include "DlgPluginDetails.h"
#include "DlgProgress.h"
#include "Element.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "ReadHttp.h"
#include <map>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static tstring TranslateCodeMap(std::vector<tstring> const& inCodes)
{
	otstringstream codes;
	for (size_t i = 0; i < inCodes.size(); ++i)
	{
		if (0 < i)
			codes << ':';
		codes << inCodes[i];
	}
	return codes.str();
}


static size_t TranslateCodeMap(
		std::map<tstring, tstring> const& inMap,
		std::vector<tstring>& outKeys)
{
	outKeys.clear();
	for (std::map<tstring, tstring>::const_iterator iMap = inMap.begin();
		iMap != inMap.end();
		++iMap)
	{
		outKeys.push_back(iMap->first);
	}
	return outKeys.size();
}

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

	void Connect();

	bool isValid() const					{return NULL != m_pSite;}
	void Unload(bool bPermanently = false);

	CString GetName() const							{return m_Name;}
	CString GetDescription() const					{return m_Desc;}
	std::map<tstring, tstring> const& QueryLocationCodes() const
		{return m_LocCodes;}
	std::map<tstring, tstring> const& QueryVenueCodes() const
		{return m_VenueCodes;}
	CStringA Process(IProgressMeter *progress,
			std::vector<tstring> const& inLocationCodes,
			std::vector<tstring> const& inVenueCodes);

private:
	void Clear();

	CString m_Pathname;
	CString m_FileName;
	HINSTANCE m_hDllInst;
	CVersionNum m_Version;
	ICalendarSite* m_pSite;
	CString m_Name;
	CString m_Desc;
	std::map<tstring, tstring> m_LocCodes;
	std::map<tstring, tstring> m_VenueCodes;
};


typedef tr1::shared_ptr<CalSiteData> CalSiteDataPtr;

/////////////////////////////////////////////////////////////////////////////

class CDlgCalendarPlugins : public CDlgBaseDialog
{
public:
	CDlgCalendarPlugins(
			CAgilityBookDoc* pDoc,
			std::map<CString, CalSiteDataPtr>& directAccess,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgCalendarPlugins)
	enum { IDD = IDD_CALENDAR_PLUGINS };
	CCheckTreeCtrl	m_ctrlPlugins;
	CEdit	m_ctrlDetails;
	CButton	m_ctrlRead;
	CButton	m_ctrlAdd;
	CButton	m_ctrlEnable;
	CButton	m_ctrlQuery;
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	std::map<CString, CalSiteDataPtr>& m_DirectAccess;

	//{{AFX_VIRTUAL(CDlgCalendarPlugins)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	void SortPlugins();

	//{{AFX_MSG(CDlgCalendarPlugins)
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnDeleteitemPluginTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkPluginTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedPluginTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSetdispinfoPluginTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfoPluginTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPluginRead();
	afx_msg void OnPluginAddCalEntry();
	afx_msg void OnPluginEnable();
	afx_msg void OnPluginQueryDetails();
	afx_msg void OnPluginNew();
	afx_msg void OnPluginEdit();
	afx_msg void OnPluginDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

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

CalSiteData::CalSiteData(CString const& pathname, CString const& filename)
	: m_Pathname(pathname)
	, m_FileName(filename)
	, m_hDllInst(NULL)
	, m_Version()
	, m_pSite(NULL)
	, m_Name()
	, m_Desc()
	, m_LocCodes()
	, m_VenueCodes()
{
	Connect();
}


CalSiteData::~CalSiteData()
{
	Unload();
}


void CalSiteData::Clear()
{
	m_Name.Empty();
	m_Desc.Empty();
	m_LocCodes.clear();
	m_VenueCodes.clear();
}


void CalSiteData::Connect()
{
	if (isValid())
	{
		// Conditions may have changed.
		if (!CAgilityBookOptions::IsCalSiteVisible(m_FileName, m_Version))
		{
			Unload(true);
		}
		return;
	}
	Clear();
	// Load the library.
	if (!m_hDllInst)
	{
		CString path(m_Pathname);
		path += m_FileName;
		m_hDllInst = LoadLibrary(path);
		m_Version = CVersionNum(m_hDllInst);
	}
	if (m_hDllInst && CAgilityBookOptions::IsCalSiteVisible(m_FileName, m_Version))
	{
		// Get the exported interface
		GETCALENDARINTERFACE pApi = reinterpret_cast<GETCALENDARINTERFACE>(GetProcAddress(m_hDllInst, "GetCalendarInterface"));
		if (pApi)
		{
			// And call it.
			m_pSite = pApi();
			// We now have an object that must be released later.
			char* pData = NULL;
			try
			{
				pData = m_pSite->GetName();
			}
			catch (...)
			{
				pData = NULL;
				Unload(true);
			}
			if (pData)
			{
				m_Name = CStringA(pData);
				try
				{
					m_pSite->releaseBuffer(pData);
				}
				catch (...)
				{
					Unload(true);
				}
			}
			if (m_pSite)
			{
				try
				{
					pData = m_pSite->GetDescription();
				}
				catch (...)
				{
					pData = NULL;
					Unload(true);
				}
				if (pData)
				{
					m_Desc = CStringA(pData);
					try
					{
						m_pSite->releaseBuffer(pData);
					}
					catch (...)
					{
						Unload(true);
					}
				}
			}
			if (m_pSite)
			{
				try
				{
					pData = m_pSite->GetLocationCodes();
				}
				catch (...)
				{
					Unload(true);
					pData = NULL;
				}
				if (pData)
				{
					CString data1(pData); // For ansi/unicode translation
					tstring data((LPCTSTR)data1);
					data1.Empty();
					std::vector<tstring> fields;
					if (0 < BreakLine('\n', data, fields))
					{
						for (std::vector<tstring>::iterator i = fields.begin();
							i != fields.end();
							++i)
						{
							std::vector<tstring> subfields;
							if (2 == BreakLine(':', *i, subfields))
							{
								m_LocCodes[subfields[0]] = subfields[1];
							}
						}
					}
					try
					{
						m_pSite->releaseBuffer(pData);
					}
					catch (...)
					{
						Unload(true);
					}
				}
			}
			if (m_pSite)
			{
				try
				{
					pData = m_pSite->GetVenueCodes();
				}
				catch (...)
				{
					Unload(true);
					pData = NULL;
				}
				if (pData)
				{
					CString data1(pData);
					tstring data((LPCTSTR)data1);
					data1.Empty();
					std::vector<tstring> fields;
					if (0 < BreakLine('\n', data, fields))
					{
						for (std::vector<tstring>::iterator i = fields.begin();
							i != fields.end();
							++i)
						{
							std::vector<tstring> subfields;
							switch (BreakLine(':', *i, subfields))
							{
							case 1:
								m_VenueCodes[subfields[0]] = subfields[0];
								break;
							case 2:
								m_VenueCodes[subfields[0]] = subfields[1];
								break;
							}
						}
					}
					try
					{
						m_pSite->releaseBuffer(pData);
					}
					catch (...)
					{
						Unload(true);
					}
				}
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


CStringA CalSiteData::Process(IProgressMeter *progress,
		std::vector<tstring> const& inLocationCodes,
		std::vector<tstring> const& inVenueCodes)
{
	CStringA data;
	if (m_pSite)
	{
		CStringA locCodes(TranslateCodeMap(inLocationCodes).c_str());
		CStringA venueCodes(TranslateCodeMap(inVenueCodes).c_str());
		char* pData = NULL;
		try
		{
			const char* pLocCodes = NULL;
			if (!locCodes.IsEmpty())
				pLocCodes = locCodes;
			const char* pVenueCodes = NULL;
			if (!venueCodes.IsEmpty())
				pVenueCodes = venueCodes;
			pData = m_pSite->Process(pLocCodes, pVenueCodes, progress);
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

	bool FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent);

private:
	CString m_PathName;
	// Map of filenames to site pointers
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
					m_DirectAccess[filename]->Connect();
				}
			}
			while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}
}


CCalendarSitesImpl::~CCalendarSitesImpl()
{
	m_DirectAccess.clear();
}


bool CCalendarSitesImpl::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent)
{
	CDlgCalendarPlugins dlg(pDoc, m_DirectAccess, pParent);
	if (IDOK != dlg.DoModal())
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class CPluginBase
{
public:
	CPluginBase() : m_hItem(NULL) {}
	virtual ~CPluginBase() {}

	HTREEITEM GetHTreeItem() const		{return m_hItem;}
	void SetHTreeItem(HTREEITEM hItem)	{m_hItem = hItem;}

	virtual CString GetName() const = 0;
	virtual CString GetDesc() const = 0;

private:
	HTREEITEM m_hItem;
};


class CPluginData : public CPluginBase
{
public:
	CPluginData() {}
	virtual CString GetSortName() const = 0;
	virtual CString GetName() const					{return m_Name;}
	virtual CString GetDesc() const					{return m_Desc;}
	virtual CStringA Process(IProgressMeter *progress) = 0;
	virtual bool HasQueryDetails() const = 0;
	virtual bool CanEdit() const					{return false;}
	virtual bool Edit(CWnd* pParent)				{return false;}
	virtual bool CanDelete() const					{return false;}
	virtual bool Delete()							{return false;}
	virtual std::map<tstring, tstring> const& QueryLocationCodes() const = 0;
	virtual std::map<tstring, tstring> const& QueryVenueCodes() const = 0;
	virtual std::vector<tstring>& LocationCodes()	{return m_LocationCodes;}
	virtual std::vector<tstring>& VenueCodes()		{return m_VenueCodes;}
	virtual bool isValid() const = 0;
	virtual bool Enable() = 0;
	virtual bool CanDisable() const = 0;
	virtual void Disable() = 0;
protected:
	CString m_Name;
	CString m_Desc;
	std::vector<tstring> m_LocationCodes;
	std::vector<tstring> m_VenueCodes;
};


class CPluginConfigData : public CPluginData
{
public:
	CPluginConfigData(CAgilityBookDoc* pDoc, ARBConfigCalSitePtr inSite)
		: m_pDoc(pDoc)
		, m_OrigSite(inSite)
		, m_Site(inSite->Clone())
		, m_Enabled(true)
	{
		m_Name = m_Site->GetName().c_str();
		m_Desc = m_Site->GetDescription().c_str();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	virtual CString GetSortName() const				{return _T("C") + m_Name;}

	virtual CStringA Process(IProgressMeter *progress);

	virtual bool HasQueryDetails() const
	{
		return 1 < m_Site->LocationCodes().size() || 1 < m_Site->VenueCodes().size();
	}

	virtual bool CanEdit() const		{return true;}
	virtual bool Edit(CWnd* pParent);
	virtual bool CanDelete() const		{return true;}
	virtual bool Delete();

	virtual std::map<tstring, tstring> const& QueryLocationCodes() const
	{
		return m_Site->LocationCodes();
	}

	virtual std::map<tstring, tstring> const& QueryVenueCodes() const
	{
		return m_Site->VenueCodes();
	}

	virtual bool isValid() const
	{
		return m_Enabled;
	}

	virtual bool Enable()
	{
		m_Enabled = true;
		return true;
	}

	virtual bool CanDisable() const
	{
		return false;
	}

	virtual void Disable()
	{
		m_Enabled = false;
	}

private:
	CAgilityBookDoc* m_pDoc;
	ARBConfigCalSitePtr m_OrigSite;
	ARBConfigCalSitePtr m_Site;
	bool m_Enabled;
};


CStringA CPluginConfigData::Process(IProgressMeter *progress)
{
	CWaitCursor wait;
	tstring url = m_Site->GetFormattedURL(m_LocationCodes, m_VenueCodes);
	CStringA data(url.c_str());
	progress->SetMessage(data);
	data.Empty();
	CReadHttp http(url.c_str(), data);
	CString username, errMsg;
	if (!http.ReadHttpFile(username, errMsg))
		data.Empty();
	return data;
}


bool CPluginConfigData::Edit(CWnd* pParent)
{
	CDlgPluginDetails dlg(m_pDoc->Book().GetConfig(), m_Site, pParent);
	if (IDOK == dlg.DoModal())
	{
		m_Name = m_Site->GetName().c_str();
		m_Desc = m_Site->GetDescription().c_str();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
		*m_OrigSite = *m_Site;
		m_pDoc->SetModifiedFlag(TRUE);
		return true;
	}
	else
		return false;
}


bool CPluginConfigData::Delete()
{
	if (m_pDoc->Book().GetConfig().GetCalSites().DeleteSite(m_OrigSite->GetName()))
	{
		m_pDoc->SetModifiedFlag(TRUE);
		m_OrigSite.reset();
		return true;
	}
	return false;
}


class CPluginDllData : public CPluginData
{
public:
	CPluginDllData(CString const& filename, CalSiteDataPtr calData)
		: m_Filename(filename)
		, m_CalData(calData)
	{
		assert(m_CalData);
		SetNameDesc();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	virtual CString GetSortName() const				{return _T("D") + m_Name;}

	virtual CStringA Process(IProgressMeter *progress)
	{
		return m_CalData->Process(progress, m_LocationCodes, m_VenueCodes);
	}

	virtual bool HasQueryDetails() const
	{
		return 1 < m_CalData->QueryLocationCodes().size() || 1 < m_CalData->QueryVenueCodes().size();
	}

	virtual std::map<tstring, tstring> const& QueryLocationCodes() const
	{
		return m_CalData->QueryLocationCodes();
	}

	virtual std::map<tstring, tstring> const& QueryVenueCodes() const
	{
		return m_CalData->QueryVenueCodes();
	}

	virtual bool isValid() const		{return m_CalData->isValid();}

	virtual bool Enable()
	{
		bool bStatusChange = false;
		if (!m_CalData->isValid())
		{
			CAgilityBookOptions::SuppressCalSite(m_Filename, false);
			m_CalData->Connect();
			if (m_CalData->isValid())
			{
				bStatusChange = true;
				SetNameDesc();
			}
		}
		return bStatusChange;
	}

	virtual bool CanDisable() const
	{
		return true;
	}

	virtual void Disable()
	{
		m_CalData->Unload(true);
		CString disabled;
		disabled.LoadString(IDS_DISABLED);
		m_Name.Format(_T("%s [%s]"), (LPCTSTR)m_Filename, (LPCTSTR)disabled);
	}

private:
	void SetNameDesc()
	{
		if (m_CalData->isValid())
		{
			m_Name = m_CalData->GetName();
			m_Desc = m_CalData->GetDescription();
		}
		else
		{
			CString disabled;
			disabled.LoadString(IDS_DISABLED);
			m_Name.Format(_T("%s [%s]"), (LPCTSTR)m_Filename, (LPCTSTR)disabled);
			m_Desc = _T("");
		}
	}

	CString m_Filename;
	CalSiteDataPtr m_CalData;
};


class CPluginCalData : public CPluginBase
{
public:
	CPluginCalData(ARBCalendarPtr cal)
		: m_Cal(cal)
	{
		ARBDate::DateFormat dFmt = CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eCalList);
		otstringstream name;
		name << m_Cal->GetStartDate().GetString(dFmt)
			<< ' '
			<< m_Cal->GetEndDate().GetString(dFmt)
			<< ": "
			<< m_Cal->GetVenue()
			<< ' '
			<< m_Cal->GetLocation()
			<< ' '
			<< m_Cal->GetClub();
		m_Name = name.str().c_str();
		otstringstream desc;
		desc << m_Cal->GetSecEmail() << '\n';
		if (m_Cal->GetOpeningDate().IsValid())
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_OPENS);
			desc << (LPCTSTR)str
				<< ' '
				<< m_Cal->GetOpeningDate().GetString(dFmt)
				<< '\n';
		}
		if (m_Cal->GetDrawDate().IsValid())
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_DRAWS);
			desc << (LPCTSTR)str
				<< ' '
				<< m_Cal->GetDrawDate().GetString(dFmt)
				<< '\n';
		}
		if (m_Cal->GetClosingDate().IsValid())
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_CLOSES);
			desc << (LPCTSTR)str
				<< ' '
				<< m_Cal->GetClosingDate().GetString(dFmt)
				<< '\n';
		}
		m_Desc = desc.str().c_str();
		m_Desc.Replace(_T("\n"), _T("\r\n"));
	}

	virtual CString GetName() const	{return m_Name;}
	virtual CString GetDesc() const	{return m_Desc;}
	ARBCalendarPtr CalEntry() const	{return m_Cal;}

private:
	ARBCalendarPtr m_Cal;
	CString m_Name;
	CString m_Desc;
};

/////////////////////////////////////////////////////////////////////////////

CDlgCalendarPlugins::CDlgCalendarPlugins(
		CAgilityBookDoc* pDoc,
		std::map<CString, CalSiteDataPtr>& directAccess,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCalendarPlugins::IDD, pParent)
	, m_pDoc(pDoc)
	, m_DirectAccess(directAccess)
{
	//{{AFX_DATA_INIT(CDlgCalendarPlugins)
	//}}AFX_DATA_INIT
}


void CDlgCalendarPlugins::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCalendarPlugins)
	DDX_Control(pDX, IDC_PLUGIN_TREE, m_ctrlPlugins);
	DDX_Control(pDX, IDC_PLUGIN_DETAILS, m_ctrlDetails);
	DDX_Control(pDX, IDC_PLUGIN_READ, m_ctrlRead);
	DDX_Control(pDX, IDC_PLUGIN_ADD, m_ctrlAdd);
	DDX_Control(pDX, IDC_PLUGIN_ENABLE, m_ctrlEnable);
	DDX_Control(pDX, IDC_PLUGIN_QUERYDETAILS, m_ctrlQuery);
	DDX_Control(pDX, IDC_PLUGIN_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_PLUGIN_DELETE, m_ctrlDelete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCalendarPlugins, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgCalendarPlugins)
	ON_NOTIFY(TVN_DELETEITEM, IDC_PLUGIN_TREE, OnTvnDeleteitemPluginTree)
	ON_NOTIFY(NM_DBLCLK, IDC_PLUGIN_TREE, OnNMDblclkPluginTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PLUGIN_TREE, OnTvnSelchangedPluginTree)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_PLUGIN_TREE, OnTvnSetdispinfoPluginTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_PLUGIN_TREE, OnTvnGetdispinfoPluginTree)
	ON_BN_CLICKED(IDC_PLUGIN_READ, OnPluginRead)
	ON_BN_CLICKED(IDC_PLUGIN_ADD, OnPluginAddCalEntry)
	ON_BN_CLICKED(IDC_PLUGIN_ENABLE, OnPluginEnable)
	ON_BN_CLICKED(IDC_PLUGIN_QUERYDETAILS, OnPluginQueryDetails)
	ON_BN_CLICKED(IDC_PLUGIN_NEW, OnPluginNew)
	ON_BN_CLICKED(IDC_PLUGIN_EDIT, OnPluginEdit)
	ON_BN_CLICKED(IDC_PLUGIN_DELETE, OnPluginDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgCalendarPlugins::UpdateControls()
{
	int nChecked = 0;
	int nCalItems = 0;
	HTREEITEM hItem;
	for (hItem = m_ctrlPlugins.GetRootItem();
		hItem != NULL;
		hItem = m_ctrlPlugins.GetNextSiblingItem(hItem))
	{
		if (m_ctrlPlugins.GetChecked(hItem))
			++nChecked;
		for (HTREEITEM hCal = m_ctrlPlugins.GetChildItem(hItem);
			hCal != NULL;
			hCal = m_ctrlPlugins.GetNextSiblingItem(hCal))
		{
			if (m_ctrlPlugins.GetChecked(hCal))
				++nCalItems;
		}
	}
	m_ctrlRead.EnableWindow(0 < nChecked);
	m_ctrlAdd.EnableWindow(0 < nCalItems);
	hItem = m_ctrlPlugins.GetSelectedItem();
	CPluginData* pData = NULL;
	if (hItem)
	{
		CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
		pData = dynamic_cast<CPluginData*>(pRawData);
	}
	m_ctrlEnable.EnableWindow(pData != NULL && !pData->isValid());
	m_ctrlQuery.EnableWindow(pData != NULL && pData->HasQueryDetails());
	m_ctrlEdit.EnableWindow(pData != NULL && pData->CanEdit());
	m_ctrlDelete.EnableWindow(pData != NULL && pData->CanDelete());
}


int CALLBACK SortPluginsProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CPluginBase* pRawData1 = reinterpret_cast<CPluginBase*>(lParam1);
	CPluginBase* pRawData2 = reinterpret_cast<CPluginBase*>(lParam2);
	CPluginData* pData1 = dynamic_cast<CPluginData*>(pRawData1);
	CPluginData* pData2 = dynamic_cast<CPluginData*>(pRawData2);
	// The root level that we're sorting should never contain CPluginCalData
	assert(pData1 && pData2);
	if (!pData1 || !pData2)
		return 0;
	return pData1->GetSortName().Compare(pData2->GetSortName());
}


void CDlgCalendarPlugins::SortPlugins()
{
	TVSORTCB tvs;
	tvs.hParent = TVI_ROOT;
	tvs.lpfnCompare = SortPluginsProc;
	tvs.lParam = 0;
	m_ctrlPlugins.SortChildrenCB(&tvs);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendarPlugins message handlers

BOOL CDlgCalendarPlugins::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	for (ARBConfigCalSiteList::const_iterator iConfig = m_pDoc->Book().GetConfig().GetCalSites().begin();
		iConfig != m_pDoc->Book().GetConfig().GetCalSites().end();
		++iConfig)
	{
		CPluginConfigData* pData = new CPluginConfigData(m_pDoc, *iConfig);
		HTREEITEM hItem = m_ctrlPlugins.InsertItem(TVIF_TEXT | TVIF_PARAM,
			LPSTR_TEXTCALLBACK,
			0, 0,
			0, 0,
			reinterpret_cast<LPARAM>(static_cast<CPluginBase*>(pData)),
			TVI_ROOT, TVI_LAST);
		pData->SetHTreeItem(hItem);
		m_ctrlPlugins.ShowCheckbox(hItem, true);
		m_ctrlPlugins.SetChecked(hItem, true, false);
	}

	for (std::map<CString, CalSiteDataPtr>::iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		CPluginDllData* pData = new CPluginDllData((*i).first, (*i).second);
		HTREEITEM hItem = m_ctrlPlugins.InsertItem(TVIF_TEXT | TVIF_PARAM,
			LPSTR_TEXTCALLBACK,
			0, 0,
			0, 0,
			reinterpret_cast<LPARAM>(static_cast<CPluginBase*>(pData)),
			TVI_ROOT, TVI_LAST);
		pData->SetHTreeItem(hItem);
		m_ctrlPlugins.ShowCheckbox(hItem, (*i).second->isValid());
		if ((*i).second->isValid())
			m_ctrlPlugins.SetChecked(hItem, true, false);
	}

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCalendarPlugins::OnTvnDeleteitemPluginTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPluginBase* pData = reinterpret_cast<CPluginBase*>(pNMTreeView->itemOld.lParam);
	delete pData;
	pNMTreeView->itemOld.lParam = 0;
	*pResult = 0;
}


void CDlgCalendarPlugins::OnNMDblclkPluginTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnPluginEdit();
	*pResult = 0;
}


void CDlgCalendarPlugins::OnTvnSelchangedPluginTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPluginBase* pData = NULL;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (NULL != hItem)
		pData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
	CString desc;
	if (pData)
		desc = pData->GetDesc();
	m_ctrlDetails.SetWindowText(desc);
	UpdateControls();
	*pResult = 0;
}


void CDlgCalendarPlugins::OnTvnSetdispinfoPluginTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVDISPINFO* pTvdi = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
	if (pTvdi->item.stateMask & TVIS_STATEIMAGEMASK)
	{
		// Check state of a button just changed.
		UpdateControls();
	}
	*pResult = 0;
}


void CDlgCalendarPlugins::OnTvnGetdispinfoPluginTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TV_DISPINFO* pDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	CPluginBase* pData = reinterpret_cast<CPluginBase*>(pDispInfo->item.lParam);
	if (pDispInfo->item.mask & TVIF_TEXT)
	{
		if (pData)
		{
			CString str = pData->GetName();
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}


void CDlgCalendarPlugins::OnPluginRead()
{
	HTREEITEM hItem;
	int nEntries = 0;
	for (hItem = m_ctrlPlugins.GetChildItem(m_ctrlPlugins.GetRootItem());
		hItem != NULL;
		hItem = m_ctrlPlugins.GetNextSiblingItem(hItem))
	{
		if (m_ctrlPlugins.GetChecked(hItem))
			++nEntries;
	}

	CProgressMeter progress(nEntries, this);

	CWaitCursor wait;
	for (hItem = m_ctrlPlugins.GetRootItem();
		hItem != NULL;
		hItem = m_ctrlPlugins.GetNextSiblingItem(hItem))
	{
		if (m_ctrlPlugins.GetChecked(hItem))
		{
			progress.StepMe();
			CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
			CPluginData* pData = dynamic_cast<CPluginData*>(pRawData);
			if (pData)
			{
				int nInserted = 0;
				CStringA data = pData->Process(&progress);
				progress.SetForegroundWindow();
				wait.Restore();
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
							CPluginCalData* pCalData = new CPluginCalData(*iter);
							HTREEITEM hCalItem = m_ctrlPlugins.InsertItem(TVIF_TEXT | TVIF_PARAM,
								LPSTR_TEXTCALLBACK,
								0, 0,
								0, 0,
								reinterpret_cast<LPARAM>(static_cast<CPluginBase*>(pCalData)),
								pData->GetHTreeItem(), TVI_LAST);
							pCalData->SetHTreeItem(hCalItem);
							m_ctrlPlugins.ShowCheckbox(hCalItem, true);
							++nInserted;
						}
					}
				}
				else
				{
					CString str(pData->GetName());
					CString err;
					err.FormatMessage(IDS_ERR_PARSING_DATA, (LPCTSTR)str);
					if (!errMsg.empty())
					{
						err += ":\n\t";
						err += errMsg.c_str();
					}
					UINT flags = MB_ICONWARNING;
					if (pData->CanDisable())
					{
						flags |= MB_YESNO | MB_DEFBUTTON2;
						err += "\n\n";
						str.LoadString(IDS_USE_PLUGIN);
						err += str;
					}
					if (IDNO == AfxMessageBox(err, flags))
						pData->Disable();
					progress.SetForegroundWindow();
					wait.Restore();
				}
				m_ctrlPlugins.ShowCheckbox(hItem, false);
				if (0 < nInserted)
					m_ctrlPlugins.Expand(hItem, TVE_EXPAND);
			}
		}
	}
	progress.Dismiss();
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginAddCalEntry()
{
	int nAdded = 0;
	int nUpdated = 0;
	for (HTREEITEM hItem = m_ctrlPlugins.GetRootItem();
		hItem != NULL;
		hItem = m_ctrlPlugins.GetNextSiblingItem(hItem))
	{
		for (HTREEITEM hCal = m_ctrlPlugins.GetChildItem(hItem);
			hCal != NULL;
			hCal = m_ctrlPlugins.GetNextSiblingItem(hCal))
		{
			if (m_ctrlPlugins.GetChecked(hCal))
			{
				CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hCal));
				CPluginCalData* pData = dynamic_cast<CPluginCalData*>(pRawData);
				if (pData)
				{
					ARBCalendarPtr cal = pData->CalEntry();
					ARBCalendarPtr calFound;
					if (!m_pDoc->Book().GetCalendar().FindCalendar(cal, false, &calFound))
					{
						if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
						{
							m_pDoc->Book().GetCalendar().AddCalendar(cal);
							++nAdded;
						}
					}
					else
					{
						if (calFound->Update(cal))
						{
							if (calFound->IsTentative())
								calFound->SetIsTentative(false);
							++nUpdated;
						}
					}
					m_ctrlPlugins.ShowCheckbox(hCal, false);
				}
			}
		}
	}
	if (0 < nAdded + nUpdated)
	{
		m_pDoc->Book().GetCalendar().sort();
		m_pDoc->UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
		m_pDoc->SetModifiedFlag();
	}
	CString str;
	str.FormatMessage(IDS_UPDATED_CAL_ITEMS, nAdded, nUpdated);
	AfxMessageBox(str, MB_ICONINFORMATION);
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginEnable()
{
	HTREEITEM hItem = m_ctrlPlugins.GetSelectedItem();
	if (hItem)
	{
		CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
		CPluginData* pData = dynamic_cast<CPluginData*>(pRawData);
		if (pData)
		{
			if (pData->Enable())
			{
				CRect rect;
				m_ctrlPlugins.GetItemRect(pData->GetHTreeItem(), &rect, TRUE);
				m_ctrlPlugins.InvalidateRect(rect);
				m_ctrlPlugins.ShowCheckbox(pData->GetHTreeItem(), true);
				CString desc(pData->GetDesc());
				m_ctrlDetails.SetWindowText(desc);
				UpdateControls();
			}
		}
	}
}


void CDlgCalendarPlugins::OnPluginQueryDetails()
{
	HTREEITEM hItem = m_ctrlPlugins.GetSelectedItem();
	if (hItem)
	{
		CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
		CPluginData* pData = dynamic_cast<CPluginData*>(pRawData);
		if (pData && pData->HasQueryDetails())
		{
			CDlgCalendarQueryDetail dlg(
				m_pDoc->Book().GetConfig(),
				pData->QueryLocationCodes(), pData->LocationCodes(),
				pData->QueryVenueCodes(), pData->VenueCodes(),
				this);
			if (IDOK == dlg.DoModal())
			{
				pData->LocationCodes() = dlg.GetSelectedLocationCodes();
				pData->VenueCodes() = dlg.GetSelectedVenueCodes();
			}
		}
	}
}


void CDlgCalendarPlugins::OnPluginNew()
{
	ARBConfigCalSitePtr site = ARBConfigCalSite::New();
	CDlgPluginDetails dlg(m_pDoc->Book().GetConfig(), site, this);
	if (IDOK == dlg.DoModal())
	{
		m_pDoc->Book().GetConfig().GetCalSites().AddSite(site);
		m_pDoc->SetModifiedFlag(TRUE);
		CPluginConfigData* pData = new CPluginConfigData(m_pDoc, site);
		HTREEITEM hItem = m_ctrlPlugins.InsertItem(TVIF_TEXT | TVIF_PARAM,
			LPSTR_TEXTCALLBACK,
			0, 0,
			0, 0,
			reinterpret_cast<LPARAM>(static_cast<CPluginBase*>(pData)),
			TVI_ROOT, TVI_LAST);
		pData->SetHTreeItem(hItem);
		m_ctrlPlugins.ShowCheckbox(hItem, true);
		m_ctrlPlugins.SetChecked(hItem, true, false);
		m_ctrlPlugins.SelectItem(hItem);
		SortPlugins();
		UpdateControls();
	}
}


void CDlgCalendarPlugins::OnPluginEdit()
{
	HTREEITEM hItem = m_ctrlPlugins.GetSelectedItem();
	if (hItem)
	{
		CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
		CPluginData* pData = dynamic_cast<CPluginData*>(pRawData);
		if (pData && pData->Edit(this))
		{
			m_ctrlPlugins.Invalidate();
			CString desc;
			if (pData)
				desc = pData->GetDesc();
			m_ctrlDetails.SetWindowText(desc);
			SortPlugins();
			UpdateControls();
		}
	}
}


void CDlgCalendarPlugins::OnPluginDelete()
{
	HTREEITEM hItem = m_ctrlPlugins.GetSelectedItem();
	if (hItem)
	{
		CPluginBase* pRawData = reinterpret_cast<CPluginBase*>(m_ctrlPlugins.GetItemData(hItem));
		CPluginData* pData = dynamic_cast<CPluginData*>(pRawData);
		if (pData && pData->Delete())
		{
			m_ctrlPlugins.DeleteItem(hItem);
		}
	}
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


bool CCalendarSites::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, CWnd* pParent)
{
	return m_Impl->FindEntries(pDoc, inCalendar, pParent);
}
