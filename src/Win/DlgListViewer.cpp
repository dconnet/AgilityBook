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

#include "AgilityBookDoc.h"
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
	CDlgListViewerData() : m_RefCount(1) {}
	void AddRef() {++m_RefCount;}
	void Release()
	{
		--m_RefCount;
		if (0 == m_RefCount)
			delete this;
	}
	virtual CString OnNeedText(int iCol) const = 0;
	virtual int Compare(CDlgListViewerData const* pRow2, int inCol) const = 0;
protected:
	virtual ~CDlgListViewerData() {}
	UINT m_RefCount;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataRun : public CDlgListViewerData
{
public:
	CDlgListViewerDataRun() {}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(CDlgListViewerData const* pRow2, int inCol) const;
};

CString CDlgListViewerDataRun::OnNeedText(int iCol) const
{
	CString str;
	//switch (iCol)
	//{
	//case 0: // Q
	//case 1: // Title Points
	//case 2: // Date
	//case 3: // Location
	//case 4: // Club
	//case 5: // Judge
	//case 6: // Place
	//case 7: // In Class
	//case 8: // Qd
	//case 9: // Mach or Partners
	//	break;
	//}
	return str;
}

int CDlgListViewerDataRun::Compare(CDlgListViewerData const* pRow2, int inCol) const
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataDblQ : public CDlgListViewerData
{
public:
	CDlgListViewerDataDblQ(ARBDate const& inDate, ARBDogTrial const* inTrial)
		: m_Date(inDate)
		, m_Trial(inTrial)
	{
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(CDlgListViewerData const* pRow2, int inCol) const;
private:
	ARBDate m_Date;
	ARBDogTrial const* m_Trial;
};

CString CDlgListViewerDataDblQ::OnNeedText(int iCol) const
{
	CString str;
	switch (iCol)
	{
	case 0: // Date
		str = m_Date.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case 1: // Location
		str = m_Trial->GetLocation().c_str();
		break;
	case 2: // Club
		str = m_Trial->GetClubs().GetPrimaryClub()->GetName().c_str();
		break;
	}
	return str;
}

int CDlgListViewerDataDblQ::Compare(CDlgListViewerData const* pRow2, int inCol) const
{
	CDlgListViewerDataDblQ const* pData = dynamic_cast<CDlgListViewerDataDblQ const*>(pRow2);
	if (!pData)
		return 0;
	std::string str1, str2;
	switch (inCol)
	{
	default:
	case 0:
		if (m_Date < pData->m_Date)
			return -1;
		else if (m_Date > pData->m_Date)
			return 1;
		else
			return 0;
	case 1:
		str1 = m_Trial->GetLocation();
		str2 = pData->m_Trial->GetLocation();
		break;
	case 2:
		str1 = m_Trial->GetClubs().GetPrimaryClub()->GetName();
		str2 = pData->m_Trial->GetClubs().GetPrimaryClub()->GetName();
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataOther : public CDlgListViewerData
{
public:
	CDlgListViewerDataOther(OtherPtInfo const& info)
		: m_info(info)
	{
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(CDlgListViewerData const* pRow2, int inCol) const;
private:
	OtherPtInfo m_info;
};

CString CDlgListViewerDataOther::OnNeedText(int iCol) const
{
	CString str;
	switch (iCol)
	{
	case 0: // Date
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		else
			str = m_info.m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case 1: // Trial/Existing Points
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetOtherPoints().c_str();
		else
			str = m_info.m_pTrial->GetLocation().c_str();
		break;
	case 2: // Club
		if (!m_info.m_pExisting)
			str = m_info.m_pTrial->GetClubs().GetPrimaryClub()->GetName().c_str();
		break;
	case 3: // Venue
		str = m_info.m_Venue.c_str();
		break;
	case 4: // Division
		str = m_info.m_Div.c_str();
		break;
	case 5: // Level
		str = m_info.m_Level.c_str();
		break;
	case 6: // Event
		str = m_info.m_Event.c_str();
		break;
	case 7: // Points
		str.Format("%d", m_info.m_Score);
		break;
	}
	return str;
}

int CDlgListViewerDataOther::Compare(CDlgListViewerData const* pRow2, int inCol) const
{
	CDlgListViewerDataOther const* pData = dynamic_cast<CDlgListViewerDataOther const*>(pRow2);
	if (!pData)
		return 0;
	std::string str1, str2;
	switch (inCol)
	{
	default:
	case 0:
		{
			ARBDate date1, date2;
			if (m_info.m_pExisting)
				date1 = m_info.m_pExisting->GetDate();
			else
				date1 = m_info.m_pRun->GetDate();
			if (m_info.m_pExisting)
				date2 = pData->m_info.m_pExisting->GetDate();
			else
				date2 = pData->m_info.m_pRun->GetDate();
			if (date1 < date2)
				return -1;
			else if (date1 > date2)
				return 1;
			else
				return 0;
		}
	case 1: // Trial/Existing Points
		if (m_info.m_pExisting)
			str1 = m_info.m_pExisting->GetOtherPoints();
		else
			str1 = m_info.m_pTrial->GetLocation();
		if (pData->m_info.m_pExisting)
			str2 = pData->m_info.m_pExisting->GetOtherPoints();
		else
			str2 = pData->m_info.m_pTrial->GetLocation();
		break;
	case 2: // Club
		if (!m_info.m_pExisting)
			str1 = m_info.m_pTrial->GetClubs().GetPrimaryClub()->GetName();
		if (!pData->m_info.m_pExisting)
			str2 = pData->m_info.m_pTrial->GetClubs().GetPrimaryClub()->GetName();
		break;
	case 3: // Venue
		str1 = m_info.m_Venue;
		str2 = pData->m_info.m_Venue;
		break;
	case 4: // Division
		str1 = m_info.m_Div;
		str2 = pData->m_info.m_Div;
		break;
	case 5: // Level
		str1 = m_info.m_Level;
		str2 = pData->m_info.m_Level;
		break;
	case 6: // Event
		str1 = m_info.m_Event;
		str2 = pData->m_info.m_Event;
		break;
	case 7:
		if (m_info.m_Score < pData->m_info.m_Score)
			return -1;
		else if (m_info.m_Score > pData->m_info.m_Score)
			return 1;
		else
			return 0;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListViewer dialog

struct SORT_COL_INFO
{
	CDlgListViewer *pThis;
	int nCol;
};

int CALLBACK CompareRows(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	SORT_COL_INFO* sortInfo = reinterpret_cast<SORT_COL_INFO*>(lParam3);
	if (!sortInfo)
		return 0;
	CDlgListViewerData* pRow1 = reinterpret_cast<CDlgListViewerData*>(lParam1);
	CDlgListViewerData* pRow2 = reinterpret_cast<CDlgListViewerData*>(lParam2);
	if (!pRow1 || !pRow2)
		return 0;
	int iCol = abs(sortInfo->nCol);
	int nRet = pRow1->Compare(pRow2, iCol - 1);
	if (0 > sortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Viewing runs
CDlgListViewer::CDlgListViewer(CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::list<RunInfo> const& inRuns,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_Runs(&inRuns)
	, m_DoubleQData(NULL)
	, m_OtherData(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
	//{{AFX_DATA_INIT(CDlgListViewer)
	//}}AFX_DATA_INIT
}

// Viewing double-Qs
CDlgListViewer::CDlgListViewer(CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::set<DoubleQdata> const& inQQs,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_Runs(NULL)
	, m_DoubleQData(&inQQs)
	, m_OtherData(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

// Viewing other points
CDlgListViewer::CDlgListViewer(CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::list<OtherPtInfo> const& inRunList,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_Runs(NULL)
	, m_DoubleQData(NULL)
	, m_OtherData(&inRunList)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
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
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_VIEWER, OnColumnclickList)
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
		int idxCol = 0;
		m_ctrlList.InsertColumn(idxCol++, "Q");
//		int idxColQ = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Title Points");
		int idxColPts = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Date");
		int idxColDate = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Location");
		int idxColLoc = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Club");
		int idxColClub = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Judge");
		int idxColJudge = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Place");
		int idxColPlace = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "In Class");
		int idxColInClass = idxCol - 1;
		m_ctrlList.InsertColumn(idxCol++, "Q'd");
		int idxColQd = idxCol - 1;
		int idxColPartner = -1;
		int idxColMach = -1;
		bool bMachHdr = false;
		bool bPartnerHdr = false;
		int iItem = 0;
		for (std::list<RunInfo>::const_iterator iter = m_Runs->begin();
			iter != m_Runs->end();
			++iter)
		{
			ARBDogTrial const* pTrial = iter->first;
			ARBDogRun const* pRun = iter->second;
			if (CAgilityBookOptions::IsFilterEnabled())
			{
				if (pRun->IsFiltered())
					continue;
			}

			ARBConfigScoring const* pScoring = m_pDoc->GetConfig().GetVenues().FindEvent(
				pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
				pRun->GetEvent(),
				pRun->GetDivision(),
				pRun->GetLevel(),
				pRun->GetDate());

			m_ctrlList.InsertItem(iItem, pRun->GetQ().str().c_str());

			CString str;
			short pts = 0;
			if (pRun->GetQ().Qualified() && pScoring)
				pts = pRun->GetTitlePoints(pScoring);
			str.Format("%hd", pts);
			m_ctrlList.SetItemText(iItem, idxColPts, str);

			m_ctrlList.SetItemText(iItem, idxColDate,
				pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());

			m_ctrlList.SetItemText(iItem, idxColLoc,
				pTrial->GetLocation().c_str());

			m_ctrlList.SetItemText(iItem, idxColClub,
				pTrial->GetClubs().GetPrimaryClub()->GetName().c_str());

			m_ctrlList.SetItemText(iItem, idxColJudge,
				pRun->GetJudge().c_str());

			str.Format("%hd", pRun->GetPlace());
			m_ctrlList.SetItemText(iItem, idxColPlace, str);

			str.Format("%hd", pRun->GetInClass());
			m_ctrlList.SetItemText(iItem, idxColInClass, str);

			str.Format("%hd", pRun->GetDogsQd());
			m_ctrlList.SetItemText(iItem, idxColQd, str);

			if (pScoring && pScoring->HasMachPts())
			{
				if (!bMachHdr)
				{
					bMachHdr = true;
					m_ctrlList.InsertColumn(idxCol++, "Mach Points");
					idxColMach = idxCol - 1;
				}
				str.Format("%hd", pRun->GetMachPoints(pScoring));
				m_ctrlList.SetItemText(iItem, idxColMach, str);
			}

			if (0 < pRun->GetPartners().size())
			{
				if (!bPartnerHdr)
				{
					bPartnerHdr = true;
					m_ctrlList.InsertColumn(idxCol++, "Partners");
					idxColPartner = idxCol - 1;
				}
				str.Empty();
				for (ARBDogRunPartnerList::const_iterator iter2 = pRun->GetPartners().begin();
					iter2 != pRun->GetPartners().end();
					++iter2)
				{
					if (!str.IsEmpty())
						str += ", ";
					str += (*iter2)->GetHandler().c_str();
					str += "/";
					str += (*iter2)->GetDog().c_str();
				}
				m_ctrlList.SetItemText(iItem, idxColPartner, str);
			}

			++iItem;
		}
		for (int i = 0; i < idxCol; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	else if (m_DoubleQData)
	{
		m_ctrlList.InsertColumn(0, "Date");
		m_ctrlList.InsertColumn(1, "Location");
		m_ctrlList.InsertColumn(2, "Club");
		int iItem = 0;
		for (std::set<DoubleQdata>::const_iterator iter = m_DoubleQData->begin();
			iter != m_DoubleQData->end();
			++iter)
		{
			CDlgListViewerDataDblQ* pData = new CDlgListViewerDataDblQ(iter->first, iter->second);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(pData);
			m_ctrlList.InsertItem(&item);
		}
		for (int i = 0; i <= 2; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	else if (m_OtherData)
	{
		m_ctrlList.InsertColumn(0, "Date");
		m_ctrlList.InsertColumn(1, "Trial / Existing Pts");
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
			OtherPtInfo const& info = *iter;
			CDlgListViewerDataOther* pData = new CDlgListViewerDataOther(info);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(pData);
			m_ctrlList.InsertItem(&item);
		}
		for (int i = 0; i <= 7; ++i)
			m_ctrlList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	SORT_COL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	m_ctrlList.SortItems(CompareRows, reinterpret_cast<LPARAM>(&info));
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1, CHeaderCtrl2::eAscending);

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

void CDlgListViewer::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1, CHeaderCtrl2::eNoSort);
	int nBackwards = 1;
	if (m_SortColumn == pNMListView->iSubItem + 1)
		nBackwards = -1;
	m_SortColumn = (pNMListView->iSubItem + 1) * nBackwards;
	SORT_COL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	m_ctrlList.SortItems(CompareRows, reinterpret_cast<LPARAM>(&info));
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1,
		nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
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
