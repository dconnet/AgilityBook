/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-18 DRC Added 'DeleteTitle' configuration action.
 * @li 2004-01-21 DRC Implemented Action items in configuration update.
 * @li 2004-01-14 DRC Use complete title name instead of nice name.
 * @li 2003-12-27 DRC Changed the scoring method to show the valid date range.
 * @li 2003-02-04 DRC Changed dbl-click to edit an item.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigure.h"

#include "AgilityBookDoc.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigVenue.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigVenue.h"
#include "DlgConfigureData.h"
#include "DlgFixup.h"
#include "DlgMessage.h"
#include "DlgName.h"
#include "DlgNameDesc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CDlgConfigureData* item1 = reinterpret_cast<CDlgConfigureData*>(lParam1);
	CDlgConfigureData* item2 = reinterpret_cast<CDlgConfigureData*>(lParam2);
	int iColumn = static_cast<int>(lParamSort);
	return item1->OnNeedText(iColumn) > item2->OnNeedText(iColumn);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure dialog

CDlgConfigure::CDlgConfigure(CAgilityBookDoc* pDoc, ARBAgilityRecordBook& book)
	: CDialog(CDlgConfigure::IDD)
	, m_pDoc(pDoc)
	, m_Book(book)
	, m_Config(m_Book.GetConfig())
	, m_Action(eNone)
{
	ASSERT(m_pDoc);
	//{{AFX_DATA_INIT(CDlgConfigure)
	//}}AFX_DATA_INIT
}

CDlgConfigure::~CDlgConfigure()
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
	{
		delete (*iter);
	}
	m_DlgFixup.clear();
}

void CDlgConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigure)
	DDX_Control(pDX, IDC_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_FAULTS, m_ctrlFaults);
	DDX_Control(pDX, IDC_OTHERPOINTS, m_ctrlOthers);
	DDX_Control(pDX, IDC_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_COMMENTS, m_ctrlComments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigure, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigure)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_VENUES, OnGetdispinfo)
	ON_NOTIFY(LVN_DELETEITEM, IDC_VENUES, OnDeleteitem)
	ON_NOTIFY(NM_DBLCLK, IDC_VENUES, OnDblclk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_VENUES, OnItemchanged)
	ON_NOTIFY(NM_SETFOCUS, IDC_VENUES, OnSetfocusVenues)
	ON_NOTIFY(NM_SETFOCUS, IDC_FAULTS, OnSetfocusFaults)
	ON_NOTIFY(NM_SETFOCUS, IDC_OTHERPOINTS, OnSetfocusOtherpoints)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FAULTS, OnGetdispinfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_OTHERPOINTS, OnGetdispinfo)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FAULTS, OnDeleteitem)
	ON_NOTIFY(LVN_DELETEITEM, IDC_OTHERPOINTS, OnDeleteitem)
	ON_NOTIFY(NM_DBLCLK, IDC_FAULTS, OnDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_OTHERPOINTS, OnDblclk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FAULTS, OnItemchanged)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OTHERPOINTS, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigure::SetAction(eAction inAction)
{
	if (m_Action != inAction)
	{
		m_Action = inAction;
		switch (m_Action)
		{
		default:
			m_ctrlComments.SetWindowText("");
			break;
		case eVenues:
			m_ctrlComments.SetWindowText("Buttons: Venues");
			break;
		case eFaults:
			m_ctrlComments.SetWindowText("Buttons: Faults");
			break;
		case eOtherPoints:
			m_ctrlComments.SetWindowText("Buttons: Other Points");
			break;
		}
	}
	UpdateButtons();
}

bool CDlgConfigure::GetActionData(CListCtrl2*& pCtrl, int& index, CDlgConfigureData*& pData)
{
	bool bOk = false;
	pCtrl = NULL;
	index = -1;
	pData = NULL;
	switch (m_Action)
	{
	case eVenues:
		pCtrl = &m_ctrlVenues;
		break;
	case eFaults:
		pCtrl = &m_ctrlFaults;
		break;
	case eOtherPoints:
		pCtrl = &m_ctrlOthers;
		break;
	}
	if (pCtrl)
	{
		bOk = true;
		index = pCtrl->GetSelection();
		if (0 <= index)
		{
			pData = reinterpret_cast<CDlgConfigureData*>(pCtrl->GetItemData(index));
		}
	}
	return bOk;
}

void CDlgConfigure::UpdateButtons()
{
	BOOL bNew = FALSE;
	BOOL bDelete = FALSE;
	BOOL bEdit = FALSE;
	BOOL bCopy = FALSE;
	CListCtrl2* pCtrl;
	int index;
	CDlgConfigureData* pData;
	if (GetActionData(pCtrl, index, pData))
	{
		bNew = TRUE;
		if (pData)
			bDelete = bEdit = bCopy = TRUE;
	}
	m_ctrlNew.EnableWindow(bNew);
	m_ctrlDelete.EnableWindow(bDelete);
	m_ctrlEdit.EnableWindow(bEdit);
	m_ctrlCopy.EnableWindow(bCopy);
}

void CDlgConfigure::LoadData()
{
	m_ctrlVenues.DeleteAllItems();
	m_ctrlFaults.DeleteAllItems();
	m_ctrlOthers.DeleteAllItems();

	for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin(); iterVenue != m_Config.GetVenues().end(); ++iterVenue)
	{
		m_ctrlVenues.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataVenue(*iterVenue)));
	}
	m_ctrlVenues.SortItems(CompareItems, 0);
	m_ctrlVenues.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlVenues.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	for (ARBConfigFaultList::iterator iterFault = m_Config.GetFaults().begin(); iterFault != m_Config.GetFaults().end(); ++iterFault)
	{
		m_ctrlFaults.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataFault(*iterFault)));
	}
	m_ctrlFaults.SortItems(CompareItems, 0);
	m_ctrlFaults.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	for (ARBConfigOtherPointsList::iterator iterOther = m_Config.GetOtherPoints().begin(); iterOther != m_Config.GetOtherPoints().end(); ++iterOther)
	{
		m_ctrlOthers.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataOtherPoints(*iterOther)));
	}
	m_ctrlOthers.SortItems(CompareItems, 0);
	m_ctrlOthers.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

int CDlgConfigure::FindCurrentVenue(const ARBConfigVenue* pVenue, bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlVenues.GetItemCount(); ++index)
	{
		CDlgConfigureDataVenue* pData = reinterpret_cast<CDlgConfigureDataVenue*>(m_ctrlVenues.GetItemData(index));
		if (pData->GetVenue() == pVenue)
		{
			idxCurrent = index;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlVenues.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlVenues.EnsureVisible(idxCurrent, FALSE);
	}
	return idxCurrent;
}

int CDlgConfigure::FindCurrentFault(const ARBConfigFault* pFault, bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlFaults.GetItemCount(); ++index)
	{
		CDlgConfigureDataFault* pData = reinterpret_cast<CDlgConfigureDataFault*>(m_ctrlFaults.GetItemData(index));
		if (pData->GetFault() == pFault)
		{
			idxCurrent = index;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlFaults.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlFaults.EnsureVisible(idxCurrent, FALSE);
	}
	return idxCurrent;
}

int CDlgConfigure::FindCurrentOtherPoints(const ARBConfigOtherPoints* pOther, bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlOthers.GetItemCount(); ++index)
	{
		CDlgConfigureDataOtherPoints* pData = reinterpret_cast<CDlgConfigureDataOtherPoints*>(m_ctrlOthers.GetItemData(index));
		if (pData->GetOtherPoints() == pOther)
		{
			idxCurrent = index;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlOthers.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlOthers.EnsureVisible(idxCurrent, FALSE);
	}
	return idxCurrent;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure message handlers

BOOL CDlgConfigure::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlVenues.SetExtendedStyle(m_ctrlVenues.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlFaults.SetExtendedStyle(m_ctrlFaults.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlOthers.SetExtendedStyle(m_ctrlOthers.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlVenues.InsertColumn(0, "Venues");
	m_ctrlVenues.InsertColumn(1, "Description");
	m_ctrlFaults.InsertColumn(0, "Faults");
	m_ctrlOthers.InsertColumn(0, "Other Points");

	LoadData();
	SetAction(eVenues);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigure::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CDlgConfigureData *pData = reinterpret_cast<CDlgConfigureData*>(pDispInfo->item.lParam);
		if (pData)
		{
			CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}

void CDlgConfigure::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CDlgConfigureData *pData = reinterpret_cast<CDlgConfigureData*>(pNMListView->lParam);
	delete pData;
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CDlgConfigure::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgConfigure::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusVenues(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eVenues);
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusFaults(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eFaults);
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusOtherpoints(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eOtherPoints);
	*pResult = 0;
}

void CDlgConfigure::OnNew() 
{
	switch (m_Action)
	{
	case eVenues:
		{
			ARBConfigVenue* pVenue = new ARBConfigVenue();
			CDlgConfigVenue dlg(m_Book, m_Config, pVenue, this);
			if (IDOK == dlg.DoModal())
			{
				ARBConfigVenue* pNewVenue = m_Config.GetVenues().AddVenue(pVenue);
				if (pNewVenue)
				{
					dlg.GetFixups(m_DlgFixup);
					m_ctrlVenues.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(new CDlgConfigureDataVenue(pNewVenue)));
					m_ctrlVenues.SortItems(CompareItems, 0);
					FindCurrentVenue(pNewVenue, true);
				}
			}
			pVenue->Release();
		}
		break;

	case eFaults:
		{
			CDlgName dlg("", IDS_FAULT_TYPE_NAME, this);
			if (IDOK == dlg.DoModal())
			{
				std::string name = dlg.GetName();
				// We could check for uniqueness, but if the user wants 2
				// strings the same, why argue! Afterall, these strings
				// are only "helper" items to fill in other data.
				if (0 < name.length())
				{
					ARBConfigFault* pNewFault = m_Config.GetFaults().AddFault(name);
					if (pNewFault)
					{
						m_ctrlFaults.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlFaults.GetItemCount(),
							LPSTR_TEXTCALLBACK, 0, 0, 0,
							reinterpret_cast<LPARAM>(new CDlgConfigureDataFault(pNewFault)));
						m_ctrlFaults.SortItems(CompareItems, 0);
						FindCurrentFault(pNewFault, true);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			ARBConfigOtherPoints* pOther = new ARBConfigOtherPoints();
			// The dialog will ensure uniqueness.
			CDlgConfigOtherPoints dlg(m_Config, pOther, this);
			if (IDOK == dlg.DoModal())
			{
				ARBConfigOtherPoints* pNewOther = m_Config.GetOtherPoints().AddOtherPoints(pOther);
				if (pOther)
				{
					m_ctrlOthers.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlOthers.GetItemCount(),
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(new CDlgConfigureDataOtherPoints(pNewOther)));
					m_ctrlOthers.SortItems(CompareItems, 0);
					FindCurrentOtherPoints(pNewOther, true);
				}
			}
			pOther->Release();
		}
		break;
	}
}

void CDlgConfigure::OnDelete() 
{
	CListCtrl2* pCtrl;
	int index;
	CDlgConfigureData* pData;
	if (!GetActionData(pCtrl, index, pData) || !pData)
		return;

	bool bDelete = true;
	switch (m_Action)
	{
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			std::string venue = pVenueData->GetVenue()->GetName();
			int nPoints = m_Book.GetDogs().NumExistingPointsInVenue(venue);
			int nRegNums = m_Book.GetDogs().NumRegNumsInVenue(venue);
			int nTitles = m_Book.GetDogs().NumTitlesInVenue(venue);
			int nTrials = m_Book.GetDogs().NumTrialsInVenue(venue);
			if (0 < nPoints || 0 < nRegNums || 0 < nTitles || 0 < nTrials)
			{
				CString msg;
				msg.FormatMessage(IDS_DELETE_VENUE,
					venue.c_str(),
					nPoints,
					nRegNums,
					nTitles,
					nTrials);
				if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
					bDelete = false;
			}
			if (bDelete)
			{
				// If we were able to delete it...
				if (m_Config.GetVenues().DeleteVenue(venue))
				{
					// Then we commit to fixing the real data.
					if (0 < nPoints || 0 < nRegNums || 0 < nTitles || 0 < nTrials)
						m_DlgFixup.push_back(new CDlgFixupDeleteVenue(venue));
					pCtrl->DeleteItem(index);
				}
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			if (m_Config.GetFaults().DeleteFault(pFaultData->GetFault()->GetName()))
			{
				// Then we commit to fixing the real data.
				pCtrl->DeleteItem(index);
				// No fixup necessary for faults.
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::string otherPoints = pOtherData->GetOtherPoints()->GetName();
			int nOther = m_Book.GetDogs().NumOtherPointsInUse(otherPoints);
			if (0 < nOther)
			{
				CString msg;
				msg.FormatMessage(IDS_DELETE_OTHERPOINTS,
					otherPoints.c_str(),
					nOther);
				if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
					bDelete = false;
			}
			if (bDelete)
			{
				if (m_Config.GetOtherPoints().DeleteOtherPoints(otherPoints))
				{
					// Then we commit to fixing the real data.
					if (0 < nOther)
						m_DlgFixup.push_back(new CDlgFixupDeleteOtherPoints(otherPoints));
					pCtrl->DeleteItem(index);
				}
			}
		}
		break;
	}
	if (bDelete)
		pCtrl->SortItems(CompareItems, 0);
}

void CDlgConfigure::OnEdit() 
{
	CListCtrl2* pCtrl;
	int index;
	CDlgConfigureData* pData;
	if (!GetActionData(pCtrl, index, pData) || !pData)
		return;

	switch (m_Action)
	{

	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			CDlgConfigVenue dlg(m_Book, m_Config, pVenueData->GetVenue(), this);
			if (IDOK == dlg.DoModal())
			{
				dlg.GetFixups(m_DlgFixup);
				m_ctrlVenues.Invalidate();
				m_ctrlVenues.SortItems(CompareItems, 0);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			bool done = false;
			std::string oldName = pFaultData->GetFault()->GetName();
			std::string name(oldName);
			while (!done)
			{
				done = true;
				CDlgName dlg(name.c_str(), IDS_FAULT_TYPE_NAME, this);
				if (IDOK == dlg.DoModal())
				{
					name = dlg.GetName();
					if (oldName != name)
					{
						if (m_Config.GetFaults().FindFault(name))
						{
							done = false;
							AfxMessageBox(IDS_NAME_IN_USE);
							continue;
						}
						pFaultData->GetFault()->SetName(name);
						// No fixup necessary for faults.
						m_ctrlFaults.Invalidate();
						m_ctrlFaults.SortItems(CompareItems, 0);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::string oldName = pOtherData->GetOtherPoints()->GetName();
			CDlgConfigOtherPoints dlg(m_Config, pOtherData->GetOtherPoints(), this);
			if (IDOK == dlg.DoModal())
			{
				if (pOtherData->GetOtherPoints()->GetName() != oldName)
				{
					// Other things may have changed, but we only care about the name for fixup.
					m_DlgFixup.push_back(new CDlgFixupRenameOtherPoints(oldName, pOtherData->GetOtherPoints()->GetName()));
					m_ctrlOthers.Invalidate();
					m_ctrlOthers.SortItems(CompareItems, 0);
				}
			}
		}
		break;
	}
}

void CDlgConfigure::OnCopy() 
{
	CListCtrl2* pCtrl;
	int index;
	CDlgConfigureData* pData;
	if (!GetActionData(pCtrl, index, pData) || !pData)
		return;

	int nColumns = 0;
	CDlgConfigureData* pNewData = NULL;
	CString copyOf;
	copyOf.LoadString(IDS_COPYOF);
	switch (m_Action)
	{
	case eVenues:
		{
			nColumns = 2;
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			std::string name(pVenueData->GetVenue()->GetName());
			while (m_Config.GetVenues().FindVenue(name))
			{
				name = (LPCSTR)copyOf + name;
			}
			ARBConfigVenue* pNewVenue = m_Config.GetVenues().AddVenue(name);
			if (pNewVenue)
			{
				*pNewVenue = *pVenueData->GetVenue();
				pNewVenue->SetName(name); // Put the name back.
				pNewData = new CDlgConfigureDataVenue(pNewVenue);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(pNewData));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentVenue(pNewVenue, true);
			}
		}
		break;

	case eFaults:
		{
			nColumns = 1;
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			std::string name(pFaultData->GetFault()->GetName());
			ARBConfigFault* pNewFault = m_Config.GetFaults().AddFault(name);
			if (pNewFault)
			{
				pNewData = new CDlgConfigureDataFault(pNewFault);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(pNewData));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentFault(pNewFault, true);
			}
		}
		break;

	case eOtherPoints:
		{
			nColumns = 1;
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::string name(pOtherData->GetOtherPoints()->GetName());
			while (m_Config.GetOtherPoints().FindOtherPoints(name))
			{
				name = (LPCSTR)copyOf + name;
			}
			ARBConfigOtherPoints* pOther = new ARBConfigOtherPoints(*(pOtherData->GetOtherPoints()));
			pOther->SetName(name);
			ARBConfigOtherPoints* pNewOther = m_Config.GetOtherPoints().AddOtherPoints(pOther);
			pOther->Release();
			if (pNewOther)
			{
				pNewData = new CDlgConfigureDataOtherPoints(pNewOther);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(pNewData));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentOtherPoints(pNewOther, true);
			}
		}
		break;
	}
	
	for (int i = 0; i < nColumns; ++i)
		pCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

// Updating a configuration is mainly an additive procedure.
void CDlgConfigure::OnUpdate() 
{
	CDlgConfigUpdate dlg(this);
	if (IDOK == dlg.DoModal())
	{
		ARBConfig& update = dlg.GetConfig();
		CString msg;
		for (ARBConfigActionList::const_iterator iterAction = update.GetActions().begin(); iterAction != update.GetActions().end(); ++iterAction)
		{
			const ARBConfigAction* action = *iterAction;
			if (action->GetVerb() == ACTION_VERB_RENAME_TITLE)
			{
				// Find the venue.
				ARBConfigVenue* venue = m_Config.GetVenues().FindVenue(action->GetVenue());
				if (venue)
				{
					// Find the title we're renaming.
					ARBConfigTitle* oldTitle = venue->GetDivisions().FindTitle(action->GetOldName());
					if (oldTitle)
					{
						CString tmp;
						tmp.Format("Action: Renaming title [%s] to [%s]",
							action->GetOldName().c_str(),
							action->GetNewName().c_str());
						msg += tmp;
						// If any titles are in use, create a fixup action.
						int nTitles = m_Book.GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
						if (0 < nTitles)
						{
							tmp.Format(", updating %d titles\n", nTitles);
							m_DlgFixup.push_back(new CDlgFixupRenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName()));
						}
						else
							tmp = "\n";
						msg += tmp;
						// If the new title exists, just delete the old. Otherwise, rename the old to new.
						const ARBConfigTitle* newTitle = venue->GetDivisions().FindTitle(action->GetNewName());
						if (newTitle)
							venue->GetDivisions().DeleteTitle(action->GetOldName());
						else
							oldTitle->SetName(action->GetNewName());
					}
				}
			}
			else if (action->GetVerb() == ACTION_VERB_DELETE_TITLE)
			{
				// Find the venue.
				ARBConfigVenue* venue = m_Config.GetVenues().FindVenue(action->GetVenue());
				if (venue)
				{
					// Find the title we're renaming.
					ARBConfigTitle* oldTitle = venue->GetDivisions().FindTitle(action->GetOldName());
					if (oldTitle)
					{
						CString tmp;
						int nTitles = m_Book.GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
						// If any titles are in use, create a fixup action.
						if (0 < nTitles)
						{
							if (0 < action->GetNewName().length())
							{
								tmp.Format("Action: Renaming existing %d title(s) [%s] to [%s]\n",
									nTitles,
									action->GetOldName().c_str(),
									action->GetNewName().c_str());
								msg += tmp;
								m_DlgFixup.push_back(new CDlgFixupRenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName()));
							}
							else
							{
								tmp.Format("Action: Deleting existing %d [%s] title(s)\n",
									nTitles,
									action->GetOldName().c_str());
								msg += tmp;
								m_DlgFixup.push_back(new CDlgFixupDeleteTitle(action->GetVenue(), action->GetOldName()));
							}
						}
						tmp.Format("Action: Deleting title [%s]\n",
							action->GetOldName().c_str());
						msg += tmp;
						venue->GetDivisions().DeleteTitle(action->GetOldName());
					}
				}
			}
		}
		update.GetActions().clear();
		std::string info;
		m_Config.Update(0, update, info);
		msg += info.c_str();
		if (0 < msg.GetLength())
		{
			CDlgMessage dlg(msg, 0, this);
			dlg.DoModal();
			LoadData();
		}
		else
			AfxMessageBox(IDS_CONFIG_NO_UPDATE, MB_ICONINFORMATION);
	}
} 

void CDlgConfigure::OnOK()
{
	CWaitCursor wait;
	if (m_Book.GetConfig() != m_Config)
	{
		// Fixup must be done before assigning the config.
		// Some of the fixup stuff needs to traverse existing config info to
		// determine what needs deleting (for instance, when deleting a
		// division, we have to iterate over the config info for the division
		// to get all the titles that also need deleting.
		// Note: there may be multiple action items on a single entry - that's
		// ok. For instance, if we rename an item twice, there will be two
		// entries. I suppose we could get clever and collapse them - but why?!
		// The end result is the same.
		for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
		{
			(*iter)->Commit(m_Book);
			delete (*iter);
		}
		m_DlgFixup.clear();
		// Fixup is done, now assign the new config info.
		m_Book.GetConfig() = m_Config;
		m_pDoc->SetModifiedFlag();
		m_pDoc->UpdateAllViews(NULL, UPDATE_CONFIG);
	}
	CDialog::OnOK();
}
