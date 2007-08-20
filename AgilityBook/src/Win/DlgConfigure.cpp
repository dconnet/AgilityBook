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
 * @li 2007-08-19 DRC Simplified UI layout into a single tree.
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

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure dialog

CDlgConfigure::CDlgConfigure(
		CAgilityBookDoc* pDoc,
		ARBAgilityRecordBook& book)
	: CDlgBaseDialog(CDlgConfigure::IDD)
	, m_ctrlItems()
	, m_pDoc(pDoc)
	, m_Book(book)
	, m_Config(m_Book.GetConfig())
	, m_hItemVenues(NULL)
	, m_hItemFaults(NULL)
	, m_hItemOtherPts(NULL)
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
	DDX_Control(pDX, IDC_CONFIG_ITEMS, m_ctrlItems);
	DDX_Control(pDX, IDC_CONFIG_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_CONFIG_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CONFIG_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CONFIG_COPY, m_ctrlCopy);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigure, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigure)
	ON_NOTIFY(TVN_DELETEITEM, IDC_CONFIG_ITEMS, OnDeleteitem)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_CONFIG_ITEMS, OnGetdispinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_ITEMS, OnDblclk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIG_ITEMS, OnSelchanged)
	ON_BN_CLICKED(IDC_CONFIG_NEW, OnNew)
	ON_BN_CLICKED(IDC_CONFIG_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CONFIG_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CONFIG_COPY, OnCopy)
	ON_BN_CLICKED(IDC_CONFIG_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CDlgConfigure::eAction CDlgConfigure::GetAction() const
{
	HTREEITEM hItem = m_ctrlItems.GetSelectedItem();
	if (hItem)
	{
		HTREEITEM hParent = m_ctrlItems.GetParentItem(hItem);
		if (!hParent)
			hParent = hItem;
		return static_cast<eAction>(m_ctrlItems.GetItemData(hParent));
	}
	return eNone;
}

CDlgConfigureData* CDlgConfigure::GetData(HTREEITEM hItem) const
{
	if (hItem)
	{
		HTREEITEM hParent = m_ctrlItems.GetParentItem(hItem);
		if (hParent)
			return reinterpret_cast<CDlgConfigureData*>(m_ctrlItems.GetItemData(hItem));
	}
	return NULL;
}

void CDlgConfigure::UpdateButtons()
{
	BOOL bNew = FALSE;
	BOOL bDelete = FALSE;
	BOOL bEdit = FALSE;
	BOOL bCopy = FALSE;
	if (eNone != GetAction())
		bNew = TRUE;
	if (GetData(m_ctrlItems.GetSelectedItem()))	
	{
		bDelete = bEdit = bCopy = TRUE;
	}
	m_ctrlNew.EnableWindow(bNew);
	m_ctrlDelete.EnableWindow(bDelete);
	m_ctrlEdit.EnableWindow(bEdit);
	m_ctrlCopy.EnableWindow(bCopy);
}

void CDlgConfigure::LoadData(eAction dataToLoad)
{
	HTREEITEM hParent = NULL;
	switch (dataToLoad)
	{
	case eVenues:
		hParent = m_hItemVenues;
		break;
	case eFaults:
		hParent = m_hItemFaults;
		break;
	case eOtherPoints:
		hParent = m_hItemOtherPts;
		break;
	}
	if (!hParent)
		return;
	while (m_ctrlItems.ItemHasChildren(hParent))
		m_ctrlItems.DeleteItem(m_ctrlItems.GetNextItem(hParent, TVGN_CHILD));

	switch (dataToLoad)
	{
	case eVenues:
		for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin(); iterVenue != m_Config.GetVenues().end(); ++iterVenue)
		{
			m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
				0, 0, 0, 0,
				reinterpret_cast<LPARAM>(
					static_cast<CDlgConfigureData*>(
						new CDlgConfigureDataVenue(*iterVenue))),
				hParent, TVI_LAST);
		}
		break;
	case eFaults:
		for (ARBConfigFaultList::iterator iterFault = m_Config.GetFaults().begin(); iterFault != m_Config.GetFaults().end(); ++iterFault)
		{
			m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
				0, 0, 0, 0,
				reinterpret_cast<LPARAM>(
					static_cast<CDlgConfigureData*>(
						new CDlgConfigureDataFault(*iterFault))),
				hParent, TVI_LAST);
		}
		break;
	case eOtherPoints:
		for (ARBConfigOtherPointsList::iterator iterOther = m_Config.GetOtherPoints().begin(); iterOther != m_Config.GetOtherPoints().end(); ++iterOther)
		{
			m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
				0, 0, 0, 0,
				reinterpret_cast<LPARAM>(
					static_cast<CDlgConfigureData*>(
						new CDlgConfigureDataOtherPoints(*iterOther))),
				hParent, TVI_LAST);
		}
		break;
	}
	m_ctrlItems.SortChildren(hParent);
}

HTREEITEM CDlgConfigure::FindCurrentVenue(
		ARBConfigVenuePtr pVenue,
		bool bSet)
{
	HTREEITEM hCurrent = NULL;
	for (HTREEITEM item = m_ctrlItems.GetNextItem(m_hItemVenues, TVGN_CHILD);
		item != NULL;
		item = m_ctrlItems.GetNextSiblingItem(item))
	{
		CDlgConfigureDataVenue* pData = dynamic_cast<CDlgConfigureDataVenue*>(GetData(item));
		if (pData && pData->GetVenue() == pVenue)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems.SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems.EnsureVisible(hCurrent);
	}
	return hCurrent;
}

HTREEITEM CDlgConfigure::FindCurrentFault(
		ARBConfigFaultPtr pFault,
		bool bSet)
{
	HTREEITEM hCurrent = NULL;
	for (HTREEITEM item = m_ctrlItems.GetNextItem(m_hItemFaults, TVGN_CHILD);
		item != NULL;
		item = m_ctrlItems.GetNextSiblingItem(item))
	{
		CDlgConfigureDataFault* pData = dynamic_cast<CDlgConfigureDataFault*>(GetData(item));
		if (pData && pData->GetFault() == pFault)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems.SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems.EnsureVisible(hCurrent);
	}
	return hCurrent;
}

HTREEITEM CDlgConfigure::FindCurrentOtherPoints(
		ARBConfigOtherPointsPtr pOther,
		bool bSet)
{
	HTREEITEM  hCurrent = NULL;
	for (HTREEITEM item = m_ctrlItems.GetNextItem(m_hItemOtherPts, TVGN_CHILD);
		item != NULL;
		item = m_ctrlItems.GetNextSiblingItem(item))
	{
		CDlgConfigureDataOtherPoints* pData = dynamic_cast<CDlgConfigureDataOtherPoints*>(GetData(item));
		if (pData && pData->GetOtherPoints() == pOther)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems.SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems.EnsureVisible(hCurrent);
	}
	return hCurrent;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure message handlers

BOOL CDlgConfigure::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	CString col;
	col.LoadString(IDS_COL_VENUES);
	m_hItemVenues = m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, (LPCTSTR)col, 0,0,0,0, eVenues, TVI_ROOT, TVI_LAST);
	LoadData(eVenues);
	m_ctrlItems.Expand(m_hItemVenues, TVE_EXPAND);

	col.LoadString(IDS_COL_FAULTS);
	m_hItemFaults = m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, (LPCTSTR)col, 0,0,0,0, eFaults, TVI_ROOT, TVI_LAST);
	LoadData(eFaults);

	col.LoadString(IDS_OTHERPOINTS);
	m_hItemOtherPts = m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, (LPCTSTR)col, 0,0,0,0, eOtherPoints, TVI_ROOT, TVI_LAST);
	LoadData(eOtherPoints);

	m_ctrlItems.SelectItem(m_hItemVenues);
	m_ctrlItems.EnsureVisible(m_hItemVenues);
	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigure::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	if (NULL != m_ctrlItems.GetParentItem(pNMTreeView->itemOld.hItem))
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(pNMTreeView->itemOld.lParam);
		delete pData;
		pNMTreeView->itemOld.lParam = 0;
	}
	*pResult = 0;
}

void CDlgConfigure::OnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	TV_DISPINFO* pDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & TVIF_TEXT)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(pDispInfo->item.lParam);
		if (pData)
		{
			CString str = pData->OnNeedText();
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

void CDlgConfigure::OnSelchanged(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigure::OnNew() 
{
	switch (GetAction())
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
					m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
						0, 0, 0, 0,
						reinterpret_cast<LPARAM>(
							static_cast<CDlgConfigureData*>(
								new CDlgConfigureDataVenue(pVenue))),
						m_hItemVenues, TVI_LAST);
					m_ctrlItems.SortChildren(m_hItemVenues);
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
						m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
							0, 0, 0, 0,
							reinterpret_cast<LPARAM>(
								static_cast<CDlgConfigureData*>(
									new CDlgConfigureDataFault(pNewFault))),
							m_hItemFaults, TVI_LAST);
						m_ctrlItems.SortChildren(m_hItemFaults);
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
					m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
						0, 0, 0, 0,
						reinterpret_cast<LPARAM>(
							static_cast<CDlgConfigureData*>(
								new CDlgConfigureDataOtherPoints(pOther))),
						m_hItemOtherPts, TVI_LAST);
					m_ctrlItems.SortChildren(m_hItemOtherPts);
					FindCurrentOtherPoints(pOther, true);
				}
			}
		}
		break;
	}
}

void CDlgConfigure::OnDelete() 
{
	HTREEITEM current = m_ctrlItems.GetSelectedItem();
	CDlgConfigureData* pData = GetData(current);
	if (!pData)
		return;

	switch (GetAction())
	{
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			ARBString venue = pVenueData->GetVenue()->GetName();
			// If we were able to delete it...
			if (m_Config.GetVenues().DeleteVenue(venue))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteVenue::New(venue));
				m_ctrlItems.DeleteItem(current);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			if (m_Config.GetFaults().DeleteFault(pFaultData->GetFault()->GetName()))
			{
				m_ctrlItems.DeleteItem(current);
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
				m_ctrlItems.DeleteItem(current);
			}
		}
		break;
	}
}

void CDlgConfigure::OnEdit() 
{
	HTREEITEM current = m_ctrlItems.GetSelectedItem();
	CDlgConfigureData* pData = GetData(current);
	if (!pData)
		return;

	switch (GetAction())
	{
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			CDlgConfigVenue dlg(m_Book, m_Config, pVenueData->GetVenue(), this);
			if (IDOK == dlg.DoModal())
			{
				dlg.GetFixups(m_Config.GetActions());
				m_ctrlItems.Invalidate();
				m_ctrlItems.SortChildren(m_hItemVenues);
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
						m_ctrlItems.Invalidate();
						m_ctrlItems.SortChildren(m_hItemFaults);
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
					m_ctrlItems.Invalidate();
					m_ctrlItems.SortChildren(m_hItemOtherPts);
				}
			}
		}
		break;
	}
}

void CDlgConfigure::OnCopy() 
{
	HTREEITEM current = m_ctrlItems.GetSelectedItem();
	CDlgConfigureData* pData = GetData(current);
	if (!pData)
		return;

	CString copyOf;
	copyOf.LoadString(IDS_COPYOF);

	switch (GetAction())
	{
	case eVenues:
		{
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
				CDlgConfigureData* pNewData = new CDlgConfigureDataVenue(pNewVenue);
				m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CDlgConfigureData*>(pNewData)),
					m_hItemVenues, TVI_LAST);
				m_ctrlItems.SortChildren(m_hItemVenues);
				FindCurrentVenue(pNewVenue, true);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			ARBString name(pFaultData->GetFault()->GetName());
			ARBConfigFaultPtr pNewFault;
			if (m_Config.GetFaults().AddFault(name, &pNewFault))
			{
				CDlgConfigureData* pNewData = new CDlgConfigureDataFault(pNewFault);
				m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CDlgConfigureData*>(pNewData)),
					m_hItemFaults, TVI_LAST);
				m_ctrlItems.SortChildren(m_hItemFaults);
				FindCurrentFault(pNewFault, true);
			}
		}
		break;

	case eOtherPoints:
		{
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
				CDlgConfigureData* pNewData = new CDlgConfigureDataOtherPoints(pOther);
				m_ctrlItems.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CDlgConfigureData*>(pNewData)),
					m_hItemOtherPts, TVI_LAST);
				m_ctrlItems.SortChildren(m_hItemOtherPts);
				FindCurrentOtherPoints(pOther, true);
			}
		}
		break;
	}
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
			CDlgMessage dlgMsg(info.str().c_str(), 0, this);
			dlgMsg.DoModal();
			LoadData(eVenues);
			LoadData(eFaults);
			LoadData(eOtherPoints);
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
