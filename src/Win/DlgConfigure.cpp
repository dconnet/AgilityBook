/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @li 2003-02-04 DRC Changed dbl-click to edit an item.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigure.h"

#include "AgilityBookDoc.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigDivision.h"
#include "ARBConfigVenue.h"
#include "DlgConfigureData.h"
#include "DlgConfigUpdate.h"
#include "DlgFixup.h"
#include "DlgMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure dialog

CDlgConfigure::CDlgConfigure(CAgilityBookDoc* pDoc, ARBAgilityRecordBook& book)
	: CDialog(CDlgConfigure::IDD)
	, m_pDoc(pDoc)
	, m_Book(book)
	, m_Config(m_Book.GetConfig())
	, m_bReset(false)
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
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigure)
	DDX_Control(pDX, IDC_VENUES, m_ctrlTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigure, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigure)
	ON_WM_INITMENUPOPUP()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_DELETEITEM, IDC_VENUES, OnTvnDeleteitemVenues)
	ON_NOTIFY(NM_RCLICK, IDC_VENUES, OnNMRclickVenues)
	ON_NOTIFY(NM_DBLCLK, IDC_VENUES, OnDblclkVenues)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_ADD, OnUpdateConfigAdd)
	ON_COMMAND(ID_CONFIG_ADD, OnConfigAdd)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_DUPLICATE, OnUpdateConfigDup)
	ON_COMMAND(ID_CONFIG_DUPLICATE, OnConfigDup)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_EDIT, OnUpdateConfigEdit)
	ON_COMMAND(ID_CONFIG_EDIT, OnConfigEdit)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_DELETE, OnUpdateConfigDelete)
	ON_COMMAND(ID_CONFIG_DELETE, OnConfigDelete)
	ON_UPDATE_COMMAND_UI(ID_EXPAND, OnUpdateExpand)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAll)
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAll)
	ON_UPDATE_COMMAND_UI(ID_COLLAPSE, OnUpdateCollapse)
	ON_COMMAND(ID_COLLAPSE, OnCollapse)
	ON_UPDATE_COMMAND_UI(ID_COLLAPSE_ALL, OnUpdateCollapseAll)
	ON_COMMAND(ID_COLLAPSE_ALL, OnCollapseAll)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Helper functions to populate the tree so the OnInitDialog code can be read!

HTREEITEM CDlgConfigure::InsertVenue(HTREEITEM hParent, ARBConfigVenue* venue)
{
	CDlgConfigureData* pData = new CDlgConfigureDataVenue(this, venue);
	HTREEITEM hVenue = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		venue->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hVenue);

	CString str;
	str.LoadString(IDS_DIVISIONS);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eDivisions);
	HTREEITEM hDivisions = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hVenue,
		TVI_LAST);
	pData->SetHTreeItem(hDivisions);
	InsertDivisions(hDivisions, venue);

	str.LoadString(IDS_EVENTS);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eEvents);
	HTREEITEM hEvents = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hVenue,
		TVI_LAST);
	pData->SetHTreeItem(hEvents);
	InsertEvents(hEvents, venue);

	return hVenue;
}

HTREEITEM CDlgConfigure::InsertFaultType(HTREEITEM hParent, ARBConfigFault* fault)
{
	CDlgConfigureData* pData = new CDlgConfigureDataFault(this, fault);
	HTREEITEM hFault = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		fault->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hFault);

	return hFault;
}

HTREEITEM CDlgConfigure::InsertOtherPoint(HTREEITEM hParent, ARBConfigOtherPoints* other)
{
	CDlgConfigureData* pData = new CDlgConfigureDataOtherPoints(this, other);
	HTREEITEM hOther = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		other->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hOther);

	return hOther;
}

HTREEITEM CDlgConfigure::InsertDivision(HTREEITEM hParent, ARBConfigDivision* div)
{
	CDlgConfigureData* pData = new CDlgConfigureDataDivision(this, div);
	HTREEITEM hDiv = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		div->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hDiv);

	CString str;
	str.LoadString(IDS_TITLES);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eTitles);
	HTREEITEM hTitles = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hDiv,
		TVI_LAST);
	pData->SetHTreeItem(hTitles);
	InsertTitles(hTitles, div);

	str.LoadString(IDS_LEVELS);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eLevels);
	HTREEITEM hLevels = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hDiv,
		TVI_LAST);
	pData->SetHTreeItem(hLevels);
	InsertLevels(hLevels, div);

	return hDiv;
}

HTREEITEM CDlgConfigure::InsertLevel(HTREEITEM hParent, ARBConfigLevel* level)
{
	CDlgConfigureData* pData = new CDlgConfigureDataLevel(this, level);
	HTREEITEM hLevel = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		level->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hLevel);
	for (ARBConfigSubLevelList::iterator iter = level->GetSubLevels().begin(); iter != level->GetSubLevels().end(); ++iter)
	{
		InsertSubLevel(hLevel, (*iter));
	}

	return hLevel;
}

HTREEITEM CDlgConfigure::InsertSubLevel(HTREEITEM hParent, ARBConfigSubLevel* subLevel)
{
	CDlgConfigureData* pData = new CDlgConfigureDataSubLevel(this, subLevel);
	HTREEITEM hSubLevel = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		subLevel->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hSubLevel);
	return hSubLevel;
}

HTREEITEM CDlgConfigure::InsertTitle(HTREEITEM hParent, ARBConfigTitle* title)
{
	CDlgConfigureData* pData = new CDlgConfigureDataTitle(this, title);
	HTREEITEM hTitle = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		title->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hTitle);

	return hTitle;
}

HTREEITEM CDlgConfigure::InsertEvent(HTREEITEM hParent, ARBConfigEvent* event)
{
	CDlgConfigureData* pData = new CDlgConfigureDataEvent(this, event);
	HTREEITEM hEvent = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		event->GetName().c_str(),
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hEvent);

	CString str;
	str.LoadString(IDS_SCORING);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eScoring);
	HTREEITEM hScoring = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hEvent,
		TVI_LAST);
	pData->SetHTreeItem(hScoring);
	InsertScorings(hScoring, event);

	return hEvent;
}

HTREEITEM CDlgConfigure::InsertScoring(HTREEITEM hParent, ARBConfigScoring* pScoring)
{
	CDlgConfigureData* pData = new CDlgConfigureDataScoring(this, pScoring);
	CString str, div, level;
	if (pScoring->GetDivision() == WILDCARD_DIVISION)
		div.LoadString(IDS_ALL);
	else
		div = pScoring->GetDivision().c_str();
	if (pScoring->GetLevel() == WILDCARD_LEVEL)
		level.LoadString(IDS_ALL);
	else
		level = pScoring->GetLevel().c_str();
	str.FormatMessage(IDS_SCORING_NAME,
		div,
		level,
		pScoring->GetScoringStyleStr().c_str());
	HTREEITEM hScore = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		hParent,
		TVI_LAST);
	pData->SetHTreeItem(hScore);

	CString tmp;
	str.Empty();
	if (pScoring->HasSuperQ())
	{
		if (!str.IsEmpty())
			str += ", ";
		tmp.LoadString(IDS_SUPER_Q);
		str += tmp;
	}
	if (pScoring->HasDoubleQ())
	{
		if (!str.IsEmpty())
			str += ", ";
		tmp.LoadString(IDS_DOUBLE_Q);
		str += tmp;
	}
	if (pScoring->HasMachPts())
	{
		if (!str.IsEmpty())
			str += ", ";
		tmp.LoadString(IDS_MACHPTS);
		str += tmp;
	}
	if (!str.IsEmpty())
	{
		pData = new CDlgConfigureData(this, CDlgConfigureData::eScoringInfo);
		HTREEITEM hInfo = m_ctrlTree.InsertItem(
			TVIF_TEXT | TVIF_PARAM,
			str,
			0, //nImage
			0, //nSelectedImage
			0, //nState
			0, //nStateMask
			reinterpret_cast<LPARAM>(pData),
			hScore,
			TVI_LAST);
		pData->SetHTreeItem(hInfo);
	}

	for (ARBConfigTitlePointsList::const_iterator iter = pScoring->GetTitlePoints().begin();
		iter != pScoring->GetTitlePoints().end();
		++iter)
	{
		str.FormatMessage(IDS_CONFIG_TITLEPTS, (*iter)->GetPoints(), (*iter)->GetFaults());
		pData = new CDlgConfigureData(this, CDlgConfigureData::eScoringInfo);
		HTREEITEM hInfo = m_ctrlTree.InsertItem(
			TVIF_TEXT | TVIF_PARAM,
			str,
			0, //nImage
			0, //nSelectedImage
			0, //nState
			0, //nStateMask
			reinterpret_cast<LPARAM>(pData),
			hScore,
			TVI_LAST);
		pData->SetHTreeItem(hInfo);
	}

	return hScore;
}

void CDlgConfigure::LoadData()
{
	m_ctrlTree.DeleteAllItems();

	CString str;
	str.LoadString(IDS_VENUES);
	CDlgConfigureData* pData = new CDlgConfigureData(this, CDlgConfigureData::eVenues);
	HTREEITEM hVenues = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		TVI_ROOT,
		TVI_LAST);
	pData->SetHTreeItem(hVenues);
	InsertVenues(hVenues);

	str.LoadString(IDS_FAULT_TYPES);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eFaultTypes);
	HTREEITEM hFaultTypes = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		TVI_ROOT,
		TVI_LAST);
	pData->SetHTreeItem(hFaultTypes);
	InsertFaultTypes(hFaultTypes);

	str.LoadString(IDS_OTHERPOINTS);
	pData = new CDlgConfigureData(this, CDlgConfigureData::eOtherPoints);
	HTREEITEM hOtherPoints = m_ctrlTree.InsertItem(
		TVIF_TEXT | TVIF_PARAM,
		str,
		0, //nImage
		0, //nSelectedImage
		0, //nState
		0, //nStateMask
		reinterpret_cast<LPARAM>(pData),
		TVI_ROOT,
		TVI_LAST);
	pData->SetHTreeItem(hOtherPoints);
	InsertOtherPoints(hOtherPoints);

	m_ctrlTree.Expand(m_ctrlTree.GetRootItem(), TVE_EXPAND);
}

void CDlgConfigure::InsertVenues(HTREEITEM hParent)
{
	for (ARBConfigVenueList::iterator iter = m_Config.GetVenues().begin(); iter != m_Config.GetVenues().end(); ++iter)
	{
		InsertVenue(hParent, (*iter));
	}
}

void CDlgConfigure::InsertFaultTypes(HTREEITEM hParent)
{
	for (ARBConfigFaultList::iterator iter = m_Config.GetFaults().begin(); iter != m_Config.GetFaults().end(); ++iter)
	{
		InsertFaultType(hParent, (*iter));
	}
}

void CDlgConfigure::InsertOtherPoints(HTREEITEM hParent)
{
	for (ARBConfigOtherPointsList::iterator iter = m_Config.GetOtherPoints().begin(); iter != m_Config.GetOtherPoints().end(); ++iter)
	{
		InsertOtherPoint(hParent, (*iter));
	}
}

void CDlgConfigure::InsertDivisions(HTREEITEM hParent, ARBConfigVenue* venue)
{
	for (ARBConfigDivisionList::iterator iter = venue->GetDivisions().begin(); iter != venue->GetDivisions().end(); ++iter)
	{
		InsertDivision(hParent, (*iter));
	}
}

void CDlgConfigure::InsertLevels(HTREEITEM hParent, ARBConfigDivision* div)
{
	for (ARBConfigLevelList::iterator iter = div->GetLevels().begin(); iter != div->GetLevels().end(); ++iter)
	{
		InsertLevel(hParent, (*iter));
	}
}

void CDlgConfigure::InsertTitles(HTREEITEM hParent, ARBConfigDivision* div)
{
	for (ARBConfigTitleList::iterator iter = div->GetTitles().begin(); iter != div->GetTitles().end(); ++iter)
	{
		InsertTitle(hParent, (*iter));
	}
}

void CDlgConfigure::InsertEvents(HTREEITEM hParent, ARBConfigVenue* venue)
{
	for (ARBConfigEventList::iterator iter = venue->GetEvents().begin(); iter != venue->GetEvents().end(); ++iter)
	{
		InsertEvent(hParent, (*iter));
	}
}

void CDlgConfigure::InsertScorings(HTREEITEM hParent, ARBConfigEvent* event)
{
	for (ARBConfigScoringList::iterator iter = event->GetScorings().begin(); iter != event->GetScorings().end(); ++iter)
	{
		InsertScoring(hParent, (*iter));
	}
}

CDlgConfigureData* CDlgConfigure::GetItemData(HTREEITEM inItem)
{
	CDlgConfigureData* pData = NULL;
	if (NULL != inItem)
		pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlTree.GetItemData(inItem));
	return pData;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure message handlers

BOOL CDlgConfigure::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigure::OnDestroy()
{
	m_ctrlTree.DeleteAllItems();
	CDialog::OnDestroy();
}

void CDlgConfigure::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	CCmdUI cmdUI;
	// (This may have changed for VC7+, but as of MFC4.2 it was required)
	// Hack to make this code work!!!!
	cmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (UINT n = 0; n < cmdUI.m_nIndexMax; ++n)
	{
		cmdUI.m_nIndex = n;
		cmdUI.m_nID = pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
		cmdUI.m_pMenu = pPopupMenu;
		CCmdTarget* pTarget = this;
		// Undocumented MFC cmd calls the ON_UPDATE_COMMAND_UI funcs.
		cmdUI.DoUpdate(pTarget, FALSE);
	}
}

void CDlgConfigure::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	HTREEITEM hCurItem = m_ctrlTree.GetSelectedItem();
	HTREEITEM hItem = m_ctrlTree.GetDropHilightItem();
	m_bReset = false;
	if (hItem)
		m_bReset = true;
	else
		hItem = hCurItem;
	CDlgConfigureData* pData = GetItemData(hItem);
	if (!pData)
		return;
	if (m_bReset)
		m_ctrlTree.SelectItem(hItem);
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		m_ctrlTree.GetItemRect(hItem, &rect, FALSE);
		point.x = rect.left + rect.Width() / 3;
		point.y = rect.top + rect.Height() / 2;
		ClientToScreen(&point);
	}
	UINT idMenu = IDR_CONFIGURE;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		UINT id = pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
		// By processing the menu id ourselves, we can control the selection
		// item. Otherwise, the menu id gets posted and arrives back to the
		// dialog after we reset the select (in case the right click was done
		// on an item other than the current item).
		switch (id)
		{
		default:
			break;
		case ID_CONFIG_ADD:
			OnConfigAdd();
			break;
		case ID_CONFIG_DUPLICATE:
			OnConfigDup();
			break;
		case ID_CONFIG_EDIT:
			OnConfigEdit();
			break;
		case ID_CONFIG_DELETE:
			OnConfigDelete();
			break;
		case ID_EXPAND:
			OnExpand();
			break;
		case ID_EXPAND_ALL:
			OnExpandAll();
			break;
		case ID_COLLAPSE:
			OnCollapse();
			break;
		case ID_COLLAPSE_ALL:
			OnCollapseAll();
			break;
		}
	}
	if (m_bReset)
		m_ctrlTree.SelectItem(hCurItem);
}

void CDlgConfigure::OnTvnDeleteitemVenues(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(pNMTreeView->itemOld.lParam);
		delete pData;
		pNMTreeView->itemOld.lParam = 0;
	}
	*pResult = 0;
}

void CDlgConfigure::OnNMRclickVenues(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_ctrlTree.GetDropHilightItem();
	if (NULL == hItem)
		hItem = m_ctrlTree.GetSelectedItem();
	if (hItem)
	{
		// Send WM_CONTEXTMENU to self (done according to Q222905)
		SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	}
	// Mark message as handled and suppress default handling
	*pResult = 1;
}

void CDlgConfigure::OnDblclkVenues(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData && pData->CanEdit())
		pData->Edit();
	*pResult = 1;
}

void CDlgConfigure::OnUpdateConfigAdd(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		bEnable = pData->CanAdd();
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnConfigAdd()
{
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		pData->Add();
}

void CDlgConfigure::OnUpdateConfigDup(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		bEnable = pData->CanDuplicate();
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnConfigDup()
{
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		pData->Duplicate();
}

void CDlgConfigure::OnUpdateConfigEdit(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		bEnable = pData->CanEdit();
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnConfigEdit()
{
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		pData->Edit();
}

void CDlgConfigure::OnUpdateConfigDelete(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		bEnable = pData->CanDelete();
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnConfigDelete()
{
	CDlgConfigureData* pData = GetItemData(m_ctrlTree.GetSelectedItem());
	if (pData)
		pData->Delete();
}

void CDlgConfigure::OnUpdateExpand(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem && m_ctrlTree.ItemHasChildren(hItem))
	{
		if (!(TVIS_EXPANDED & m_ctrlTree.GetItemState(hItem, TVIS_EXPANDED)))
			bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnExpand()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	m_ctrlTree.Expand(hItem, TVE_EXPAND);
	m_ctrlTree.EnsureVisible(hItem);
}

void CDlgConfigure::OnUpdateExpandAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem && m_ctrlTree.ItemHasChildren(hItem))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnExpandAll()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	ExpandAll(m_ctrlTree, hItem, TVE_EXPAND);
	m_ctrlTree.EnsureVisible(hItem);
}

void CDlgConfigure::OnUpdateCollapse(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem && m_ctrlTree.ItemHasChildren(hItem))
	{
		if ((TVIS_EXPANDED & m_ctrlTree.GetItemState(hItem, TVIS_EXPANDED)))
			bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnCollapse()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	m_ctrlTree.Expand(hItem, TVE_COLLAPSE);
	m_ctrlTree.EnsureVisible(hItem);
}

void CDlgConfigure::OnUpdateCollapseAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem && m_ctrlTree.ItemHasChildren(hItem))
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CDlgConfigure::OnCollapseAll()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	ExpandAll(m_ctrlTree, hItem, TVE_COLLAPSE);
	m_ctrlTree.EnsureVisible(hItem);
}

// Updating a configuration is mainly an additive procedure.
void CDlgConfigure::OnUpdate() 
{
	CDlgConfigUpdate dlg(this);
	if (IDOK == dlg.DoModal())
	{
		ARBConfig& update = dlg.GetConfig();
		CString msg;
		msg = m_Config.Update(0, update).c_str();
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

void CDlgConfigure::OnCancel()
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
	{
		delete (*iter);
	}
	m_DlgFixup.clear();
	CDialog::OnCancel();
}
