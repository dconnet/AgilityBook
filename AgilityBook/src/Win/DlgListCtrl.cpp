/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgListCtrl class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-12-26 DRC Made dialog resizable.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-24 DRC Adding a new calendar item didn't set the icon properly.
 * @li 2003-09-08 DRC In Partners, the dog's number wasn't being saved.
 * @li 2003-08-25 DRC Set listctrl selection when adding/deleting items.
 * @li 2003-07-14 DRC Fixed bug where a new faulttype didn't show up until the
 *                    run was actually saved.
 */

#include "stdafx.h"
#include "DlgListCtrl.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBCalendar.h"
#include "ARBConfigVenue.h"
#include "ARBDogRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogRunPartner.h"
#include "DlgCalendar.h"
#include "DlgFault.h"
#include "DlgOtherPoint.h"
#include "DlgPartner.h"
#include "ListCtrl.h"
#include "ListData.h"

#include "res/CalTentative.xpm"
#include "res/CalPlan.xpm"
#include "res/CalPlanTentative.xpm"
#include "res/CalEntered.xpm"
#include "res/CalEnteredTentative.xpm"


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
	info.SetText(OnNeedText(iCol));
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
			ARBCalendarPtr pCal)
		: CDlgListCtrlData(parent->m_ctrlList)
		, m_Parent(parent)
		, m_pCal(pCal)
	{
	}
	virtual bool HasIcon() const				{return true;}
	virtual int OnNeedIcon() const;
	virtual wxString OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
	virtual ARBCalendarPtr GetCalendar() const	{return m_pCal;}
private:
	CDlgListCtrl* m_Parent;
	ARBCalendarPtr m_pCal;
};
typedef tr1::shared_ptr<CDlgListCtrlDataCalendar> CDlgListCtrlDataCalendarPtr;


int CDlgListCtrlDataCalendar::OnNeedIcon() const
{
	switch (GetCalendar()->GetEntered())
	{
	default:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgTentative : m_Parent->m_imgEmpty;
	case ARBCalendar::ePlanning:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgPlanTentative : m_Parent->m_imgPlan;
	case ARBCalendar::eEntered:
		return GetCalendar()->IsTentative() ? m_Parent->m_imgEnteredTentative : m_Parent->m_imgEntered;
	}
}


wxString CDlgListCtrlDataCalendar::OnNeedText(long iCol) const
{
	wxString str;
	switch (iCol)
	{
	case 1: // Start Date
		str = m_pCal->GetStartDate().GetString(ARBDate::eDashYMD).c_str();
		break;
	case 2: // End Date
		str = m_pCal->GetEndDate().GetString(ARBDate::eDashYMD).c_str();
		break;
	case 3: // Location
		str = m_pCal->GetLocation().c_str();
		break;
	case 4: // Club
		str = m_pCal->GetClub().c_str();
		break;
	case 5: // Venue
		str = m_pCal->GetVenue().c_str();
		break;
	case 6: // Notes
		str = m_pCal->GetNote().c_str();
		str.Replace(wxT("\n"), wxT(" "));
		break;
	}
	return str;
}


bool CDlgListCtrlDataCalendar::OnEdit()
{
	CDlgCalendar dlg(m_pCal, m_Parent->m_pDoc);
	if (wxID_OK == dlg.ShowModal())
	{
		if (CAgilityBookOptions::AutoDeleteCalendarEntries() && m_pCal->GetEndDate() < ARBDate::Today())
			return true;
	}
	return false;
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
			CReportListCtrl* ctrl,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr pRun,
			std::set<tstring>& faults);
	CDlgListCtrlDataFaults(
			CReportListCtrl* list,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr pRun,
			tstring fault)
		: CDlgListCtrlData(list)
		, m_pDoc(pDoc)
		, m_pRun(pRun)
		, m_Fault(fault)
	{
	}
	virtual wxString OnNeedText(long iCol) const	{return m_Fault.c_str();}
	virtual bool OnEdit();
	virtual void Apply();
private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRunPtr m_pRun;
	tstring m_Fault;
};
typedef tr1::shared_ptr<CDlgListCtrlDataFaults> CDlgListCtrlDataFaultsPtr;


void CDlgListCtrlDataFaults::GetAllFaults(
		CReportListCtrl* ctrl,
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr pRun,
		std::set<tstring>& faults)
{
	faults.clear();
	pDoc->Book().GetAllFaultTypes(faults);
	if (pRun)
	{
		for (ARBDogFaultList::const_iterator iterFault = pRun->GetFaults().begin();
			iterFault != pRun->GetFaults().end();
			++iterFault)
		{
			faults.insert((*iterFault));
		}
	}
	for (int index = 0; index < ctrl->GetItemCount(); ++index)
	{
		wxListItem info;
		info.SetMask(wxLIST_MASK_TEXT);
		ctrl->GetColumn(index, info);
		faults.insert(info.GetText().c_str());
	}
}


bool CDlgListCtrlDataFaults::OnEdit()
{
	std::set<tstring> faults;
	CDlgListCtrlDataFaults::GetAllFaults(m_List, m_pDoc, m_pRun, faults);
	CDlgFault dlg(faults, m_Fault.c_str());
	if (wxID_OK == dlg.ShowModal())
	{
		wxString fault = dlg.GetFault();
		fault.Trim(true);
		fault.Trim(false);
		m_Fault = fault.c_str();
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
public:
	CDlgListCtrlDataOtherPoints(
			CReportListCtrl* list,
			ARBConfig& config,
			ARBDogRunPtr pRun,
			ARBDogRunOtherPointsPtr pOther)
		: CDlgListCtrlData(list)
		, m_pConfig(config)
		, m_pRun(pRun)
		, m_Other(pOther)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
private:
	ARBConfig& m_pConfig;
	ARBDogRunPtr m_pRun;
	ARBDogRunOtherPointsPtr m_Other;
};
typedef tr1::shared_ptr<CDlgListCtrlDataOtherPoints> CDlgListCtrlDataOtherPointsPtr;


wxString CDlgListCtrlDataOtherPoints::OnNeedText(long iCol) const
{
	wxString str;
	switch (iCol)
	{
	default:
	case 0:
		str = m_Other->GetName().c_str();
		break;
	case 1:
		{
			otstringstream tmp;
			tmp << m_Other->GetPoints();
			str = tmp.str().c_str();
		}
		break;
	}
	return str;
}


bool CDlgListCtrlDataOtherPoints::OnEdit()
{
	CDlgOtherPoint dlg(m_pConfig, m_Other);
	if (wxID_OK == dlg.ShowModal())
		return true;
	else
		return false;
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
			ARBDogRunPtr pRun,
			ARBDogRunPartnerPtr pPartner)
		: CDlgListCtrlData(list)
		, m_pDlg(pDlg)
		, m_pRun(pRun)
		, m_Partner(pPartner)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
private:
	CDlgListCtrl* m_pDlg;
	ARBDogRunPtr m_pRun;
	ARBDogRunPartnerPtr m_Partner;
};
typedef tr1::shared_ptr<CDlgListCtrlDataPartners> CDlgListCtrlDataPartnersPtr;


wxString CDlgListCtrlDataPartners::OnNeedText(long iCol) const
{
	tstring str;
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
	return str.c_str();
}


bool CDlgListCtrlDataPartners::OnEdit()
{
	std::set<tstring> handlers, dogs;
	m_pDlg->GetAllPartners(handlers, dogs);
	CDlgPartner dlg(m_Partner, handlers, dogs);
	if (wxID_OK == dlg.ShowModal())
		return true;
	else
		return false;
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
	, m_ctrlList(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlUp(NULL)
	, m_ctrlDown(NULL)
	, m_ctrlCreateTrial(NULL)
	, m_What(eCalendar)
	, m_pDoc(pDoc)
	, m_Date(date)
	, m_CalEntries(entries)
	, m_pTabView(pTabView)
	, m_pConfig(NULL)
	, m_pRun()
{
	Create(_("IDS_CALENDAR"), pParent);
	m_ctrlCreateTrial->Show();

	int nCols = 0;
	m_ctrlList->InsertColumn(nCols++, wxT(""));
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
		ARBDogRunPtr run,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlUp(NULL)
	, m_ctrlDown(NULL)
	, m_ctrlCreateTrial(NULL)
	, m_What(inType)
	, m_pDoc(pDoc)
	, m_Date()
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(NULL)
	, m_pRun(run)
{
	int nCols = 0;
	std::vector<CDlgListCtrlDataPtr> items;

	if (eFaults == m_What)
	{
		Create(_("IDS_FAULT_TYPES"), pParent);
		m_ctrlList->InsertColumn(nCols++, _("IDS_COL_FAULT"));
		for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin(); iter != m_pRun->GetFaults().end(); ++iter)
		{
			CDlgListCtrlDataFaultsPtr pData(new CDlgListCtrlDataFaults(m_ctrlList, m_pDoc, m_pRun, (*iter)));
			items.push_back(pData);
		}
	}

	else if (ePartners == m_What)
	{
		Create(_("IDS_PARTNERS"), pParent);
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
		ARBDogRunPtr run,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlUp(NULL)
	, m_ctrlDown(NULL)
	, m_ctrlCreateTrial(NULL)
	, m_What(eOtherPoints)
	, m_pDoc(NULL)
	, m_Date()
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(&pConfig)
	, m_pRun(run)
{
	Create(_("IDS_OTHERPOINTS"), pParent);

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
		wxString const& inCaption,
		wxWindow* pParent)
{
	if (!wxDialog::Create(pParent, wxID_ANY, inCaption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

	// Controls (these are done first to control tab order)

	m_ctrlList = new CReportListCtrl(this,
		wxDefaultPosition, wxSize(400,150),
		true, CReportListCtrl::eNoSortHeader, true);
	m_imgEmpty = m_ctrlList->ImageEmpty();
	m_imgTentative = m_ctrlList->AddIcon(wxIcon(CalTentative_xpm));
	m_imgPlan = m_ctrlList->AddIcon(wxIcon(CalPlan_xpm));
	m_imgPlanTentative = m_ctrlList->AddIcon(wxIcon(CalPlanTentative_xpm));
	m_imgEntered = m_ctrlList->AddIcon(wxIcon(CalEntered_xpm));
	m_imgEnteredTentative = m_ctrlList->AddIcon(wxIcon(CalEnteredTentative_xpm));
	m_ctrlList->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgListCtrl::OnItemSelected), NULL, this);
	m_ctrlList->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgListCtrl::OnDoubleClick), NULL, this);
	m_ctrlList->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgListCtrl::OnKeyDown), NULL, this);
	m_ctrlList->SetHelpText(_("HIDC_LIST"));
	m_ctrlList->SetToolTip(_("HIDC_LIST"));

	wxButton* btnOk = new wxButton(this, wxID_OK);

	wxButton* btnCancel = new wxButton(this, wxID_CANCEL);

	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_LIST_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnMoveUp), NULL, this);
	m_ctrlUp->SetHelpText(_("HIDC_LIST_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_LIST_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_LIST_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnMoveDown), NULL, this);
	m_ctrlDown->SetHelpText(_("HIDC_LIST_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_LIST_MOVE_DOWN"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_LIST_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnNew), NULL, this);
	m_ctrlNew->SetHelpText(_("HIDC_LIST_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_LIST_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_LIST_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_LIST_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_LIST_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_LIST_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnDelete), NULL, this);
	m_ctrlDelete->SetHelpText(_("HIDC_LIST_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_LIST_DELETE"));

	m_ctrlCreateTrial = new wxButton(this, wxID_ANY,
		_("IDC_LIST_TRIAL"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCreateTrial->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListCtrl::OnCreateTrial), NULL, this);
	m_ctrlCreateTrial->SetHelpText(_("HIDC_LIST_TRIAL"));
	m_ctrlCreateTrial->SetToolTip(_("HIDC_LIST_TRIAL"));
	m_ctrlCreateTrial->Show(false);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizerList = new wxBoxSizer(wxHORIZONTAL);
	bSizerList->Add(m_ctrlList, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* bSizerSide = new wxBoxSizer(wxVERTICAL);
	bSizerSide->Add(btnOk, 0, wxALL, 5);
	bSizerSide->Add(btnCancel, 0, wxALL, 5);
	bSizerSide->Add(0, 0, 1, wxEXPAND, 5);
	bSizerSide->Add(m_ctrlUp, 0, wxALL, 5);
	bSizerSide->Add(m_ctrlDown, 0, wxALL, 5);

	bSizerList->Add(bSizerSide, 0, wxEXPAND, 5);

	bSizer->Add(bSizerList, 1, wxEXPAND, 5);

	wxBoxSizer* bSizerEdit = new wxBoxSizer(wxHORIZONTAL);
	bSizerEdit->Add(m_ctrlNew, 0, wxALL, 5);
	bSizerEdit->Add(m_ctrlEdit, 0, wxALL, 5);
	bSizerEdit->Add(m_ctrlDelete, 0, wxALL, 5);
	bSizerEdit->Add(0, 0, 1, wxEXPAND, 5);
	bSizerEdit->Add(m_ctrlCreateTrial, 0, wxALL, 5);

	bSizer->Add(bSizerEdit, 0, wxEXPAND, 5);

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
	UpdateControls();
}


CDlgListCtrlDataPtr CDlgListCtrl::GetItemListData(long item) const
{
	return tr1::dynamic_pointer_cast<CDlgListCtrlData, CListData>(m_ctrlList->GetData(item));
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


void CDlgListCtrl::OnEdit()
{
	int nItem = m_ctrlList->GetFirstSelected();
	if (0 <= nItem)
	{
		CDlgListCtrlDataPtr pData = GetItemListData(nItem);
		if (pData && pData->OnEdit())
		{
			m_ctrlList->RefreshItem(nItem);
			UpdateControls();
		}
	}
}


void CDlgListCtrl::GetAllPartners(
		std::set<tstring>& ioPartners,
		std::set<tstring>& ioDogs) const
{
	if (m_pDoc)
	{
		m_pDoc->Book().GetAllPartners(ioPartners, ioDogs);
		for (int index = 0; index < m_ctrlList->GetItemCount(); ++index)
		{
			CDlgListCtrlDataPtr pData = GetItemListData(index);
			if (pData)
			{
				ioPartners.insert(pData->OnNeedText(2).c_str());
				ioDogs.insert(pData->OnNeedText(0).c_str());
			}
		}
	}
}


void CDlgListCtrl::OnItemSelected(wxListEvent& /*evt*/)
{
	UpdateControls();
}


void CDlgListCtrl::OnDoubleClick(wxMouseEvent& evt)
{
	OnEdit();
	evt.Skip();
}


void CDlgListCtrl::OnKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		OnEdit();
		break;
	}
	evt.Skip();
}


void CDlgListCtrl::OnNew(wxCommandEvent& evt)
{
	bool bUpdate = false;
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
					m_ctrlList->Select(m_ctrlList->InsertItem(pData));
				}
			}
		}
		break;

	case eFaults:
		{
			std::set<tstring> faults;
			CDlgListCtrlDataFaults::GetAllFaults(m_ctrlList, m_pDoc, m_pRun, faults);
			CDlgFault dlg(faults, wxEmptyString, this);
			if (wxID_OK == dlg.ShowModal())
			{
				bUpdate = true;
				CDlgListCtrlDataFaultsPtr pData(new CDlgListCtrlDataFaults(m_ctrlList, m_pDoc, m_pRun, dlg.GetFault().c_str()));
				m_ctrlList->Select(m_ctrlList->InsertItem(pData));
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
				m_ctrlList->Select(m_ctrlList->InsertItem(pData));
			}
		}
		break;

	case ePartners:
		{
			ARBDogRunPartnerPtr partner(ARBDogRunPartner::New());
			std::set<tstring> handlers, dogs;
			GetAllPartners(handlers, dogs);
			CDlgPartner dlg(partner, handlers, dogs);
			if (wxID_OK == dlg.ShowModal())
			{
				bUpdate = true;
				CDlgListCtrlDataPartnersPtr pData(new CDlgListCtrlDataPartners(this, m_ctrlList, m_pRun, partner));
				m_ctrlList->Select(m_ctrlList->InsertItem(pData));
			}
		}
		break;
	}
	if (bUpdate)
	{
		int nColumns = m_ctrlList->GetColumnCount();
		for (int i = 0; i < nColumns; ++i)
			m_ctrlList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	}
	UpdateControls();
}


void CDlgListCtrl::OnEdit(wxCommandEvent& evt)
{
	OnEdit();
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
			m_pDoc->UpdateAllViews(NULL, &hint);
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
