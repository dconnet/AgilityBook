/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlData
{
public:
	CDlgListCtrlData(CListCtrl& list) : m_List(list), m_RefCount(1) {}
	void AddRef() {++m_RefCount;}
	void Release()
	{
		--m_RefCount;
		if (0 == m_RefCount)
			delete this;
	}
	virtual bool HasIcon() const					{return false;}
	virtual CString OnNeedText(int iCol) = 0;
	virtual bool OnEdit() = 0;
	virtual void Apply() = 0;
	virtual ARBCalendar const* GetCalendar() const	{return NULL;}
protected:
	virtual ~CDlgListCtrlData() {}
	CListCtrl& m_List;
	UINT m_RefCount;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataCalendar : public CDlgListCtrlData
{
public:
	CDlgListCtrlDataCalendar(CListCtrl& list, CAgilityBookDoc* pDoc, ARBCalendar* pCal)
		: CDlgListCtrlData(list)
		, m_pDoc(pDoc)
		, m_pCal(pCal)
	{
		m_pCal->AddRef();
	}
	virtual bool HasIcon() const					{return true;}
	virtual CString OnNeedText(int iCol);
	virtual bool OnEdit();
	virtual void Apply();
	virtual ARBCalendar const* GetCalendar() const	{return m_pCal;}
protected:
	~CDlgListCtrlDataCalendar()
	{
		m_pCal->Release();
	}
private:
	CAgilityBookDoc* m_pDoc;
	ARBCalendar* m_pCal;
};

CString CDlgListCtrlDataCalendar::OnNeedText(int iCol)
{
	CString str;
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
		str.Replace("\n", " ");
		break;
	}
	return str;
}

bool CDlgListCtrlDataCalendar::OnEdit()
{
	CDlgCalendar dlg(m_pCal, m_pDoc);
	if (IDOK == dlg.DoModal())
	{
		if (CAgilityBookOptions::AutoDeleteCalendarEntries() && m_pCal->GetEndDate() < ARBDate::Today())
			return true;
	}
	return false;
}

void CDlgListCtrlDataCalendar::Apply()
{
	m_pDoc->GetCalendar().AddCalendar(m_pCal);
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListCtrlDataFaults : public CDlgListCtrlData
{
public:
	static void GetAllFaults(CListCtrl& ctrl, CAgilityBookDoc* pDoc, ARBDogRun* pRun, std::set<std::string>& faults);
	CDlgListCtrlDataFaults(CListCtrl& list, CAgilityBookDoc* pDoc, ARBDogRun* pRun, std::string fault)
		: CDlgListCtrlData(list)
		, m_pDoc(pDoc)
		, m_pRun(pRun)
		, m_Fault(fault)
	{
	}
	virtual CString OnNeedText(int iCol)	{return m_Fault.c_str();}
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataFaults()
	{
	}
private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRun* m_pRun;
	std::string m_Fault;
};

void CDlgListCtrlDataFaults::GetAllFaults(CListCtrl& ctrl, CAgilityBookDoc* pDoc, ARBDogRun* pRun, std::set<std::string>& faults)
{
	faults.clear();
	pDoc->GetAllFaultTypes(faults);
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
	std::set<std::string> faults;
	CDlgListCtrlDataFaults::GetAllFaults(m_List, m_pDoc, m_pRun, faults);
	CDlgFault dlg(faults, m_Fault.c_str());
	if (IDOK == dlg.DoModal())
	{
		CString fault = dlg.GetFault();
		fault.TrimRight();
		fault.TrimLeft();
		m_Fault = (LPCSTR)fault;
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
	CDlgListCtrlDataOtherPoints(CListCtrl& list, ARBConfig const* config, ARBDogRun* pRun, ARBDogRunOtherPoints* pOther)
		: CDlgListCtrlData(list)
		, m_pConfig(config)
		, m_pRun(pRun)
		, m_Other(pOther)
	{
		m_Other->AddRef();
	}
	virtual CString OnNeedText(int iCol);
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataOtherPoints()
	{
		m_Other->Release();
	}
private:
	ARBConfig const* m_pConfig;
	ARBDogRun* m_pRun;
	ARBDogRunOtherPoints* m_Other;
};

CString CDlgListCtrlDataOtherPoints::OnNeedText(int iCol)
{
	CString str;
	switch (iCol)
	{
	default:
	case 0:
		str = m_Other->GetName().c_str();
		break;
	case 1:
		str.Format("%hd", m_Other->GetPoints());
		break;
	}
	return str;
}

bool CDlgListCtrlDataOtherPoints::OnEdit()
{
	CDlgOtherPoint dlg(*m_pConfig, m_Other);
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
	CDlgListCtrlDataPartners(CListCtrl& list, ARBDogRun* pRun, ARBDogRunPartner* pPartner)
		: CDlgListCtrlData(list)
		, m_pRun(pRun)
		, m_Partner(pPartner)
	{
		m_Partner->AddRef();
	}
	virtual CString OnNeedText(int iCol);
	virtual bool OnEdit();
	virtual void Apply();
protected:
	~CDlgListCtrlDataPartners()
	{
		m_Partner->Release();
	}
private:
	ARBDogRun* m_pRun;
	ARBDogRunPartner* m_Partner;
};

CString CDlgListCtrlDataPartners::OnNeedText(int iCol)
{
	CString str;
	switch (iCol)
	{
	default:
	case 0:
		str = m_Partner->GetDog().c_str();
		break;
	case 1:
		str = m_Partner->GetRegNum().c_str();
		break;
	case 2:
		str = m_Partner->GetHandler().c_str();
		break;
	}
	return str;
}

bool CDlgListCtrlDataPartners::OnEdit()
{
	CDlgPartner dlg(m_Partner);
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
CDlgListCtrl::CDlgListCtrl(CAgilityBookDoc* pDoc, ARBDate const& date, std::vector<ARBCalendar*> const* entries, CTabView* pTabView, CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_What(eCalendar)
	, m_pDoc(pDoc)
	, m_Date(date)
	, m_CalEntries(entries)
	, m_pTabView(pTabView)
	, m_pConfig(NULL)
	, m_pRun(NULL)
{
	//{{AFX_DATA_INIT(CDlgListCtrl)
	//}}AFX_DATA_INIT
}

// Faults
CDlgListCtrl::CDlgListCtrl(CAgilityBookDoc* pDoc, ARBDogRun* run, CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_What(eFaults)
	, m_pDoc(pDoc)
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(NULL)
	, m_pRun(run)
{
}

// OtherPoints
CDlgListCtrl::CDlgListCtrl(ARBConfig const* pConfig, ARBDogRun* run, CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_What(eOtherPoints)
	, m_pDoc(NULL)
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(pConfig)
	, m_pRun(run)
{
}

// Partners
CDlgListCtrl::CDlgListCtrl(ARBDogRun* run, CWnd* pParent)
	: CDlgBaseDialog(CDlgListCtrl::IDD, pParent)
	, m_What(ePartners)
	, m_pDoc(NULL)
	, m_CalEntries(NULL)
	, m_pTabView(NULL)
	, m_pConfig(NULL)
	, m_pRun(run)
{
}

void CDlgListCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListCtrl)
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_LIST_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_LIST_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_LIST_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_LIST_MOVE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_LIST_TRIAL, m_ctrlCreateTrial);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgListCtrl, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgListCtrl)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, OnDeleteitemList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
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
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(m_ctrlList.GetItemData(index));
		if (pData)
		{
			ARBCalendar const* pCal = pData->GetCalendar();
			if (pCal)
			{
				if (m_pDoc->GetConfig().GetVenues().FindVenue(pCal->GetVenue()))
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

void CDlgListCtrl::SwapEntries(int oldIndex, int newIndex)
{
	LPARAM data1 = m_ctrlList.GetItemData(oldIndex);
	LPARAM data2 = m_ctrlList.GetItemData(newIndex);
	m_ctrlList.SetItemData(oldIndex, data2);
	m_ctrlList.SetItemData(newIndex, data1);
	m_ctrlList.SetSelection(newIndex);
	m_ctrlList.Invalidate();
	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListCtrl message handlers

BOOL CDlgListCtrl::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlList.SetExtendedStyle(m_ctrlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlCreateTrial.ShowWindow(SW_HIDE);

	m_ImageList.Create(16, 16, ILC_MASK, 6, 0);
	CWinApp* app = AfxGetApp();
	m_imgEmpty = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_EMPTY));
	m_imgTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_TENTATIVE));
	m_imgPlan = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN));
	m_imgPlanTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN_TENTATIVE));
	m_imgEntered = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED));
	m_imgEnteredTentative = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_ENTERED_TENTATIVE));

	int nCols = 0;
	std::vector<CDlgListCtrlData*> items;

	CString caption, str;
	switch (m_What)
	{
	default:
		ASSERT(0);
		return TRUE;

	case eCalendar:
		caption.LoadString(IDS_CALENDAR);
		m_ctrlCreateTrial.ShowWindow(SW_SHOW);
		m_ctrlList.SetImageList(&m_ImageList, LVSIL_SMALL); // We 'know' only this one has icons.
		m_ctrlList.InsertColumn(0, "");
		str.LoadString(IDS_COL_START_DATE); m_ctrlList.InsertColumn(1, str);
		str.LoadString(IDS_COL_END_DATE); m_ctrlList.InsertColumn(2, str);
		str.LoadString(IDS_COL_LOCATION); m_ctrlList.InsertColumn(3, str);
		str.LoadString(IDS_COL_CLUB); m_ctrlList.InsertColumn(4, str);
		str.LoadString(IDS_COL_VENUE); m_ctrlList.InsertColumn(5, str);
		str.LoadString(IDS_COL_NOTES); m_ctrlList.InsertColumn(6, str);
		nCols = 7;
		{
			for (std::vector<ARBCalendar*>::const_iterator iter = m_CalEntries->begin(); iter != m_CalEntries->end(); ++iter)
			{
				ARBCalendar* pCal = new ARBCalendar(*(*iter));
				CDlgListCtrlDataCalendar* pData = new CDlgListCtrlDataCalendar(m_ctrlList, m_pDoc, pCal);
				pCal->Release();
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
				ARBDogRunOtherPoints* pOther = new ARBDogRunOtherPoints(*(*iter));
				CDlgListCtrlDataOtherPoints* pData = new CDlgListCtrlDataOtherPoints(m_ctrlList, m_pConfig, m_pRun, pOther);
				pOther->Release();
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
				ARBDogRunPartner* pPartner = new ARBDogRunPartner(*(*iter));
				CDlgListCtrlDataPartners* pData = new CDlgListCtrlDataPartners(m_ctrlList, m_pRun, pPartner);
				pPartner->Release();
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
		item.lParam = reinterpret_cast<LPARAM>((*iter));
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

void CDlgListCtrl::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(pDispInfo->item.lParam);
		CDlgListCtrlDataCalendar *pCal = dynamic_cast<CDlgListCtrlDataCalendar*>(pData);
		if (pCal)
		{
			switch (pCal->GetCalendar()->GetEntered())
			{
			default:
				if (pCal->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgTentative;
				else
					pDispInfo->item.iImage = m_imgEmpty;
				break;
			case ARBCalendar::ePlanning:
				if (pCal->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgPlanTentative;
				else
					pDispInfo->item.iImage = m_imgPlan;
				break;
			case ARBCalendar::eEntered:
				if (pCal->GetCalendar()->IsTentative())
					pDispInfo->item.iImage = m_imgEnteredTentative;
				else
					pDispInfo->item.iImage = m_imgEntered;
				break;
			}
		}
	}
	*pResult = 0;
}

void CDlgListCtrl::OnItemchangedList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateControls();
	*pResult = 0;
}

void CDlgListCtrl::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CDlgListCtrl::OnDblclkList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgListCtrl::OnNew() 
{
	switch (m_What)
	{
	default:
		ASSERT(0);
		break;
	case eCalendar:
		{
			ARBCalendar* cal = new ARBCalendar();
			cal->SetStartDate(m_Date);
			cal->SetEndDate(m_Date);
			CDlgCalendar dlg(cal, m_pDoc);
			if (IDOK == dlg.DoModal())
			{
				if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
				{
					CDlgListCtrlDataCalendar* pData = new CDlgListCtrlDataCalendar(m_ctrlList, m_pDoc, cal);
					LV_ITEM item;
					item.mask = LVIF_TEXT | LVIF_PARAM;
					if (pData->HasIcon())
						item.mask |= LVIF_IMAGE;
					item.pszText = LPSTR_TEXTCALLBACK;
					item.iItem = m_ctrlList.GetItemCount();
					item.iSubItem = 0;
					item.iImage = I_IMAGECALLBACK;
					item.lParam = reinterpret_cast<LPARAM>(pData);
					m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
				}
			}
			cal->Release();
		}
		break;

	case eFaults:
		{
			std::set<std::string> faults;
			CDlgListCtrlDataFaults::GetAllFaults(m_ctrlList, m_pDoc, m_pRun, faults);
			CDlgFault dlg(faults, NULL, this);
			if (IDOK == dlg.DoModal())
			{
				CDlgListCtrlDataFaults* pData = new CDlgListCtrlDataFaults(m_ctrlList, m_pDoc, m_pRun, (LPCSTR)dlg.GetFault());
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(pData);
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
		}
		break;

	case eOtherPoints:
		{
			ARBDogRunOtherPoints* pOther = new ARBDogRunOtherPoints();
			CDlgOtherPoint dlg(*m_pConfig, pOther, this);
			if (IDOK == dlg.DoModal())
			{
				CDlgListCtrlDataOtherPoints* pData = new CDlgListCtrlDataOtherPoints(m_ctrlList, m_pConfig, m_pRun, pOther);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(pData);
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
			pOther->Release();
		}
		break;

	case ePartners:
		{
			ARBDogRunPartner* partner = new ARBDogRunPartner();
			CDlgPartner dlg(partner, this);
			if (IDOK == dlg.DoModal())
			{
				CDlgListCtrlDataPartners* pData = new CDlgListCtrlDataPartners(m_ctrlList, m_pRun, partner);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				if (pData->HasIcon())
					item.mask |= LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = m_ctrlList.GetItemCount();
				item.iSubItem = 0;
				item.iImage = I_IMAGECALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(pData);
				m_ctrlList.SetSelection(m_ctrlList.InsertItem(&item));
			}
			partner->Release();
		}
		break;
	}
	UpdateControls();
}

void CDlgListCtrl::OnEdit() 
{
	int nItem = m_ctrlList.GetSelection();
	if (0 <= nItem)
	{
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(m_ctrlList.GetItemData(nItem));
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
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(m_ctrlList.GetItemData(nItem));
		if (pData)
		{
			ARBCalendar const* pCal = pData->GetCalendar();
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
		ASSERT(0);
		break;
	case eCalendar:
		{
			for (std::vector<ARBCalendar*>::const_iterator iter = m_CalEntries->begin(); iter != m_CalEntries->end(); ++iter)
				m_pDoc->GetCalendar().DeleteCalendar((*iter));
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
		CDlgListCtrlData *pData = reinterpret_cast<CDlgListCtrlData*>(m_ctrlList.GetItemData(index));
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
			m_pDoc->GetCalendar().TrimEntries(today);
		}
		m_pDoc->GetCalendar().sort();
		m_pDoc->UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
		break;
	default:
		break;
	}

	CDlgBaseDialog::OnOK();
}
