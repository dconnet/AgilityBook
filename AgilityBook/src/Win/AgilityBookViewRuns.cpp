/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief implementation of the CAgilityBookViewRuns class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookViewRuns.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBTypes.h"
#include "MainFrm.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// These columns must agree with CAgilityBookViewRunsData.
static const struct
{
	UINT col;
	UINT fmt;
} scColumns[] = {
	{IDS_COL_Q, LVCFMT_CENTER},
	{IDS_COL_PTS, LVCFMT_RIGHT},
	{IDS_COL_DATE, LVCFMT_LEFT},
	{IDS_COL_VENUE, LVCFMT_LEFT},
	{IDS_COL_EVENT, LVCFMT_LEFT},
	{IDS_COL_DIVISION, LVCFMT_LEFT},
	{IDS_COL_LEVEL, LVCFMT_LEFT},
	{IDS_COL_JUDGE, LVCFMT_LEFT},
	//SCT,Time,YPS,[DQ/Mach],Score
	{IDS_COL_PLACE, LVCFMT_CENTER},
	{IDS_COL_IN_CLASS, LVCFMT_CENTER},
	{IDS_COL_Q_D, LVCFMT_CENTER},
	{IDS_COL_NOTES, LVCFMT_LEFT},
};
static const int scNumColumns = sizeof(scColumns) / sizeof(scColumns[0]);
#define COL_Q			0
#define COL_Points		1
#define COL_Date		2
#define COL_Venue		3
#define COL_Event		4
#define COL_Division	5
#define COL_Level		6
#define COL_Judge		7
#define COL_Place		8
#define COL_InClass		9
#define COL_Qd			10
#define COL_Notes		11

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRunsData

class CAgilityBookViewRunsData
{
public:
	CAgilityBookViewRunsData(CAgilityBookViewRuns* pView, ARBDogTrial* pTrial, ARBDogRun* pRun)
		: m_RefCount(1)
		, m_pView(pView)
		, m_pTrial(pTrial)
		, m_pRun(pRun)
	{
	}

	void AddRef();
	void Release();

	ARBDogTrial* GetTrial()		{return m_pTrial;}
	ARBDogRun* GetRun()			{return m_pRun;}
	CString OnNeedText(int iCol) const;

protected:
	~CAgilityBookViewRunsData()		{}
	UINT m_RefCount;
	CAgilityBookViewRuns* m_pView;
	ARBDogTrial* m_pTrial;
	ARBDogRun* m_pRun;
};

void CAgilityBookViewRunsData::AddRef()
{
	++m_RefCount;
}

void CAgilityBookViewRunsData::Release()
{
	--m_RefCount;
	if (0 == m_RefCount)
		delete this;
}

CString CAgilityBookViewRunsData::OnNeedText(int iCol) const
{
	short val;
	CString str;
	if (m_pRun)
	{
		switch (iCol)
		{
		default:
			break;
		case COL_Q:
			str = m_pRun->GetQ().str().c_str();
			if (m_pRun->GetQ().Qualified())
			{
				int nQs = m_pTrial->NumQs(
					m_pRun->GetDate(),
					m_pView->GetDocument()->GetConfig(),
					m_pRun->GetDivision(),
					m_pRun->GetLevel());
				if (2 == nQs)
					str.LoadString(IDS_QQ);
				if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					str.LoadString(IDS_SQ);
			}
			break;
		case COL_Points:
			{
				short pts = 0;
				if (m_pRun->GetQ().Qualified())
				{
					const ARBConfigScoring* pScoring = m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel());
					if (pScoring)
						pts = m_pRun->GetTitlePoints(pScoring);
				}
				str.Format("%hd", pts);
			}
			break;
		case COL_Date:
			str = m_pRun->GetDate().GetString(false, true).c_str();
			break;
		case COL_Venue:
			str = m_pTrial->GetClubs().GetPrimaryClub()->GetVenue().c_str();
			break;
		case COL_Event:
			str = m_pRun->GetEvent().c_str();
			break;
		case COL_Division:
			str = m_pRun->GetDivision().c_str();
			break;
		case COL_Level:
			str = m_pRun->GetLevel().c_str();
			break;
		case COL_Judge:
			str = m_pRun->GetJudge().c_str();
			break;
		case COL_Place:
			val = m_pRun->GetPlace();
			if (0 > val)
				str = "?";
			else if (0 == val)
				str = "-";
			else
				str.Format("%hd", val);
			break;
		case COL_InClass:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str = "?";
			else
				str.Format("%hd", val);
			break;
		case COL_Qd:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str = "?";
			else
				str.Format("%hd", m_pRun->GetDogsQd());
			break;
		case COL_Notes:
			str = m_pRun->GetNote().c_str();
			str.Replace("\n", " ");
			break;
		}
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRuns

IMPLEMENT_DYNCREATE(CAgilityBookViewRuns, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewRuns, CListView2)
	//{{AFX_MSG_MAP(CAgilityBookViewRuns)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_RUN, OnUpdateAgilityEditRun)
	ON_COMMAND(ID_AGILITY_EDIT_RUN, OnAgilityEditRun)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_RUN, OnUpdateAgilityDeleteRun)
	ON_COMMAND(ID_AGILITY_DELETE_RUN, OnAgilityDeleteRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewRuns construction/destruction

CAgilityBookViewRuns::CAgilityBookViewRuns()
{
}

CAgilityBookViewRuns::~CAgilityBookViewRuns()
{
}

BOOL CAgilityBookViewRuns::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_NOSORTHEADER;
	return CListView2::PreCreateWindow(cs);
}

int CAgilityBookViewRuns::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < scNumColumns; ++i)
	{
		CString str;
		str.LoadString(scColumns[i].col);
		col.fmt = scColumns[i].fmt;
		col.pszText = str.GetBuffer(0);
		col.iSubItem = i;
		GetListCtrl().InsertColumn(i, &col);
		str.ReleaseBuffer();
	}

	return 0;
}

void CAgilityBookViewRuns::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg);
}

void CAgilityBookViewRuns::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (0 == lHint || (UPDATE_RUNS_VIEW & lHint) || (UPDATE_OPTIONS & lHint) || (UPDATE_CONFIG & lHint))
		LoadData();
}

#ifdef _DEBUG
// CAgilityBookViewRuns diagnostics
void CAgilityBookViewRuns::AssertValid() const
{
	CListView2::AssertValid();
}

void CAgilityBookViewRuns::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewRuns::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewRuns::GetPrintLine(int nItem, CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewRuns::GetMessage(CString& msg) const
{
	int nQs = 0;
	for (int index = 0; index < GetListCtrl().GetItemCount(); ++index)
	{
		CAgilityBookViewRunsData* pData = GetItemData(index);
		if (pData && pData->GetRun() && pData->GetRun()->GetQ().Qualified())
			++nQs;
	}
	msg.FormatMessage(IDS_NUM_RUNS_QS, GetListCtrl().GetItemCount(), nQs);
	return true;
}

CAgilityBookViewRunsData* CAgilityBookViewRuns::GetItemData(int index) const
{
	CAgilityBookViewRunsData *pData = NULL;
	if (0 <= index && index < GetListCtrl().GetItemCount())
		pData = reinterpret_cast<CAgilityBookViewRunsData*>(GetListCtrl().GetItemData(index));
	return pData;
}

void CAgilityBookViewRuns::LoadData()
{
	CWaitCursor wait;

	// Remember what's selected.
	CAgilityBookViewRunsData* pCurData = GetItemData(GetSelection());
	if (pCurData)
		pCurData->AddRef();

	// Reduce flicker.
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Add items.
	std::vector<CVenueFilter> venues;
	CAgilityBookOptions::GetFilterVenue(venues);
	list<ARBDogTrial*> trials;
	ARBDogTrial* pTrial = GetDocument()->GetCurrentTrial();
	if (pTrial)
		trials.push_back(pTrial);
	else
	{
		ARBDog* pDog = GetDocument()->GetCurrentDog();
		if (pDog)
		{
			for (ARBDogTrialList::iterator iter = pDog->GetTrials().begin();
			iter != pDog->GetTrials().end();
			++iter)
			{
				ARBDogTrial* pTrial = (*iter);
				if (CAgilityBookOptions::IsTrialVisible(venues, pTrial))
					trials.push_back(pTrial);
			}
		}
	}
	int nQs = 0;
	if (0 < trials.size())
	{
		int i = 0;
		for (list<ARBDogTrial*>::iterator iter = trials.begin(); iter != trials.end(); ++iter)
		{
			ARBDogTrial* pTrial = (*iter);
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++i, ++iterRun)
			{
				ARBDogRun* pRun = (*iterRun);
				if (!CAgilityBookOptions::IsRunVisible(venues, pTrial, pRun))
					continue;
				if (pRun->GetQ().Qualified())
					++nQs;
				CAgilityBookViewRunsData* pData = new CAgilityBookViewRunsData(this, pTrial, pRun);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iItem = i;
				item.iSubItem = 0;
				item.lParam = reinterpret_cast<LPARAM>(pData);
				int index = GetListCtrl().InsertItem(&item);
				if (pCurData && pCurData->GetRun() && *(pCurData->GetRun()) == *pRun)
				{
					SetSelection(index, true);
				}
			}
		}
	}
	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (GetMessage(msg) && IsWindowVisible())
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg);

	// Cleanup.
	if (pCurData)
		pCurData->Release();
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRuns message handlers

void CAgilityBookViewRuns::OnDestroy() 
{
	GetListCtrl().DeleteAllItems();
	CListView2::OnDestroy();
}

void CAgilityBookViewRuns::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewRuns::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CListView2::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
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

void CAgilityBookViewRuns::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int index = GetSelection();
	if (0 > index)
		return;
	CAgilityBookViewRunsData* pData = GetItemData(index);
	if (!pData)
		return;
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		GetListCtrl().GetItemRect(index, &rect, FALSE);
		point.x = rect.left + rect.Width() / 3;
		point.y = rect.top + rect.Height() / 2;
		ClientToScreen(&point);
	}
	UINT idMenu = IDR_RUNS;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CAgilityBookViewRuns::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CAgilityBookViewRuns::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	GetDocument()->EditRun(pData->GetRun());
	*pResult = 0;
}

void CAgilityBookViewRuns::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		{
			CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
			GetDocument()->EditRun(pData->GetRun());
		}
		break;
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnUpdateAgilityEditRun(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityEditRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->EditRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityDeleteRun(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityDeleteRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->DeleteRun(pData->GetRun());
}
