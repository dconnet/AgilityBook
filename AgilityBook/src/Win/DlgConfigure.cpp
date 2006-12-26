/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-08 DRC Added 'RenameEvent' action.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-03-26 DRC Added code to migrate runs to the new table-in-run form.
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
#include "DlgListViewer.h"
#include "DlgMessage.h"
#include "DlgMessageBox.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDetails : public IMessageBoxCallback
{
public:
	CDetails(
			CAgilityBookDoc* inDoc,
			std::list<ScoringRunInfo> const& inScoringRuns)
		: m_pDoc(inDoc)
		, m_ScoringRuns(inScoringRuns)
	{
	}
	virtual void OnDetails(CWnd* pParent);
protected:
	CAgilityBookDoc* m_pDoc;
	std::list<ScoringRunInfo> const& m_ScoringRuns;
};

void CDetails::OnDetails(CWnd* pParent)
{
	CString str;
	str.LoadString(IDS_AFFECTED_RUNS);
	CDlgListViewer dlg(m_pDoc, str, m_ScoringRuns, pParent);
	dlg.DoModal();
}

int CALLBACK CompareItems(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParamSort)
{
	CDlgConfigureData* item1 = reinterpret_cast<CDlgConfigureData*>(lParam1);
	CDlgConfigureData* item2 = reinterpret_cast<CDlgConfigureData*>(lParam2);
	int iColumn = static_cast<int>(lParamSort);
	return item1->OnNeedText(iColumn) > item2->OnNeedText(iColumn);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure dialog

CDlgConfigure::CDlgConfigure(
		CAgilityBookDoc* pDoc,
		ARBAgilityRecordBook& book)
	: CDlgBaseDialog(CDlgConfigure::IDD)
	, m_ctrlVenues(true)
	, m_ctrlFaults(true)
	, m_ctrlOthers(true)
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
}

void CDlgConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigure)
	DDX_Control(pDX, IDC_CONFIG_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_CONFIG_FAULTS, m_ctrlFaults);
	DDX_Control(pDX, IDC_CONFIG_OTHERPOINTS, m_ctrlOthers);
	DDX_Control(pDX, IDC_CONFIG_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_CONFIG_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CONFIG_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CONFIG_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_CONFIG_COMMENTS, m_ctrlComments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigure, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigure)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUES, OnGetdispinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUES, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUES, OnKeydown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUES, OnItemchanged)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUES, OnSetfocusVenues)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_FAULTS, OnSetfocusFaults)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_OTHERPOINTS, OnSetfocusOtherpoints)
	ON_BN_CLICKED(IDC_CONFIG_NEW, OnNew)
	ON_BN_CLICKED(IDC_CONFIG_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CONFIG_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CONFIG_COPY, OnCopy)
	ON_BN_CLICKED(IDC_CONFIG_UPDATE, OnUpdate)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_FAULTS, OnGetdispinfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_OTHERPOINTS, OnGetdispinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_FAULTS, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_FAULTS, OnKeydown)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_OTHERPOINTS, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_OTHERPOINTS, OnKeydown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_FAULTS, OnItemchanged)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_OTHERPOINTS, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigure::SetAction(eAction inAction)
{
	if (m_Action != inAction)
	{
		m_Action = inAction;
		CString str("");
		switch (m_Action)
		{
		default:
			break;
		case eVenues:
			str.LoadString(IDS_BUTTONS_VENUES);
			break;
		case eFaults:
			str.LoadString(IDS_BUTTONS_FAULTS);
			break;
		case eOtherPoints:
			str.LoadString(IDS_BUTTONS_OTHERPOINTS);
			break;
		}
		m_ctrlComments.SetWindowText(str);
	}
	UpdateButtons();
}

bool CDlgConfigure::GetActionData(
		CListCtrl2*& pCtrl,
		int& index,
		CDlgConfigureData*& pData)
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
			pData = dynamic_cast<CDlgConfigureData*>(pCtrl->GetData(index));
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
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataVenue(*iterVenue))));
	}
	m_ctrlVenues.SortItems(CompareItems, 0);
	m_ctrlVenues.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlVenues.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlVenues.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	for (ARBConfigFaultList::iterator iterFault = m_Config.GetFaults().begin(); iterFault != m_Config.GetFaults().end(); ++iterFault)
	{
		m_ctrlFaults.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataFault(*iterFault))));
	}
	m_ctrlFaults.SortItems(CompareItems, 0);
	m_ctrlFaults.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	for (ARBConfigOtherPointsList::iterator iterOther = m_Config.GetOtherPoints().begin(); iterOther != m_Config.GetOtherPoints().end(); ++iterOther)
	{
		m_ctrlOthers.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataOtherPoints(*iterOther))));
	}
	m_ctrlOthers.SortItems(CompareItems, 0);
	m_ctrlOthers.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

int CDlgConfigure::FindCurrentVenue(
		ARBConfigVenuePtr pVenue,
		bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlVenues.GetItemCount(); ++index)
	{
		CDlgConfigureDataVenue* pData = dynamic_cast<CDlgConfigureDataVenue*>(m_ctrlVenues.GetData(index));
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

int CDlgConfigure::FindCurrentFault(
		ARBConfigFaultPtr pFault,
		bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlFaults.GetItemCount(); ++index)
	{
		CDlgConfigureDataFault* pData = dynamic_cast<CDlgConfigureDataFault*>(m_ctrlFaults.GetData(index));
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

int CDlgConfigure::FindCurrentOtherPoints(
		ARBConfigOtherPointsPtr pOther,
		bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlOthers.GetItemCount(); ++index)
	{
		CDlgConfigureDataOtherPoints* pData = dynamic_cast<CDlgConfigureDataOtherPoints*>(m_ctrlOthers.GetData(index));
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
	CDlgBaseDialog::OnInitDialog();
	m_ctrlVenues.SetExtendedStyle(m_ctrlVenues.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlFaults.SetExtendedStyle(m_ctrlFaults.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlOthers.SetExtendedStyle(m_ctrlOthers.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	CString col;
	col.LoadString(IDS_COL_VENUES);
	m_ctrlVenues.InsertColumn(0, col);
	col.LoadString(IDS_COL_URL);
	m_ctrlVenues.InsertColumn(1, col);
	col.LoadString(IDS_COL_DESCRIPTION);
	m_ctrlVenues.InsertColumn(2, col);

	col.LoadString(IDS_COL_FAULTS);
	m_ctrlFaults.InsertColumn(0, col);

	col.LoadString(IDS_OTHERPOINTS);
	m_ctrlOthers.InsertColumn(0, col);

	LoadData();
	SetAction(eVenues);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigure::OnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
		CDlgConfigureData* pData = dynamic_cast<CDlgConfigureData*>(pRawData);
		if (pData)
		{
			CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}

void CDlgConfigure::OnDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgConfigure::OnKeydown(
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

void CDlgConfigure::OnItemchanged(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusVenues(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eVenues);
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusFaults(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eFaults);
	*pResult = 0;
}

void CDlgConfigure::OnSetfocusOtherpoints(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
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
			ARBConfigVenuePtr pVenue(ARBConfigVenue::New());
			CDlgConfigVenue dlg(m_Book, m_Config, pVenue, this);
			if (IDOK == dlg.DoModal())
			{
				if (m_Config.GetVenues().AddVenue(pVenue))
				{
					dlg.GetFixups(m_Config.GetActions());
					m_ctrlVenues.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlVenues.GetItemCount(),
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(
							static_cast<CListData*>(
								new CDlgConfigureDataVenue(pVenue))));
					m_ctrlVenues.SortItems(CompareItems, 0);
					FindCurrentVenue(pVenue, true);
				}
			}
		}
		break;

	case eFaults:
		{
			CDlgName dlg(_T(""), IDS_FAULT_TYPE_NAME, this);
			if (IDOK == dlg.DoModal())
			{
				ARBString name = dlg.GetName();
				// We could check for uniqueness, but if the user wants 2
				// strings the same, why argue! Afterall, these strings
				// are only "helper" items to fill in other data.
				if (0 < name.length())
				{
					ARBConfigFaultPtr pNewFault;
					if (m_Config.GetFaults().AddFault(name, &pNewFault))
					{
						m_ctrlFaults.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlFaults.GetItemCount(),
							LPSTR_TEXTCALLBACK, 0, 0, 0,
							reinterpret_cast<LPARAM>(
								static_cast<CListData*>(
									new CDlgConfigureDataFault(pNewFault))));
						m_ctrlFaults.SortItems(CompareItems, 0);
						FindCurrentFault(pNewFault, true);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			ARBConfigOtherPointsPtr pOther(ARBConfigOtherPoints::New());
			// The dialog will ensure uniqueness.
			CDlgConfigOtherPoints dlg(m_Config, pOther, this);
			if (IDOK == dlg.DoModal())
			{
				if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
				{
					m_ctrlOthers.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlOthers.GetItemCount(),
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(
							static_cast<CListData*>(
								new CDlgConfigureDataOtherPoints(pOther))));
					m_ctrlOthers.SortItems(CompareItems, 0);
					FindCurrentOtherPoints(pOther, true);
				}
			}
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

	switch (m_Action)
	{
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			ARBString venue = pVenueData->GetVenue()->GetName();
			// If we were able to delete it...
			if (m_Config.GetVenues().DeleteVenue(venue))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteVenue::New(venue));
				pCtrl->DeleteItem(index);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			if (m_Config.GetFaults().DeleteFault(pFaultData->GetFault()->GetName()))
			{
				pCtrl->DeleteItem(index);
				// No fixup necessary for faults.
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			ARBString otherPoints = pOtherData->GetOtherPoints()->GetName();
			if (m_Config.GetOtherPoints().DeleteOtherPoints(otherPoints))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteOtherPoints::New(otherPoints));
				pCtrl->DeleteItem(index);
			}
		}
		break;
	}
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
				dlg.GetFixups(m_Config.GetActions());
				m_ctrlVenues.Invalidate();
				m_ctrlVenues.SortItems(CompareItems, 0);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			bool done = false;
			ARBString oldName = pFaultData->GetFault()->GetName();
			ARBString name(oldName);
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
			ARBString oldName = pOtherData->GetOtherPoints()->GetName();
			CDlgConfigOtherPoints dlg(m_Config, pOtherData->GetOtherPoints(), this);
			if (IDOK == dlg.DoModal())
			{
				if (pOtherData->GetOtherPoints()->GetName() != oldName)
				{
					// Other things may have changed, but we only care about the name for fixup.
					m_Config.GetActions().push_back(ARBConfigActionRenameOtherPoints::New(oldName, pOtherData->GetOtherPoints()->GetName()));
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
			ARBString name(pVenueData->GetVenue()->GetName());
			while (m_Config.GetVenues().FindVenue(name))
			{
				name = (LPCTSTR)copyOf + name;
			}
			ARBConfigVenuePtr pNewVenue;
			if (m_Config.GetVenues().AddVenue(name, &pNewVenue))
			{
				*pNewVenue = *pVenueData->GetVenue();
				pNewVenue->SetName(name); // Put the name back.
				pNewData = new CDlgConfigureDataVenue(pNewVenue);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CListData*>(pNewData)));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentVenue(pNewVenue, true);
			}
		}
		break;

	case eFaults:
		{
			nColumns = 1;
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			ARBString name(pFaultData->GetFault()->GetName());
			ARBConfigFaultPtr pNewFault;
			if (m_Config.GetFaults().AddFault(name, &pNewFault))
			{
				pNewData = new CDlgConfigureDataFault(pNewFault);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CListData*>(pNewData)));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentFault(pNewFault, true);
			}
		}
		break;

	case eOtherPoints:
		{
			nColumns = 1;
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			ARBString name(pOtherData->GetOtherPoints()->GetName());
			while (m_Config.GetOtherPoints().FindOtherPoints(name))
			{
				name = (LPCTSTR)copyOf + name;
			}
			ARBConfigOtherPointsPtr pOther = pOtherData->GetOtherPoints()->Clone();
			pOther->SetName(name);
			if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
			{
				pNewData = new CDlgConfigureDataOtherPoints(pOther);
				pCtrl->InsertItem(LVIF_TEXT | LVIF_PARAM, pCtrl->GetItemCount(),
					LPSTR_TEXTCALLBACK, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CListData*>(pData)));
				pCtrl->SortItems(CompareItems, 0);
				FindCurrentOtherPoints(pOther, true);
			}
		}
		break;
	}
	
	for (int i = 0; i < nColumns; ++i)
		pCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

// During import here, we do nothing.
class CDlgConfigCallback : public IConfigActionCallback
{
public:
	CDlgConfigCallback() {}
	virtual void PreDelete(ARBString const& inMsg) {}
	void PostDelete(ARBString const& inMsg) const {}
};

// Updating a configuration is mainly an additive procedure.
void CDlgConfigure::OnUpdate() 
{
	CDlgConfigUpdate dlg(this);
	if (IDOK == dlg.DoModal())
	{
		ARBConfig& update = dlg.GetConfig();
		// Update our current config (not runs, later)
		ARBostringstream info;
		CDlgConfigCallback callback;
		if (0 < update.GetActions().Apply(m_Config, NULL, info, callback))
		{
			// Now move the actions into our config so we can fully apply them.
			m_Config.GetActions().insert(m_Config.GetActions().end(), update.GetActions().begin(), update.GetActions().end());
			update.GetActions().clear();
		}

		// Update the config.
		if (m_Config.Update(0, update, info))
		{
			CDlgMessage dlg(info.str().c_str(), 0, this);
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
		m_pDoc->ImportConfiguration(m_Config);
	}
	CDlgBaseDialog::OnOK();
}
