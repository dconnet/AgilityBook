/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigVenue class
 * @author David Connet
 *
 * There's one caveat to this dialog - there is no verification done to
 * make sure that a division/level pair isn't multiply defined. If that
 * happens, when we go to check things, the first match in the list will
 * be the only one used.
 * Note: The UI checks when creating div/level, so this shouldn't happen.
 * If the file is hand-edited, it could... But just how paranoid do we get?!
 * (Plus, the paranoia checking should be done when the file is loaded.)
 *
 * Revision History
 * @li 2004-01-24 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigVenue.h"

#include "ARBAgilityRecordBook.h"
#include "DlgConfigEvent.h"
#include "DlgConfigTitle.h"
#include "ARBConfigVenue.h"
#include "DlgConfigureData.h"
#include "DlgFixup.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigVenue dialog

CDlgConfigVenue::CDlgConfigVenue(ARBAgilityRecordBook& book, ARBConfig& config, ARBConfigVenue* pVenue, CWnd* pParent)
	: CDialog(CDlgConfigVenue::IDD, pParent)
	, m_Book(book)
	, m_Config(config)
	, m_pVenue(pVenue)
	, m_Action(eNone)
{
	ASSERT(m_pVenue);
	//{{AFX_DATA_INIT(CDlgConfigVenue)
	//}}AFX_DATA_INIT
}

CDlgConfigVenue::~CDlgConfigVenue()
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
	{
		delete (*iter);
	}
	m_DlgFixup.clear();
}

void CDlgConfigVenue::GetFixups(std::vector<CDlgFixup*>& ioFixups)
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
		ioFixups.push_back(*iter);
	m_DlgFixup.clear();
}

void CDlgConfigVenue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigVenue)
	DDX_Control(pDX, IDC_VENUE, m_ctrlName);
	DDX_Control(pDX, IDC_DESC, m_ctrlDesc);
	DDX_Control(pDX, IDC_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_MOVE_UP, m_ctrlMoveUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_ctrlMoveDown);
	DDX_Control(pDX, IDC_COMMENTS, m_ctrlComments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigVenue, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigVenue)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_DIVISION, OnGetdispinfoList)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_LEVEL, OnGetdispinfoTree)
	ON_NOTIFY(LVN_DELETEITEM, IDC_DIVISION, OnDeleteitemList)
	ON_NOTIFY(TVN_DELETEITEM, IDC_LEVEL, OnDeleteitemTree)
	ON_NOTIFY(NM_DBLCLK, IDC_DIVISION, OnDblclk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DIVISION, OnItemchangedDivision)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TITLES, OnItemchangedTitles)
	ON_NOTIFY(NM_SETFOCUS, IDC_DIVISION, OnSetfocusDivision)
	ON_NOTIFY(NM_SETFOCUS, IDC_LEVEL, OnSetfocusLevel)
	ON_NOTIFY(NM_SETFOCUS, IDC_TITLES, OnSetfocusTitles)
	ON_NOTIFY(NM_SETFOCUS, IDC_EVENT, OnSetfocusEvent)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_TITLES, OnGetdispinfoList)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_EVENT, OnGetdispinfoTree)
	ON_NOTIFY(LVN_DELETEITEM, IDC_TITLES, OnDeleteitemList)
	ON_NOTIFY(TVN_DELETEITEM, IDC_EVENT, OnDeleteitemTree)
	ON_NOTIFY(NM_DBLCLK, IDC_LEVEL, OnDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_TITLES, OnDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_EVENT, OnDblclk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LEVEL, OnItemchanged)
	ON_NOTIFY(TVN_SELCHANGED, IDC_EVENT, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigVenue::SetAction(eAction inAction)
{
	if (m_Action != inAction)
	{
		m_Action = inAction;
		switch (m_Action)
		{
		default:
			m_ctrlComments.SetWindowText("");
			break;
		case eDivisions:
			m_ctrlComments.SetWindowText("Buttons: Divisions");
			break;
		case eLevels:
			m_ctrlComments.SetWindowText("Buttons: Levels");
			break;
		case eTitles:
			m_ctrlComments.SetWindowText("Buttons: Titles");
			break;
		case eEvents:
			m_ctrlComments.SetWindowText("Buttons: Events");
			break;
		}
	}
	UpdateButtons();
}

CDlgConfigureData* CDlgConfigVenue::GetActionData(CWnd*& pWnd)
{
	CDlgConfigureData* pData = NULL;
	switch (m_Action)
	{
	case eDivisions:
		{
			pWnd = &m_ctrlDivisions;
			int index = m_ctrlDivisions.GetSelection();
			if (0 <= index)
				pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlDivisions.GetItemData(index));
		}
		break;
	case eLevels:
		{
			pWnd = &m_ctrlLevels;
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			if (NULL != hItem)
				pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
		}
		break;
	case eTitles:
		{
			pWnd = &m_ctrlTitles;
			int index = m_ctrlTitles.GetSelection();
			if (0 <= index)
				pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlTitles.GetItemData(index));
		}
		break;
	case eEvents:
		{
			pWnd = &m_ctrlEvents;
			HTREEITEM hItem = m_ctrlEvents.GetSelectedItem();
			if (NULL != hItem)
				pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlEvents.GetItemData(hItem));
		}
		break;
	}
	return pData;
}

void CDlgConfigVenue::UpdateButtons()
{
	BOOL bNew = FALSE;
	BOOL bDelete = FALSE;
	BOOL bEdit = FALSE;
	BOOL bCopy = FALSE;
	BOOL bMoveUp = FALSE;
	BOOL bMoveDown = FALSE;
	CWnd* pWnd;
	CDlgConfigureData* pData = GetActionData(pWnd);
	if (pWnd)
	{
		bNew = TRUE;
		if (pData)
		{
			bDelete = bEdit = TRUE;
			// if we're in here, we know something is valid...
			switch (m_Action)
			{
			case eDivisions:
				{
					int index = m_ctrlDivisions.GetSelection();
					if (0 < index)
						bMoveUp = TRUE;
					if (index < m_ctrlDivisions.GetItemCount() - 1)
						bMoveDown = TRUE;
				}
				break;
			case eLevels:
				{
					HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
					if (NULL != m_ctrlLevels.GetPrevSiblingItem(hItem))
						bMoveUp = TRUE;
					if (NULL != m_ctrlLevels.GetNextSiblingItem(hItem))
						bMoveDown = TRUE;
				}
				break;
			case eTitles:
				{
					int index = m_ctrlTitles.GetSelection();
					if (0 < index)
						bMoveUp = TRUE;
					if (index < m_ctrlTitles.GetItemCount() - 1)
						bMoveDown = TRUE;
				}
				break;
			case eEvents:
				bCopy = TRUE;
				{
					CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
					if (pEventData)
					{
						HTREEITEM hItem = m_ctrlEvents.GetSelectedItem();
						if (NULL != m_ctrlEvents.GetPrevSiblingItem(hItem))
							bMoveUp = TRUE;
						if (NULL != m_ctrlEvents.GetNextSiblingItem(hItem))
							bMoveDown = TRUE;
					}
				}
				break;
			}
		}
	}
	m_ctrlNew.EnableWindow(bNew);
	m_ctrlDelete.EnableWindow(bDelete);
	m_ctrlEdit.EnableWindow(bEdit);
	m_ctrlCopy.EnableWindow(bCopy);
	m_ctrlMoveUp.EnableWindow(bMoveUp);
	m_ctrlMoveDown.EnableWindow(bMoveDown);
}

void CDlgConfigVenue::LoadDivisionData()
{
	ARBConfigDivision* pDiv = NULL;
	int index = m_ctrlDivisions.GetSelection();
	if (0 <= index)
	{
		pDiv = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index))->GetDivision();
	}
	m_ctrlDivisions.DeleteAllItems();
	for (ARBConfigDivisionList::iterator iterDiv = m_pVenue->GetDivisions().begin(); iterDiv != m_pVenue->GetDivisions().end(); ++iterDiv)
	{
		index = m_ctrlDivisions.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlDivisions.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataDivision(*iterDiv)));
		if (pDiv == *iterDiv)
			m_ctrlDivisions.SetSelection(index);
	}
	m_ctrlDivisions.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	LoadLevelData();
	LoadTitleData();
}

void CDlgConfigVenue::LoadLevelData()
{
	ARBConfigLevel* pLevel = NULL;
	ARBConfigSubLevel* pSubLevel = NULL;
	HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
	if (NULL != hItem)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
		CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
		CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
		if (pLevelData)
			pLevel = pLevelData->GetLevel();
		else if (pSubLevelData)
		{
			pLevel = pSubLevelData->GetLevel();
			pSubLevel = pSubLevelData->GetSubLevel();
		}
	}
	m_ctrlLevels.DeleteAllItems();
	int idxDiv = m_ctrlDivisions.GetSelection();
	if (0 > idxDiv)
		return;
	CDlgConfigureDataDivision* pData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(idxDiv));
	ASSERT(pData);
	for (ARBConfigLevelList::iterator iterLevel = pData->GetDivision()->GetLevels().begin();
	iterLevel != pData->GetDivision()->GetLevels().end();
	++iterLevel)
	{
		hItem = m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
			0, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataLevel(pData->GetDivision(), *iterLevel)),
			TVI_ROOT, TVI_LAST);
		if (NULL == pSubLevel && pLevel == *iterLevel)
			m_ctrlLevels.SelectItem(hItem);
		if (0 < (*iterLevel)->GetSubLevels().size())
		{
			for (ARBConfigSubLevelList::iterator iterSubLevel = (*iterLevel)->GetSubLevels().begin(); iterSubLevel != (*iterLevel)->GetSubLevels().end(); ++iterSubLevel)
			{
				HTREEITEM hSub = m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(new CDlgConfigureDataSubLevel(pData->GetDivision(), *iterLevel, *iterSubLevel)),
					hItem, TVI_LAST);
				if (pSubLevel == *iterSubLevel)
					m_ctrlLevels.SelectItem(hSub);
			}
			m_ctrlLevels.Expand(hItem, TVE_EXPAND);
		}
	}
}

void CDlgConfigVenue::LoadTitleData()
{
	ARBConfigTitle* pTitle = NULL;
	int index = m_ctrlTitles.GetSelection();
	if (0 <= index)
	{
		pTitle = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index))->GetTitle();
	}
	m_ctrlTitles.DeleteAllItems();
	int idxDiv = m_ctrlDivisions.GetSelection();
	if (0 > idxDiv)
		return;
	CDlgConfigureDataDivision* pData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(idxDiv));
	ASSERT(pData);
	for (ARBConfigTitleList::iterator iterTitle = pData->GetDivision()->GetTitles().begin();
	iterTitle != pData->GetDivision()->GetTitles().end();
	++iterTitle)
	{
		index = m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlTitles.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataTitle(pData->GetDivision(), *iterTitle)));
		if (pTitle == *iterTitle)
			m_ctrlTitles.SetSelection(index);
	}
	m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

void CDlgConfigVenue::LoadEventData()
{
	ARBConfigEvent* pEvent = NULL;
	HTREEITEM hItem = m_ctrlEvents.GetSelectedItem();
	if (NULL != hItem)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlEvents.GetItemData(hItem));
		CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
		CDlgConfigureDataScoring* pScoringData = dynamic_cast<CDlgConfigureDataScoring*>(pData);
		if (pEventData)
			pEvent = pEventData->GetEvent();
		else if (pScoringData)
			pEvent = pScoringData->GetEvent();
	}
	m_ctrlEvents.DeleteAllItems();
	for (ARBConfigEventList::iterator iterEvent = m_pVenue->GetEvents().begin(); iterEvent != m_pVenue->GetEvents().end(); ++iterEvent)
	{
		HTREEITEM hItem = m_ctrlEvents.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
			0, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataEvent(*iterEvent)),
			TVI_ROOT, TVI_LAST);
		if (0 < (*iterEvent)->GetScorings().size())
		{
			for (ARBConfigScoringList::iterator iterScoring = (*iterEvent)->GetScorings().begin(); iterScoring != (*iterEvent)->GetScorings().end(); ++iterScoring)
			{
				m_ctrlEvents.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(new CDlgConfigureDataScoring(*iterEvent, *iterScoring)),
					hItem, TVI_LAST);
			}
			m_ctrlEvents.Expand(hItem, TVE_EXPAND);
		}
		if (pEvent == *iterEvent)
			m_ctrlEvents.SelectItem(hItem);
	}
}

int CDlgConfigVenue::FindCurrentDivision(const ARBConfigDivision* pDiv, bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlDivisions.GetItemCount(); ++index)
	{
		CDlgConfigureDataDivision* pData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
		if (pData->GetDivision() == pDiv)
		{
			idxCurrent = index;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlDivisions.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlDivisions.EnsureVisible(idxCurrent, FALSE);
	}
	return idxCurrent;
}

HTREEITEM CDlgConfigVenue::FindCurrentLevel(const ARBConfigLevel* pLevel, bool bSet)
{
	HTREEITEM hCurrent = NULL;
	HTREEITEM hItem = m_ctrlLevels.GetRootItem();
	while (NULL != hItem)
	{
		CDlgConfigureDataLevel* pData = reinterpret_cast<CDlgConfigureDataLevel*>(m_ctrlLevels.GetItemData(hItem));
		if (pData->GetLevel() == pLevel)
		{
			hCurrent = hItem;
			break;
		}
		hItem = m_ctrlLevels.GetNextSiblingItem(hItem);
	}
	if (bSet)
	{
		m_ctrlLevels.SelectItem(hCurrent);
		if (NULL != hItem)
			m_ctrlLevels.EnsureVisible(hCurrent);
		UpdateButtons();
	}
	return hCurrent;
}

HTREEITEM CDlgConfigVenue::FindCurrentSubLevel(const ARBConfigSubLevel* pSubLevel, bool bSet)
{
	HTREEITEM hCurrent = NULL;
	HTREEITEM hItem = m_ctrlLevels.GetRootItem();
	while (NULL == hCurrent && NULL != hItem)
	{
		if (m_ctrlLevels.ItemHasChildren(hItem))
		{
			HTREEITEM hChildItem = m_ctrlLevels.GetChildItem(hItem);
			while (NULL == hCurrent && NULL != hChildItem)
			{
				CDlgConfigureDataSubLevel* pData = reinterpret_cast<CDlgConfigureDataSubLevel*>(m_ctrlLevels.GetItemData(hChildItem));
				if (pData->GetSubLevel() == pSubLevel)
				{
					hCurrent = hChildItem;
					break;
				}
				hChildItem = m_ctrlLevels.GetNextSiblingItem(hChildItem);
			}
		}
		hItem = m_ctrlLevels.GetNextSiblingItem(hItem);
	}
	if (bSet)
	{
		m_ctrlLevels.SelectItem(hCurrent);
		if (NULL != hCurrent)
			m_ctrlLevels.EnsureVisible(hCurrent);
		UpdateButtons();
	}
	return hCurrent;
}

int CDlgConfigVenue::FindCurrentTitle(const ARBConfigTitle* pTitle, bool bSet)
{
	int idxCurrent = -1;
	for (int index = 0; index < m_ctrlTitles.GetItemCount(); ++index)
	{
		CDlgConfigureDataTitle* pData = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
		if (pData->GetTitle() == pTitle)
		{
			idxCurrent = index;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlTitles.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlTitles.EnsureVisible(idxCurrent, FALSE);
		UpdateButtons();
	}
	return idxCurrent;
}

HTREEITEM CDlgConfigVenue::FindCurrentEvent(const ARBConfigEvent* pEvent, bool bSet)
{
	HTREEITEM hCurrent = NULL;
	HTREEITEM hItem = m_ctrlEvents.GetRootItem();
	while (NULL == hCurrent && NULL != hItem)
	{
		CDlgConfigureDataEvent* pData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(hItem));
		if (pData->GetEvent() == pEvent)
		{
			hCurrent = hItem;
			break;
		}
		hItem = m_ctrlEvents.GetNextSiblingItem(hItem);
	}
	if (bSet)
	{
		m_ctrlEvents.SelectItem(hCurrent);
		if (NULL != hCurrent)
			m_ctrlEvents.EnsureVisible(hCurrent);
		UpdateButtons();
	}
	return hCurrent;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigVenue message handlers

BOOL CDlgConfigVenue::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlDivisions.SetExtendedStyle(m_ctrlDivisions.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlDivisions.InsertColumn(0, "Divisions");
	m_ctrlTitles.InsertColumn(0, "Titles");

	m_ctrlName.SetWindowText(m_pVenue->GetName().c_str());
	CString str(m_pVenue->GetDesc().c_str());
	str.Replace("\n", "\r\n");
	m_ctrlDesc.SetWindowText(str);

	LoadDivisionData();
	LoadEventData();
	SetAction(eDivisions);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigVenue::OnDestroy() 
{
	m_ctrlLevels.DeleteAllItems();
	m_ctrlEvents.DeleteAllItems();
	CDialog::OnDestroy();
}

void CDlgConfigVenue::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CDlgConfigVenue::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CDlgConfigureData *pData = reinterpret_cast<CDlgConfigureData*>(pNMListView->lParam);
	delete pData;
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CDlgConfigVenue::OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CDlgConfigureData *pData = reinterpret_cast<CDlgConfigureData*>(pNMTreeView->itemOld.lParam);
	delete pData;
	pNMTreeView->itemOld.lParam = 0;
	*pResult = 0;
}

void CDlgConfigVenue::OnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (pTVDispInfo->item.mask & TVIF_TEXT)
	{
		CDlgConfigureData *pData = reinterpret_cast<CDlgConfigureData*>(pTVDispInfo->item.lParam);
		if (pData)
		{
			CString str = pData->OnNeedText(0);
			::lstrcpyn(pTVDispInfo->item.pszText, str, pTVDispInfo->item.cchTextMax);
			pTVDispInfo->item.pszText[pTVDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}

void CDlgConfigVenue::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgConfigVenue::OnItemchangedDivision(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LoadLevelData();
	LoadTitleData();
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigVenue::OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigVenue::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigVenue::OnSetfocusDivision(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eDivisions);
	*pResult = 0;
}

void CDlgConfigVenue::OnSetfocusLevel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eLevels);
	*pResult = 0;
}

void CDlgConfigVenue::OnSetfocusTitles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eTitles);
	*pResult = 0;
}

void CDlgConfigVenue::OnSetfocusEvent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetAction(eEvents);
	*pResult = 0;
}

void CDlgConfigVenue::OnNew() 
{
	CWnd* pWnd = NULL;
	CDlgConfigureData* pData = GetActionData(pWnd);
	bool done = false;
	std::string name;
	switch (m_Action)
	{
	case eDivisions:
		while (!done)
		{
			done = true;
			CDlgName dlg(name.c_str(), IDS_DIVISION_NAME, this);
			if (IDOK == dlg.DoModal())
			{
				CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
				name = dlg.GetName();
				if (pVenueData->GetVenue()->GetDivisions().FindDivision(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigDivision* pNewDiv = pVenueData->GetVenue()->GetDivisions().AddDivision(name);
				if (pNewDiv)
				{
					int nInsertAt = m_ctrlDivisions.GetSelection();
					if (0 > nInsertAt)
						nInsertAt = m_ctrlDivisions.GetItemCount();
					else
						++nInsertAt;
					int index = m_ctrlDivisions.InsertItem(LVIF_TEXT | LVIF_PARAM, nInsertAt,
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(new CDlgConfigureDataDivision(pNewDiv)));
					m_ctrlDivisions.SetSelection(index);
					FindCurrentDivision(pNewDiv, true);
				}
			}
		}
		break;

	case eLevels:
		{
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
			CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
			if (pLevelData)
			{
				while (!done)
				{
					done = true;
					CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, this);
					if (IDOK == dlg.DoModal())
					{
						name = dlg.GetName();
						if (pLevelData->GetDivision()->GetLevels().FindTrueLevel(name))
						{
							done = false;
							AfxMessageBox(IDS_NAME_IN_USE);
							continue;
						}
						ARBConfigLevel* pNewLevel = pLevelData->GetDivision()->GetLevels().AddLevel(name);
						if (pNewLevel)
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(new CDlgConfigureDataLevel(pLevelData->GetDivision(), pNewLevel)),
								TVI_ROOT, hItem);
							FindCurrentLevel(pNewLevel, true);
						}
					}
				}
			}
			else if (pSubLevelData)
			{
				while (!done)
				{
					done = true;
					CDlgName dlg(name.c_str(), IDS_SUBLEVEL_NAME, this);
					if (IDOK == dlg.DoModal())
					{
						name = dlg.GetName();
						if (pSubLevelData->GetDivision()->GetLevels().FindTrueLevel(name))
						{
							done = false;
							AfxMessageBox(IDS_NAME_IN_USE);
							continue;
						}
						ARBConfigSubLevel* pNewSubLevel = pSubLevelData->GetLevel()->GetSubLevels().AddSubLevel(name);
						if (pNewSubLevel)
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(new CDlgConfigureDataSubLevel(pLevelData->GetDivision(), pSubLevelData->GetLevel(), pNewSubLevel)),
								m_ctrlLevels.GetParentItem(hItem), hItem);
							FindCurrentSubLevel(pNewSubLevel, true);
						}
					}
				}
			}
		}
		break;

	case eTitles:
		while (!done)
		{
			done = true;
			CDlgConfigTitle dlg(name.c_str(), "", "", this);
			if (IDOK == dlg.DoModal())
			{
				CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(pData);
				name = dlg.GetName();
				if (m_pVenue->GetDivisions().FindTitle(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigTitle* pTitle = pTitleData->GetDivision()->GetTitles().AddTitle(name);
				pTitle->SetLongName(dlg.GetLongName());
				pTitle->SetDescription(dlg.GetDesc());
				if (pTitle)
				{
					int nInsertAt = m_ctrlDivisions.GetSelection();
					if (0 > nInsertAt)
						nInsertAt = m_ctrlDivisions.GetItemCount();
					else
						++nInsertAt;
					int index = m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, nInsertAt,
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(new CDlgConfigureDataTitle(pTitleData->GetDivision(), pTitle)));
					m_ctrlTitles.SetSelection(index);
				}
			}
		}
		break;

	case eEvents:
		{
			// The dialog will ensure uniqueness.
			ARBConfigEvent* event = new ARBConfigEvent();
			CDlgConfigEvent dlg(m_pVenue, event, this);
			if (IDOK == dlg.DoModal())
			{
				ARBConfigEvent* pEvent = m_pVenue->GetEvents().AddEvent(event);
				if (pEvent)
				{
					LoadEventData();
					FindCurrentEvent(pEvent, true);
				}
			}
			event->Release();
		}
		break;
	}
}

void CDlgConfigVenue::OnDelete() 
{
	CWnd *pWnd;
	CDlgConfigureData* pData = GetActionData(pWnd);
	if (!pData)
		return;
	int index;
	HTREEITEM hItem;
	bool bDelete = true;
	switch (m_Action)
	{
	case eDivisions:
		if (0 <= (index = m_ctrlDivisions.GetSelection()))
		{
			CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(pData);
			ASSERT(NULL != pDivData);
			std::string div = pDivData->GetDivision()->GetName();
			int nTrials = m_Book.GetDogs().NumRunsInDivision(m_pVenue, div);
			int nTitles = m_Book.GetDogs().NumTitlesInDivision(m_pVenue, div);
			if (0 < nTrials || 0 < nTitles)
			{
				CString msg;
				msg.FormatMessage(IDS_DELETE_DIVISION,
					div.c_str(),
					nTitles,
					nTrials);
				if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
					bDelete = false;
			}
			if (bDelete)
			{
				// If we were able to delete it...
				if (m_pVenue->GetDivisions().DeleteDivision(div, m_pVenue->GetEvents()))
				{
					// Then we commit to fixing the real data.
					if (0 < nTrials || 0 < nTitles)
						m_DlgFixup.push_back(new CDlgFixupDeleteDivision(m_pVenue->GetName(), div));
					m_ctrlDivisions.DeleteItem(index);
				}
			}
		}
		break;

	case eLevels:
		if (NULL != (hItem = m_ctrlLevels.GetSelectedItem()))
		{
			CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
			CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
			if (pLevelData)
			{
				std::string level = pLevelData->GetLevel()->GetName();
				int nLevels = m_Book.GetDogs().NumLevelsInUse(
					m_pVenue->GetName(),
					pLevelData->GetDivision()->GetName(),
					level);
				if (0 < nLevels)
				{
					CString msg;
					msg.FormatMessage(IDS_DELETE_LEVEL,
						level.c_str(),
						nLevels);
					if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
						bDelete = false;
				}
				if (bDelete)
				{
					if (pLevelData->GetDivision()->GetLevels().DeleteLevel(level, m_pVenue->GetEvents()))
					{
						// Then we commit to fixing the real data.
						if (0 < nLevels)
							m_DlgFixup.push_back(new CDlgFixupDeleteLevel(
								m_pVenue->GetName(),
								pLevelData->GetDivision()->GetName(),
								level));
						m_ctrlLevels.DeleteItem(hItem);
					}
				}
			}
			else if (pSubLevelData)
			{
				std::string level = pSubLevelData->GetLevel()->GetName();
				bool bDelete = true;
				std::string subLevel = pSubLevelData->GetSubLevel()->GetName();
				int nLevels = m_Book.GetDogs().NumLevelsInUse(
					m_pVenue->GetName(),
					pSubLevelData->GetDivision()->GetName(),
					subLevel);
				if (0 < nLevels)
				{
					CString msg;
					msg.FormatMessage(IDS_DELETE_LEVEL,
						subLevel.c_str(),
						nLevels);
					if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
						bDelete = false;
				}
				if (bDelete)
				{
					bool bLevelModified = false;
					if (pSubLevelData->GetDivision()->GetLevels().DeleteSubLevel(subLevel, bLevelModified))
					{
						// Then we commit to fixing the real data.
						if (bLevelModified)
						{
							m_pVenue->GetEvents().RenameLevel(level, pLevelData->GetLevel()->GetName());
							m_DlgFixup.push_back(new CDlgFixupRenameLevel(
								m_pVenue->GetName(),
								pSubLevelData->GetDivision()->GetName(),
								level, pLevelData->GetLevel()->GetName()));
							m_ctrlLevels.Invalidate();
						}
						if (0 < nLevels)
							m_DlgFixup.push_back(new CDlgFixupDeleteLevel(
								m_pVenue->GetName(),
								pSubLevelData->GetDivision()->GetName(),
								subLevel));
						m_ctrlLevels.DeleteItem(hItem);
					}
				}
			}
		}
		break;

	case eTitles:
		if (0 <= (index = m_ctrlTitles.GetSelection()))
		{
			CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(pData);
			ASSERT(NULL != pTitleData);
			std::string title = pTitleData->GetTitle()->GetName();
			int nTitles = m_Book.GetDogs().NumTitlesInUse(m_pVenue->GetName(), title);
			if (0 < nTitles)
			{
				CString msg;
				msg.FormatMessage(IDS_DELETE_TITLE,
					title.c_str(),
					nTitles);
				if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
					bDelete = false;
			}
			if (bDelete)
			{
				if (pTitleData->GetDivision()->GetTitles().DeleteTitle(title))
				{
					// Then we commit to fixing the real data.
					if (0 < nTitles)
						m_DlgFixup.push_back(new CDlgFixupDeleteTitle(m_pVenue->GetName(), title));
					m_ctrlTitles.DeleteItem(index);
				}
			}
		}
		break;

	case eEvents:
		if (NULL != (hItem = m_ctrlEvents.GetSelectedItem()))
		{
			CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
			CDlgConfigureDataScoring* pScoringData = dynamic_cast<CDlgConfigureDataScoring*>(pData);
			ARBConfigEvent* pEvent = NULL;
			if (pEventData)
				pEvent = pEventData->GetEvent();
			else if (pScoringData)
				pEvent = pScoringData->GetEvent();
			std::string event = pEvent->GetName();
			int nEvents = m_Book.GetDogs().NumEventsInUse(m_pVenue->GetName(), event);
			if (0 < nEvents)
			{
				CString msg;
				msg.FormatMessage(IDS_DELETE_EVENT,
					event.c_str(),
					nEvents);
				if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
					bDelete = false;
			}
			if (bDelete)
			{
				if (m_pVenue->GetEvents().DeleteEvent(event))
				{
					// Then we commit to fixing the real data.
					if (0 < nEvents)
						m_DlgFixup.push_back(new CDlgFixupDeleteEvent(m_pVenue->GetName(), event));
					m_ctrlEvents.DeleteItem(hItem);
				}
			}
		}
		break;
	}
}

void CDlgConfigVenue::OnEdit() 
{
	CWnd* pWnd;
	CDlgConfigureData* pData = GetActionData(pWnd);
	switch (m_Action)
	{
	case eDivisions:
		{
			CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(pData);
			bool done = false;
			std::string oldName = pDivData->GetDivision()->GetName();
			std::string name(oldName);
			if (0 < m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_Book.GetConfig(), m_pVenue->GetName(), name))
			{
				if (IDYES != AfxMessageBox(IDS_CHANGEDIV_ISSUES, MB_ICONQUESTION | MB_YESNO))
					return;
			}
			while (!done)
			{
				done = true;
				CDlgName dlg(name.c_str(), IDS_DIVISION_NAME, this);
				if (IDOK == dlg.DoModal())
				{
					name = dlg.GetName();
					if (oldName != name)
					{
						if (m_pVenue->GetDivisions().FindDivision(name))
						{
							done = false;
							AfxMessageBox(IDS_NAME_IN_USE);
							continue;
						}
						pDivData->GetDivision()->SetName(name);
						m_pVenue->GetEvents().RenameDivision(oldName, name);
						m_DlgFixup.push_back(new CDlgFixupRenameDivision(m_pVenue->GetName(), oldName, name));
						m_ctrlDivisions.Invalidate();
					}
				}
			}
		}
		break;
	
	case eLevels:
		{
			CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
			CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
			if (pLevelData)
			{
				bool done = false;
				std::string oldName = pLevelData->GetLevel()->GetName();
				std::string name(oldName);
				// If there are sublevels, don't ask the following question. If a level has
				// sublevels, the level name isn't allowed to be used for an event.
				if (0 == pLevelData->GetLevel()->GetSubLevels().size())
				{
					if (0 < m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_Book.GetConfig(), m_pVenue->GetName(), pLevelData->GetDivision()->GetName()))
					{
						if (IDYES != AfxMessageBox(IDS_CHANGELEVEL_ISSUES, MB_ICONQUESTION | MB_YESNO))
							return;
					}
				}
				while (!done)
				{
					done = true;
					CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, this);
					if (IDOK == dlg.DoModal())
					{
						name = dlg.GetName();
						// If the name hasn't changed, don't do the inuse verification.
						if (oldName != name)
						{
							bool bInUse = false;
							if (0 == pLevelData->GetLevel()->GetSubLevels().size())
							{
								if (pLevelData->GetDivision()->GetLevels().FindTrueLevel(name))
									bInUse = true;
							}
							else
							{
								if (pLevelData->GetDivision()->GetLevels().VerifyLevel(name, false))
									bInUse = true;
							}
							if (bInUse)
							{
								done = false;
								AfxMessageBox(IDS_NAME_IN_USE);
								continue;
							}
							pLevelData->GetLevel()->SetName(name);
							if (0 == pLevelData->GetLevel()->GetSubLevels().size())
								m_pVenue->GetEvents().RenameLevel(oldName, name);
							m_DlgFixup.push_back(new CDlgFixupRenameLevel(
								m_pVenue->GetName(),
								pLevelData->GetDivision()->GetName(),
								oldName, pLevelData->GetLevel()->GetName()));
							m_ctrlLevels.Invalidate();
						}
					}
				}
			}
			else if (pSubLevelData)
			{
				bool done = false;
				std::string oldName = pSubLevelData->GetSubLevel()->GetName();
				std::string name(oldName);
				if (0 < m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_Book.GetConfig(), m_pVenue->GetName(), pSubLevelData->GetDivision()->GetName()))
				{
					if (IDYES != AfxMessageBox(IDS_CHANGESUBLEVEL_ISSUES, MB_ICONQUESTION | MB_YESNO))
						return;
				}
				while (!done)
				{
					done = true;
					CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, this);
					if (IDOK == dlg.DoModal())
					{
						name = dlg.GetName();
						if (oldName != name)
						{
							if (pSubLevelData->GetDivision()->GetLevels().FindTrueLevel(name))
							{
								done = false;
								AfxMessageBox(IDS_NAME_IN_USE);
								continue;
							}
							pSubLevelData->GetSubLevel()->SetName(name);
							// No need to fix ARBConfigEventList cause we don't do sublevel names in events.
							m_DlgFixup.push_back(new CDlgFixupRenameLevel(
								m_pVenue->GetName(),
								pSubLevelData->GetDivision()->GetName(),
								oldName, pSubLevelData->GetSubLevel()->GetName()));
							m_ctrlLevels.Invalidate();
						}
					}
				}
			}
		}
		break;

	case eTitles:
		{
			CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(pData);
			bool done = false;
			std::string oldName = pTitleData->GetTitle()->GetName();
			std::string oldLongName = pTitleData->GetTitle()->GetLongName();
			std::string name(oldName);
			std::string longname(oldLongName);
			while (!done)
			{
				done = true;
				CDlgConfigTitle dlg(name.c_str(), pTitleData->GetTitle()->GetLongName().c_str(), pTitleData->GetTitle()->GetDescription().c_str(), this);
				if (IDOK == dlg.DoModal())
				{
					name = dlg.GetName();
					longname = dlg.GetLongName();
					if (oldName != name)
					{
						if (m_pVenue->GetDivisions().FindTitle(name))
						{
							int nTitles = m_Book.GetDogs().NumTitlesInUse(m_pVenue->GetName(), oldName);
							bool bInUse = true;
							if (0 < nTitles)
							{
								if (IDYES == AfxMessageBox("This name is currently in use. Do you want to merge your data into this existing name?", MB_YESNO | MB_ICONQUESTION))
								{
									bInUse = false;
									m_DlgFixup.push_back(new CDlgFixupRenameTitle(m_pVenue->GetName(), oldName, name));
									if (pTitleData->GetDivision()->GetTitles().DeleteTitle(oldName))
									{
										m_ctrlTitles.DeleteItem(m_ctrlTitles.GetSelection());
										// This will break out of the loop on the 'continue';
									}
								}
							}
							if (bInUse)
							{
								done = false;
								AfxMessageBox(IDS_NAME_IN_USE);
							}
							continue;
						}
						pTitleData->GetTitle()->SetName(name);
					}
					pTitleData->GetTitle()->SetLongName(longname);
					pTitleData->GetTitle()->SetDescription(dlg.GetDesc());
					if (name != oldName || longname != oldLongName)
					{
						if (name != oldName)
							m_DlgFixup.push_back(new CDlgFixupRenameTitle(m_pVenue->GetName(), oldName, name));
						m_ctrlTitles.Invalidate();
					}
				}
			}
		}
		break;

	case eEvents:
		{
			CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
			CDlgConfigureDataScoring* pScoringData = dynamic_cast<CDlgConfigureDataScoring*>(pData);
			ARBConfigEvent* pEvent = NULL;
			if (pEventData)
				pEvent = pEventData->GetEvent();
			else if (pScoringData)
				pEvent = pScoringData->GetEvent();
			std::string event = pEvent->GetName();
			CDlgConfigEvent dlg(m_pVenue, pEvent, this);
			if (IDOK == dlg.DoModal())
			{
				if (pEvent->GetName() != event)
				{
					m_DlgFixup.push_back(new CDlgFixupRenameEvent(m_pVenue->GetName(), event, pEvent->GetName()));
				}
				LoadEventData();
				FindCurrentEvent(pEvent, true);
			}
		}
		break;
	}
}

void CDlgConfigVenue::OnCopy() 
{
	HTREEITEM hItem;
	switch (m_Action)
	{
	case eDivisions:
	case eLevels:
	case eTitles:
		// We don't currently support copying these.
		break;

	case eEvents:
		if (NULL != (hItem = m_ctrlEvents.GetSelectedItem()))
		{
			CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlEvents.GetItemData(hItem));
			CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
			CDlgConfigureDataScoring* pScoringData = dynamic_cast<CDlgConfigureDataScoring*>(pData);
			ARBConfigEvent* pEvent = NULL;
			if (pEventData)
				pEvent = pEventData->GetEvent();
			else if (pScoringData)
				pEvent = pScoringData->GetEvent();
			CString copyOf;
			copyOf.LoadString(IDS_COPYOF);
			std::string name(pEvent->GetName());
			while (m_pVenue->GetEvents().FindEvent(name))
			{
				name = (LPCSTR)copyOf + name;
			}
			ARBConfigEvent* event = new ARBConfigEvent(*pEvent);
			event->SetName(name);
			ARBConfigEvent* pNewEvent = m_pVenue->GetEvents().AddEvent(event);
			if (pNewEvent)
			{
				LoadEventData();
				FindCurrentEvent(pNewEvent, true);
			}
			event->Release();
		}
		break;
	}
}

void CDlgConfigVenue::OnMoveUp() 
{
	CWnd* pWnd;
	CDlgConfigureData* pData = GetActionData(pWnd);
	if (pWnd && pData)
	{
		switch (m_Action)
		{
		case eDivisions:
			{
				int index = m_ctrlDivisions.GetSelection();
				if (0 < index)
				{
					CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(pData);
					m_pVenue->GetDivisions().Move(pDivData->GetDivision(), -1);
					LoadDivisionData();
					UpdateButtons();
				}
			}
			break;
		case eLevels:
			{
				HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
				HTREEITEM hPrevItem = m_ctrlLevels.GetPrevSiblingItem(hItem);
				if (NULL != hPrevItem)
				{
					CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
					CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
					if (pLevelData)
						pLevelData->GetDivision()->GetLevels().Move(pLevelData->GetLevel(), -1);
					else if (pSubLevelData)
						pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevelData->GetSubLevel(), -1);
					LoadLevelData();
					UpdateButtons();
				}
			}
			break;
		case eTitles:
			{
				int index = m_ctrlTitles.GetSelection();
				if (0 < index)
				{
					CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(pData);
					pTitleData->GetDivision()->GetTitles().Move(pTitleData->GetTitle(), -1);
					LoadTitleData();
					UpdateButtons();
				}
			}
			break;
		case eEvents:
			{
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
				if (pEventData)
				{
					HTREEITEM hItem = m_ctrlEvents.GetSelectedItem();
					HTREEITEM hPrevItem = m_ctrlEvents.GetPrevSiblingItem(hItem);
					if (NULL != hPrevItem)
					{
						m_pVenue->GetEvents().Move(pEventData->GetEvent(), -1);
						LoadEventData();
						UpdateButtons();
					}
				}
			}
			break;
		}
	}
}

void CDlgConfigVenue::OnMoveDown() 
{
	CWnd* pWnd;
	CDlgConfigureData* pData = GetActionData(pWnd);
	if (pWnd && pData)
	{
		switch (m_Action)
		{
		case eDivisions:
			{
				int index = m_ctrlDivisions.GetSelection();
				if (index < m_ctrlDivisions.GetItemCount() - 1)
				{
					CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(pData);
					m_pVenue->GetDivisions().Move(pDivData->GetDivision(), 1);
					LoadDivisionData();
					UpdateButtons();
				}
			}
			break;
		case eLevels:
			{
				HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
				HTREEITEM hNextItem = m_ctrlLevels.GetNextSiblingItem(hItem);
				if (NULL != hNextItem)
				{
					CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
					CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
					if (pLevelData)
						pLevelData->GetDivision()->GetLevels().Move(pLevelData->GetLevel(), 1);
					else if (pSubLevelData)
						pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevelData->GetSubLevel(), 1);
					LoadLevelData();
					UpdateButtons();
				}
			}
			break;
		case eTitles:
			{
				int index = m_ctrlTitles.GetSelection();
				if (index < m_ctrlTitles.GetItemCount() - 1)
				{
					CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(pData);
					pTitleData->GetDivision()->GetTitles().Move(pTitleData->GetTitle(), 1);
					LoadTitleData();
					UpdateButtons();
				}
			}
			break;
		case eEvents:
			{
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(pData);
				if (pEventData)
				{
					HTREEITEM hItem = m_ctrlEvents.GetSelectedItem();
					HTREEITEM hNextItem = m_ctrlEvents.GetNextSiblingItem(hItem);
					if (NULL != hNextItem)
					{
						m_pVenue->GetEvents().Move(pEventData->GetEvent(), 1);
						LoadEventData();
						UpdateButtons();
					}
				}
			}
			break;
		}
	}
}

void CDlgConfigVenue::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CString str;
	m_ctrlName.GetWindowText(str);
	str.TrimRight();
	str.TrimLeft();
	if (0 == str.GetLength())
	{
		AfxMessageBox("Invalidate name");
		GotoDlgCtrl(&m_ctrlName);
		return;
	}
	std::string name((LPCTSTR)str);
	std::string oldName = m_pVenue->GetName();
	if (oldName != name)
	{
		if (m_Config.GetVenues().FindVenue(name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			return;
		}
		m_pVenue->SetName(name);
	}
	m_ctrlDesc.GetWindowText(str);
	str.TrimRight();
	str.Replace("\r\n", "\n");
	m_pVenue->SetDesc((LPCTSTR)str);
	if (oldName != name)
		m_DlgFixup.push_back(new CDlgFixupRenameVenue(oldName, name));

	// The rest is already taken care of.

	CDialog::OnOK();
}
