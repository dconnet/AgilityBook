/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Manage the direct download of calendar data from the web.
 * @author David Connet
 *
 * Revision History
 * @li 2010-03-05 DRC Changed architecture of ICalendarSite.
 * @li 2009-11-01 DRC Fixed canonical parsing of dll names on unix.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-06 DRC Fix plugin enabling (full path was being stored in map)
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"
#include "CalendarSites.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "CalendarSiteUSDAA.h"
#include "CheckTreeCtrl.h"
#include "DlgAssignColumns.h"
#include "DlgCalendarQueryDetail.h"
#include "DlgPluginDetails.h"
#include "DlgProgress.h"
#include "Element.h"
#include "Globals.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "ListData.h"
#include "ReadHttp.h"
#include "VersionNum.h"
#include <map>
#include <vector>
#include <wx/dir.h>
#include <wx/dynlib.h>
#include <wx/file.h>
#include <wx/filesys.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>

/////////////////////////////////////////////////////////////////////////////

ICalendarSite::~ICalendarSite()
{
}

/////////////////////////////////////////////////////////////////////////////

static size_t TranslateCodeMap(
		std::map<wxString, wxString> const& inMap,
		std::vector<wxString>& outKeys)
{
	outKeys.clear();
	for (std::map<wxString, wxString>::const_iterator iMap = inMap.begin();
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
	CProgressMeter(int nEntries, wxWindow* pParent);
	~CProgressMeter();

	void SetForegroundWindow();
	void Dismiss();
	void StepMe();

	virtual void SetMessage(char const* pMessage);
	virtual void SetRange(int inRange);
	virtual void SetStep(int inStep);
	virtual void StepIt();
	virtual void SetPos(int pos);
	virtual int GetPos();
	virtual bool HasCanceled() const;

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
	CalSiteData(ICalendarSite* pSite);
	~CalSiteData();

	void Connect();

	bool isValid() const							{return NULL != m_pSite;}
	void Unload(bool bPermanently = false);

	wxString GetID() const							{return m_pSite->GetID();}
	wxString GetName() const						{return m_pSite->GetName();}
	wxString GetDescription() const					{return m_pSite->GetDescription();}
	std::map<wxString, wxString> const& QueryLocationCodes() const
		{return m_LocCodes;}
	std::map<wxString, wxString> const& QueryVenueCodes() const
		{return m_VenueCodes;}
	std::string Process(IProgressMeter *progress,
			std::vector<wxString> const& inLocationCodes,
			std::vector<wxString> const& inVenueCodes);

private:
	ICalendarSite* m_pSite;
	wxString m_id;
	CVersionNum m_Version;
	std::map<wxString, wxString> m_LocCodes;
	std::map<wxString, wxString> m_VenueCodes;
};

typedef std::tr1::shared_ptr<CalSiteData> CalSiteDataPtr;

/////////////////////////////////////////////////////////////////////////////

class CSortCheckTreeCtrl;

class CDlgCalendarPlugins : public wxDialog
{
public:
	CDlgCalendarPlugins(
			CAgilityBookDoc* pDoc,
			std::vector<CalSiteDataPtr>& directAccess,
			wxWindow* pParent = NULL);

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
	void OnCheckChange(wxTreeEvent& evt);
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
	, m_pProgress(NULL)
{
	int nBars = 1;
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
		wxString msg;
		if (pMessage)
			msg = tstringUtil::TString(pMessage, strlen(pMessage));
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

CalSiteData::CalSiteData(ICalendarSite* pSite)
	: m_pSite(pSite)
	, m_id()
	, m_Version(false)
	, m_LocCodes()
	, m_VenueCodes()
{
	Connect();
}


CalSiteData::~CalSiteData()
{
	Unload();
	delete m_pSite;
	m_pSite = NULL;
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
		std::vector<wxString> const& inLocationCodes,
		std::vector<wxString> const& inVenueCodes)
{
	return m_pSite->Process(inLocationCodes, inVenueCodes, progress);
}

/////////////////////////////////////////////////////////////////////////////

class CCalendarSitesImpl
{
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
	m_DirectAccess.push_back(CalSiteDataPtr(new CalSiteData(new CCalendarSiteUSDAA())));
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
public:
	CPluginBase() {}
	virtual ~CPluginBase() {}

	virtual wxString GetDesc() const = 0;
};


class CPluginData : public CPluginBase
{
public:
	CPluginData() {}
	virtual wxString GetSortName() const = 0;
	virtual wxString OnNeedText() const					{return m_Name;}
	virtual wxString GetDesc() const					{return m_Desc;}
	virtual std::string Process(IProgressMeter *progress) = 0;
	virtual bool HasQueryDetails() const = 0;
	virtual bool CanEdit() const					{return false;}
	virtual bool Edit(wxWindow* pParent)			{return false;}
	virtual bool CanDelete() const					{return false;}
	virtual bool Delete()							{return false;}
	virtual std::map<wxString, wxString> const& QueryLocationCodes() const = 0;
	virtual std::map<wxString, wxString> const& QueryVenueCodes() const = 0;
	virtual std::vector<wxString>& LocationCodes()	{return m_LocationCodes;}
	virtual std::vector<wxString>& VenueCodes()		{return m_VenueCodes;}
	virtual bool isValid() const = 0;
	virtual bool Enable() = 0;
	virtual bool CanDisable() const = 0;
	virtual void Disable() = 0;
protected:
	wxString m_Name;
	wxString m_Desc;
	std::vector<wxString> m_LocationCodes;
	std::vector<wxString> m_VenueCodes;
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
		m_Name = m_Site->GetName();
		m_Desc = m_Site->GetDescription();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	virtual wxString GetSortName() const			{return wxT("C") + m_Name;}

	virtual std::string Process(IProgressMeter *progress);

	virtual bool HasQueryDetails() const
	{
		return 1 < m_Site->LocationCodes().size() || 1 < m_Site->VenueCodes().size();
	}

	virtual bool CanEdit() const		{return true;}
	virtual bool Edit(wxWindow* pParent);
	virtual bool CanDelete() const		{return true;}
	virtual bool Delete();

	virtual std::map<wxString, wxString> const& QueryLocationCodes() const
	{
		return m_Site->LocationCodes();
	}

	virtual std::map<wxString, wxString> const& QueryVenueCodes() const
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


std::string CPluginConfigData::Process(IProgressMeter *progress)
{
	wxBusyCursor wait;
	wxString url = m_Site->GetFormattedURL(m_LocationCodes, m_VenueCodes);
	std::string data(url.ToUTF8());
	progress->SetMessage(data.c_str());
	data.erase();
	CReadHttp http(url, &data);
	wxString username, errMsg;
	if (!http.ReadHttpFile(username, errMsg, wxGetApp().GetTopWindow()))
		data.erase();
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
	CPluginDllData(CalSiteDataPtr calData)
		: m_CalData(calData)
	{
		assert(m_CalData);
		SetNameDesc();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	virtual wxString GetSortName() const			{return wxT("D") + m_Name;}

	virtual std::string Process(IProgressMeter *progress)
	{
		return m_CalData->Process(progress, m_LocationCodes, m_VenueCodes);
	}

	virtual bool HasQueryDetails() const
	{
		return 1 < m_CalData->QueryLocationCodes().size() || 1 < m_CalData->QueryVenueCodes().size();
	}

	virtual std::map<wxString, wxString> const& QueryLocationCodes() const
	{
		return m_CalData->QueryLocationCodes();
	}

	virtual std::map<wxString, wxString> const& QueryVenueCodes() const
	{
		return m_CalData->QueryVenueCodes();
	}

	virtual bool isValid() const		{return m_CalData->isValid();}

	virtual bool Enable()
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

	virtual bool CanDisable() const
	{
		return true;
	}

	virtual void Disable()
	{
		m_CalData->Unload(true);
		wxString disabled(_("IDS_DISABLED"));
		m_Name = wxString::Format(wxT("%s [%s]"), m_Filename.c_str(), disabled.c_str());
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
			wxString disabled(_("IDS_DISABLED"));
			m_Name = wxString::Format(wxT("%s [%s]"), m_Filename.c_str(), disabled.c_str());
			m_Desc = wxT("");
		}
	}

	wxString m_Filename;
	CalSiteDataPtr m_CalData;
};


class CPluginCalData : public CPluginBase
{
public:
	CPluginCalData(ARBCalendarPtr cal)
		: m_Cal(cal)
	{
		m_Name << m_Cal->GetStartDate().GetString()
			<< wxT(" ")
			<< m_Cal->GetEndDate().GetString()
			<< wxT(": ")
			<< m_Cal->GetVenue()
			<< wxT(" ")
			<< m_Cal->GetLocation()
			<< wxT(" ")
			<< m_Cal->GetClub();
		wxString desc;
		desc << m_Cal->GetSecEmail() << wxT("\n");
		if (m_Cal->GetOpeningDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_OPENS);
			desc << str
				<< wxT(" ")
				<< m_Cal->GetOpeningDate().GetString()
				<< wxT("\n");
		}
		if (m_Cal->GetDrawDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_DRAWS);
			desc << str
				<< wxT(" ")
				<< m_Cal->GetDrawDate().GetString()
				<< wxT("\n");
		}
		if (m_Cal->GetClosingDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_CLOSES);
			desc << str
				<< wxT(" ")
				<< m_Cal->GetClosingDate().GetString()
				<< wxT("\n");
		}
		m_Desc = desc;
	}

	virtual wxString OnNeedText() const	{return m_Name;}
	virtual wxString GetDesc() const	{return m_Desc;}
	ARBCalendarPtr CalEntry() const		{return m_Cal;}

private:
	ARBCalendarPtr m_Cal;
	wxString m_Name;
	wxString m_Desc;
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

	int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
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
	, m_ctrlPlugins(NULL)
	, m_ctrlDetails(NULL)
	, m_ctrlRead(NULL)
	, m_ctrlAdd(NULL)
	, m_ctrlEnable(NULL)
	, m_ctrlQuery(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_pDoc(pDoc)
	, m_DirectAccess(directAccess)
{
	Create(pParent, wxID_ANY, _("IDD_CALENDAR_PLUGINS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlPlugins = new CSortCheckTreeCtrl(this, wxDefaultPosition, wxSize(400, -1));
	m_ctrlPlugins->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CDlgCalendarPlugins::OnSelectionChanged), NULL, this);
	m_ctrlPlugins->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CDlgCalendarPlugins::OnItemActivated), NULL, this);
	m_ctrlPlugins->Connect(wxEVT_COMMAND_TREE_CHECK_CHANGED, wxTreeEventHandler(CDlgCalendarPlugins::OnCheckChange), NULL, this);
	m_ctrlPlugins->SetHelpText(_("HIDC_PLUGIN_TREE"));
	m_ctrlPlugins->SetToolTip(_("HIDC_PLUGIN_TREE"));

	m_ctrlDetails = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 75),
		wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);
	m_ctrlDetails->SetBackgroundColour(GetBackgroundColour());
	m_ctrlDetails->SetHelpText(_("HIDC_PLUGIN_DETAILS"));
	m_ctrlDetails->SetToolTip(_("HIDC_PLUGIN_DETAILS"));

	m_ctrlRead = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_READ"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRead->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginRead), NULL, this);
	m_ctrlRead->SetHelpText(_("HIDC_PLUGIN_READ"));
	m_ctrlRead->SetToolTip(_("HIDC_PLUGIN_READ"));

	m_ctrlAdd = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_ADD"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginAddCalEntry), NULL, this);
	m_ctrlAdd->SetHelpText(_("HIDC_PLUGIN_ADD"));
	m_ctrlAdd->SetToolTip(_("HIDC_PLUGIN_ADD"));

	m_ctrlEnable = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_ENABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEnable->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginEnable), NULL, this);
	m_ctrlEnable->SetHelpText(_("HIDC_PLUGIN_ENABLE"));
	m_ctrlEnable->SetToolTip(_("HIDC_PLUGIN_ENABLE"));

	m_ctrlQuery = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_QUERYDETAILS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlQuery->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginQueryDetails), NULL, this);
	m_ctrlQuery->SetHelpText(_("HIDC_PLUGIN_QUERYDETAILS"));
	m_ctrlQuery->SetToolTip(_("HIDC_PLUGIN_QUERYDETAILS"));

	wxButton* btnNew = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginNew), NULL, this);
	btnNew->SetHelpText(_("HIDC_PLUGIN_NEW"));
	btnNew->SetToolTip(_("HIDC_PLUGIN_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_PLUGIN_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_PLUGIN_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_PLUGIN_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarPlugins::OnPluginDelete), NULL, this);
	m_ctrlDelete->SetHelpText(_("HIDC_PLUGIN_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_PLUGIN_DELETE"));

	wxButton* btnClose = new wxButton(this, wxID_CANCEL,
		_("IDC_PLUGIN_CLOSE"),
		wxDefaultPosition, wxDefaultSize, 0);

	wxTreeItemId root = m_ctrlPlugins->AddRoot(wxT("root"));
	for (ARBConfigCalSiteList::const_iterator iConfig = m_pDoc->Book().GetConfig().GetCalSites().begin();
		iConfig != m_pDoc->Book().GetConfig().GetCalSites().end();
		++iConfig)
	{
		CPluginConfigData* pData = new CPluginConfigData(m_pDoc, *iConfig);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root,
			pData->OnNeedText(), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, true);
		m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	for (std::vector<CalSiteDataPtr>::iterator i = m_DirectAccess.begin();
		i != m_DirectAccess.end();
		++i)
	{
		CPluginDllData* pData = new CPluginDllData(*i);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root,
			pData->OnNeedText(), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, (*i)->isValid());
		if ((*i)->isValid())
			m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerPlugins = new wxBoxSizer(wxVERTICAL);
	sizerPlugins->Add(m_ctrlPlugins, 1, wxALL|wxEXPAND, 5);
	sizerPlugins->Add(m_ctrlDetails, 0, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerPlugins, 1, wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlRead, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlAdd, 0, wxALL, 5);
	sizerBtns->Add( 
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxALL, 5 );
	sizerBtns->Add(m_ctrlEnable, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlQuery, 0, wxALL, 5);
	sizerBtns->Add( 
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxALL, 5 );
	sizerBtns->Add(btnNew, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlEdit, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlDelete, 0, wxALL, 5);
	sizerBtns->Add( 
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0, wxEXPAND | wxALL, 5 );
	sizerBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerBtns->Add(btnClose, 0, wxALL, 5);

	bSizer->Add(sizerBtns, 0, wxEXPAND, 5);

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
	CPluginData* pData = NULL;
	if (hItem)
	{
		pData = dynamic_cast<CPluginData*>(m_ctrlPlugins->GetItemData(hItem));
	}
	m_ctrlEnable->Enable(pData != NULL && !pData->isValid());
	m_ctrlQuery->Enable(pData != NULL && pData->HasQueryDetails());
	m_ctrlEdit->Enable(pData != NULL && pData->CanEdit());
	m_ctrlDelete->Enable(pData != NULL && pData->CanDelete());
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
			wxString desc;
			if (pData)
				desc = pData->GetDesc();
			m_ctrlDetails->SetValue(desc);
			m_ctrlPlugins->SortChildren(m_ctrlPlugins->GetRootItem());
			UpdateControls();
		}
	}
}


void CDlgCalendarPlugins::OnSelectionChanged(wxTreeEvent& evt)
{
	CPluginBase* pData = NULL;
	if (evt.GetItem().IsOk())
		pData = dynamic_cast<CPluginBase*>(m_ctrlPlugins->GetItemData(evt.GetItem()));
	wxString desc;
	if (pData)
		desc = pData->GetDesc();
	m_ctrlDetails->SetValue(desc);
	UpdateControls();
}


void CDlgCalendarPlugins::OnItemActivated(wxTreeEvent& evt)
{
	EditPlugin();
}


void CDlgCalendarPlugins::OnCheckChange(wxTreeEvent& evt)
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

	wxBusyCursor wait;
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
					wxString errMsg;
					bool bOk = false;
					if (!data.empty())
					{
						bOk = tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg);
						data.erase();
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
								wxMessageBox(err.m_ErrMsg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
								progress.SetForegroundWindow();
							}
							for (ARBCalendarList::iterator iter = book.GetCalendar().begin(); iter != book.GetCalendar().end(); ++iter)
							{
								CPluginCalData* pCalData = new CPluginCalData(*iter);
								wxTreeItemId hCalItem = m_ctrlPlugins->AppendItem(pData->GetId(),
									pCalData->OnNeedText(), -1, -1, pCalData);
								m_ctrlPlugins->ShowCheckbox(hCalItem, true);
								++nInserted;
							}
						}
					}
					else
					{
						wxString str(pData->OnNeedText());
						wxString err = wxString::Format(_("IDS_ERR_PARSING_DATA"), str.c_str());
						if (!errMsg.empty())
						{
							err += wxT(":\n\t");
							err += errMsg;
						}
						int flags = wxCENTRE | wxICON_WARNING;
						if (pData->CanDisable())
						{
							flags |= wxYES_NO | wxNO_DEFAULT;
							err += wxT("\n\n");
							err += _("IDS_USE_PLUGIN");
						}
						if (wxNO == wxMessageBox(err, wxMessageBoxCaptionStr, flags))
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
		m_pDoc->UpdateAllViews(NULL, &hint);
		m_pDoc->Modify(true);
	}
	wxString str = wxString::Format(_("IDS_UPDATED_CAL_ITEMS"), nAdded, nUpdated);
	wxMessageBox(str, wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
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
				wxString desc(pData->GetDesc());
				m_ctrlDetails->SetValue(desc);
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
			pData->OnNeedText(), -1, -1, pData);
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
	: m_Impl(new CCalendarSitesImpl())
{
}


CCalendarSites::~CCalendarSites()
{
	delete m_Impl;
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
