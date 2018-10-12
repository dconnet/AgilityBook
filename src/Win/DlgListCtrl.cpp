/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgListCtrl class
 * @author David Connet
 *
 * Revision History
 * 2016-02-28 Make sure new item is visible.
 *            Fix a bug in initializing the fault list.
 * 2015-01-01 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-12-26 Made dialog resizable.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-24 Adding a new calendar item didn't set the icon properly.
 * 2003-09-08 In Partners, the dog's number wasn't being saved.
 * 2003-08-25 Set listctrl selection when adding/deleting items.
 * 2003-07-14 Fixed bug where a new faulttype didn't show up until the
 *            run was actually saved.
 */

#include "stdafx.h"
#include "DlgListCtrl.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgCalendar.h"
#include "DlgFault.h"
#include "DlgOtherPoint.h"
#include "DlgPartner.h"
#include "ImageHelper.h"

#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfigVenue.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogRunOtherPoints.h"
#include "ARB/ARBDogRunPartner.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListCtrl.h"
#include "LibARBWin/ListData.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CDlgListCtrlData : public CListData
{
public:
	CDlgListCtrlData(CReportListCtrl* list)
		: m_List(list)
	{
	}
	virtual ~CDlgListCtrlData() {}
	virtual bool HasIcon() const				{return false;}
	virtual int OnNeedIcon() const				{return -1;}
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
	virtual bool OnEdit() = 0;
	virtual void Apply() = 0;
	virtual ARBCalendarPtr GetCalendar() const	{return ARBCalendarPtr();}
protected:
	CReportListCtrl* m_List;
};


void CDlgListCtrlData::OnNeedListItem(long iCol, wxListItem& info) const
{
	info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
	info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
	if (0 == iCol && HasIcon())
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
		info.SetImage(OnNeedIcon());
	}
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataCalendar : public CDlgListCtrlData
{
public:
	CDlgListCtrlDataCalendar(
			CDlgListCtrl* parent,
			ARBCalendarPtr const& inCal)
		: CDlgListCtrlData(parent->m_ctrlList)
		, m_Parent(parent)
		, m_pCal(inCal)
	{
	}
	virtual bool HasIcon() const				{return true;}
	virtual int OnNeedIcon() const;
	virtual std::wstring OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
	virtual ARBCalendarPtr GetCalendar() const	{return m_pCal;}
private:
	CDlgListCtrl* m_Parent;
	ARBCalendarPtr m_pCal;
};
typedef std::shared_ptr<CDlgListCtrlDataCalendar> CDlgListCtrlDataCalendarPtr;


int CDlgListCtrlDataCalendar::OnNeedIcon() const
{
	switch (GetCalendar()->GetEntered())
	{
	default:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgTentative : m_Parent->m_ctrlList->ImageEmpty();
	case ARBCalendar::ePlanning:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgPlanTentative : m_Parent->m_imgPlan;
	case ARBCalendar::ePending:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgPendingTentative : m_Parent->m_imgPending;
	case ARBCalendar::eEntered:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgEnteredTentative : m_Parent->m_imgEntered;
	}
}


std::wstring CDlgListCtrlDataCalendar::OnNeedText(long iCol) const
{
	std::wstring str;
	switch (iCol)
	{
	case 1: // Start Date
		str = m_pCal->GetStartDate().GetString();
		break;
	case 2: // End Date
		str = m_pCal->GetEndDate().GetString();
		break;
	case 3: // Location
		str = m_pCal->GetLocation();
		break;
	case 4: // Club
		str = m_pCal->GetClub();
		break;
	case 5: // Venue
		str = m_pCal->GetVenue();
		break;
	case 6: // Notes
		str = StringUtil::Replace(m_pCal->GetNote(), L"\n", L" ");
		break;
	}
	return str;
}


bool CDlgListCtrlDataCalendar::OnEdit()
{
	CDlgCalendar dlg(m_pCal, m_Parent->m_pDoc);
	return wxID_OK == dlg.ShowModal();
}


void CDlgListCtrlDataCalendar::Apply()
{
	m_Parent->m_pDoc->Book().GetCalendar().AddCalendar(m_pCal);
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataFaults : public CDlgListCtrlData
{
public:
	static void GetAllFaults(
			CDlgListCtrl* pDlg,
			CReportListCtrl* ctrl,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr const& inRun,
			std::set<std::wstring>& faults);
	CDlgListCtrlDataFaults(
			CDlgListCtrl* pDlg,
			CReportListCtrl* list,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr const& inRun,
			std::wstring fault)
		: CDlgListCtrlData(list)
		, m_pDoc(pDoc)
		, m_pDlg(pDlg)
		, m_pRun(inRun)
		, m_Fault(fault)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const	{return m_Fault;}
	virtual bool OnEdit();
	virtual void Apply();
private:
	CAgilityBookDoc* m_pDoc;
	CDlgListCtrl* m_pDlg;
	ARBDogRunPtr m_pRun;
	std::wstring m_Fault;
};
typedef std::shared_ptr<CDlgListCtrlDataFaults> CDlgListCtrlDataFaultsPtr;


void CDlgListCtrlDataFaults::GetAllFaults(
		CDlgListCtrl* pDlg,
		CReportListCtrl* ctrl,
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr const& inRun,
		std::set<std::wstring>& faults)
{
	faults.clear();
	pDoc->Book().GetAllFaultTypes(faults);
	if (inRun)
	{
		for (ARBDogFaultList::const_iterator iterFault = inRun->GetFaults().begin();
			iterFault != inRun->GetFaults().end();
			++iterFault)
		{
			faults.insert((*iterFault));
		}
	}
	for (int index = 0; index < ctrl->GetItemCount(); ++index)
	{
		CDlgListCtrlDataPtr pData = pDlg->GetItemListData(index);
		if (pData)
		{
			faults.insert(pData->OnNeedText(0));
		}
	}
}


bool CDlgListCtrlDataFaults::OnEdit()
{
	std::set<std::wstring> faults;
	CDlgListCtrlDataFaults::GetAllFaults(m_pDlg, m_List, m_pDoc, m_pRun, faults);
	CDlgFault dlg(faults, m_Fault);
	if (wxID_OK == dlg.ShowModal())
	{
		std::wstring fault = StringUtil::Trim(dlg.GetFault());
		m_Fault = fault;
		return true;
	}
	else
		return false;
}


void CDlgListCtrlDataFaults::Apply()
{
	if (0 < m_Fault.length())
		m_pRun->GetFaults().push_back(m_Fault);
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataOtherPoints : public CDlgListCtrlData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListCtrlDataOtherPoints)
public:
	CDlgListCtrlDataOtherPoints(
			CReportListCtrl* list,
			ARBConfig& config,
			ARBDogRunPtr const& inRun,
			ARBDogRunOtherPointsPtr const& inOther)
		: CDlgListCtrlData(list)
		, m_pConfig(config)
		, m_pRun(inRun)
		, m_Other(inOther)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
private:
	ARBConfig& m_pConfig;
	ARBDogRunPtr m_pRun;
	ARBDogRunOtherPointsPtr m_Other;
};
typedef std::shared_ptr<CDlgListCtrlDataOtherPoints> CDlgListCtrlDataOtherPointsPtr;


std::wstring CDlgListCtrlDataOtherPoints::OnNeedText(long iCol) const
{
	std::wostringstream str;
	switch (iCol)
	{
	default:
	case 0:
		str << m_Other->GetName();
		break;
	case 1:
		str << ARBDouble::ToString(m_Other->GetPoints(), -1);
		break;
	}
	return str.str();
}


bool CDlgListCtrlDataOtherPoints::OnEdit()
{
	CDlgOtherPoint dlg(m_pConfig, m_Other);
	return wxID_OK == dlg.ShowModal();
}


void CDlgListCtrlDataOtherPoints::Apply()
{
	m_pRun->GetOtherPoints().AddOtherPoints(m_Other);
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataPartners : public CDlgListCtrlData
{
public:
	CDlgListCtrlDataPartners(
			CDlgListCtrl* pDlg,
			CReportListCtrl* list,
			ARBDogRunPtr const& inRun,
			ARBDogRunPartnerPtr const& inPartner)
		: CDlgListCtrlData(list)
		, m_pDlg(pDlg)
		, m_pRun(inRun)
		, m_Partner(inPartner)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
private:
	CDlgListCtrl* m_pDlg;
	ARBDogRunPtr m_pRun;
	ARBDogRunPartnerPtr m_Partner;
};
typedef std::shared_ptr<CDlgListCtrlDataPartners> CDlgListCtrlDataPartnersPtr;


std::wstring CDlgListCtrlDataPartners::OnNeedText(long iCol) const
{
	std::wstring str;
	switch (iCol)
	{
	default:
	case 0:
		str = m_Partner->GetDog();
		break;
	case 1:
		str = m_Partner->GetRegNum();
		break;
	case 2:
		str = m_Partner->GetHandler();
		break;
	}
	return str;
}


bool CDlgListCtrlDataPartners::OnEdit()
{
	std::set<std::wstring> handlers, dogs;
	m_pDlg->GetAllPartners(handlers, dogs);
	CDlgPartner dlg(m_Partner, handlers, dogs);
	return wxID_OK == dlg.ShowModal();
}


void CDlgListCtrlDataPartners::Apply()
{
	if (0 < m_Partner->GetHandler().length() && 0 < m_Partner->GetDog().length())
		m_pRun->GetPartners().AddPartner(m_Partner);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListCtrl dialog

BEGIN_EVENT_TABLE(CDlgListCtrl, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgListCtrl::OnOk)
END_EVENT_TABLE()


// Calendar
CDlgListCtrl::CDlgListCtrl(
		CAgilityBookDoc* pDoc,
		ARBDate const& date,
		std::vector<ARBCalendarPtr> const* entries,
		CTabView* pTabView,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
	, m_ctrlCreateTrial(nullptr)
	, m_What(eCalendar)
	, m_pDoc(pDoc)
	, m_Date(date)
	, m_CalEntries(entries)
	, m_pTabView(pTabView)
	, m_pConfig(nullptr)
	, m_pRun()
{
	Create(StringUtil::stringW(_("IDS_CALENDAR")), pParent, true);
	m_ctrlCreateTrial->Show();

	int nCols = 0;
	m_ctrlList->InsertColumn(nCols++, L"");
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_START_DATE"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_END_DATE"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_LOCATION"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_CLUB"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_VENUE"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_NOTES"));
	std::vector<CDlgListCtrlDataPtr> items;
	for (std::vector<ARBCalendarPtr>::const_iterator iter = m_CalEntries->begin(); iter != m_CalEntries->end(); ++iter)
	{
		CDlgListCtrlDataCalendarPtr pData(new CDlgListCtrlDataCalendar(this, *iter));
		items.push_back(pData);
	}
	FinishCreate(nCols, items);
}


// Faults/Partners
CDlgListCtrl::CDlgListCtrl(
		CDlgListCtrl::WhatToList inType,
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr const& inRun,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
	, m_ctrlCreateTrial(nullptr)
	, m_What(inType)
	, m_pDoc(pDoc)
	, m_Date()
	, m_CalEntries(nullptr)
	, m_pTabView(nullptr)
	, m_pConfig(nullptr)
	, m_pRun(inRun)
{
	int nCols = 0;
	std::vector<CDlgListCtrlDataPtr> items;

	if (eFaults == m_What)
	{
		Create(StringUtil::stringW(_("IDS_FAULT_TYPES")), pParent, false);
		m_ctrlList->InsertColumn(nCols++, _("IDS_COL_FAULT"));
		for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin(); iter != m_pRun->GetFaults().end(); ++iter)
		{
			CDlgListCtrlDataFaultsPtr pData(new CDlgListCtrlDataFaults(this, m_ctrlList, m_pDoc, m_pRun, (*iter)));
			items.push_back(pData);
		}
	}

	else if (ePartners == m_What)
	{
		Create(StringUtil::stringW(_("IDS_PARTNERS")), pParent, false);
		m_ctrlList->InsertColumn(nCols++, _("IDS_COL_DOG"));
		m_ctrlList->InsertColumn(nCols++, _("IDS_COL_NUMBER"));
		m_ctrlList->InsertColumn(nCols++, _("IDS_COL_HANDLER"));
		for (ARBDogRunPartnerList::const_iterator iter = m_pRun->GetPartners().begin(); iter != m_pRun->GetPartners().end(); ++iter)
		{
			CDlgListCtrlDataPartnersPtr pData(new CDlgListCtrlDataPartners(this, m_ctrlList, m_pRun, *iter));
			items.push_back(pData);
		}
	}

	else
		assert(0);

	FinishCreate(nCols, items);
}


// OtherPoints
CDlgListCtrl::CDlgListCtrl(
		ARBConfig& pConfig,
		ARBDogRunPtr const& inRun,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
	, m_ctrlCreateTrial(nullptr)
	, m_What(eOtherPoints)
	, m_pDoc(nullptr)
	, m_Date()
	, m_CalEntries(nullptr)
	, m_pTabView(nullptr)
	, m_pConfig(&pConfig)
	, m_pRun(inRun)
{
	Create(StringUtil::stringW(_("IDS_OTHERPOINTS")), pParent, false);

	int nCols = 0;
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_NAME"));
	m_ctrlList->InsertColumn(nCols++, _("IDS_COL_POINTS"));
	std::vector<CDlgListCtrlDataPtr> items;
	for (ARBDogRunOtherPointsList::const_iterator iter = m_pRun->GetOtherPoints().begin(); iter != m_pRun->GetOtherPoints().end(); ++iter)
	{
		CDlgListCtrlDataOtherPointsPtr pData(new CDlgListCtrlDataOtherPoints(m_ctrlList, *m_pConfig, m_pRun, *iter));
		items.push_back(pData);
	}
	FinishCreate(nCols, items);
}


bool CDlgListCtrl::Create(
		std::wstring const& inCaption,
		wxWindow* pParent,
		bool bHasImageList)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	if (!wxDialog::Create(pParent, wxID_ANY, inCaption.c_str(), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	// Controls (these are done first to control tab order)

	m_ctrlList = new CReportListCtrl(this,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(250, 80)),
		true, CReportListCtrl::eNoSortHeader, true, bHasImageList);
	if (bHasImageList)
	{
		m_imgTentative = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrQuestion));
		m_imgPlan = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCheck));
		m_imgPlanTentative = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCalPlanTentative));
		m_imgPending = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCalPending));
		m_imgPendingTentative = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCalPendingTentative));
		m_imgEntered = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCalEntered));
		m_imgEnteredTentative = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrCalEnteredTentative));
	}
	BIND_OR_CONNECT_CTRL(m_ctrlList, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgListCtrl::OnItemSelected);
	BIND_OR_CONNECT_CTRL(m_ctrlList, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CDlgListCtrl::OnItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlList, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgListCtrl::OnKeyDown);
	m_ctrlList->SetHelpText(_("HIDC_LIST"));
	m_ctrlList->SetToolTip(_("HIDC_LIST"));

	wxButton* btnOk = new wxButton(this, wxID_OK);

	wxButton* btnCancel = new wxButton(this, wxID_CANCEL);

	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_LIST_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlUp, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnMoveUp);
	m_ctrlUp->SetHelpText(_("HIDC_LIST_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_LIST_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_LIST_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDown, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnMoveDown);
	m_ctrlDown->SetHelpText(_("HIDC_LIST_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_LIST_MOVE_DOWN"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_LIST_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnNew);
	m_ctrlNew->SetHelpText(_("HIDC_LIST_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_LIST_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_LIST_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnEdit);
	m_ctrlEdit->SetHelpText(_("HIDC_LIST_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_LIST_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_LIST_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnDelete);
	m_ctrlDelete->SetHelpText(_("HIDC_LIST_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_LIST_DELETE"));

	m_ctrlCreateTrial = new wxButton(this, wxID_ANY,
		_("IDC_LIST_TRIAL"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlCreateTrial, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgListCtrl::OnCreateTrial);
	m_ctrlCreateTrial->SetHelpText(_("HIDC_LIST_TRIAL"));
	m_ctrlCreateTrial->SetToolTip(_("HIDC_LIST_TRIAL"));
	m_ctrlCreateTrial->Show(false);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizerList = new wxBoxSizer(wxHORIZONTAL);
	bSizerList->Add(m_ctrlList, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerSide = new wxBoxSizer(wxVERTICAL);
	bSizerSide->Add(btnOk, 0, 0, 0);
	bSizerSide->Add(btnCancel, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	bSizerSide->Add(0, 0, 1, wxEXPAND, 0);
	bSizerSide->Add(m_ctrlUp, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	bSizerSide->Add(m_ctrlDown, 0, wxTOP, wxDLG_UNIT_X(this, 2));

	bSizerList->Add(bSizerSide, 0, wxEXPAND, 0);

	bSizer->Add(bSizerList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerEdit = new wxBoxSizer(wxHORIZONTAL);
	bSizerEdit->Add(m_ctrlNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 2));
	bSizerEdit->Add(m_ctrlEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 2));
	bSizerEdit->Add(m_ctrlDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 2));
	bSizerEdit->Add(0, 0, 1, wxEXPAND, 0);
	bSizerEdit->Add(m_ctrlCreateTrial, 0, 0, 0);

	bSizer->Add(bSizerEdit, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlList->SetFocus();

	return true;
}


void CDlgListCtrl::FinishCreate(
		int nCols,
		std::vector<CDlgListCtrlDataPtr>& items)
{
	int i = 0;
	for (std::vector<CDlgListCtrlDataPtr>::const_iterator iter = items.begin(); iter != items.end(); ++i, ++iter)
	{
		m_ctrlList->InsertItem(*iter);
	}
	if (0 < i)
		m_ctrlList->SetSelection(0);

	for (i = 0; i < nCols; ++i)
		m_ctrlList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);

	m_ctrlList->SetFocus();
	UpdateControls();
}


CDlgListCtrlDataPtr CDlgListCtrl::GetItemListData(long item) const
{
	return std::dynamic_pointer_cast<CDlgListCtrlData, CListData>(m_ctrlList->GetData(item));
}


void CDlgListCtrl::UpdateControls()
{
	bool bEnable = false;
	int index = m_ctrlList->GetFirstSelected();
	if (0 <= index)
		bEnable = true;
	m_ctrlEdit->Enable(bEnable);
	m_ctrlDelete->Enable(bEnable);

	bool bEnableCreate = false;
	if (bEnable && eCalendar == m_What)
	{
		CDlgListCtrlDataPtr pData = GetItemListData(index);
		if (pData)
		{
			ARBCalendarPtr pCal = pData->GetCalendar();
			if (pCal)
			{
				if (m_pDoc->Book().GetConfig().GetVenues().FindVenue(pCal->GetVenue()))
					bEnableCreate = true;
			}
		}
	}
	m_ctrlCreateTrial->Enable(bEnableCreate);

	bool bEnableUp = bEnable;
	bool bEnableDown = bEnable;
	if (0 <= index)
	{
		bEnableUp = bEnableDown = true;
		if (0 == index)
			bEnableUp = false;
		if (index == m_ctrlList->GetItemCount() - 1)
			bEnableDown = false;
	}
	m_ctrlUp->Enable(bEnableUp);
	m_ctrlDown->Enable(bEnableDown);
}


void CDlgListCtrl::SwapEntries(
		long oldIndex,
		long newIndex)
{
	long data1 = static_cast<long>(m_ctrlList->GetItemData(oldIndex));
	long data2 = static_cast<long>(m_ctrlList->GetItemData(newIndex));
	m_ctrlList->SetItemData(oldIndex, data2);
	m_ctrlList->SetItemData(newIndex, data1);
	m_ctrlList->SetSelection(newIndex);
	m_ctrlList->RefreshItem(oldIndex);
	m_ctrlList->RefreshItem(newIndex);
	UpdateControls();
}


void CDlgListCtrl::DoEdit()
{
	int nItem = m_ctrlList->GetFirstSelected();
	if (0 <= nItem)
	{
		CDlgListCtrlDataPtr pData = GetItemListData(nItem);
		if (pData && pData->OnEdit())
		{
			m_ctrlList->RefreshItem(nItem);
			m_ctrlList->EnsureVisible(nItem);
			UpdateControls();
		}
	}
}


void CDlgListCtrl::GetAllPartners(
		std::set<std::wstring>& ioPartners,
		std::set<std::wstring>& ioDogs) const
{
	if (m_pDoc)
	{
		m_pDoc->Book().GetAllPartners(ioPartners, ioDogs);
		for (int index = 0; index < m_ctrlList->GetItemCount(); ++index)
		{
			CDlgListCtrlDataPtr pData = GetItemListData(index);
			if (pData)
			{
				ioPartners.insert(pData->OnNeedText(2));
				ioDogs.insert(pData->OnNeedText(0));
			}
		}
	}
}


void CDlgListCtrl::OnItemSelected(wxListEvent& /*evt*/)
{
	UpdateControls();
}


void CDlgListCtrl::OnItemActivated(wxListEvent& evt)
{
	DoEdit();
}


void CDlgListCtrl::OnKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		DoEdit();
		break;
	}
}


void CDlgListCtrl::OnNew(wxCommandEvent& evt)
{
	bool bUpdate = false;
	long nItem = wxNOT_FOUND;
	switch (m_What)
	{
	default:
		assert(0);
		break;
	case eCalendar:
		{
			ARBCalendarPtr cal(ARBCalendar::New());
			cal->SetStartDate(m_Date);
			cal->SetEndDate(m_Date + 1);
			CDlgCalendar dlg(cal, m_pDoc);
			if (wxID_OK == dlg.ShowModal())
			{
				if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
				{
					bUpdate = true;
					CDlgListCtrlDataCalendarPtr pData(new CDlgListCtrlDataCalendar(this, cal));
					nItem = m_ctrlList->InsertItem(pData);
				}
			}
		}
		break;

	case eFaults:
		{
			std::set<std::wstring> faults;
			CDlgListCtrlDataFaults::GetAllFaults(this, m_ctrlList, m_pDoc, m_pRun, faults);
			CDlgFault dlg(faults, wxEmptyString, this);
			if (wxID_OK == dlg.ShowModal())
			{
				bUpdate = true;
				CDlgListCtrlDataFaultsPtr pData(new CDlgListCtrlDataFaults(this, m_ctrlList, m_pDoc, m_pRun, dlg.GetFault()));
				nItem = m_ctrlList->InsertItem(pData);
			}
		}
		break;

	case eOtherPoints:
		{
			ARBDogRunOtherPointsPtr pOther(ARBDogRunOtherPoints::New());
			CDlgOtherPoint dlg(*m_pConfig, pOther, this);
			if (wxID_OK == dlg.ShowModal())
			{
				bUpdate = true;
				CDlgListCtrlDataOtherPointsPtr pData(new CDlgListCtrlDataOtherPoints(m_ctrlList, *m_pConfig, m_pRun, pOther));
				nItem = m_ctrlList->InsertItem(pData);
			}
		}
		break;

	case ePartners:
		{
			ARBDogRunPartnerPtr partner(ARBDogRunPartner::New());
			std::set<std::wstring> handlers, dogs;
			GetAllPartners(handlers, dogs);
			CDlgPartner dlg(partner, handlers, dogs);
			if (wxID_OK == dlg.ShowModal())
			{
				bUpdate = true;
				CDlgListCtrlDataPartnersPtr pData(new CDlgListCtrlDataPartners(this, m_ctrlList, m_pRun, partner));
				nItem = m_ctrlList->InsertItem(pData);
			}
		}
		break;
	}
	if (bUpdate)
	{
		if (wxNOT_FOUND != nItem)
		{
			m_ctrlList->Select(nItem);
			m_ctrlList->EnsureVisible(nItem);
		}
		int nColumns = m_ctrlList->GetColumnCount();
		for (int i = 0; i < nColumns; ++i)
			m_ctrlList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	}
	UpdateControls();
}


void CDlgListCtrl::OnEdit(wxCommandEvent& evt)
{
	DoEdit();
}


void CDlgListCtrl::OnDelete(wxCommandEvent& evt)
{
	int nItem = m_ctrlList->GetFirstSelected();
	if (0 <= nItem)
	{
		m_ctrlList->DeleteItem(nItem);
		if (nItem == m_ctrlList->GetItemCount())
			--nItem;
		if (0 <= nItem)
			m_ctrlList->Select(nItem);
		UpdateControls();
	}
}


void CDlgListCtrl::OnMoveUp(wxCommandEvent& evt)
{
	int nItem = m_ctrlList->GetSelection(true);
	if (0 <= nItem)
	{
		int newIndex = nItem - 1;
		if (0 <= newIndex)
		{
			SwapEntries(nItem, newIndex);
		}
	}
}


void CDlgListCtrl::OnMoveDown(wxCommandEvent& evt)
{
	int nItem = m_ctrlList->GetSelection(true);
	if (0 <= nItem)
	{
		int newIndex = nItem + 1;
		if (newIndex < m_ctrlList->GetItemCount())
			SwapEntries(nItem, newIndex);
	}
}


void CDlgListCtrl::OnCreateTrial(wxCommandEvent& evt)
{
	int nItem = m_ctrlList->GetFirstSelected();
	if (0 <= nItem)
	{
		CDlgListCtrlDataPtr pData = GetItemListData(nItem);
		if (pData)
		{
			ARBCalendarPtr pCal = pData->GetCalendar();
			if (pCal)
			{
				if (m_pDoc->CreateTrialFromCalendar(*pCal, m_pTabView))
				{
					DoOK();
				}
			}
		}
	}
}


bool CDlgListCtrl::DoOK()
{
	// Pre-apply
	switch (m_What)
	{
	default:
		assert(0);
		break;
	case eCalendar:
		{
			for (std::vector<ARBCalendarPtr>::const_iterator iter = m_CalEntries->begin(); iter != m_CalEntries->end(); ++iter)
				m_pDoc->Book().GetCalendar().DeleteCalendar((*iter));
		}
		break;
	case eFaults:
		m_pRun->GetFaults().clear();
		break;
	case eOtherPoints:
		m_pRun->GetOtherPoints().clear();
		break;
	case ePartners:
		m_pRun->GetPartners().clear();
		break;
	}

	// Apply
	for (int index = 0; index < m_ctrlList->GetItemCount(); ++index)
	{
		CDlgListCtrlDataPtr pData = GetItemListData(index);
		if (pData)
			pData->Apply();
	}

	// Post-apply
	switch (m_What)
	{
	case eCalendar:
		{
			if (CAgilityBookOptions::AutoDeleteCalendarEntries())
			{
				ARBDate today(ARBDate::Today());
				today -= CAgilityBookOptions::DaysTillEntryIsPast();
				m_pDoc->Book().GetCalendar().TrimEntries(today);
			}
			m_pDoc->Book().GetCalendar().sort();
			CUpdateHint hint(UPDATE_CALENDAR_VIEW);
			m_pDoc->UpdateAllViews(nullptr, &hint);
		}
		break;
	default:
		break;
	}
	return true;
}


void CDlgListCtrl::OnOk(wxCommandEvent& evt)
{
	if (Validate() && TransferDataFromWindow()
	&& DoOK())
	{
		EndDialog(wxID_OK);
	}
}
