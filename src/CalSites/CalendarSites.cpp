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

#include "ARBConfigCalSite.h"
#include "CalSites.h"
#include "CalendarSiteUSDAA.h"
#include "DlgCalendarQueryDetail.h"
#include "DlgPluginDetails.h"
#include "ICalendarSite.h"
#include "IProgressMeter.h"
#include "VersionNumber.h"

#include "../Win/AgilityBookOptions.h"
#include "../Win/DlgAssignColumns.h"
#include "../Win/ImageHelper.h"
#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfig.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/CheckTreeCtrl.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/DlgProgress.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReadHttp.h"
#include "LibARBWin/ResourceManager.h"
#include <wx/dir.h>
#include <wx/dynlib.h>
#include <wx/file.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <map>
#include <vector>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{

// CalSites
constexpr wchar_t CFG_KEY_CALSITES[] = L"CalSites";
//	DW (DLL names in EXE directory)

// CalSites2 - used for permanently disabling a version
constexpr wchar_t CFG_KEY_CALSITES2[] = L"CalSites2";
//	ST (DLL names in EXE directory)

CVersionNum GetCalSitePermanentStatus(wxString const& filename)
{
	CVersionNum ver;
	if (!filename.empty())
	{
		auto section = wxString::Format(L"%s/%s", CFG_KEY_CALSITES2, filename);
		wxString str = wxConfig::Get()->Read(section, wxString());
		if (!str.empty())
			ver.Parse(str);
	}
	return ver;
}

bool IsCalSiteVisible(wxString const& filename, CVersionNum const& inVer)
{
	assert(inVer.Valid());
	if (filename.empty())
		return true;
	bool bVisible = true;
	auto section = wxString::Format(L"%s/%s", CFG_KEY_CALSITES, filename);
	bool bCheckStatus = true;
	wxConfig::Get()->Read(section, &bCheckStatus);
	if (bCheckStatus)
	{
		CVersionNum ver = GetCalSitePermanentStatus(filename);
		if (ver.Valid() && inVer <= ver)
			bVisible = false;
	}
	else
		bVisible = false;
	return bVisible;
}


void SuppressCalSite(wxString const& filename, bool bSuppress)
{
	if (filename.empty())
		return;
	auto section = wxString::Format(L"%s/%s", CFG_KEY_CALSITES, filename);
	wxConfig::Get()->Write(section, !bSuppress);
}


/*
void SuppressCalSitePermanently(
	wxString const& filename,
	CVersionNum const& inVer,
	bool bSuppress)
{
	if (filename.empty())
		return;
	auto section = wxString::Format(L"%s/%s", CFG_KEY_CALSITES, filename);
	if (bSuppress)
		wxConfig::Get()->Write(section, inVer.GetVersionString());
	else
	{
		// If we're clearing one, make sure we haven't written a different version
		CVersionNum ver;
		wxString str = StringUtil::stringW(wxConfig::Get()->Read(section, wxString()));
		if (!str.empty())
			ver.Parse(str);
		if (ver == inVer)
			wxConfig::Get()->DeleteEntry(section);
	}
}
*/


size_t TranslateCodeMap(std::map<wxString, wxString> const& inMap, std::vector<wxString>& outKeys)
{
	outKeys.clear();
	for (std::map<wxString, wxString>::const_iterator iMap = inMap.begin(); iMap != inMap.end(); ++iMap)
	{
		outKeys.push_back(iMap->first);
	}
	return outKeys.size();
}
} // namespace

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

	void SetMessage(wxString const& message) override;
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

class CSortCheckTreeCtrl;

class CDlgCalendarPlugins : public wxDialog
{
public:
	CDlgCalendarPlugins(
		ARBConfigCalSiteList& sites,
		std::vector<CalSiteDataPtr>& directAccess,
		wxWindow* pParent = nullptr);

private:
	void UpdateControls();
	void EditPlugin();

	ARBConfigCalSiteList& m_sites;
	CSortCheckTreeCtrl* m_ctrlPlugins;
	CTextCtrl* m_ctrlDetails;
	wxButton* m_ctrlRead;
	wxButton* m_ctrlAdd;
	wxButton* m_ctrlEnable;
	wxButton* m_ctrlQuery;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
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


void CProgressMeter::SetMessage(wxString const& message)
{
	if (m_pProgress)
		m_pProgress->SetMessage(message);
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
	if (!m_pSite->GetVersion(m_Version) || !IsCalSiteVisible(m_id, m_Version))
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
		SuppressCalSite(m_id, true);
	}
}


std::string CalSiteData::Process(
	IProgressMeter* progress,
	std::vector<wxString> const& inLocationCodes,
	std::vector<wxString> const& inVenueCodes)
{
	return m_pSite->Process(inLocationCodes, inVenueCodes, progress);
}

/////////////////////////////////////////////////////////////////////////////

class CPluginBase : public CTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CPluginBase)
public:
	CPluginBase()
	{
	}
	~CPluginBase()
	{
	}

	virtual wxString GetDesc() const = 0;
};


class CPluginData : public CPluginBase
{
public:
	CPluginData()
	{
	}
	virtual wxString GetSortName() const = 0;
	wxString OnNeedText() const override
	{
		return m_Name;
	}
	wxString GetDesc() const override
	{
		return m_Desc;
	}
	virtual std::string Process(IProgressMeter* progress) = 0;
	virtual bool HasQueryDetails() const = 0;
	virtual bool CanEdit() const
	{
		return false;
	}
	virtual bool Edit(wxWindow* pParent)
	{
		return false;
	}
	virtual bool CanDelete() const
	{
		return false;
	}
	virtual bool Delete()
	{
		return false;
	}
	virtual std::map<wxString, wxString> const& QueryLocationCodes() const = 0;
	virtual std::map<wxString, wxString> const& QueryVenueCodes() const = 0;
	virtual std::vector<wxString>& LocationCodes()
	{
		return m_LocationCodes;
	}
	virtual std::vector<wxString>& VenueCodes()
	{
		return m_VenueCodes;
	}
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
	CPluginConfigData(ARBConfigCalSiteList& sites, ARBConfigCalSitePtr const& inSite)
		: m_sites(sites)
		, m_OrigSite(inSite)
		, m_Site(inSite->Clone())
		, m_Enabled(true)
	{
		m_Name = m_Site->GetName();
		m_Desc = m_Site->GetDescription();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
	}

	wxString GetSortName() const override
	{
		return wxString(L"C") + m_Name;
	}

	std::string Process(IProgressMeter* progress) override;

	bool HasQueryDetails() const override
	{
		return 1 < m_Site->LocationCodes().size() || 1 < m_Site->VenueCodes().size();
	}

	bool CanEdit() const override
	{
		return true;
	}
	bool Edit(wxWindow* pParent) override;
	bool CanDelete() const override
	{
		return true;
	}
	bool Delete() override;

	std::map<wxString, wxString> const& QueryLocationCodes() const override
	{
		return m_Site->LocationCodes();
	}

	std::map<wxString, wxString> const& QueryVenueCodes() const override
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
	ARBConfigCalSiteList& m_sites;
	ARBConfigCalSitePtr m_OrigSite;
	ARBConfigCalSitePtr m_Site;
	bool m_Enabled;
};


std::string CPluginConfigData::Process(IProgressMeter* progress)
{
	wxString url = m_Site->GetFormattedURL(m_LocationCodes, m_VenueCodes);
	progress->SetMessage(url);
	std::string data;
	wxString errMsg;
#pragma PRAGMA_TODO(use wxWebRequest to read url asynchronously)
	// if (!ReadHttp::ReadHttpFileSync(errMsg, url, data))
	data.clear();
	return data;
}


bool CPluginConfigData::Edit(wxWindow* pParent)
{
	CDlgPluginDetails dlg(m_sites, m_Site, pParent);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Name = m_Site->GetName();
		m_Desc = m_Site->GetDescription();
		TranslateCodeMap(QueryLocationCodes(), m_LocationCodes);
		TranslateCodeMap(QueryVenueCodes(), m_VenueCodes);
		*m_OrigSite = *m_Site;
		// m_pDoc->Modify(true);
		return true;
	}
	else
		return false;
}


bool CPluginConfigData::Delete()
{
	if (m_sites.DeleteSite(m_OrigSite->GetName()))
	{
		// m_pDoc->Modify(true);
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

	wxString GetSortName() const override
	{
		return wxString(L"D") + m_Name;
	}

	std::string Process(IProgressMeter* progress) override
	{
		return m_CalData->Process(progress, m_LocationCodes, m_VenueCodes);
	}

	bool HasQueryDetails() const override
	{
		return 1 < m_CalData->QueryLocationCodes().size() || 1 < m_CalData->QueryVenueCodes().size();
	}

	std::map<wxString, wxString> const& QueryLocationCodes() const override
	{
		return m_CalData->QueryLocationCodes();
	}

	std::map<wxString, wxString> const& QueryVenueCodes() const override
	{
		return m_CalData->QueryVenueCodes();
	}

	bool isValid() const override
	{
		return m_CalData->isValid();
	}

	bool Enable() override
	{
		bool bStatusChange = false;
		if (!m_CalData->isValid())
		{
			SuppressCalSite(m_CalData->GetID(), false);
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
		m_Name.clear();
		m_Name << m_Filename << L" [" << _("IDS_DISABLED") << L"]";
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
			m_Name.clear();
			m_Name << m_Filename << L" [" << _("IDS_DISABLED") << L"]";
			m_Desc = L"";
		}
	}

	wxString m_Filename;
	CalSiteDataPtr m_CalData;
};


class CPluginCalData : public CPluginBase
{
public:
	CPluginCalData(ARBCalendarPtr const& inCal)
		: m_Cal(inCal)
	{
		m_Name = wxString::Format(
			L"%s %s: %s %s %s",
			m_Cal->GetStartDate().GetString(),
			m_Cal->GetEndDate().GetString(),
			m_Cal->GetVenue(),
			m_Cal->GetLocation(),
			m_Cal->GetClub());
		wxString desc;
		desc << m_Cal->GetSecEmail() << L"\n";
		if (m_Cal->GetOpeningDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_OPENS);
			desc << str << L" " << m_Cal->GetOpeningDate().GetString() << L"\n";
		}
		if (m_Cal->GetDrawDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_DRAWS);
			desc << str << L" " << m_Cal->GetDrawDate().GetString() << L"\n";
		}
		if (m_Cal->GetClosingDate().IsValid())
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(IO_CAL_CLOSES);
			desc << str << L" " << m_Cal->GetClosingDate().GetString() << L"\n";
		}
		m_Desc = desc;
	}

	wxString OnNeedText() const override
	{
		return m_Name;
	}
	wxString GetDesc() const override
	{
		return m_Desc;
	}
	ARBCalendarPtr CalEntry() const
	{
		return m_Cal;
	}

private:
	ARBCalendarPtr m_Cal;
	wxString m_Name;
	wxString m_Desc;
};

/////////////////////////////////////////////////////////////////////////////

class CSortCheckTreeCtrl : public CCheckTreeCtrl
{
public:
	CSortCheckTreeCtrl(wxWindow* pParent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize)
		: CCheckTreeCtrl(pParent, pos, size)
	{
	}

	int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2) override;
};


int CSortCheckTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
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
	ARBConfigCalSiteList& sites,
	std::vector<CalSiteDataPtr>& directAccess,
	wxWindow* pParent)
	: wxDialog()
	, m_sites(sites)
	, m_ctrlPlugins(nullptr)
	, m_ctrlDetails(nullptr)
	, m_ctrlRead(nullptr)
	, m_ctrlAdd(nullptr)
	, m_ctrlEnable(nullptr)
	, m_ctrlQuery(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_DirectAccess(directAccess)
{
	Create(
		pParent,
		wxID_ANY,
		wxString::Format(
			L"%s %s",
			_("IDD_CALENDAR_PLUGINS"),
			CVersionNum(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD).GetVersionString(3)),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxIconBundle icons = CResourceManager::Get()->CreateIconBundle(ImageMgrAppBundle, wxART_OTHER);
	SetIcons(icons);

	// Controls (these are done first to control tab order)

	m_ctrlPlugins = new CSortCheckTreeCtrl(this, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 230), -1));
	m_ctrlPlugins->Bind(wxEVT_COMMAND_TREE_SEL_CHANGED, &CDlgCalendarPlugins::OnSelectionChanged, this);
	m_ctrlPlugins->Bind(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &CDlgCalendarPlugins::OnItemActivated, this);
	m_ctrlPlugins->Bind(wxEVT_COMMAND_TREE_CHECK_CHANGED, &CDlgCalendarPlugins::OnCheckChange, this);
	m_ctrlPlugins->SetHelpText(_("HIDC_PLUGIN_TREE"));
	m_ctrlPlugins->SetToolTip(_("HIDC_PLUGIN_TREE"));

	m_ctrlDetails = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 40)),
		wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
	m_ctrlDetails->SetBackgroundColour(GetBackgroundColour());
	m_ctrlDetails->SetHelpText(_("HIDC_PLUGIN_DETAILS"));
	m_ctrlDetails->SetToolTip(_("HIDC_PLUGIN_DETAILS"));

	m_ctrlRead = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_READ"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRead->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginRead, this);
	m_ctrlRead->SetHelpText(_("HIDC_PLUGIN_READ"));
	m_ctrlRead->SetToolTip(_("HIDC_PLUGIN_READ"));

	m_ctrlAdd = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_ADD"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAdd->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginAddCalEntry, this);
	m_ctrlAdd->SetHelpText(_("HIDC_PLUGIN_ADD"));
	m_ctrlAdd->SetToolTip(_("HIDC_PLUGIN_ADD"));

	m_ctrlEnable = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_ENABLE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEnable->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginEnable, this);
	m_ctrlEnable->SetHelpText(_("HIDC_PLUGIN_ENABLE"));
	m_ctrlEnable->SetToolTip(_("HIDC_PLUGIN_ENABLE"));

	m_ctrlQuery = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_QUERYDETAILS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlQuery->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginQueryDetails, this);
	m_ctrlQuery->SetHelpText(_("HIDC_PLUGIN_QUERYDETAILS"));
	m_ctrlQuery->SetToolTip(_("HIDC_PLUGIN_QUERYDETAILS"));

	wxButton* btnNew = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	btnNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginNew, this);
	btnNew->SetHelpText(_("HIDC_PLUGIN_NEW"));
	btnNew->SetToolTip(_("HIDC_PLUGIN_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginEdit, this);
	m_ctrlEdit->SetHelpText(_("HIDC_PLUGIN_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_PLUGIN_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY, _("IDC_PLUGIN_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarPlugins::OnPluginDelete, this);
	m_ctrlDelete->SetHelpText(_("HIDC_PLUGIN_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_PLUGIN_DELETE"));

	wxButton* btnClose = new wxButton(this, wxID_CANCEL, _("IDC_PLUGIN_CLOSE"), wxDefaultPosition, wxDefaultSize, 0);

	wxTreeItemId root = m_ctrlPlugins->AddRoot(L"root");
	for (ARBConfigCalSiteList::const_iterator iConfig = m_sites.begin(); iConfig != m_sites.end(); ++iConfig)
	{
		CPluginConfigData* pData = new CPluginConfigData(m_sites, *iConfig);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root, pData->OnNeedText(), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, true);
		m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	for (std::vector<CalSiteDataPtr>::iterator i = m_DirectAccess.begin(); i != m_DirectAccess.end(); ++i)
	{
		CPluginDllData* pData = new CPluginDllData(*i);
		wxTreeItemId hItem = m_ctrlPlugins->AppendItem(root, pData->OnNeedText(), -1, -1, pData);
		m_ctrlPlugins->ShowCheckbox(hItem, (*i)->isValid());
		if ((*i)->isValid())
			m_ctrlPlugins->SetChecked(hItem, true, false);
	}

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerPlugins = new wxBoxSizer(wxVERTICAL);
	sizerPlugins->Add(m_ctrlPlugins, 1, wxEXPAND | wxBOTTOM, padding.Controls());
	sizerPlugins->Add(m_ctrlDetails, 0, wxEXPAND, 0);

	bSizer->Add(sizerPlugins, 1, wxEXPAND | wxALL, padding.Controls());

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlRead, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlAdd, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0,
		wxEXPAND | wxBOTTOM,
		padding.Inner());
	sizerBtns->Add(m_ctrlEnable, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlQuery, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0,
		wxEXPAND | wxBOTTOM,
		padding.Inner());
	sizerBtns->Add(btnNew, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlEdit, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlDelete, 0, wxBOTTOM, padding.Inner());
	sizerBtns->Add(
		new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
		0,
		wxEXPAND | wxBOTTOM,
		padding.Inner());
	sizerBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerBtns->Add(btnClose, 0, 0, 0);

	bSizer->Add(sizerBtns, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, padding.Controls());

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
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie); hItem.IsOk();
		 hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		if (m_ctrlPlugins->GetChecked(hItem))
			++nChecked;
		wxTreeItemIdValue cookie2;
		for (wxTreeItemId hCal = m_ctrlPlugins->GetFirstChild(hItem, cookie2); hCal.IsOk();
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
	CPluginBase* pData = nullptr;
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


void CDlgCalendarPlugins::OnCheckChange(wxEvent& evt)
{
	// Check state of a button just changed.
	UpdateControls();
}


void CDlgCalendarPlugins::OnPluginRead(wxCommandEvent& evt)
{
#if 0
	int nEntries = 0;
	wxTreeItemId hItem;
	wxTreeItemIdValue cookie;
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie); hItem.IsOk();
		 hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		if (m_ctrlPlugins->GetChecked(hItem))
			++nEntries;
	}

	CProgressMeter progress(nEntries, this);

	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie); hItem.IsOk();
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
								wxMessageBox(err.m_ErrMsg, _("Agility Record Book"),
									wxOK | wxCENTRE | wxICON_INFORMATION);
								progress.SetForegroundWindow();
							}
							for (ARBCalendarList::iterator iter = book.GetCalendar().begin();
								 iter != book.GetCalendar().end();
								 ++iter)
							{
								CPluginCalData* pCalData = new CPluginCalData(*iter);
								wxTreeItemId hCalItem = m_ctrlPlugins->AppendItem(
									pData->GetId(),
									pCalData->OnNeedText(),
									-1,
									-1,
									pCalData);
								m_ctrlPlugins->ShowCheckbox(hCalItem, true);
								++nInserted;
							}
						}
					}
					else
					{
						wxString err;
						err << wxString::Format(_("IDS_ERR_PARSING_DATA"), pData->OnNeedText());
						if (0 < errMsg.size())
						{
							err << L":\n\t" << errMsg;
						}
						int flags = wxCENTRE | wxICON_WARNING;
						if (pData->CanDisable())
						{
							flags |= wxYES_NO | wxNO_DEFAULT;
							err << L"\n\n" << _("IDS_USE_PLUGIN");
						}
						else
							flags |= wxOK;
						if (wxNO == wxMessageBox(err, _("Agility Record Book"), flags))
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
#endif
}


void CDlgCalendarPlugins::OnPluginAddCalEntry(wxCommandEvent& evt)
{
#if 0
	int nAdded = 0;
	int nUpdated = 0;
	wxTreeItemId hItem;
	wxTreeItemIdValue cookie;
	for (hItem = m_ctrlPlugins->GetFirstChild(m_ctrlPlugins->GetRootItem(), cookie); hItem.IsOk();
		 hItem = m_ctrlPlugins->GetNextChild(m_ctrlPlugins->GetRootItem(), cookie))
	{
		wxTreeItemIdValue cookie2;
		for (wxTreeItemId hCal = m_ctrlPlugins->GetFirstChild(hItem, cookie2); hCal.IsOk();
			 hCal = m_ctrlPlugins->GetNextChild(hItem, cookie2))
		{
			if (m_ctrlPlugins->GetChecked(hCal))
			{
				CPluginCalData* pData = dynamic_cast<CPluginCalData*>(m_ctrlPlugins->GetItemData(hCal));
				if (pData)
				{
					ARBCalendarPtr cal = pData->CalEntry();
					ARBCalendarPtr calFound;
					if (!m_book.GetCalendar().FindCalendar(cal, false, &calFound))
					{
						if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
						{
							m_book.GetCalendar().AddCalendar(cal);
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
		m_book.GetCalendar().sort();
		//CUpdateHint hint(UPDATE_CALENDAR_VIEW);
		//m_pDoc->UpdateAllViews(nullptr, &hint);
		//m_pDoc->Modify(true);
	}
	auto str = wxString::Format(_("IDS_UPDATED_CAL_ITEMS"), nAdded, nUpdated);
	wxMessageBox(str, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_INFORMATION);
	UpdateControls();
#endif
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
				m_ctrlDetails->SetValue(pData->GetDesc());
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
				m_sites,
				pData->QueryLocationCodes(),
				pData->LocationCodes(),
				pData->QueryVenueCodes(),
				pData->VenueCodes(),
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
	CDlgPluginDetails dlg(m_sites, site, this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_sites.AddSite(site);
		// m_pDoc->Modify(true);
		CPluginConfigData* pData = new CPluginConfigData(m_sites, site);
		wxTreeItemId hItem
			= m_ctrlPlugins->AppendItem(m_ctrlPlugins->GetRootItem(), pData->OnNeedText(), -1, -1, pData);
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
	: m_DirectAccess()
{
	m_DirectAccess.push_back(std::make_shared<CalSiteData>(CCalendarSiteUSDAA::Create()));
}


CCalendarSites::~CCalendarSites()
{
	m_DirectAccess.clear();
}


bool CCalendarSites::FindEntries(ARBConfigCalSiteList& sites, wxWindow* pParent)
{
	CDlgCalendarPlugins dlg(sites, m_DirectAccess, pParent);
	if (wxID_OK != dlg.ShowModal())
		return false;
	return true;
}

} // namespace dconSoft
