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
 * There is some internal problem that happens when editing this via a specific
 * series of events:
 *  Using NADAC, dbl-click on Gamblers. Then 'OK' to close. Then 'OK' to close
 *  this. Somewhere in the final sequence, the program will die in the debugger
 *  with a msg that the pgm has modified memory that was deleted. (USDAA/snooker
 *  also triggers the problem)
 *   HEAP[AgilityBook.exe]: HEAP: Free Heap block 170830 modified at 170acc after it was freed
 *   Unhandled exception at 0x77f75a58 in AgilityBook.exe: User breakpoint.
 *  The block number and memory address both vary. Simply twitching the mouse
 *  over the event tree control is enough to trigger it. SmartHeap was useless.
 *  Happens in both VC6 and 7. Commenting out ALL of my deletes does not solve
 *  it - it's something windows is accessing. By not adding the scoring events
 *  under the events in the tree, that seems to make the problem go away - so
 *  I have modified the dialog to only display the event name, not scoring
 *  methods. Hopefully this actually solves things and doesn't just move it.
 *  The scoringmethod vector in the event dialog did seem to show up alot.
 *
 * Revision History
 * @li 2004-08-14 DRC Fixed a problem dbl-clicking when nothing was selected.
 * @li 2004-04-29 DRC Changed the way events are displayed (from tree to list).
 * @li 2004-02-09 DRC Fixed some bugs when creating/modifying venues.
 * @li 2004-02-02 DRC Added ExistingPoints.
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

CDlgConfigVenue::CDlgConfigVenue(CAgilityBookDoc* pDoc,
	ARBAgilityRecordBook& book, ARBConfig& config,
	ARBConfigVenue* pVenue, CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigVenue::IDD, pParent)
	, m_pDoc(pDoc)
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
		delete (*iter);
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
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigVenue)
	DDX_Control(pDX, IDC_CONFIG_VENUE, m_ctrlName);
	DDX_Control(pDX, IDC_CONFIG_VENUE_DESC, m_ctrlDesc);
	DDX_Control(pDX, IDC_CONFIG_VENUE_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_CONFIG_VENUE_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_CONFIG_VENUE_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_CONFIG_VENUE_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_CONFIG_VENUE_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_CONFIG_VENUE_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CONFIG_VENUE_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CONFIG_VENUE_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_CONFIG_VENUE_MOVE_UP, m_ctrlMoveUp);
	DDX_Control(pDX, IDC_CONFIG_VENUE_MOVE_DOWN, m_ctrlMoveDown);
	DDX_Control(pDX, IDC_CONFIG_VENUE_COMMENTS, m_ctrlComments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigVenue, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigVenue)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_DIVISION, OnGetdispinfoList)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_CONFIG_VENUE_LEVEL, OnGetdispinfoTree)
	ON_NOTIFY(LVN_DELETEITEM, IDC_CONFIG_VENUE_DIVISION, OnDeleteitemList)
	ON_NOTIFY(TVN_DELETEITEM, IDC_CONFIG_VENUE_LEVEL, OnDeleteitemTree)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_DIVISION, OnDblclk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_DIVISION, OnItemchangedDivision)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_TITLES, OnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_EVENT, OnItemchangedList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIG_VENUE_LEVEL, OnItemchanged)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_DIVISION, OnSetfocusDivision)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_LEVEL, OnSetfocusLevel)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_TITLES, OnSetfocusTitles)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_EVENT, OnSetfocusEvent)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_NEW, OnNew)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_COPY, OnCopy)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_MOVE_DOWN, OnMoveDown)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_TITLES, OnGetdispinfoList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_CONFIG_VENUE_TITLES, OnDeleteitemList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_EVENT, OnGetdispinfoList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_CONFIG_VENUE_EVENT, OnDeleteitemList)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_LEVEL, OnDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_TITLES, OnDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_EVENT, OnDblclk)
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

void CDlgConfigVenue::UpdateButtons()
{
	BOOL bNew = FALSE;
	BOOL bDelete = FALSE;
	BOOL bEdit = FALSE;
	BOOL bCopy = FALSE;
	BOOL bMoveUp = FALSE;
	BOOL bMoveDown = FALSE;

	switch (m_Action)
	{
	case eDivisions:
		{
			bNew = TRUE;
			int index = m_ctrlDivisions.GetSelection();
			if (0 <= index)
			{
				bDelete = bEdit = TRUE;
				if (0 < index)
					bMoveUp = TRUE;
				if (index < m_ctrlDivisions.GetItemCount() - 1)
					bMoveDown = TRUE;
			}
		}
		break;

	case eLevels:
		{
			if (GetCurrentDivisionData())
				bNew = TRUE;
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			if (NULL != hItem)
			{
				bDelete = bEdit = TRUE;
				if (NULL != m_ctrlLevels.GetPrevSiblingItem(hItem))
					bMoveUp = TRUE;
				if (NULL != m_ctrlLevels.GetNextSiblingItem(hItem))
					bMoveDown = TRUE;
			}
		}
		break;

	case eTitles:
		{
			if (GetCurrentDivisionData())
				bNew = TRUE;
			int index = m_ctrlTitles.GetSelection();
			if (0 <= index)
			{
				bDelete = bEdit = TRUE;
				if (0 < index)
					bMoveUp = TRUE;
				if (index < m_ctrlTitles.GetItemCount() - 1)
					bMoveDown = TRUE;
			}
		}
		break;

	case eEvents:
		{
			bNew = TRUE;
			int index = m_ctrlEvents.GetSelection();
			if (0 <= index)
			{
				bDelete = bEdit = TRUE;
				CDlgConfigureDataEvent* pEventData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
				if (pEventData)
				{
					bCopy = TRUE;
					if (0 < index)
						bMoveUp = TRUE;
					if (index < m_ctrlEvents.GetItemCount() - 1)
						bMoveDown = TRUE;
				}
			}
		}
		break;
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
	CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
	if (pDivData)
	{
		pDiv = pDivData->GetDivision();
		pDiv->AddRef();
	}
	m_ctrlDivisions.DeleteAllItems();
	for (ARBConfigDivisionList::iterator iterDiv = m_pVenue->GetDivisions().begin(); iterDiv != m_pVenue->GetDivisions().end(); ++iterDiv)
	{
		m_ctrlDivisions.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlDivisions.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataDivision(*iterDiv)));
	}
	m_ctrlDivisions.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentDivision(pDiv, true);
	if (pDiv)
		pDiv->Release();
	LoadLevelData();
	LoadTitleData();
}

void CDlgConfigVenue::LoadLevelData()
{
	ARBConfigLevel* pLevel = NULL;
	ARBConfigSubLevel* pSubLevel = NULL;
	CDlgConfigureDataLevel* pLevelData = GetCurrentLevelData();
	CDlgConfigureDataSubLevel* pSubLevelData = GetCurrentSubLevelData();
	if (pLevelData)
	{
		pLevel = pLevelData->GetLevel();
		pLevel->AddRef();
	}
	else if (pSubLevelData)
	{
		pLevel = pSubLevelData->GetLevel();
		pSubLevel = pSubLevelData->GetSubLevel();
		pSubLevel->AddRef();
	}
	m_ctrlLevels.DeleteAllItems();
	CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
	if (!pDivData)
		return;
	for (ARBConfigLevelList::iterator iterLevel = pDivData->GetDivision()->GetLevels().begin();
	iterLevel != pDivData->GetDivision()->GetLevels().end();
	++iterLevel)
	{
		HTREEITEM hItem = m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
			0, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataLevel(pDivData->GetDivision(), *iterLevel)),
			TVI_ROOT, TVI_LAST);
		if (0 < (*iterLevel)->GetSubLevels().size())
		{
			for (ARBConfigSubLevelList::iterator iterSubLevel = (*iterLevel)->GetSubLevels().begin(); iterSubLevel != (*iterLevel)->GetSubLevels().end(); ++iterSubLevel)
			{
				m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(new CDlgConfigureDataSubLevel(pDivData->GetDivision(), *iterLevel, *iterSubLevel)),
					hItem, TVI_LAST);
			}
			m_ctrlLevels.Expand(hItem, TVE_EXPAND);
		}
	}
	if (pSubLevel)
	{
		FindCurrentSubLevel(pSubLevel, true);
		pSubLevel->Release();
	}
	else if (pLevel)
	{
		FindCurrentLevel(pLevel, true);
		pLevel->Release();
	}
}

void CDlgConfigVenue::LoadTitleData()
{
	ARBConfigTitle* pTitle = NULL;
	CDlgConfigureDataTitle* pTitleData = GetCurrentTitleData();
	if (pTitleData)
	{
		pTitle = pTitleData->GetTitle();
		pTitle->AddRef();
	}
	m_ctrlTitles.DeleteAllItems();
	CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
	if (!pDivData)
		return;
	for (ARBConfigTitleList::iterator iterTitle = pDivData->GetDivision()->GetTitles().begin();
	iterTitle != pDivData->GetDivision()->GetTitles().end();
	++iterTitle)
	{
		m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlTitles.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataTitle(pDivData->GetDivision(), *iterTitle)));
	}
	m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentTitle(pTitle, true);
	if (pTitle)
		pTitle->Release();
}

void CDlgConfigVenue::LoadEventData()
{
	ARBConfigEvent* pOldEvent = NULL;
	CDlgConfigureDataEvent* pEventData = GetCurrentEventData();
	if (pEventData)
	{
		pOldEvent = pEventData->GetEvent();
		pOldEvent->AddRef();
	}
	m_ctrlEvents.DeleteAllItems();
	for (ARBConfigEventList::iterator iterEvent = m_pVenue->GetEvents().begin(); iterEvent != m_pVenue->GetEvents().end(); ++iterEvent)
	{
		m_ctrlEvents.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlEvents.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(new CDlgConfigureDataEvent(*iterEvent)));
	}
	m_ctrlEvents.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentEvent(pOldEvent, true);
	if (pOldEvent)
		pOldEvent->Release();
}

int CDlgConfigVenue::FindCurrentDivision(ARBConfigDivision const* pDiv, bool bSet)
{
	int idxCurrent = -1;
	if (pDiv)
	{
		for (int index = 0; index < m_ctrlDivisions.GetItemCount(); ++index)
		{
			CDlgConfigureDataDivision* pData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
			if (pData->GetDivision() == pDiv)
			{
				idxCurrent = index;
				break;
			}
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

HTREEITEM CDlgConfigVenue::FindCurrentLevel(ARBConfigLevel const* pLevel, bool bSet)
{
	HTREEITEM hCurrent = NULL;
	if (pLevel)
	{
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

HTREEITEM CDlgConfigVenue::FindCurrentSubLevel(ARBConfigSubLevel const* pSubLevel, bool bSet)
{
	HTREEITEM hCurrent = NULL;
	if (pSubLevel)
	{
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

int CDlgConfigVenue::FindCurrentTitle(ARBConfigTitle const* pTitle, bool bSet)
{
	int idxCurrent = -1;
	if (pTitle)
	{
		for (int index = 0; index < m_ctrlTitles.GetItemCount(); ++index)
		{
			CDlgConfigureDataTitle* pData = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
			if (pData->GetTitle() == pTitle)
			{
				idxCurrent = index;
				break;
			}
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

int CDlgConfigVenue::FindCurrentEvent(ARBConfigEvent const* pEvent, bool bSet)
{
	int idxCurrent = -1;
	if (pEvent)
	{
		for (int index = 0; index < m_ctrlEvents.GetItemCount(); ++index)
		{
			CDlgConfigureDataEvent* pData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
			if (pData->GetEvent() == pEvent)
			{
				idxCurrent = index;
				break;
			}
		}
	}
	if (bSet)
	{
		m_ctrlEvents.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlEvents.EnsureVisible(idxCurrent, FALSE);
		UpdateButtons();
	}
	return idxCurrent;
}

CDlgConfigureDataDivision* CDlgConfigVenue::GetCurrentDivisionData()
{
	int index = m_ctrlDivisions.GetSelection();
	if (LB_ERR != index)
		return reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
	else
		return NULL;
}

CDlgConfigureDataLevel* CDlgConfigVenue::GetCurrentLevelData()
{
	HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
	if (NULL != hItem)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
		return dynamic_cast<CDlgConfigureDataLevel*>(pData);
	}
	else
		return NULL;
}

CDlgConfigureDataSubLevel* CDlgConfigVenue::GetCurrentSubLevelData()
{
	HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
	if (NULL != hItem)
	{
		CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
		return dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
	}
	else
		return NULL;
}

CDlgConfigureDataTitle* CDlgConfigVenue::GetCurrentTitleData()
{
	int index = m_ctrlTitles.GetSelection();
	if (0 <= index)
		return reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
	else
		return NULL;
}

CDlgConfigureDataEvent* CDlgConfigVenue::GetCurrentEventData()
{
	int index = m_ctrlEvents.GetSelection();
	if (0 <= index)
		return reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigVenue message handlers

BOOL CDlgConfigVenue::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlDivisions.SetExtendedStyle(m_ctrlDivisions.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlEvents.SetExtendedStyle(m_ctrlEvents.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlDivisions.InsertColumn(0, "Divisions");
	m_ctrlTitles.InsertColumn(0, "Titles");
	m_ctrlEvents.InsertColumn(0, "Events");

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
	CDlgBaseDialog::OnDestroy();
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

void CDlgConfigVenue::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
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
				name = dlg.GetName();
				if (m_pVenue->GetDivisions().FindDivision(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigDivision* pNewDiv = m_pVenue->GetDivisions().AddDivision(name);
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
			CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
			if (!pDivData)
				return;
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			CDlgConfigureDataLevel* pLevelData = GetCurrentLevelData();
			CDlgConfigureDataSubLevel* pSubLevelData = GetCurrentSubLevelData();
			UINT id = IDS_LEVEL_NAME;
			HTREEITEM hParentItem = TVI_ROOT;
			ARBConfigLevel* pLevel = NULL;
			if (pLevelData
			&& IDNO == AfxMessageBox("Would you like to create a Level? (Answer 'No' to create a sub-level)", MB_YESNO | MB_ICONQUESTION))
			{
				id = IDS_SUBLEVEL_NAME;
				hParentItem = hItem;
				pLevel = pLevelData->GetLevel();
			}
			else if (pSubLevelData)
			{
				id = IDS_SUBLEVEL_NAME;
				hParentItem = m_ctrlLevels.GetParentItem(hItem);
				pLevel = pSubLevelData->GetLevel();
			}

			while (!done)
			{
				done = true;
				CDlgName dlg(name.c_str(), id, this);
				if (IDOK == dlg.DoModal())
				{
					name = dlg.GetName();
					if (pDivData->GetDivision()->GetLevels().FindTrueLevel(name))
					{
						done = false;
						AfxMessageBox(IDS_NAME_IN_USE);
						continue;
					}
					if (IDS_LEVEL_NAME == id)
					{
						ARBConfigLevel* pNewLevel = pDivData->GetDivision()->GetLevels().AddLevel(name);
						if (pNewLevel)
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(new CDlgConfigureDataLevel(pDivData->GetDivision(), pNewLevel)),
								hParentItem, hItem);
							FindCurrentLevel(pNewLevel, true);
						}
					}
					else
					{
						ARBConfigSubLevel* pNewSubLevel = pLevel->GetSubLevels().AddSubLevel(name);
						if (pNewSubLevel)
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(new CDlgConfigureDataSubLevel(pDivData->GetDivision(), pLevel, pNewSubLevel)),
								hParentItem, hItem);
							FindCurrentSubLevel(pNewSubLevel, true);
						}
					}
				}
			}
		}
		break;

	case eTitles:
		{
			CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
			if (!pDivData)
				return;
			while (!done)
			{
				done = true;
				CDlgConfigTitle dlg(name.c_str(), "", "", this);
				if (IDOK == dlg.DoModal())
				{
					name = dlg.GetName();
					if (m_pVenue->GetDivisions().FindTitle(name))
					{
						done = false;
						AfxMessageBox(IDS_NAME_IN_USE);
						continue;
					}
					ARBConfigTitle* pTitle = pDivData->GetDivision()->GetTitles().AddTitle(name);
					pTitle->SetLongName(dlg.GetLongName());
					pTitle->SetDescription(dlg.GetDesc());
					if (pTitle)
					{
						int nInsertAt = m_ctrlTitles.GetSelection();
						if (0 > nInsertAt)
							nInsertAt = m_ctrlTitles.GetItemCount();
						else
							++nInsertAt;
						int index = m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, nInsertAt,
							LPSTR_TEXTCALLBACK, 0, 0, 0,
							reinterpret_cast<LPARAM>(new CDlgConfigureDataTitle(pDivData->GetDivision(), pTitle)));
						m_ctrlTitles.SetSelection(index);
						m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
						m_ctrlTitles.Invalidate();
					}
				}
			}
		}
		break;

	case eEvents:
		{
			// The dialog will ensure uniqueness.
			ARBConfigEvent* event = new ARBConfigEvent();
			CDlgConfigEvent dlg(m_pDoc, NULL, NULL, m_pVenue, event, this);
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
	bool bDelete = true;
	switch (m_Action)
	{
	case eDivisions:
		{
			int index;
			if (0 <= (index = m_ctrlDivisions.GetSelection()))
			{
				CDlgConfigureDataDivision* pDivData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
				ASSERT(NULL != pDivData);
				std::string div = pDivData->GetDivision()->GetName();
				int nPoints = m_Book.GetDogs().NumExistingPointsInDivision(m_pVenue, div);
				int nTrials = m_Book.GetDogs().NumRunsInDivision(m_pVenue, div);
				int nTitles = m_Book.GetDogs().NumTitlesInDivision(m_pVenue, div);
				if (0 < nTrials || 0 < nTitles)
				{
					CString msg;
					msg.FormatMessage(IDS_DELETE_DIVISION,
						div.c_str(),
						nPoints,
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
		}
		break;

	case eLevels:
		{
			HTREEITEM hItem;
			if (NULL != (hItem = m_ctrlLevels.GetSelectedItem()))
			{
				CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
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
		}
		break;

	case eTitles:
		{
			int index;
			if (0 <= (index = m_ctrlTitles.GetSelection()))
			{
				CDlgConfigureDataTitle* pTitleData = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
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
		}
		break;

	case eEvents:
		{
			int index;
			if (0 <= (index = m_ctrlEvents.GetSelection()))
			{
				CDlgConfigureDataEvent* pEventData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
				ASSERT(NULL != pEventData);
				std::string event = pEventData->GetEvent()->GetName();
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
						m_ctrlEvents.DeleteItem(index);
					}
				}
			}
		}
		break;
	}
}

void CDlgConfigVenue::OnEdit() 
{
	switch (m_Action)
	{
	case eDivisions:
		{
			CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
			if (!pDivData)
				return;
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
			CDlgConfigureDataLevel* pLevelData = GetCurrentLevelData();
			CDlgConfigureDataSubLevel* pSubLevelData = GetCurrentSubLevelData();
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
					CDlgName dlg(name.c_str(), IDS_SUBLEVEL_NAME, this);
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
			CDlgConfigureDataTitle* pTitleData = GetCurrentTitleData();
			if (!pTitleData)
				return;
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
					m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
					m_ctrlTitles.Invalidate();
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
			CDlgConfigureDataEvent* pEventData = GetCurrentEventData();
			if (!pEventData)
				return;
			ARBConfigEvent* pEvent = pEventData->GetEvent();
			CDlgConfigEvent dlg(m_pDoc, &m_Book, &m_Config, m_pVenue, pEvent, this);
			pEvent->AddRef();
			if (IDOK == dlg.DoModal())
			{
				dlg.GetFixups(m_DlgFixup);
				LoadEventData();
				FindCurrentEvent(pEvent, true);
			}
			pEvent->Release();
		}
		break;
	}
}

void CDlgConfigVenue::OnCopy() 
{
	switch (m_Action)
	{
	case eDivisions:
	case eLevels:
	case eTitles:
		// We don't currently support copying these.
		break;

	case eEvents:
		{
			int index;
			if (0 <= (index = m_ctrlEvents.GetSelection()))
			{
				// Since index is valid, I know pEventData will be too.
				CDlgConfigureDataEvent* pEventData = GetCurrentEventData();
				CString copyOf;
				copyOf.LoadString(IDS_COPYOF);
				std::string name(pEventData->GetEvent()->GetName());
				while (m_pVenue->GetEvents().FindEvent(name))
				{
					name = (LPCSTR)copyOf + name;
				}
				ARBConfigEvent* event = new ARBConfigEvent(*(pEventData->GetEvent()));
				event->SetName(name);
				ARBConfigEvent* pNewEvent = m_pVenue->GetEvents().AddEvent(event);
				if (pNewEvent)
				{
					LoadEventData();
					FindCurrentEvent(pNewEvent, true);
				}
				event->Release();
			}
		}
		break;
	}
}

void CDlgConfigVenue::OnMoveUp() 
{
	switch (m_Action)
	{
	case eDivisions:
		{
			int index = m_ctrlDivisions.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataDivision* pDivData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
				ARBConfigDivision* pDiv = pDivData->GetDivision();
				pDiv->AddRef();
				m_pVenue->GetDivisions().Move(pDiv, -1);
				LoadDivisionData();
				UpdateButtons();
				FindCurrentDivision(pDiv, true);
				pDiv->Release();
			}
		}
		break;
	case eLevels:
		{
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			HTREEITEM hPrevItem = m_ctrlLevels.GetPrevSiblingItem(hItem);
			if (NULL != hPrevItem)
			{
				CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
				CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
				CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
				ARBConfigLevel* pLevel = NULL;
				ARBConfigSubLevel* pSubLevel = NULL;
				if (pLevelData)
				{
					pLevel = pLevelData->GetLevel();
					pLevel->AddRef();
					pLevelData->GetDivision()->GetLevels().Move(pLevel, -1);
				}
				else if (pSubLevelData)
				{
					pSubLevel = pSubLevelData->GetSubLevel();
					pSubLevel->AddRef();
					pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevel, -1);
				}
				LoadLevelData();
				UpdateButtons();
				if (pLevel)
				{
					FindCurrentLevel(pLevel, true);
					pLevel->Release();
				}
				else if (pSubLevel)
				{
					FindCurrentSubLevel(pSubLevel, true);
					pSubLevel->Release();
				}
			}
		}
		break;
	case eTitles:
		{
			int index = m_ctrlTitles.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataTitle* pTitleData = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
				ARBConfigTitle* pTitle = pTitleData->GetTitle();
				pTitle->AddRef();
				pTitleData->GetDivision()->GetTitles().Move(pTitle, -1);
				LoadTitleData();
				UpdateButtons();
				FindCurrentTitle(pTitle, true);
				pTitle->Release();
			}
		}
		break;
	case eEvents:
		{
			int index = m_ctrlEvents.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataEvent* pEventData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
				ARBConfigEvent* pEvent = pEventData->GetEvent();
				pEvent->AddRef();
				m_pVenue->GetEvents().Move(pEvent, -1);
				LoadEventData();
				UpdateButtons();
				FindCurrentEvent(pEvent, true);
				pEvent->Release();
			}
		}
		break;
	}
}

void CDlgConfigVenue::OnMoveDown() 
{
	switch (m_Action)
	{
	case eDivisions:
		{
			int index = m_ctrlDivisions.GetSelection();
			if (index < m_ctrlDivisions.GetItemCount() - 1)
			{
				CDlgConfigureDataDivision* pDivData = reinterpret_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetItemData(index));
				ARBConfigDivision* pDiv = pDivData->GetDivision();
				pDiv->AddRef();
				m_pVenue->GetDivisions().Move(pDiv, 1);
				LoadDivisionData();
				UpdateButtons();
				FindCurrentDivision(pDiv, true);
				pDiv->Release();
			}
		}
		break;
	case eLevels:
		{
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			HTREEITEM hNextItem = m_ctrlLevels.GetNextSiblingItem(hItem);
			if (NULL != hNextItem)
			{
				CDlgConfigureData* pData = reinterpret_cast<CDlgConfigureData*>(m_ctrlLevels.GetItemData(hItem));
				CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
				CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
				ARBConfigLevel* pLevel = NULL;
				ARBConfigSubLevel* pSubLevel = NULL;
				if (pLevelData)
				{
					pLevel = pLevelData->GetLevel();
					pLevel->AddRef();
					pLevelData->GetDivision()->GetLevels().Move(pLevel, 1);
				}
				else if (pSubLevelData)
				{
					pSubLevel = pSubLevelData->GetSubLevel();
					pSubLevel->AddRef();
					pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevel, 1);
				}
				LoadLevelData();
				UpdateButtons();
				if (pLevel)
				{
					FindCurrentLevel(pLevel, true);
					pLevel->Release();
				}
				else if (pSubLevel)
				{
					FindCurrentSubLevel(pSubLevel, true);
					pSubLevel->Release();
				}
			}
		}
		break;
	case eTitles:
		{
			int index = m_ctrlTitles.GetSelection();
			if (index < m_ctrlTitles.GetItemCount() - 1)
			{
				CDlgConfigureDataTitle* pTitleData = reinterpret_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetItemData(index));
				ARBConfigTitle* pTitle = pTitleData->GetTitle();
				pTitle->AddRef();
				pTitleData->GetDivision()->GetTitles().Move(pTitle, 1);
				LoadTitleData();
				UpdateButtons();
				FindCurrentTitle(pTitle, true);
				pTitle->Release();
			}
		}
		break;
	case eEvents:
		{
			int index = m_ctrlEvents.GetSelection();
			if (index < m_ctrlEvents.GetItemCount() - 1)
			{
				CDlgConfigureDataEvent* pEventData = reinterpret_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetItemData(index));
				ARBConfigEvent* pEvent = pEventData->GetEvent();
				pEvent->AddRef();
				m_pVenue->GetEvents().Move(pEvent, 1);
				LoadEventData();
				UpdateButtons();
				FindCurrentEvent(pEvent, true);
				pEvent->Release();
			}
		}
		break;
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

	CDlgBaseDialog::OnOK();
}
