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
 * @brief implementation of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2004-07-29 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgListViewer.h"

#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBDate.h"
#include "ARBDogExistingPoints.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerData
{
public:
	CDlgListViewerData(CListCtrl& list) : m_List(list), m_RefCount(1) {}
	void AddRef() {++m_RefCount;}
	void Release()
	{
		--m_RefCount;
		if (0 == m_RefCount)
			delete this;
	}
	CString OnNeedText(int iCol);
protected:
	~CDlgListViewerData() {}
	CListCtrl& m_List;
	UINT m_RefCount;
};

CString CDlgListViewerData::OnNeedText(int iCol)
{
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListViewer dialog

// Viewing runs
CDlgListViewer::CDlgListViewer(CString const& inCaption, std::list<ARBDogRun const*> const& inRuns, CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_Caption(inCaption)
	, m_Runs(&inRuns)
	, m_DoubleQData(NULL)
	, m_OtherData(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
{
	//{{AFX_DATA_INIT(CDlgListViewer)
	//}}AFX_DATA_INIT
}

// Viewing double-Qs
CDlgListViewer::CDlgListViewer(CString const& inCaption, std::set<DoubleQdata> const& inQQs, CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_Caption(inCaption)
	, m_Runs(NULL)
	, m_DoubleQData(&inQQs)
	, m_OtherData(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
{
}

// Viewing other points
CDlgListViewer::CDlgListViewer(CString const& inCaption, std::list<OtherPtInfo> const& inRunList, CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_Caption(inCaption)
	, m_Runs(NULL)
	, m_DoubleQData(NULL)
	, m_OtherData(&inRunList)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
{
}

void CDlgListViewer::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListViewer)
	DDX_Control(pDX, IDC_LIST_VIEWER, m_ctrlList);
	DDX_Control(pDX, IDOK, m_ctrlClose);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgListViewer, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgListViewer)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_VIEWER, OnGetdispinfoList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_VIEWER, OnDeleteitemList)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgListViewer message handlers

BOOL CDlgListViewer::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlList.SetExtendedStyle(m_ctrlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);	// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);	// Set small icon

	GetWindowRect(m_rWin);
	GetClientRect(m_rDlg);
	m_ctrlList.GetWindowRect(m_rList);
	ScreenToClient(m_rList);
	m_ctrlClose.GetWindowRect(m_rOK);
	ScreenToClient(m_rOK);

	SetWindowText(m_Caption);
	if (m_Runs)
	{
		m_ctrlList.InsertColumn(0, "Date");
		m_ctrlList.InsertColumn(1, "Judge");
		m_ctrlList.InsertColumn(2, "Q");
		m_ctrlList.InsertColumn(3, "Place");
		m_ctrlList.InsertColumn(4, "In Class");
		m_ctrlList.InsertColumn(5, "Q'd");
		bool bPartnerHdr = false;
		int iItem = 0;
		for (std::list<ARBDogRun const*>::const_iterator iter = m_Runs->begin();
			iter != m_Runs->end();
			++iter)
		{
			m_ctrlList.InsertItem(iItem, (*iter)->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
			m_ctrlList.SetItemText(iItem, 1, (*iter)->GetJudge().c_str());
			m_ctrlList.SetItemText(iItem, 2, (*iter)->GetQ().str().c_str());
			CString str;
			str.Format("%hd", (*iter)->GetPlace());
			m_ctrlList.SetItemText(iItem, 3, str);
			str.Format("%hd", (*iter)->GetInClass());
			m_ctrlList.SetItemText(iItem, 4, str);
			str.Format("%hd", (*iter)->GetDogsQd());
			m_ctrlList.SetItemText(iItem, 5, str);
			if (0 < (*iter)->GetPartners().size())
			{
				if (!bPartnerHdr)
				{
					bPartnerHdr = true;
					m_ctrlList.InsertColumn(6, "Partners");
				}
				str.Empty();
				for (ARBDogRunPartnerList::const_iterator iter2 = (*iter)->GetPartners().begin();
					iter2 != (*iter)->GetPartners().end();
					++iter2)
				{
					if (!str.IsEmpty())
						str += ", ";
					str += (*iter2)->GetHandler().c_str();
					str += "/";
					str += (*iter2)->GetDog().c_str();
				}
				m_ctrlList.SetItemText(iItem, 6, str);
			}
			++iItem;
		}
		for (int i = 0; i <= 6; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	else if (m_DoubleQData)
	{
		m_ctrlList.InsertColumn(0, "Date");
		m_ctrlList.InsertColumn(1, "Trial");
		m_ctrlList.InsertColumn(2, "Club");
		int iItem = 0;
		for (std::set<DoubleQdata>::const_iterator iter = m_DoubleQData->begin();
			iter != m_DoubleQData->end();
			++iter)
		{
			m_ctrlList.InsertItem(iItem, iter->first.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
			m_ctrlList.SetItemText(iItem, 1, iter->second->GetLocation().c_str());
			m_ctrlList.SetItemText(iItem, 2, iter->second->GetClubs().GetPrimaryClub()->GetName().c_str());
			++iItem;
		}
		for (int i = 0; i <= 2; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	else if (m_OtherData)
	{
		m_ctrlList.InsertColumn(0, "Date");
		m_ctrlList.InsertColumn(1, "Trial");
		m_ctrlList.InsertColumn(2, "Club");
		m_ctrlList.InsertColumn(3, "Venue");
		m_ctrlList.InsertColumn(4, "Division");
		m_ctrlList.InsertColumn(5, "Level");
		m_ctrlList.InsertColumn(6, "Event");
		m_ctrlList.InsertColumn(7, "Points");
		int iItem = 0;
		for (std::list<OtherPtInfo>::const_iterator iter = m_OtherData->begin();
			iter != m_OtherData->end();
			++iter)
		{
			const OtherPtInfo& info = *iter;
			if (info.m_pExisting)
			{
				m_ctrlList.InsertItem(iItem, info.m_pExisting->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
				m_ctrlList.SetItemText(iItem, 1, "Existing Points");
			}
			else
			{
				m_ctrlList.InsertItem(iItem, info.m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
				m_ctrlList.SetItemText(iItem, 1, info.m_pTrial->GetLocation().c_str());
				m_ctrlList.SetItemText(iItem, 2, info.m_pTrial->GetClubs().GetPrimaryClub()->GetName().c_str());
			}
			m_ctrlList.SetItemText(iItem, 3, info.m_Venue.c_str());
			m_ctrlList.SetItemText(iItem, 4, info.m_Div.c_str());
			m_ctrlList.SetItemText(iItem, 5, info.m_Level.c_str());
			m_ctrlList.SetItemText(iItem, 6, info.m_Event.c_str());
			CString str;
			str.Format("%d", info.m_Score);
			m_ctrlList.SetItemText(iItem, 7, str);
			++iItem;
		}
		for (int i = 0; i <= 7; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgListViewer::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CDlgListViewerData *pData = reinterpret_cast<CDlgListViewerData*>(pDispInfo->item.lParam);
		if (pData)
		{
			CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
		else
			pDispInfo->item.pszText[0] = '\0';
	}
	*pResult = 0;
}

void CDlgListViewer::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CDlgListViewerData *pData = reinterpret_cast<CDlgListViewerData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CDlgListViewer::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rWin.Width();
	lpMMI->ptMinTrackSize.y = m_rWin.Height();
	CDlgBaseDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgListViewer::OnSize(UINT nType, int cx, int cy)
{
	CDlgBaseDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ctrlList.GetSafeHwnd()))
	{
		m_ctrlClose.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rOK.left), cy - (m_rDlg.Height() - m_rOK.bottom) - m_rOK.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlList.SetWindowPos(NULL,
			0, 0,
			cx - (m_rDlg.Width() - m_rList.Width()), cy - (m_rDlg.Height() - m_rList.Height()),
			SWP_NOZORDER | SWP_NOMOVE);
	}
}
