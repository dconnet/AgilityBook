/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
#include "AgilityBook.h"
#include "DlgListCtrl.h"

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
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlData : public CListDataDispInfo
{
public:
	CDlgListCtrlData(CListCtrl2& list)
		: m_List(list)
	{
	}
	virtual ~CDlgListCtrlData() {}
	virtual bool HasIcon() const				{return false;}
	virtual bool OnEdit() = 0;
	virtual void Apply() = 0;
	virtual ARBCalendarPtr GetCalendar() const	{return ARBCalendarPtr();}
protected:
	CListCtrl2& m_List;
};

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
	virtual tstring OnNeedText(int iCol) const;
	virtual int OnNeedIcon() const;
	virtual bool OnEdit();
	virtual void Apply();
	virtual ARBCalendarPtr GetCalendar() const	{return m_pCal;}
protected:
	~CDlgListCtrlDataCalendar()
	{
	}
private:
	CDlgListCtrl* m_Parent;
	ARBCalendarPtr m_pCal;
};


tstring CDlgListCtrlDataCalendar::OnNeedText(int iCol) const
{
	tstring str;
	switch (iCol)
	{
	case 1: // Start Date
		str = m_pCal->GetStartDate().GetString(ARBDate::eDashYMD);
		break;
	case 2: // End Date
		str = m_pCal->GetEndDate().GetString(ARBDate::eDashYMD);
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
		{
			CString tmp(m_pCal->GetNote().c_str());
			tmp.Replace(_T("\n"), _T(" "));
			str = (LPCTSTR)tmp;
		}
		break;
	}
	return str;
}


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


bool CDlgListCtrlDataCalendar::OnEdit()
{
	CDlgCalendar dlg(m_pCal, m_Parent->m_pDoc);
	if (IDOK == dlg.DoModal())
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
			CListCtrl2& ctrl,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr pRun,
			std::set<tstring>& faults);
	CDlgListCtrlDataFaults(
			CListCtrl2& list,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr pRun,
			tstring fault)
		: CDlgListCtrlData(list)
		, m_pDoc(pDoc)
		, m_pRun(pRun)
		, m_Fault(fault)
	{
	}
	virtual tstring OnNeedText(int iCol) const	{return m_Fault;}
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataFaults()
	{
	}
private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRunPtr m_pRun;
	tstring m_Fault;
};


void CDlgListCtrlDataFaults::GetAllFaults(
		CListCtrl2& ctrl,
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
	for (int index = 0; index < ctrl.GetItemCount(); ++index)
	{
		CString text = ctrl.GetItemText(index, 0);
		faults.insert((LPCTSTR)text);
	}
}


bool CDlgListCtrlDataFaults::OnEdit()
{
	std::set<tstring> faults;
	CDlgListCtrlDataFaults::GetAllFaults(m_List, m_pDoc, m_pRun, faults);
	CDlgFault dlg(faults, m_Fault.c_str());
	if (IDOK == dlg.DoModal())
	{
		CString fault = dlg.GetFault();
		fault.TrimRight();
		fault.TrimLeft();
		m_Fault = (LPCTSTR)fault;
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
			CListCtrl2& list,
			ARBConfig& config,
			ARBDogRunPtr pRun,
			ARBDogRunOtherPointsPtr pOther)
		: CDlgListCtrlData(list)
		, m_pConfig(config)
		, m_pRun(pRun)
		, m_Other(pOther)
	{
	}
	virtual tstring OnNeedText(int iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataOtherPoints()
	{
	}
private:
	ARBConfig& m_pConfig;
	ARBDogRunPtr m_pRun;
	ARBDogRunOtherPointsPtr m_Other;
};


tstring CDlgListCtrlDataOtherPoints::OnNeedText(int iCol) const
{
	tstring str;
	switch (iCol)
	{
	default:
	case 0:
		str = m_Other->GetName();
		break;
	case 1:
		{
			otstringstream tmp;
			tmp << m_Other->GetPoints();
			str = tmp.str();
		}
		break;
	}
	return str;
}


bool CDlgListCtrlDataOtherPoints::OnEdit()
{
	CDlgOtherPoint dlg(m_pConfig, m_Other);
	if (IDOK == dlg.DoModal())
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
			CListCtrl2& list,
			ARBDogRunPtr pRun,
			ARBDogRunPartnerPtr pPartner)
		: CDlgListCtrlData(list)
		, m_pDlg(pDlg)
		, m_pRun(pRun)
		, m_Partner(pPartner)
	{
	}
	virtual tstring OnNeedText(int iCol) const;
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataPartners()
	{
	}
private:
	CDlgListCtrl* m_pDlg;
	ARBDogRunPtr m_pRun;
	ARBDogRunPartnerPtr m_Partner;
};


tstring CDlgListCtrlDataPartners::OnNeedText(int iCol) const
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
	return str;
}


bool CDlgListCtrlDataPartners::OnEdit()
{
	std::set<tstring> handlers, dogs;
	m_pDlg->GetAllPartners(handlers, dogs);
	CDlgPartner dlg(m_Partner, handlers, dogs);
	if (IDOK == dlg.DoModal())
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

// Calendar
CDlgListCtrl::CDlgListCtrl(
		CAgilityBookDoc* pDoc,
		ARBDate const& date,
		std::vector<ARBCalendarPtr> const* entries,
		CTabView* pTabView,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_ctrlList(true)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rNew(0,0,0,0)
	, m_rEdit(0,0,0,0)
	, m_rDelete(0,0,0,0)
	, m_rUp(0,0,0,0)
	, m_rDown(0,0,0,0)
	, m_rTrial(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_rCancel(0,0,0,0)
	, m_What(eCalendar)
	, m_pDoc(pDoc)
	, m_Date(date)
	, m_CalEntries(entries)
	, m_pTabView(pTabView)
	, m_pConfig(NULL)
	, m_pRun()
{
	//{{AFX_DATA_INIT(CDlgListCtrl)
	//}}AFX_DATA_INIT
}


// Faults/Partners
CDlgListCtrl::CDlgListCtrl(
		CDlgListCtrl::WhatToList inType,
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr run,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_ctrlList(true)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rNew(0,0,0,0)
	, m_rEdit(0,0,0,0)
	, m_rDelete(0,0,0,0)
	, m_rUp(0,0,0,0)
	, m_rDown(0,0,0,0)
	, m_rTrial(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_rCancel(0,0,0,0)
	, m_What(inType)
	, m_pDoc(pDoc)
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(NULL)
	, m_pRun(run)
{
	assert(m_What == eFaults || m_What == ePartners);
}


// OtherPoints
CDlgListCtrl::CDlgListCtrl(
		ARBConfig& pConfig,
		ARBDogRunPtr run,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_ctrlList(true)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rNew(0,0,0,0)
	, m_rEdit(0,0,0,0)
	, m_rDelete(0,0,0,0)
	, m_rUp(0,0,0,0)
	, m_rDown(0,0,0,0)
	, m_rTrial(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_rCancel(0,0,0,0)
	, m_What(eOtherPoints)
	, m_pDoc(NULL)
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(&pConfig)
	, m_pRun(run)
{
}


void CDlgListCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListCtrl)
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_LIST_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_LIST_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_LIST_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_LIST_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_LIST_MOVE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_LIST_TRIAL, m_ctrlCreateTrial);
	DDX_Control(pDX, IDOK, m_ctrlOk);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgListCtrl, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgListCtrl)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	ON_BN_CLICKED(IDC_LIST_NEW, OnNew)
	ON_BN_CLICKED(IDC_LIST_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_LIST_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_LIST_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_LIST_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_LIST_TRIAL, OnCreateTrial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgListCtrl::UpdateControls()
{
	BOOL bEnable = FALSE;
	int index = m_ctrlList.GetSelection();
	if (0 <= index)
		bEnable = TRUE;
	m_ctrlEdit.EnableWindow(bEnable);
	m_ctrlDelete.EnableWindow(bEnable);

	BOOL bEnableCreate = FALSE;
	if (bEnable && eCalendar == m_What)
	{
		CListData* pRawData = m_ctrlList.GetData(index);
		CDlgListCtrlData* pData = dynamic_cast<CDlgListCtrlData*>(pRawData);
		if (pData)
		{
			ARBCalendarPtr pCal = pData->GetCalendar();
			if (pCal)
			{
				if (m_pDoc->Book().GetConfig().GetVenues().FindVenue(pCal->GetVenue()))
					bEnableCreate = TRUE;
			}
		}
	}
	m_ctrlCreateTrial.EnableWindow(bEnableCreate);

	BOOL bEnableUp = bEnable;
	BOOL bEnableDown = bEnable;
	if (0 <= index)
	{
		bEnableUp = bEnableDown = TRUE;
		if (0 == index)
			bEnableUp = FALSE;
		if (index == m_ctrlList.GetItemCount() - 1)
			bEnableDown = FALSE;
	}
	m_ctrlUp.EnableWindow(bEnableUp);
	m_ctrlDown.EnableWindow(bEnableDown);
}


void CDlgListCtrl::SwapEntries(
		int oldIndex,
		int newIndex)
{
	LPARAM data1 = m_ctrlList.GetItemData(oldIndex);
	LPARAM data2 = m_ctrlList.GetItemData(newIndex);
	m_ctrlList.SetItemData(oldIndex, data2);
	m_ctrlList.SetItemData(newIndex, data1);
	m_ctrlList.SetSelection(newIndex);
	m_ctrlList.Invalidate();
	UpdateControls();
}


void CDlgListCtrl::GetAllPartners(
		std::set<tstring>& ioPartners,
		std::set<tstring>& ioDogs) const
{
	if (m_pDoc)
	{
		m_pDoc->Book().GetAllPartners(ioPartners, ioDogs);
		for (int index = 0; index < m_ctrlList.GetItemCount(); ++index)
		{
			CListData* pRawData = m_ctrlList.GetData(index);
			CDlgListCtrlDataPartners* pData = dynamic_cast<CDlgListCtrlDataPartners*>(pRawData);
			if (pData)
			{
				ioPartners.insert(pData->OnNeedText(2));
				ioDogs.insert(pData->OnNeedText(0));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListCtrl message handlers

BOOL CDlgListCtrl::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlList.SetExtendedStyle(m_ctrlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlCreateTrial.ShowWindow(SW_HIDE);

	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 6, 0);
	m_imgEmpty = m_ImageList.Add(theApp.LoadIcon(IDI_EMPTY));
	m_imgTentative = m_ImageList.Add(theApp.LoadIcon(IDI_QUESTIONMARK));
	m_imgPlan = m_ImageList.Add(theApp.LoadIcon(IDI_CHECKMARK));
	m_imgPlanTentative = m_ImageList.Add(theApp.LoadIcon(IDI_CALENDAR_PLAN_TENTATIVE));
	m_imgEntered = m_ImageList.Add(theApp.LoadIcon(IDI_CALENDAR_ENTERED));
	m_imgEnteredTentative = m_ImageList.Add(theApp.LoadIcon(IDI_CALENDAR_ENTERED_TENTATIVE));

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), TRUE);	// Set big icon
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);	// Set small icon

	GetWindowRect(m_rWin);
	GetClientRect(m_rDlg);
	m_ctrlList.GetWindowRect(m_rList);
	ScreenToClient(m_rList);
	m_ctrlNew.GetWindowRect(m_rNew);
	ScreenToClient(m_rNew);
	m_ctrlEdit.GetWindowRect(m_rEdit);
	ScreenToClient(m_rEdit);
	m_ctrlDelete.GetWindowRect(m_rDelete);
	ScreenToClient(m_rDelete);
	m_ctrlUp.GetWindowRect(m_rUp);
	ScreenToClient(m_rUp);
	m_ctrlDown.GetWindowRect(m_rDown);
	ScreenToClient(m_rDown);
	m_ctrlCreateTrial.GetWindowRect(m_rTrial);
	ScreenToClient(m_rTrial);
	m_ctrlOk.GetWindowRect(m_rOK);
	ScreenToClient(m_rOK);
	m_ctrlCancel.GetWindowRect(m_rCancel);
	ScreenToClient(m_rCancel);

	int nCols = 0;
	std::vector<CDlgListCtrlData*> items;

	CString caption, str;
	switch (m_What)
	{
	default:
		assert(0);
		return TRUE;

	case eCalendar:
		caption.LoadString(IDS_CALENDAR);
		m_ctrlCreateTrial.ShowWindow(SW_SHOW);
		m_ctrlList.SetImageList(&m_ImageList, LVSIL_SMALL); // We 'know' only this one has icons.
		m_ctrlList.InsertColumn(0, _T(""));
		str.LoadString(IDS_COL_START_DATE); m_ctrlList.InsertColumn(1, str);
		str.LoadString(IDS_COL_END_DATE); m_ctrlList.InsertColumn(2, str);
		str.LoadString(IDS_COL_LOCATION); m_ctrlList.InsertColumn(3, str);
		str.LoadString(IDS_COL_CLUB); m_ctrlList.InsertColumn(4, str);
		str.LoadString(IDS_COL_VENUE); m_ctrlList.InsertColumn(5, str);
		str.LoadString(IDS_COL_NOTES); m_ctrlList.InsertColumn(6, str);
		nCols = 7;
		{
			for (std::vector<ARBCalendarPtr>::const_iterator iter = m_CalEntries->begin(); iter != m_CalEntries->end(); ++iter)
			{
				CDlgListCtrlDataCalendar* pData = new CDlgListCtrlDataCalendar(this, *iter);
				items.push_back(pData);
			}
		}
		break;

	case eFaults:
		caption.LoadString(IDS_FAULT_TYPES);
		str.LoadString(IDS_COL_FAULT); m_ctrlList.InsertColumn(0, str);
		nCols = 1;
		{
			for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin(); iter != m_pRun->GetFaults().end(); ++iter)
			{
				CDlgListCtrlDataFaults* pData = new CDlgListCtrlDataFaults(m_ctrlList, m_pDoc, m_pRun, (*iter));
				items.push_back(pData);
			}
		}
		break;

	case eOtherPoints:
		caption.LoadString(IDS_OTHERPOINTS);
		str.LoadString(IDS_COL_NAME); m_ctrlList.InsertColumn(0, str);
		str.LoadString(IDS_COL_POINTS); m_ctrlList.InsertColumn(1, str);
		nCols = 2;
		{
			for (ARBDogRunOtherPointsList::const_iterator iter = m_pRun->GetOtherPoints().begin(); iter != m_pRun->GetOtherPoints().end(); ++iter)
			{
				CDlgListCtrlDataOtherPoints* pData = new CDlgListCtrlDataOtherPoints(m_ctrlList, *m_pConfig, m_pRun, *iter);
				items.push_back(pData);
			}
		}
		break;

	case ePartners:
		caption.LoadString(IDS_PARTNERS);
		str.LoadString(IDS_COL_DOG); m_ctrlList.InsertColumn(0, str);
		str.LoadString(IDS_COL_NUMBER); m_ctrlList.InsertColumn(1, str);
		str.LoadString(IDS_COL_HANDLER); m_ctrlList.InsertColumn(2, str);
		nCols = 3;
		{
			for (ARBDogRunPartnerList::const_iterator iter = m_pRun->GetPartners().begin(); iter != m_pRun->GetPartners().end(); ++iter)
			{
				CDlgListCtrlDataPartners* pData = new CDlgListCtrlDataPartners(this, m_ctrlList, m_pRun, *iter);
				items.push_back(pData);
			}
		}
		break;
	}
	SetWindowText(caption);

	int i = 0;
	for (std::vector<CDlgListCtrlData*>::const_iterator iter = items.begin(); iter != items.end(); ++i, ++iter)
	{
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		if ((*iter)->HasIcon())
			item.mask |= LVIF_IMAGE;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = i;
		item.iSubItem = 0;
		item.iImage = I_IMAGECALLBACK;
		item.lParam = reinterpret_cast<LPARAM>(
			static_cast<CListData*>(
				(*iter)));
		m_ctrlList.InsertItem(&item);
	}
	if (0 < i)
		m_ctrlList.SetSelection(0);

	for (i = 0; i < nCols; ++i)
		m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgListCtrl::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rWin.Width();
	lpMMI->ptMinTrackSize.y = m_rWin.Height();
	CDlgBaseDialog::OnGetMinMaxInfo(lpMMI);
}


void CDlgListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDlgBaseDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ctrlList.GetSafeHwnd()))
	{
		m_ctrlList.SetWindowPos(NULL,
			0, 0,
			cx - (m_rDlg.Width() - m_rList.Width()), cy - (m_rDlg.Height() - m_rList.Height()),
			SWP_NOZORDER | SWP_NOMOVE);
		m_ctrlNew.SetWindowPos(NULL,
			m_rNew.left, cy - (m_rDlg.Height() - m_rNew.bottom) - m_rNew.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlEdit.SetWindowPos(NULL,
			m_rEdit.left, cy - (m_rDlg.Height() - m_rEdit.bottom) - m_rEdit.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlDelete.SetWindowPos(NULL,
			m_rDelete.left, cy - (m_rDlg.Height() - m_rDelete.bottom) - m_rDelete.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlUp.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rUp.left), cy - (m_rDlg.Height() - m_rUp.bottom) - m_rUp.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlDown.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rDown.left), cy - (m_rDlg.Height() - m_rDown.bottom) - m_rDown.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlCreateTrial.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rTrial.left), cy - (m_rDlg.Height() - m_rTrial.bottom) - m_rTrial.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlOk.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rOK.left), m_rOK.top,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlCancel.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rCancel.left), m_rCancel.top,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}


void CDlgListCtrl::OnItemchangedList(
		NMHDR* /*pNMHDR*/,
		LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateControls();
	*pResult = 0;
}


void CDlgListCtrl::OnDblclkList(
		NMHDR* /*pNMHDR*/,
		LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}


void CDlgListCtrl::OnKeydownList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnEdit();
		break;
	}
	*pResult = 0;
}


void CDlgListCtrl::OnNew() 
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
			if (IDOK == dlg.DoModal())
			{
				if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
				{
					bUpdate = true;
					CDlgListCtrlDataCalendar* pData = new CDlgListCtrlDataCalendar(this, cal);
					LV_ITEM item;
					item.mask = LVIF_TEXT | LVIF_PARAM;
					if (pData->HasIcon())
						item.mask |= LVIF_IMAGE;
					item.pszText = LPSTR_TEXTCALLBACK;
					item.iItem = m_ctrlList.GetItemCount();
					item.iSubItem = 0;
					item.iImage = I_IMAGECALLBACK;
					item.lParam = reinterpret_cast<LPARAM>(
						static_cast<CListData*>(pData));
					m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
				}
			}
		}
		break;

	case eFaults:
		{
			std::set<tstring> faults;
			CDlgListCtrlDataFaults::GetAllFaults(m_ctrlList, m_pDoc, m_pRun, faults);
			CDlgFault dlg(faults, NULL, this);
			if (IDOK == dlg.DoModal())
			{
				bUpdate = true;
				CDlgListCtrlDataFaults* pData = new CDlgListCtrlDataFaults(m_ctrlList, m_pDoc, m_pRun, (LPCTSTR)dlg.GetFault());
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(
					static_cast<CListData*>(pData));
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
		}
		break;

	case eOtherPoints:
		{
			ARBDogRunOtherPointsPtr pOther(ARBDogRunOtherPoints::New());
			CDlgOtherPoint dlg(*m_pConfig, pOther, this);
			if (IDOK == dlg.DoModal())
			{
				bUpdate = true;
				CDlgListCtrlDataOtherPoints* pData = new CDlgListCtrlDataOtherPoints(m_ctrlList, *m_pConfig, m_pRun, pOther);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(
					static_cast<CListData*>(pData));
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
		}
		break;

	case ePartners:
		{
			ARBDogRunPartnerPtr partner(ARBDogRunPartner::New());
			std::set<tstring> handlers, dogs;
			GetAllPartners(handlers, dogs);
			CDlgPartner dlg(partner, handlers, dogs);
			if (IDOK == dlg.DoModal())
			{
				bUpdate = true;
				CDlgListCtrlDataPartners* pData = new CDlgListCtrlDataPartners(this, m_ctrlList, m_pRun, partner);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(
					static_cast<CListData*>(pData));
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
		}
		break;
	}
	if (bUpdate)
	{
		int nColumns = m_ctrlList.GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < nColumns; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	UpdateControls();
}


void CDlgListCtrl::OnEdit() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		CListData* pRawData = m_ctrlList.GetData(nItem);
		CDlgListCtrlData* pData = dynamic_cast<CDlgListCtrlData*>(pRawData);
		if (pData && pData->OnEdit())
		{
			m_ctrlList.Invalidate();
			UpdateControls();
		}
	}
}


void CDlgListCtrl::OnDelete() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		m_ctrlList.DeleteItem(nItem);
		if (nItem == m_ctrlList.GetItemCount())
			--nItem;
		if (0 <= nItem)
			m_ctrlList.SetSelection(nItem);
		UpdateControls();
	}
}


void CDlgListCtrl::OnMoveUp() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		int newIndex = nItem - 1;
		if (0 <= newIndex)
		{
			SwapEntries(nItem, newIndex);
		}
	}
}


void CDlgListCtrl::OnMoveDown() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		int newIndex = nItem + 1;
		if (newIndex < m_ctrlList.GetItemCount())
			SwapEntries(nItem, newIndex);
	}
}


void CDlgListCtrl::OnCreateTrial() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		CListData* pRawData = m_ctrlList.GetData(nItem);
		CDlgListCtrlData* pData = dynamic_cast<CDlgListCtrlData*>(pRawData);
		if (pData)
		{
			ARBCalendarPtr pCal = pData->GetCalendar();
			if (pCal)
			{
				if (m_pDoc->CreateTrialFromCalendar(*pCal, m_pTabView))
					OnOK();
			}
		}
	}
}


void CDlgListCtrl::OnOK() 
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
	for (int index = 0; index < m_ctrlList.GetItemCount(); ++index)
	{
		CListData* pRawData = m_ctrlList.GetData(index);
		CDlgListCtrlData* pData = dynamic_cast<CDlgListCtrlData*>(pRawData);
		if (pData)
			pData->Apply();
	}

	// Post-apply
	switch (m_What)
	{
	case eCalendar:
		if (CAgilityBookOptions::AutoDeleteCalendarEntries())
		{
			ARBDate today(ARBDate::Today());
			today -= CAgilityBookOptions::DaysTillEntryIsPast();
			m_pDoc->Book().GetCalendar().TrimEntries(today);
		}
		m_pDoc->Book().GetCalendar().sort();
		m_pDoc->UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
		break;
	default:
		break;
	}

	CDlgBaseDialog::OnOK();
}
