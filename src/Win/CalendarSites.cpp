/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Manage the direct download of calendar data from the web.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-03-05 Changed architecture of ICalendarSite.
 * 2009-11-01 Fixed canonical parsing of dll names on unix.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-06 Fix plugin enabling (full path was being stored in map)
 * 2009-02-10 Ported to wxWidgets.
 * 2007-08-12 Created
 */

#include "stdafx.h"
#include "CalendarSites.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "CalendarSiteUSDAA.h"
#include "DlgAssignColumns.h"
#include "DlgCalendarQueryDetail.h"
#include "DlgPluginDetails.h"
#include "Globals.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/CheckTreeCtrl.h"
#include "LibARBWin/DlgProgress.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReadHttp.h"
#include <map>
#include <vector>
#include <wx/dir.h>
#include <wx/dynlib.h>
#include <wx/file.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static size_t TranslateCodeMap(
		std::map<std::wstring, std::wstring> const& inMap,
		std::vector<std::wstring>& outKeys)
{
	outKeys.clear();
	for (std::map<std::wstring, std::wstring>::const_iterator iMap = inMap.begin();
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
	DECLARE_NO_COPY_IMPLEMENTED(CProgressMeter)
public:
	CProgressMeter(int nEntries, wxWindow* pParent);
	virtual ~CProgressMeter();

	void SetForegroundWindow();
	void Dismiss();
	void StepMe();

	void SetMessage(wchar_t const* pMessage) override;
	void SetRange(int inRange) override;
	void SetStep(int inStep) override;
	void StepIt() override;
	void SetPos(int pos) override;
	int GetPos() override;
	bool HasCanceled() const override;

private:
	int m_nEntries;
	IDlgProgress* m_pProgress;
};

/////////////////////////////////////////////////////////////////////////////

class CalSiteData
{
	// Copy semantics don't work well with our cleanup code!
	DECLARE_NO_COPY_IMPLEMENTED(CalSiteData);
public:
	CalSiteData(ICalendarSitePtr pSite);
	~CalSiteData();

	void Connect();

	bool isValid() const					{return !!m_pSite;}
	void Unload(bool bPermanently = false);

	std::wstring GetID() const				{return m_pSite->GetID();}
	std::wstring GetName() const			{return m_pSite->GetName();}
	std::wstring GetDescription() const		{return m_pSite->GetDescription();}
	std::map<std::wstring, std::wstring> const& QueryLocationCodes() const
		{return m_LocCodes;}
	std::map<std::wstring, std::wstring> const& QueryVenueCodes() const
		{return m_VenueCodes;}
	std::string Process(IProgressMeter *progress,
			std::vector<std::wstring> const& inLocationCodes,
			std::vector<std::wstring> const& inVenueCodes);

private:
	ICalendarSitePtr m_pSite;
	std::wstring m_id;
	CVersionNum m_Version;
	std::map<std::wstring, std::wstring> m_LocCodes;
	std::map<std::wstring, std::wstring> m_VenueCodes;
};

typedef std::shared_ptr<CalSiteData> CalSiteDataPtr;

/////////////////////////////////////////////////////////////////////////////

class CSortCheckTreeCtrl;

class CDlgCalendarPlugins : public wxDialog
{
public:
	CDlgCalendarPlugins(
			CAgilityBookDoc* pDoc,
			std::vector<CalSiteDataPtr>& directAccess,
			wxWindow* pParent = nullptr);

private:
	void UpdateControls();
	void EditPlugin();

	CSortCheckTreeCtrl* m_ctrlPlugins;
	CTextCtrl* m_ctrlDetails;
	wxButton* m_ctrlRead;
	wxButton* m_ctrlAdd;
	wxButton* m_ctrlEnable;
	wxButton* m_ctrlQuery;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	CAgilityBookDoc* m_pDoc;
	std::vector<CalSiteDataPtr>& m_DirectAccess;

	void OnSelectionChanged(wxTreeEvent& evt);
	void OnItemActivated(wxTreeEvent& evt);
	void OnCheckChange(wxEvent& evt);
	void OnPluginRead(wxCommandEvent& evt);
	void OnPluginAddCalEntry(wxCommandEvent& evt);
	void OnPluginEnable(wxCommandEvent& evt);
	void OnPluginQueryDetails(wxCommandEvent& evt);
	void OnPluginNew(wxCommandEvent& evt);
	void OnPluginEdit(wxCommandEvent& evt);
	void OnPluginDelete(wxCommandEvent& evt);
};

/////////////////////////////////////////////////////////////////////////////

CProgressMeter::CProgressMeter(int nEntries, wxWindow* pParent)
	: m_nEntries(nEntries)
	, m_pProgress(nullptr)
{
	short nBars = 1;
	if (1 < m_nEntries)
		nBars = 2;
	m_pProgress = IDlgProgress::CreateProgress(nBars, pParent);
	if (1 < m_nEntries)
		m_pProgress->SetRange(1, m_nEntries);
}


CProgressMeter::~CProgressMeter()
{
	Dismiss();
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
		m_pProgress = nullptr;
	}
}


void CProgressMeter::StepMe()
{
	if (1 < m_nEntries)
		m_pProgress->StepIt(1);
}


void CProgressMeter::SetMessage(wchar_t const* pMessage)
{
	if (m_pProgress)
	{
		std::wstring msg;
		if (pMessage)
			msg = pMessage;
		m_pProgress->SetMessage(msg);
	}
}


void CProgressMeter::SetRange(int inRange)
{
	if (m_pProgress)
	{
		short nBar = 1 < m_nEntries ? 2 : 1;
		m_pProgress->SetRange(nBar, inRange);
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
		pos = m_pProgress->GetPos(nBar);
	}
	return pos;
}


bool CProgressMeter::HasCanceled() const
{
	bool bCanceled = false;
	if (m_pProgress)
		bCanceled = m_pProgress->HasCanceled();
	return bCanceled;
}

/////////////////////////////////////////////////////////////////////////////

CalSiteData::CalSiteData(ICalendarSitePtr pSite)
	: m_pSite(pSite)
	, m_id()
	, m_Version()
	, m_LocCodes()
	, m_VenueCodes()
{
	Connect();
}


CalSiteData::~CalSiteData()
{
	Unload();
}


void CalSiteData::Connect()
{
	if (!isValid())
		return;
	m_id = m_pSite->GetID();
	m_LocCodes.clear();
	m_VenueCodes.clear();
	if (!m_pSite->GetVersion(m_Version)
	|| !CAgilityBookOptions::IsCalSiteVisible(m_id, m_Version))
	{
		Unload(true);
	}
	else
	{
		m_pSite->GetLocationCodes(m_LocCodes);
		m_pSite->GetVenueCodes(m_VenueCodes);
	}
}


void CalSiteData::Unload(bool bPermanently)
{
	if (bPermanently)
	{
		CAgilityBookOptions::SuppressCalSite(m_id, true);
	}
}


std::string CalSiteData::Process(IProgressMeter *progress,
		std::vector<std::wstring> const& inLocationCodes,
		std::vector<std::wstring> const& inVenueCodes)
{
	return m_pSite->Process(inLocationCodes, inVenueCodes, progress);
}

/////////////////////////////////////////////////////////////////////////////

class CCalendarSitesImpl
{
	DECLARE_NO_COPY_IMPLEMENTED(CCalendarSitesImpl)
public:
	CCalendarSitesImpl();
	~CCalendarSitesImpl();

	bool FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, wxWindow* pParent);

private:
	std::vector<CalSiteDataPtr> m_DirectAccess;
};


CCalendarSitesImpl::CCalendarSitesImpl()
	: m_DirectAccess()
{
	m_DirectAccess.push_back(std::make_shared<CalSiteData>(CCalendarSiteUSDAA::Create()));
}


CCalendarSitesImpl::~CCalendarSitesImpl()
{
	m_DirectAccess.clear();
}


bool CCalendarSitesImpl::FindEntries(CAgilityBookDoc* pDoc, ARBCalendarList& inCalendar, wxWindow* pParent)
{
	CDlgCalendarPlugins dlg(pDoc, m_DirectAccess, pParent);
	if (wxID_OK != dlg.ShowModal())
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class CPluginBase : public CTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CPluginBase)
public:
	CPluginBase() {}
	~CPluginBase() {}

	virtual std::wstring GetDesc() const = 0;
};


class CPluginData : public CPluginBase
{
public:
	CPluginData() {}
	virtual wxString GetSortName() const = 0;
	std::wstring OnNeedText() const override		{return m_Name;}
	std::wstring GetDesc() const override			{return m_Desc;}
	virtual std::string Process(IProgressMeter *progress) = 0;
	virtual bool HasQueryDetails() const = 0;
	virtual bool CanEdit() const					{return false;}
	virtual bool Edit(wxWindow* pParent)			{return false;}
	virtual bool CanDelete() const					{return false;}
	virtual bool Delete()							{return false;}
	virtual std::map<std::wstring, std::wstring> const& QueryLocationCodes() const = 0;
	virtual std::map<std::wstring, std::wstring> const& QueryVenueCodes() const = 0;
	virtual std::vector<std::wstring>& LocationCodes()	{return m_LocationCodes;}
	virtual std::vector<std::wstring>& VenueCodes()		{return m_VenueCodes;}
	virtual bool isValid() const = 0;
	virtual bool Enable() = 0;
	virtual bool CanDisable() const = 0;
	virtual void Disable() = 0;
protected:
	std::wstring m_Name;
	std::wstring m_Desc;
	std::vector<std::wstring> m_LocationCodes;
	std::vector<std::wstring> m_VenueCodes;
};


class CPluginConfigData : public CPluginData
{
public:
	CPluginConfigData(CAgilityBookDoc* pDoc, ARBConfigCalSitePtr const& inSite)
		: m_pDoc(pDoc)
		, m_OrigSite(inSite)
		, m_Site(inSite->Clone())
		, m_Enabled(true)
	{
		m_Name = m_Site->GetName();
		m_Desc = m_Site->GetDescription();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	wxString GetSortName() const override		{return wxString(L"C") + m_Name.c_str();}

	std::string Process(IProgressMeter *progress) override;

	bool HasQueryDetails() const override
	{
		return 1 < m_Site->LocationCodes().size() || 1 < m_Site->VenueCodes().size();
	}

	bool CanEdit() const override		{return true;}
	bool Edit(wxWindow* pParent) override;
	bool CanDelete() const override		{return true;}
	bool Delete() override;

	std::map<std::wstring, std::wstring> const& QueryLocationCodes() const override
	{
		return m_Site->LocationCodes();
	}

	std::map<std::wstring, std::wstring> const& QueryVenueCodes() const override
	{
		return m_Site->VenueCodes();
	}

	bool isValid() const override
	{
		return m_Enabled;
	}

	bool Enable() override
	{
		m_Enabled = true;
		return true;
	}

	bool CanDisable() const override
	{
		return false;
	}

	void Disable() override
	{
		m_Enabled = false;
	}

private:
	CAgilityBookDoc* m_pDoc;
	ARBConfigCalSitePtr m_OrigSite;
	ARBConfigCalSitePtr m_Site;
	bool m_Enabled;
};


std::string CPluginConfigData::Process(IProgressMeter *progress)
{
	std::wstring url = m_Site->GetFormattedURL(m_LocationCodes, m_VenueCodes);
	progress->SetMessage(url.c_str());
	std::string data;
	CReadHttp http(url, &data);
	std::wstring username, errMsg;
	if (!http.ReadHttpFile(username, errMsg, wxGetApp().GetTopWindow()))
		data.clear();
	return data;
}


bool CPluginConfigData::Edit(wxWindow* pParent)
{
	CDlgPluginDetails dlg(m_pDoc->Book().GetConfig(), m_Site, pParent);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Name = m_Site->GetName();
		m_Desc = m_Site->GetDescription();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
		*m_OrigSite = *m_Site;
		m_pDoc->Modify(true);
		return true;
	}
	else
		return false;
}


bool CPluginConfigData::Delete()
{
	if (m_pDoc->Book().GetConfig().GetCalSites().DeleteSite(m_OrigSite->GetName()))
	{
		m_pDoc->Modify(true);
		m_OrigSite.reset();
		return true;
	}
	return false;
}


class CPluginDllData : public CPluginData
{
public:
	CPluginDllData(CalSiteDataPtr const& inCalData)
		: m_CalData(inCalData)
	{
		assert(m_CalData);
		SetNameDesc();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	wxString GetSortName() const override	{return wxString(L"D") + m_Name.c_str();}

	std::string Process(IProgressMeter *progress) override
	{
		return m_CalData->Process(progress, m_LocationCodes, m_VenueCodes);
	}

	bool HasQueryDetails() const override
	{
		return 1 < m_CalData->QueryLocationCodes().size() || 1 < m_CalData->QueryVenueCodes().size();
	}

	std::map<std::wstring, std::wstring> const& QueryLocationCodes() const override
	{
		return m_CalData->QueryLocationCodes();
	}

	std::map<std::wstring, std::wstring> const& QueryVenueCodes() const override
	{
		return m_CalData->QueryVenueCodes();
	}

	bool isValid() const override		{return m_CalData->isValid();}

	bool Enable() override
	{
		bool bStatusChange = false;
		if (!m_CalData->isValid())
		{
			CAgilityBookOptions::SuppressCalSite(m_CalData->GetID(), false);
			m_CalData->Connect();
			if (m_CalData->isValid())
			{
				bStatusChange = true;
				SetNameDesc();
			}
		}
		return bStatusChange;
	}

	bool CanDisable() const override
	{
		return true;
	}

	void Disable() override
	{
		m_CalData->Unload(true);
		m_Name = fmt::format(L"{} [{}]", m_Filename, _("IDS_DISABLED").wx_str());
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
			m_Name = fmt::format(L"{} [{}]", m_Filename, _("IDS_DISABLED").wx_str());
			m_Desc = L"";
		}
	}

	std::wstring m_Filename;
	CalSiteDataPtr m_CalData;
};


class CPluginCalData : public CPluginBase
{
public:
	CPluginCalData(ARBCalendarPtr const& inCal)
		: m_Cal(inCal)
	{
		m_Name = fmt::format(L"{} {}: {} {} {}",
			m_Cal->GetStartDate().GetString(),
			m_Cal->GetEndDate().GetString(),
			m_Cal->GetVenue(),
			m_Cal->GetLocation(),
			m_Cal->GetClub());
		fmt::wmemory_buffer desc;
		fmt::format_to(desc, L"{}\n", m_Cal->GetSecEmail());
		if (m_Cal->GetOpeningDate().IsValid())
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_OPENS);
			fmt::format_to(desc, L"{} {}\n", str, m_Cal->GetOpeningDate().GetString());
		}
		if (m_Cal->GetDrawDate().IsValid())
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_DRAWS);
			fmt::format_to(desc, L"{} {}\n", str, m_Cal->GetDrawDate().GetString());
		}
		if (m_Cal->GetClosingDate().IsValid())
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_CLOSES);
			fmt::format_to(desc, L"{} {}\n", str, m_Cal->GetClosingDate().GetString());
		}
		m_Desc = fmt::to_string(desc);
	}

	std::wstring OnNeedText() const override	{return m_Name;}
	std::wstring GetDesc() const override		{return m_Desc;}
	ARBCalendarPtr CalEntry() const				{return m_Cal;}

private:
	ARBCalendarPtr m_Cal;
	std::wstring m_Name;
	std::wstring m_Desc;
};

/////////////////////////////////////////////////////////////////////////////

class CSortCheckTreeCtrl : public CCheckTreeCtrl
{
public:
	CSortCheckTreeCtrl(
			wxWindow* pParent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize)
		: CCheckTreeCtrl(pParent, pos, size)
	{
	}

	int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2) override;
};


int CSortCheckTreeCtrl::OnCompareItems(
		const wxTreeItemId& item1,
		const wxTreeItemId& item2)
{
	CPluginData* pData1 = dynamic_cast<CPluginData*>(GetItemData(item1));
	CPluginData* pData2 = dynamic_cast<CPluginData*>(GetItemData(item2));

	// The root level that we're sorting should never contain CPluginCalData
	assert(pData1 && pData2);
	if (!pData1 || !pData2)
		return 0;
	return pData1->GetSortName().CmpNoCase(pData2->GetSortName());
}

/////////////////////////////////////////////////////////////////////////////

CDlgCalendarPlugins::CDlgCalendarPlugins(
		CAgilityBookDoc* pDoc,
		std::vector<CalSiteDataPtr>& directAccess,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlPlugins(nullptr)
	, m_ctrlDetails(nullptr)
	, m_ctrlRead(nullptr)
	, m_ctrlAdd(nullptr)
	, m_ctrlEnable(nullptr)
	, m_ctrlQuery(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_pDoc(pDoc)
	, m_DirectAccess(directAccess)
{
	Create(pParent, wxID_ANY, _("IDD_CALENDAR_PLUGINS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlPlugins = new CSortCheckTreeCtrl(this, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 230), -1));
	BIND_OR_CONNECT_CTRL(m_ctrlPlugins, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler, CDlgCalendarPlugins::OnSelectionChanged);
	BIND_OR_CONNECT_CTRL(m_ctrlPlugins, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler, CDlgCalendarPlugins::OnItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlPlugins, wxEVT_COMMAND_TREE_CHECK_CHANGED, wxEventHandler, CDlgCalendarPlugins::OnCheckChange);
	m_ctrlPlugins->SetHelpText(_("HIDC_PLUGIN_TREE"));
	m_ctrlPlugins->SetToolTip(_("HIDC_PLUGIN_TREE"));

	m_ctrlDetails = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, wxDLG_UNIT_Y(this, 40)),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);
	m_ctrlDetails->SetBackgroundColour(GetBackgroundColour());
	m_ctrlDetails->SetHelpText(_("HIDC_PLUGIN_DETAILS"));
	m_ctrlDetails->SetToolTip(_("HIDC_PLUGIN_DETAILS"));

	m_ctrlRead = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_READ"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlRead, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginRead);
	m_ctrlRead->SetHelpText(_("HIDC_PLUGIN_READ"));
	m_ctrlRead->SetToolTip(_("HIDC_PLUGIN_READ"));

	m_ctrlAdd = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_ADD"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlAdd, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginAddCalEntry);
	m_ctrlAdd->SetHelpText(_("HIDC_PLUGIN_ADD"));
	m_ctrlAdd->SetToolTip(_("HIDC_PLUGIN_ADD"));

	m_ctrlEnable = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_ENABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlEnable, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginEnable);
	m_ctrlEnable->SetHelpText(_("HIDC_PLUGIN_ENABLE"));
	m_ctrlEnable->SetToolTip(_("HIDC_PLUGIN_ENABLE"));

	m_ctrlQuery = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_QUERYDETAILS"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlQuery, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginQueryDetails);
	m_ctrlQuery->SetHelpText(_("HIDC_PLUGIN_QUERYDETAILS"));
	m_ctrlQuery->SetToolTip(_("HIDC_PLUGIN_QUERYDETAILS"));

	wxButton* btnNew = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginNew);
	btnNew->SetHelpText(_("HIDC_PLUGIN_NEW"));
	btnNew->SetToolTip(_("HIDC_PLUGIN_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginEdit);
	m_ctrlEdit->SetHelpText(_("HIDC_PLUGIN_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_PLUGIN_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgCalendarPlugins::OnPluginDelete);
	m_ctrlDelete->SetHelpText(_("HIDC_PLUGIN_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_PLUGIN_DELETE"));

	wxButton* btnClose = new wxButton(this, wxID_CANCEL,
		_("IDC_PLUGIN_CLOSE"),
		wxDefaultPosition, wxDefaultSize, 0);

	wxTreeItemId root = m_ctrlPlugins->AddRoot(L"root");
	for (ARBConfigCalSiteList::const_iterator iConfig = m_pDoc->Book().GetConfig().GetCalSites().begin();
		iConfig != m_pDoc->Book().GetConfig().GetCalSites().end();
		++iConfig)
	{
		CPluginConfigData* pData = new CPluginConfigData(m_pDoc, *iConfig);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root,
			StringUtil::stringWX(pData->OnNeedText()), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, true);
		m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	for (std::vector<CalSiteDataPtr>::iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		CPluginDllData* pData = new CPluginDllData(*i);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root,
			StringUtil::stringWX(pData->OnNeedText()), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, (*i)->isValid());
		if ((*i)->isValid())
			m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerPlugins = new wxBoxSizer(wxVERTICAL);
	sizerPlugins->Add(m_ctrlPlugins, 1, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 5));
	sizerPlugins->Add(m_ctrlDetails, 0, wxEXPAND, 0);

	bSizer->Add(sizerPlugins, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlRead, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlAdd, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlEnable, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlQuery, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(btnNew, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlEdit, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlDelete, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerBtns->Add(btnClose, 0, 0, 0);

	bSizer->Add(sizerBtns, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateControls();
}


void CDlgCalendarPlugins::UpdateControls()
{
	int nChecked = 0;
	int nCalItems = 0;
	wxTreeItemId hItem;
	wxTreeItemIdValue cookie;
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie);
		hItem.IsOk();
		hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		if (m_ctrlPlugins->GetChecked(hItem))
			++nChecked;
		wxTreeItemIdValue cookie2;
		for (wxTreeItemId hCal = m_ctrlPlugins->GetFirstChild(hItem, cookie2);
			hCal.IsOk();
			hCal = m_ctrlPlugins->GetNextChild(hItem, cookie2))
		{
			if (m_ctrlPlugins->GetChecked(hCal))
				++nCalItems;
		}
	}
	m_ctrlRead->Enable(0 < nChecked);
	m_ctrlAdd->Enable(0 < nCalItems);
	hItem = m_ctrlPlugins->GetSelection();
	CPluginData* pData = nullptr;
	if (hItem)
	{
		pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
	}
	m_ctrlEnable->Enable(pData && !pData->isValid());
	m_ctrlQuery->Enable(pData && pData->HasQueryDetails());
	m_ctrlEdit->Enable(pData && pData->CanEdit());
	m_ctrlDelete->Enable(pData && pData->CanDelete());
}


void CDlgCalendarPlugins::EditPlugin()
{
	wxTreeItemId hItem = m_ctrlPlugins->GetSelection();
	if (hItem.IsOk())
	{
		CPluginData* pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
		if (pData && pData->Edit(this))
		{
			RefreshTreeItem(m_ctrlPlugins, hItem);
			std::wstring desc;
			if (pData)
				desc = pData->GetDesc();
			m_ctrlDetails->SetValue(StringUtil::stringWX(desc));
			m_ctrlPlugins->SortChildren(m_ctrlPlugins->GetRootItem());
			UpdateControls();
		}
	}
}


void CDlgCalendarPlugins::OnSelectionChanged(wxTreeEvent& evt)
{
	CPluginBase* pData = nullptr;
	if (evt.GetItem().IsOk())
		pData = dynamic_cast<CPluginBase*>(m_ctrlPlugins->GetItemData(evt.GetItem()));
	std::wstring desc;
	if (pData)
		desc = pData->GetDesc();
	m_ctrlDetails->SetValue(StringUtil::stringWX(desc));
	UpdateControls();
}


void CDlgCalendarPlugins::OnItemActivated(wxTreeEvent& evt)
{
	EditPlugin();
}


void CDlgCalendarPlugins::OnCheckChange(wxEvent& evt)
{
	// Check state of a button just changed.
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginRead(wxCommandEvent& evt)
{
	int nEntries = 0;
	wxTreeItemId hItem;
	wxTreeItemIdValue cookie;
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie);
		hItem.IsOk();
		hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		if (m_ctrlPlugins->GetChecked(hItem))
			++nEntries;
	}

	CProgressMeter progress(nEntries, this);

	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie);
		hItem.IsOk();
		hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		if (m_ctrlPlugins->GetChecked(hItem))
		{
			progress.StepMe();
			CPluginData* pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
			if (pData)
			{
				int nInserted = 0;
				std::string data = pData->Process(&progress);
				if (!progress.HasCanceled())
				{
					progress.SetForegroundWindow();
					ElementNodePtr tree(ElementNode::New());
					fmt::wmemory_buffer errMsg;
					bool bOk = false;
					if (!data.empty())
					{
						bOk = tree->LoadXML(data.c_str(), data.length(), errMsg);
						data.clear();
					}
					if (bOk)
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						bOk = book.Load(tree, true, false, false, false, false, err);
						tree.reset();
						if (bOk)
						{
							if (0 < err.m_ErrMsg.size())
							{
								wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
								progress.SetForegroundWindow();
							}
							for (ARBCalendarList::iterator iter = book.GetCalendar().begin(); iter != book.GetCalendar().end(); ++iter)
							{
								CPluginCalData* pCalData = new CPluginCalData(*iter);
								wxTreeItemId hCalItem = m_ctrlPlugins->AppendItem(pData->GetId(),
									StringUtil::stringWX(pCalData->OnNeedText()), -1, -1, pCalData);
								m_ctrlPlugins->ShowCheckbox(hCalItem, true);
								++nInserted;
							}
						}
					}
					else
					{
						fmt::wmemory_buffer err;
						fmt::format_to(err, _("IDS_ERR_PARSING_DATA").wx_str(), pData->OnNeedText());
						if (0 < errMsg.size())
						{
							fmt::format_to(err, L":\n\t", fmt::to_string(errMsg));
						}
						int flags = wxCENTRE | wxICON_WARNING;
						if (pData->CanDisable())
						{
							flags |= wxYES_NO | wxNO_DEFAULT;
							fmt::format_to(err, L"\n\n{}", _("IDS_USE_PLUGIN").wx_str());
						}
						else
							flags |= wxOK;
						if (wxNO == wxMessageBox(fmt::to_string(err), wxMessageBoxCaptionStr, flags))
							pData->Disable();
						progress.SetForegroundWindow();
					}
					m_ctrlPlugins->ShowCheckbox(hItem, false);
					if (0 < nInserted)
						m_ctrlPlugins->Expand(hItem);
				}
			}
		}
	}
	progress.Dismiss();
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginAddCalEntry(wxCommandEvent& evt)
{
	int nAdded = 0;
	int nUpdated = 0;
	wxTreeItemId hItem;
	wxTreeItemIdValue cookie;
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie);
		hItem.IsOk();
		hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		wxTreeItemIdValue cookie2;
		for (wxTreeItemId hCal = m_ctrlPlugins->GetFirstChild(hItem, cookie2);
			hCal.IsOk();
			hCal = m_ctrlPlugins->GetNextChild(hItem, cookie2))
		{
			if (m_ctrlPlugins->GetChecked(hCal))
			{
				CPluginCalData* pData = dynamic_cast<CPluginCalData*>(m_ctrlPlugins->GetItemData(hCal));
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
					m_ctrlPlugins->ShowCheckbox(hCal, false);
				}
			}
		}
	}
	if (0 < nAdded + nUpdated)
	{
		m_pDoc->Book().GetCalendar().sort();
		CUpdateHint hint(UPDATE_CALENDAR_VIEW);
		m_pDoc->UpdateAllViews(nullptr, &hint);
		m_pDoc->Modify(true);
	}
	std::wstring str = fmt::format(_("IDS_UPDATED_CAL_ITEMS").wx_str(), nAdded, nUpdated);
	wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginEnable(wxCommandEvent& evt)
{
	wxTreeItemId hItem = m_ctrlPlugins->GetSelection();
	if (hItem.IsOk())
	{
		CPluginData* pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
		if (pData)
		{
			if (pData->Enable())
			{
				RefreshTreeItem(m_ctrlPlugins, pData->GetId());
				m_ctrlPlugins->ShowCheckbox(pData->GetId(), true);
				m_ctrlDetails->SetValue(StringUtil::stringWX(pData->GetDesc()));
				UpdateControls();
			}
		}
	}
}


void CDlgCalendarPlugins::OnPluginQueryDetails(wxCommandEvent& evt)
{
	wxTreeItemId hItem = m_ctrlPlugins->GetSelection();
	if (hItem.IsOk())
	{
		CPluginData* pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
		if (pData && pData->HasQueryDetails())
		{
			CDlgCalendarQueryDetail dlg(
				m_pDoc->Book().GetConfig(),
				pData->QueryLocationCodes(), pData->LocationCodes(),
				pData->QueryVenueCodes(), pData->VenueCodes(),
				this);
			if (wxID_OK == dlg.ShowModal())
			{
				pData->LocationCodes() = dlg.GetSelectedLocationCodes();
				pData->VenueCodes() = dlg.GetSelectedVenueCodes();
			}
		}
	}
}


void CDlgCalendarPlugins::OnPluginNew(wxCommandEvent& evt)
{
	ARBConfigCalSitePtr site = ARBConfigCalSite::New();
	CDlgPluginDetails dlg(m_pDoc->Book().GetConfig(), site, this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_pDoc->Book().GetConfig().GetCalSites().AddSite(site);
		m_pDoc->Modify(true);
		CPluginConfigData* pData = new CPluginConfigData(m_pDoc, site);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(m_ctrlPlugins->GetRootItem(),
			StringUtil::stringWX(pData->OnNeedText()), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, true);
		m_ctrlPlugins->SetChecked(hItem, true, false);
		m_ctrlPlugins->SelectItem(hItem);
		m_ctrlPlugins->SortChildren(m_ctrlPlugins->GetRootItem());
		UpdateControls();
	}
}


void CDlgCalendarPlugins::OnPluginEdit(wxCommandEvent& evt)
{
	EditPlugin();
}


void CDlgCalendarPlugins::OnPluginDelete(wxCommandEvent& evt)
{
	wxTreeItemId hItem = m_ctrlPlugins->GetSelection();
	if (hItem.IsOk())
	{
		CPluginData* pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
		if (pData && pData->Delete())
		{
			m_ctrlPlugins->Delete(hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CCalendarSites::CCalendarSites()
	: m_Impl(std::make_unique<CCalendarSitesImpl>())
{
}


CCalendarSites::~CCalendarSites()
{
}


bool CCalendarSites::FindEntries(
		CAgilityBookDoc* pDoc,
		ARBCalendarList& inCalendar,
		wxWindow* pParent)
{
	if (!pDoc->OnSaveModified())
		return false;
	return m_Impl->FindEntries(pDoc, inCalendar, pParent);
}
