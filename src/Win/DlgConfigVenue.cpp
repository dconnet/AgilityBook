/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-12-10 DRC Enable duplication of a title.
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
#include "ARBConfigVenue.h"
#include "DlgConfigEvent.h"
#include "DlgConfigMultiQ.h"
#include "DlgConfigTitle.h"
#include "DlgConfigureData.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigVenue dialog

CDlgConfigVenue::CDlgConfigVenue(
		ARBAgilityRecordBook const& book,
		ARBConfig const& config,
		ARBConfigVenuePtr pVenue,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigVenue::IDD, pParent)
	, m_ctrlDivisions(true)
	, m_ctrlEvents(true)
	, m_ctrlMultiQ(true)
	, m_ctrlTitles(true)
	, m_Book(book)
	, m_Config(config)
	, m_pVenueOrig(pVenue)
	, m_pVenue(pVenue->Clone())
	, m_Action(eNone)
{
	ASSERT(m_pVenueOrig);
	ASSERT(m_pVenue);
	//{{AFX_DATA_INIT(CDlgConfigVenue)
	//}}AFX_DATA_INIT
}


CDlgConfigVenue::~CDlgConfigVenue()
{
	m_DlgFixup.clear();
}


void CDlgConfigVenue::GetFixups(ARBConfigActionList& ioFixups)
{
	ioFixups.insert(ioFixups.end(), m_DlgFixup.begin(), m_DlgFixup.end());
	m_DlgFixup.clear();
}


void CDlgConfigVenue::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigVenue)
	DDX_Control(pDX, IDC_CONFIG_VENUE, m_ctrlName);
	DDX_Control(pDX, IDC_CONFIG_VENUE_LONGNAME, m_ctrlLongName);
	DDX_Control(pDX, IDC_CONFIG_VENUE_URL, m_ctrlURL);
	DDX_Control(pDX, IDC_CONFIG_VENUE_DESC, m_ctrlDesc);
	DDX_Control(pDX, IDC_CONFIG_VENUE_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_CONFIG_VENUE_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_CONFIG_VENUE_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_CONFIG_VENUE_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_CONFIG_VENUE_MULTIQ, m_ctrlMultiQ);
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
	ON_NOTIFY(TVN_DELETEITEM, IDC_CONFIG_VENUE_LEVEL, OnDeleteitemTree)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_DIVISION, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUE_DIVISION, OnKeydown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_DIVISION, OnItemchangedDivision)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_TITLES, OnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_EVENT, OnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_VENUE_MULTIQ, OnItemchangedList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIG_VENUE_LEVEL, OnItemchanged)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_DIVISION, OnSetfocusDivision)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_LEVEL, OnSetfocusLevel)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_TITLES, OnSetfocusTitles)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_EVENT, OnSetfocusEvent)
	ON_NOTIFY(NM_SETFOCUS, IDC_CONFIG_VENUE_MULTIQ, OnSetfocusMultiQ)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_NEW, OnNew)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_COPY, OnCopy)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_CONFIG_VENUE_MOVE_DOWN, OnMoveDown)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_TITLES, OnGetdispinfoList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_EVENT, OnGetdispinfoList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_VENUE_MULTIQ, OnGetdispinfoList)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_LEVEL, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUE_LEVEL, OnKeydown)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_TITLES, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUE_TITLES, OnKeydown)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_EVENT, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUE_EVENT, OnKeydown)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VENUE_MULTIQ, OnDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_VENUE_MULTIQ, OnKeydown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigVenue::SetAction(eAction inAction)
{
	if (m_Action != inAction)
	{
		m_Action = inAction;
		CString str(_T(""));
		switch (m_Action)
		{
		case eDivisions:
			str.LoadString(IDS_BUTTONS_DIVISIONS);
			break;
		case eLevels:
			str.LoadString(IDS_BUTTONS_LEVELS);
			break;
		case eTitles:
			str.LoadString(IDS_BUTTONS_TITLES);
			break;
		case eEvents:
			str.LoadString(IDS_BUTTONS_EVENTS);
			break;
		case eMultiQ:
			str.LoadString(IDS_BUTTONS_MULTIQS);
			break;
		}
		m_ctrlComments.SetWindowText(str);
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
			bNew = TRUE;
			int index = m_ctrlTitles.GetSelection();
			if (0 <= index)
			{
				bDelete = bEdit = TRUE;
				CDlgConfigureDataTitle* pData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
				if (pData)
				{
					bCopy = TRUE;
					if (0 < index)
						bMoveUp = TRUE;
					if (index < m_ctrlTitles.GetItemCount() - 1)
						bMoveDown = TRUE;
				}
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
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
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

	case eMultiQ:
		{
			bNew = TRUE;
			int index = m_ctrlMultiQ.GetSelection();
			if (0 <= index)
			{
				bDelete = bEdit = TRUE;
				CDlgConfigureDataMultiQ* pData = dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
				if (pData)
				{
					bCopy = TRUE;
					if (0 < index)
						bMoveUp = TRUE;
					if (index < m_ctrlMultiQ.GetItemCount() - 1)
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
	ARBConfigDivisionPtr pDiv;
	CDlgConfigureDataDivision* pDivData = GetCurrentDivisionData();
	if (pDivData)
		pDiv = pDivData->GetDivision();
	m_ctrlDivisions.DeleteAllItems();
	for (ARBConfigDivisionList::iterator iterDiv = m_pVenue->GetDivisions().begin(); iterDiv != m_pVenue->GetDivisions().end(); ++iterDiv)
	{
		m_ctrlDivisions.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlDivisions.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataDivision(*iterDiv))));
	}
	m_ctrlDivisions.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentDivision(pDiv, true);
	LoadLevelData();
}


void CDlgConfigVenue::LoadLevelData()
{
	ARBConfigLevelPtr pLevel;
	ARBConfigSubLevelPtr pSubLevel;
	CDlgConfigureDataLevel* pLevelData = GetCurrentLevelData();
	CDlgConfigureDataSubLevel* pSubLevelData = GetCurrentSubLevelData();
	if (pLevelData)
	{
		pLevel = pLevelData->GetLevel();
	}
	else if (pSubLevelData)
	{
		pLevel = pSubLevelData->GetLevel();
		pSubLevel = pSubLevelData->GetSubLevel();
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
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataLevel(pDivData->GetDivision(), *iterLevel))),
			TVI_ROOT, TVI_LAST);
		if (0 < (*iterLevel)->GetSubLevels().size())
		{
			for (ARBConfigSubLevelList::iterator iterSubLevel = (*iterLevel)->GetSubLevels().begin(); iterSubLevel != (*iterLevel)->GetSubLevels().end(); ++iterSubLevel)
			{
				m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
					0, 0, 0, 0,
					reinterpret_cast<LPARAM>(
						static_cast<CListData*>(
							new CDlgConfigureDataSubLevel(pDivData->GetDivision(), *iterLevel, *iterSubLevel))),
					hItem, TVI_LAST);
			}
			m_ctrlLevels.Expand(hItem, TVE_EXPAND);
		}
	}
	if (pSubLevel)
	{
		FindCurrentSubLevel(pSubLevel, true);
	}
	else if (pLevel)
	{
		FindCurrentLevel(pLevel, true);
	}
}


void CDlgConfigVenue::LoadTitleData()
{
	ARBConfigTitlePtr pTitle;
	CDlgConfigureDataTitle* pTitleData = GetCurrentTitleData();
	if (pTitleData)
		pTitle = pTitleData->GetTitle();
	m_ctrlTitles.DeleteAllItems();
	for (ARBConfigTitleList::iterator iterTitle = m_pVenue->GetTitles().begin();
		iterTitle != m_pVenue->GetTitles().end();
		++iterTitle)
	{
		m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlTitles.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataTitle(*iterTitle))));
	}
	m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentTitle(pTitle, true);
}


void CDlgConfigVenue::LoadEventData()
{
	ARBConfigEventPtr pOldEvent;
	CDlgConfigureDataEvent* pEventData = GetCurrentEventData();
	if (pEventData)
		pOldEvent = pEventData->GetEvent();
	m_ctrlEvents.DeleteAllItems();
	for (ARBConfigEventList::iterator iterEvent = m_pVenue->GetEvents().begin(); iterEvent != m_pVenue->GetEvents().end(); ++iterEvent)
	{
		m_ctrlEvents.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlEvents.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataEvent(*iterEvent))));
	}
	m_ctrlEvents.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentEvent(pOldEvent, true);
}


void CDlgConfigVenue::LoadMultiQData()
{
	ARBConfigMultiQPtr pOldMultiQ;
	CDlgConfigureDataMultiQ* pData = GetCurrentMultiQData();
	if (pData)
		pOldMultiQ = pData->GetMultiQ();
	m_ctrlMultiQ.DeleteAllItems();
	for (ARBConfigMultiQList::iterator iter = m_pVenue->GetMultiQs().begin();
		iter != m_pVenue->GetMultiQs().end();
		++iter)
	{
		m_ctrlMultiQ.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlMultiQ.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CDlgConfigureDataMultiQ(*iter))));
	}
	m_ctrlMultiQ.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	FindCurrentMultiQ(pOldMultiQ, true);
}


int CDlgConfigVenue::FindCurrentDivision(
		ARBConfigDivisionPtr pDiv,
		bool bSet)
{
	int idxCurrent = -1;
	if (pDiv)
	{
		for (int index = 0; index < m_ctrlDivisions.GetItemCount(); ++index)
		{
			CDlgConfigureDataDivision* pData = dynamic_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetData(index));
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


HTREEITEM CDlgConfigVenue::FindCurrentLevel(
		ARBConfigLevelPtr pLevel,
		bool bSet)
{
	HTREEITEM hCurrent = NULL;
	if (pLevel)
	{
		HTREEITEM hItem = m_ctrlLevels.GetRootItem();
		while (NULL != hItem)
		{
			CListData* pRawData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
			CDlgConfigureDataLevel* pData = dynamic_cast<CDlgConfigureDataLevel*>(pRawData);
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


HTREEITEM CDlgConfigVenue::FindCurrentSubLevel(
		ARBConfigSubLevelPtr pSubLevel,
		bool bSet)
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
					CListData* pRawData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hChildItem));
					CDlgConfigureDataSubLevel* pData = dynamic_cast<CDlgConfigureDataSubLevel*>(pRawData);
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


int CDlgConfigVenue::FindCurrentTitle(
		ARBConfigTitlePtr pTitle,
		bool bSet)
{
	int idxCurrent = -1;
	if (pTitle)
	{
		for (int index = 0; index < m_ctrlTitles.GetItemCount(); ++index)
		{
			CDlgConfigureDataTitle* pData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
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


int CDlgConfigVenue::FindCurrentEvent(
		ARBConfigEventPtr pEvent,
		bool bSet)
{
	int idxCurrent = -1;
	if (pEvent)
	{
		for (int index = 0; index < m_ctrlEvents.GetItemCount(); ++index)
		{
			CDlgConfigureDataEvent* pData = dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
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


int CDlgConfigVenue::FindCurrentMultiQ(
		ARBConfigMultiQPtr pMultiQ,
		bool bSet)
{
	int idxCurrent = -1;
	if (pMultiQ)
	{
		for (int index = 0; index < m_ctrlMultiQ.GetItemCount(); ++index)
		{
			CDlgConfigureDataMultiQ* pData = dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
			if (pData->GetMultiQ() == pMultiQ)
			{
				idxCurrent = index;
				break;
			}
		}
	}
	if (bSet)
	{
		m_ctrlMultiQ.SetSelection(idxCurrent);
		if (0 < idxCurrent)
			m_ctrlMultiQ.EnsureVisible(idxCurrent, FALSE);
		UpdateButtons();
	}
	return idxCurrent;
}


CDlgConfigureDataDivision* CDlgConfigVenue::GetCurrentDivisionData()
{
	int index = m_ctrlDivisions.GetSelection();
	if (LB_ERR != index)
		return dynamic_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetData(index));
	else
		return NULL;
}


CDlgConfigureDataLevel* CDlgConfigVenue::GetCurrentLevelData()
{
	HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
	if (NULL != hItem)
	{
		CListData* pData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
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
		CListData* pData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
		return dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
	}
	else
		return NULL;
}


CDlgConfigureDataTitle* CDlgConfigVenue::GetCurrentTitleData()
{
	int index = m_ctrlTitles.GetSelection();
	if (0 <= index)
		return dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
	else
		return NULL;
}


CDlgConfigureDataEvent* CDlgConfigVenue::GetCurrentEventData()
{
	int index = m_ctrlEvents.GetSelection();
	if (0 <= index)
		return dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
	else
		return NULL;
}


CDlgConfigureDataMultiQ* CDlgConfigVenue::GetCurrentMultiQData()
{
	int index = m_ctrlMultiQ.GetSelection();
	if (0 <= index)
		return dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
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
	m_ctrlMultiQ.SetExtendedStyle(m_ctrlMultiQ.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	CString col;
	col.LoadString(IDS_COL_DIVISIONS);
	m_ctrlDivisions.InsertColumn(0, col);
	col.LoadString(IDS_COL_TITLES);
	m_ctrlTitles.InsertColumn(0, col);
	col.LoadString(IDS_COL_EVENTS);
	m_ctrlEvents.InsertColumn(0, col);
	col.LoadString(IDS_COL_MULTIQ);
	m_ctrlMultiQ.InsertColumn(0, col);

	m_ctrlName.SetWindowText(m_pVenue->GetName().c_str());
	m_ctrlLongName.SetWindowText(m_pVenue->GetLongName().c_str());
	m_ctrlURL.SetWindowText(m_pVenue->GetURL().c_str());
	CString str(m_pVenue->GetDesc().c_str());
	str.Replace(_T("\n"), _T("\r\n"));
	m_ctrlDesc.SetWindowText(str);

	LoadDivisionData();
	LoadEventData();
	LoadTitleData();
	LoadMultiQData();
	SetAction(eDivisions);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgConfigVenue::OnDestroy() 
{
	m_ctrlLevels.DeleteAllItems();
	CDlgBaseDialog::OnDestroy();
}


void CDlgConfigVenue::OnGetdispinfoList(
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


void CDlgConfigVenue::OnGetdispinfoTree(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	if (pTVDispInfo->item.mask & TVIF_TEXT)
	{
		CListData* pRawData = reinterpret_cast<CListData*>(pTVDispInfo->item.lParam);
		CDlgConfigureData* pData = dynamic_cast<CDlgConfigureData*>(pRawData);
		if (pData)
		{
			CString str = pData->OnNeedText(0);
			::lstrcpyn(pTVDispInfo->item.pszText, str, pTVDispInfo->item.cchTextMax);
			pTVDispInfo->item.pszText[pTVDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}


void CDlgConfigVenue::OnDeleteitemTree(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	CListData* pData = reinterpret_cast<CListData*>(pNMTreeView->itemOld.lParam);
	delete pData;
	pNMTreeView->itemOld.lParam = 0;
	*pResult = 0;
}


void CDlgConfigVenue::OnDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}


void CDlgConfigVenue::OnKeydown(
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


void CDlgConfigVenue::OnItemchangedDivision(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LoadLevelData();
	UpdateButtons();
	*pResult = 0;
}


void CDlgConfigVenue::OnItemchangedList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}


void CDlgConfigVenue::OnItemchanged(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}


void CDlgConfigVenue::OnSetfocusDivision(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eDivisions);
	*pResult = 0;
}


void CDlgConfigVenue::OnSetfocusLevel(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eLevels);
	*pResult = 0;
}


void CDlgConfigVenue::OnSetfocusTitles(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eTitles);
	*pResult = 0;
}


void CDlgConfigVenue::OnSetfocusEvent(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eEvents);
	*pResult = 0;
}


void CDlgConfigVenue::OnSetfocusMultiQ(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	SetAction(eMultiQ);
	*pResult = 0;
}


void CDlgConfigVenue::OnNew() 
{
	bool done = false;
	tstring name;
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
				ARBConfigDivisionPtr pNewDiv;
				if (m_pVenue->GetDivisions().AddDivision(name, &pNewDiv))
				{
					int nInsertAt = m_ctrlDivisions.GetSelection();
					if (0 > nInsertAt)
						nInsertAt = m_ctrlDivisions.GetItemCount();
					else
						++nInsertAt;
					int index = m_ctrlDivisions.InsertItem(LVIF_TEXT | LVIF_PARAM, nInsertAt,
						LPSTR_TEXTCALLBACK, 0, 0, 0,
						reinterpret_cast<LPARAM>(
							static_cast<CListData*>(
								new CDlgConfigureDataDivision(pNewDiv))));
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
			ARBConfigLevelPtr pLevel;
			if (pLevelData
			&& IDNO == AfxMessageBox(IDS_CREATE_LEVEL, MB_YESNO | MB_ICONQUESTION))
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
					if (pDivData->GetDivision()->GetLevels().FindSubLevel(name))
					{
						done = false;
						AfxMessageBox(IDS_NAME_IN_USE);
						continue;
					}
					if (IDS_LEVEL_NAME == id)
					{
						ARBConfigLevelPtr pNewLevel;
						if (pDivData->GetDivision()->GetLevels().AddLevel(name, &pNewLevel))
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(
									static_cast<CListData*>(
										new CDlgConfigureDataLevel(pDivData->GetDivision(), pNewLevel))),
								hParentItem, hItem);
							FindCurrentLevel(pNewLevel, true);
						}
					}
					else
					{
						ARBConfigSubLevelPtr pNewSubLevel;
						if (pLevel->GetSubLevels().AddSubLevel(name, &pNewSubLevel))
						{
							m_ctrlLevels.InsertItem(TVIF_TEXT | TVIF_PARAM, LPSTR_TEXTCALLBACK,
								0, 0, 0, 0,
								reinterpret_cast<LPARAM>(
									static_cast<CListData*>(
										new CDlgConfigureDataSubLevel(pDivData->GetDivision(), pLevel, pNewSubLevel))),
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
			while (!done)
			{
				done = true;
				ARBConfigTitlePtr title(ARBConfigTitle::New());
				title->SetName(name);
				CDlgConfigTitle dlg(title, this);
				if (IDOK == dlg.DoModal())
				{
					name = title->GetName();
					if (m_pVenue->GetTitles().FindTitle(name))
					{
						done = false;
						AfxMessageBox(IDS_NAME_IN_USE);
						continue;
					}
					ARBConfigTitlePtr pTitle;
					if (m_pVenue->GetTitles().AddTitle(name, &pTitle))
					{
						*pTitle = *title;
						//pTitle->SetMultiple(dlg.GetMultiple());
						//pTitle->SetLongName(dlg.GetLongName());
						//pTitle->SetDescription(dlg.GetDesc());
						int nInsertAt = m_ctrlTitles.GetSelection();
						if (0 > nInsertAt)
							nInsertAt = m_ctrlTitles.GetItemCount();
						else
							++nInsertAt;
						int index = m_ctrlTitles.InsertItem(LVIF_TEXT | LVIF_PARAM, nInsertAt,
							LPSTR_TEXTCALLBACK, 0, 0, 0,
							reinterpret_cast<LPARAM>(
								static_cast<CListData*>(
									new CDlgConfigureDataTitle(pTitle))));
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
			ARBConfigEventPtr pEvent(ARBConfigEvent::New());
			CDlgConfigEvent dlg(true, m_pVenue, pEvent, this);
			if (IDOK == dlg.DoModal())
			{
				if (m_pVenue->GetEvents().AddEvent(pEvent))
				{
					LoadEventData();
					FindCurrentEvent(pEvent, true);
				}
			}
		}
		break;

	case eMultiQ:
		{
			// The dialog will ensure uniqueness.
			ARBConfigMultiQPtr multiq(ARBConfigMultiQ::New());
			CDlgConfigMultiQ dlg(m_pVenue, multiq, this);
			if (IDOK == dlg.DoModal())
			{
				if (m_pVenue->GetMultiQs().AddMultiQ(multiq))
				{
					LoadMultiQData();
					FindCurrentMultiQ(multiq, true);
				}
			}
		}
		break;
	}
}


void CDlgConfigVenue::OnDelete() 
{
	switch (m_Action)
	{
	case eDivisions:
		{
			int index;
			if (0 <= (index = m_ctrlDivisions.GetSelection()))
			{
				CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetData(index));
				ASSERT(NULL != pDivData);
				tstring div = pDivData->GetDivision()->GetName();
				// If we were able to delete it...
				if (m_pVenue->GetDivisions().DeleteDivision(div, m_pVenue->GetEvents()))
				{
					m_pVenue->GetMultiQs().DeleteDivision(div);
					m_DlgFixup.push_back(ARBConfigActionDeleteDivision::New(m_pVenue->GetName(), div));
					m_ctrlDivisions.DeleteItem(index);
				}
			}
		}
		break;

	case eLevels:
		{
			HTREEITEM hItem;
			if (NULL != (hItem = m_ctrlLevels.GetSelectedItem()))
			{
				CListData* pData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
				CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
				CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
				if (pLevelData)
				{
					tstring level = pLevelData->GetLevel()->GetName();
					if (pLevelData->GetDivision()->GetLevels().DeleteLevel(level, m_pVenue->GetEvents()))
					{
						m_pVenue->GetMultiQs().DeleteLevel(level);
						m_DlgFixup.push_back(ARBConfigActionDeleteLevel::NewLevel(
							m_pVenue->GetName(),
							pLevelData->GetDivision()->GetName(),
							level));
						m_ctrlLevels.DeleteItem(hItem);
					}
				}
				else if (pSubLevelData)
				{
					tstring level = pSubLevelData->GetLevel()->GetName();
					tstring subLevel = pSubLevelData->GetSubLevel()->GetName();
					bool bLevelModified = false;
					if (pSubLevelData->GetDivision()->GetLevels().DeleteSubLevel(subLevel, bLevelModified))
					{
						m_pVenue->GetMultiQs().DeleteLevel(subLevel);
						// Note, if deleting the sublevel caused the level's name
						// to change, just leave it. It causes more trouble to
						// try modifing the name to the old sublevel name.
						m_DlgFixup.push_back(ARBConfigActionDeleteLevel::NewSubLevel(
							m_pVenue->GetName(),
							pSubLevelData->GetDivision()->GetName(),
							level, subLevel));
						m_ctrlLevels.DeleteItem(hItem);
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
				CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
				ASSERT(NULL != pTitleData);
				tstring title = pTitleData->GetTitle()->GetName();
				if (m_pVenue->GetTitles().DeleteTitle(title))
				{
					m_DlgFixup.push_back(ARBConfigActionDeleteTitle::New(m_pVenue->GetName(), tstring(), title, tstring()));
					m_ctrlTitles.DeleteItem(index);
				}
			}
		}
		break;

	case eEvents:
		{
			int index;
			if (0 <= (index = m_ctrlEvents.GetSelection()))
			{
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
				ASSERT(NULL != pEventData);
				tstring evt = pEventData->GetEvent()->GetName();
				if (m_pVenue->GetEvents().DeleteEvent(evt))
				{
					m_pVenue->GetMultiQs().DeleteEvent(evt);
					m_DlgFixup.push_back(ARBConfigActionDeleteEvent::New(m_pVenue->GetName(), evt));
					m_ctrlEvents.DeleteItem(index);
				}
			}
		}
		break;

	case eMultiQ:
		{
			int index;
			if (0 <= (index = m_ctrlMultiQ.GetSelection()))
			{
				CDlgConfigureDataMultiQ* pData = dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
				ASSERT(NULL != pData);
				tstring multiQ = pData->GetMultiQ()->GetName();
				if (m_pVenue->GetMultiQs().DeleteMultiQ(pData->GetMultiQ()))
				{
					m_DlgFixup.push_back(ARBConfigActionDeleteMultiQ::New(m_pVenue->GetName(), multiQ));
					m_ctrlMultiQ.DeleteItem(index);
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
			tstring oldName = pDivData->GetDivision()->GetName();
			tstring name(oldName);
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
						m_pVenue->GetMultiQs().RenameDivision(oldName, name);
						m_DlgFixup.push_back(ARBConfigActionRenameDivision::New(m_pVenue->GetName(), oldName, name));
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
				tstring oldName = pLevelData->GetLevel()->GetName();
				tstring name(oldName);
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
								if (pLevelData->GetDivision()->GetLevels().FindSubLevel(name))
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
							{
								m_pVenue->GetEvents().RenameLevel(oldName, name);
								m_pVenue->GetMultiQs().RenameLevel(
									pLevelData->GetDivision()->GetName(),
									oldName, name);
							}
							m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewLevel(
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
				tstring oldName = pSubLevelData->GetSubLevel()->GetName();
				tstring name(oldName);
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
							if (pSubLevelData->GetDivision()->GetLevels().FindSubLevel(name))
							{
								done = false;
								AfxMessageBox(IDS_NAME_IN_USE);
								continue;
							}
							pSubLevelData->GetSubLevel()->SetName(name);
							// No need to fix ARBConfigEventList cause we don't do sublevel names in events.
							m_pVenue->GetMultiQs().RenameLevel(
								pSubLevelData->GetDivision()->GetName(),
								oldName, pSubLevelData->GetSubLevel()->GetName());
							m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewSubLevel(
								m_pVenue->GetName(),
								pSubLevelData->GetDivision()->GetName(),
								pSubLevelData->GetLevel()->GetName(),
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
			tstring oldName = pTitleData->GetTitle()->GetName();
			tstring oldLongName = pTitleData->GetTitle()->GetLongName();
			tstring name(oldName);
			tstring longname(oldLongName);
			while (!done)
			{
				done = true;
				ARBConfigTitlePtr title = pTitleData->GetTitle()->Clone();
				CDlgConfigTitle dlg(title, this);
				if (IDOK == dlg.DoModal())
				{
					name = title->GetName();
					longname = title->GetLongName();
					if (oldName != name)
					{
						if (m_pVenue->GetTitles().FindTitle(name))
						{
							int nTitles = m_Book.GetDogs().NumTitlesInUse(m_pVenue->GetName(), oldName);
							bool bInUse = true;
							if (0 < nTitles)
							{
								if (IDYES == AfxMessageBox(IDS_NAME_IN_USE_MERGE, MB_YESNO | MB_ICONQUESTION))
								{
									bInUse = false;
									m_DlgFixup.push_back(ARBConfigActionRenameTitle::New(m_pVenue->GetName(), oldName, name));
									if (m_pVenue->GetTitles().DeleteTitle(oldName))
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
						title->SetName(name);
					}
					*(pTitleData->GetTitle()) = *title;
					m_ctrlTitles.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
					m_ctrlTitles.Invalidate();
					if (name != oldName || longname != oldLongName)
					{
						if (name != oldName)
							m_DlgFixup.push_back(ARBConfigActionRenameTitle::New(m_pVenue->GetName(), oldName, name));
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
			ARBConfigEventPtr pEvent = pEventData->GetEvent();
			tstring oldName = pEvent->GetName();
			CDlgConfigEvent dlg(false, m_pVenue, pEvent, this);
			if (IDOK == dlg.DoModal())
			{
				m_pVenue->GetMultiQs().RenameEvent(oldName, pEvent->GetName());
				dlg.GetFixups(m_DlgFixup);
				LoadEventData();
				FindCurrentEvent(pEvent, true);
			}
		}
		break;

	case eMultiQ:
		{
			CDlgConfigureDataMultiQ* pData = GetCurrentMultiQData();
			if (!pData)
				return;
			ARBConfigMultiQPtr pMultiQ = pData->GetMultiQ();
			tstring oldName = pMultiQ->GetName();
			bool done = false;
			while (!done)
			{
				done = true;
				CDlgConfigMultiQ dlg(m_pVenue, pMultiQ, this);
				if (IDOK == dlg.DoModal())
				{
					tstring name = pMultiQ->GetName();
					if (name != oldName)
						m_DlgFixup.push_back(ARBConfigActionRenameMultiQ::New(m_pVenue->GetName(), oldName, name));
					LoadMultiQData();
					FindCurrentMultiQ(pMultiQ, true);
				}
			}
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
		// We don't currently support copying these.
		break;

	case eTitles:
		{
			int index;
			if (0 <= (index = m_ctrlTitles.GetSelection()))
			{
				// Since index is valid, I know pData will be too.
				CDlgConfigureDataTitle* pData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
				tstring name(pData->GetTitle()->GetName());
				tstring longname(pData->GetTitle()->GetLongName());
				while (m_pVenue->GetTitles().FindTitle(name))
				{
					CString copyOf;
					copyOf.FormatMessage(IDS_COPYOF, name.c_str());
					name = (LPCTSTR)copyOf;
					copyOf.FormatMessage(IDS_COPYOF, longname.c_str());
					longname = (LPCTSTR)copyOf;
				}
				ARBConfigTitlePtr title = pData->GetTitle()->Clone();
				title->SetName(name);
				title->SetLongName(longname);
				if (m_pVenue->GetTitles().AddTitle(title))
				{
					LoadTitleData();
					FindCurrentTitle(title, true);
				}
			}
		}
		break;

	case eEvents:
		{
			int index;
			if (0 <= (index = m_ctrlEvents.GetSelection()))
			{
				// Since index is valid, I know pEventData will be too.
				CDlgConfigureDataEvent* pEventData = GetCurrentEventData();
				tstring name(pEventData->GetEvent()->GetName());
				while (m_pVenue->GetEvents().FindEvent(name))
				{
					CString copyOf;
					copyOf.FormatMessage(IDS_COPYOF, name.c_str());
					name = (LPCTSTR)copyOf;
				}
				ARBConfigEventPtr pEvent = pEventData->GetEvent()->Clone();
				pEvent->SetName(name);
				if (m_pVenue->GetEvents().AddEvent(pEvent))
				{
					LoadEventData();
					FindCurrentEvent(pEvent, true);
				}
			}
		}
		break;

	case eMultiQ:
		{
			int index;
			if (0 <= (index = m_ctrlMultiQ.GetSelection()))
			{
				// Since index is valid, I know pData will be too.
				CDlgConfigureDataMultiQ* pData = GetCurrentMultiQData();
				tstring name(pData->GetMultiQ()->GetName());
				while (m_pVenue->GetMultiQs().FindMultiQ(name))
				{
					CString copyOf;
					copyOf.FormatMessage(IDS_COPYOF, name.c_str());
					name = (LPCTSTR)copyOf;
				}
				ARBConfigMultiQPtr multiq = pData->GetMultiQ()->Clone();
				multiq->SetName(name);
				if (m_pVenue->GetMultiQs().AddMultiQ(multiq))
				{
					LoadMultiQData();
					FindCurrentMultiQ(multiq, true);
				}
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
				CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetData(index));
				ARBConfigDivisionPtr pDiv = pDivData->GetDivision();
				m_pVenue->GetDivisions().Move(pDiv, -1);
				LoadDivisionData();
				UpdateButtons();
				FindCurrentDivision(pDiv, true);
			}
		}
		break;
	case eLevels:
		{
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			HTREEITEM hPrevItem = m_ctrlLevels.GetPrevSiblingItem(hItem);
			if (NULL != hPrevItem)
			{
				CListData* pData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
				CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
				CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
				ARBConfigLevelPtr pLevel;
				ARBConfigSubLevelPtr pSubLevel;
				if (pLevelData)
				{
					pLevel = pLevelData->GetLevel();
					pLevelData->GetDivision()->GetLevels().Move(pLevel, -1);
				}
				else if (pSubLevelData)
				{
					pSubLevel = pSubLevelData->GetSubLevel();
					pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevel, -1);
				}
				LoadLevelData();
				UpdateButtons();
				if (pLevel)
				{
					FindCurrentLevel(pLevel, true);
				}
				else if (pSubLevel)
				{
					FindCurrentSubLevel(pSubLevel, true);
				}
			}
		}
		break;
	case eTitles:
		{
			int index = m_ctrlTitles.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
				ARBConfigTitlePtr pTitle = pTitleData->GetTitle();
				m_pVenue->GetTitles().Move(pTitle, -1);
				LoadTitleData();
				UpdateButtons();
				FindCurrentTitle(pTitle, true);
			}
		}
		break;
	case eEvents:
		{
			int index = m_ctrlEvents.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
				ARBConfigEventPtr pEvent = pEventData->GetEvent();
				m_pVenue->GetEvents().Move(pEvent, -1);
				LoadEventData();
				UpdateButtons();
				FindCurrentEvent(pEvent, true);
			}
		}
		break;
	case eMultiQ:
		{
			int index = m_ctrlMultiQ.GetSelection();
			if (0 < index)
			{
				CDlgConfigureDataMultiQ* pData = dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
				ARBConfigMultiQPtr pMultiQ = pData->GetMultiQ();
				m_pVenue->GetMultiQs().Move(pMultiQ, -1);
				LoadMultiQData();
				UpdateButtons();
				FindCurrentMultiQ(pMultiQ, true);
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
				CDlgConfigureDataDivision* pDivData = dynamic_cast<CDlgConfigureDataDivision*>(m_ctrlDivisions.GetData(index));
				ARBConfigDivisionPtr pDiv = pDivData->GetDivision();
				m_pVenue->GetDivisions().Move(pDiv, 1);
				LoadDivisionData();
				UpdateButtons();
				FindCurrentDivision(pDiv, true);
			}
		}
		break;
	case eLevels:
		{
			HTREEITEM hItem = m_ctrlLevels.GetSelectedItem();
			HTREEITEM hNextItem = m_ctrlLevels.GetNextSiblingItem(hItem);
			if (NULL != hNextItem)
			{
				CListData* pData = reinterpret_cast<CListData*>(m_ctrlLevels.GetItemData(hItem));
				CDlgConfigureDataLevel* pLevelData = dynamic_cast<CDlgConfigureDataLevel*>(pData);
				CDlgConfigureDataSubLevel* pSubLevelData = dynamic_cast<CDlgConfigureDataSubLevel*>(pData);
				ARBConfigLevelPtr pLevel;
				ARBConfigSubLevelPtr pSubLevel;
				if (pLevelData)
				{
					pLevel = pLevelData->GetLevel();
					pLevelData->GetDivision()->GetLevels().Move(pLevel, 1);
				}
				else if (pSubLevelData)
				{
					pSubLevel = pSubLevelData->GetSubLevel();
					pSubLevelData->GetLevel()->GetSubLevels().Move(pSubLevel, 1);
				}
				LoadLevelData();
				UpdateButtons();
				if (pLevel)
				{
					FindCurrentLevel(pLevel, true);
				}
				else if (pSubLevel)
				{
					FindCurrentSubLevel(pSubLevel, true);
				}
			}
		}
		break;
	case eTitles:
		{
			int index = m_ctrlTitles.GetSelection();
			if (index < m_ctrlTitles.GetItemCount() - 1)
			{
				CDlgConfigureDataTitle* pTitleData = dynamic_cast<CDlgConfigureDataTitle*>(m_ctrlTitles.GetData(index));
				ARBConfigTitlePtr pTitle = pTitleData->GetTitle();
				m_pVenue->GetTitles().Move(pTitle, 1);
				LoadTitleData();
				UpdateButtons();
				FindCurrentTitle(pTitle, true);
			}
		}
		break;
	case eEvents:
		{
			int index = m_ctrlEvents.GetSelection();
			if (index < m_ctrlEvents.GetItemCount() - 1)
			{
				CDlgConfigureDataEvent* pEventData = dynamic_cast<CDlgConfigureDataEvent*>(m_ctrlEvents.GetData(index));
				ARBConfigEventPtr pEvent = pEventData->GetEvent();
				m_pVenue->GetEvents().Move(pEvent, 1);
				LoadEventData();
				UpdateButtons();
				FindCurrentEvent(pEvent, true);
			}
		}
		break;
	case eMultiQ:
		{
			int index = m_ctrlMultiQ.GetSelection();
			if (index < m_ctrlMultiQ.GetItemCount() - 1)
			{
				CDlgConfigureDataMultiQ* pData = dynamic_cast<CDlgConfigureDataMultiQ*>(m_ctrlMultiQ.GetData(index));
				ARBConfigMultiQPtr pMultiQ = pData->GetMultiQ();
				m_pVenue->GetMultiQs().Move(pMultiQ, 1);
				LoadMultiQData();
				UpdateButtons();
				FindCurrentMultiQ(pMultiQ, true);
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
		AfxMessageBox(IDS_INVALID_NAME);
		GotoDlgCtrl(&m_ctrlName);
		return;
	}
	tstring name((LPCTSTR)str);
	tstring oldName = m_pVenue->GetName();
	if (oldName != name)
	{
		if (m_Config.GetVenues().FindVenue(name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			return;
		}
		m_pVenue->SetName(name);
	}

	m_ctrlLongName.GetWindowText(str);
	m_pVenue->SetLongName((LPCTSTR)str);

	m_ctrlURL.GetWindowText(str);
	str.Replace(_T("\""), _T(""));
#if _MSC_VER >= 1300
	str.Trim();
#else
	str.TrimRight();
	str.TrimLeft();
#endif
	m_pVenue->SetURL((LPCTSTR)str);

	m_ctrlDesc.GetWindowText(str);
	str.TrimRight();
	str.Replace(_T("\r\n"), _T("\n"));
	m_pVenue->SetDesc((LPCTSTR)str);
	if (oldName != name)
		m_DlgFixup.push_back(ARBConfigActionRenameVenue::New(oldName, name));

	// Push the copy back.
	*m_pVenueOrig = *m_pVenue;

	// The rest is already taken care of.

	CDlgBaseDialog::OnOK();
}
