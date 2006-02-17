/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgFindLinks class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-02 DRC Added 'Open' button.
 * @li 2004-03-31 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgFindLinks.h"

#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "CheckLink.h"
#include "DlgSelectURL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDlgFindLinks::CDlgFindLinksData::CDlgFindLinksData(
		ARBDogPtr pDog,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		ARBString const& inLink)
	: m_pDog(pDog)
	, m_pTrial(pTrial)
	, m_pRun(pRun)
	, m_OldLink(inLink)
	, m_Link(inLink)
{
}

CDlgFindLinks::CDlgFindLinksData::~CDlgFindLinksData()
{
}

/////////////////////////////////////////////////////////////////////////////

static struct
{
	int fmt;
	int cx;
	UINT idText;
} const colLinkInfo[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_NAME},
	{LVCFMT_LEFT, 50, IDS_COL_DOG},
	{LVCFMT_LEFT, 50, IDS_COL_TRIAL},
	{LVCFMT_LEFT, 50, IDS_COL_EVENT},
};
static int const nColLinkInfo = sizeof(colLinkInfo) / sizeof(colLinkInfo[0]);

typedef struct
{
	CDlgFindLinks* pThis;
	CColumnOrder* pCols;
} SORTINFO;

int CALLBACK CompareLinks(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	int rc = 0;
	int data1 = static_cast<int>(lParam1);
	int data2 = static_cast<int>(lParam2);
	SORTINFO* psi = reinterpret_cast<SORTINFO*>(lParam3);

	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		ARBString str1, str2;
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{
		case 0:
			str1 = psi->pThis->m_Data[data1].m_Link;
			str2 = psi->pThis->m_Data[data2].m_Link;
			break;
		case 2:
			str1 = psi->pThis->m_Data[data1].m_pDog->GetGenericName();
			str2 = psi->pThis->m_Data[data2].m_pDog->GetGenericName();
			break;
		case 1:
			str1 = psi->pThis->m_Data[data1].m_pTrial->GetGenericName();
			str2 = psi->pThis->m_Data[data2].m_pTrial->GetGenericName();
			break;
		case 3:
			str1 = psi->pThis->m_Data[data1].m_pRun->GetGenericName();
			str2 = psi->pThis->m_Data[data2].m_pRun->GetGenericName();
			break;
		}
		if (str1 < str2)
			rc = -1;
		else if (str1 > str2)
			rc = 1;
		if (rc)
		{
			if (psi->pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgFindLinks dialog

CDlgFindLinks::CDlgFindLinks(
		ARBDogList& inDogs,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgFindLinks::IDD, pParent)
	, m_ctrlLinks(false)
	, m_sortLinks(_T("Links"))
	, m_Session(_T("findLink"))
{
	m_sortLinks.Initialize(nColLinkInfo);
	m_ImageList.Create(16, 16, ILC_MASK, 2, 0);
	CWinApp* app = AfxGetApp();
	m_imgEmpty = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_EMPTY));
	m_imgOk = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_PLAN));
	m_imgMissing = m_ImageList.Add(app->LoadIcon(IDI_CALENDAR_TENTATIVE));
	//{{AFX_DATA_INIT(CDlgFindLinks)
	//}}AFX_DATA_INIT

	for (ARBDogList::iterator iterDog = inDogs.begin();
		iterDog != inDogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = *iterDog;
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = *iterTrial;
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				ARBDogRunPtr pRun = *iterRun;
				std::set<ARBString> links;
				pRun->GetLinks(links);
				for (std::set<ARBString>::iterator iter = links.begin();
					iter != links.end();
					++iter)
				{
					m_Data.push_back(CDlgFindLinksData(pDog, pTrial, pRun, *iter));
				}
			}
		}
	}
}

CDlgFindLinks::~CDlgFindLinks()
{
}

void CDlgFindLinks::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFindLinks)
	DDX_Control(pDX, IDC_FINDLINKS_LIST, m_ctrlLinks);
	DDX_Control(pDX, IDC_FINDLINKS_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_FINDLINKS_OPEN, m_ctrlOpen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgFindLinks, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgFindLinks)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FINDLINKS_LIST, OnColumnclickList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FINDLINKS_LIST, OnGetdispinfoList)
	ON_NOTIFY(NM_DBLCLK, IDC_FINDLINKS_LIST, OnDblclkList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_FINDLINKS_LIST, OnKeydownList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FINDLINKS_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_FINDLINKS_COPY, OnCopy)
	ON_BN_CLICKED(IDC_FINDLINKS_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_FINDLINKS_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

int CDlgFindLinks::GetImageIndex(ARBString const& inLink)
{
	CWaitCursor wait;
	int img = m_imgEmpty;
	if (0 < inLink.length())
	{
		if (CheckLink(m_Session, inLink))
			img = m_imgOk;
		else
			img = m_imgMissing;
	}
	return img;
}

void CDlgFindLinks::SetColumnHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColLinkInfo; ++i)
	{
		CString str;
		if (0 < colLinkInfo[i].idText)
		{
			str.LoadString(colLinkInfo[i].idText);
			str += ' ';
		}
		ARBostringstream tmp;
		tmp << (LPCTSTR)str << '(' << m_sortLinks.FindColumnOrder(i) + 1 << ')';
		CString order(tmp.str().c_str());
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlLinks.SetColumn(i, &col);
		m_ctrlLinks.HeaderSort(i, m_sortLinks.IsDescending(i) ? CHeaderCtrl2::eDescending : CHeaderCtrl2::eAscending);
		order.ReleaseBuffer();
	}
}

void CDlgFindLinks::UpdateButtons()
{
	BOOL bEnable = FALSE;
	if (0 <= m_ctrlLinks.GetSelection())
		bEnable = TRUE;
	m_ctrlEdit.EnableWindow(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgFindLinks message handlers

BOOL CDlgFindLinks::OnInitDialog()
{
	CWaitCursor wait;
	CDlgBaseDialog::OnInitDialog();
	m_ctrlLinks.SetExtendedStyle(m_ctrlLinks.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlLinks.SetImageList(&m_ImageList, LVSIL_SMALL);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int i;
	for (i = 0; i < nColLinkInfo; ++i)
	{
		col.fmt = colLinkInfo[i].fmt;
		col.cx = colLinkInfo[i].cx;
		col.pszText = _T("");
		col.iSubItem = i;
		m_ctrlLinks.InsertColumn(i, &col);
	}

	i = 0;
	for (i = 0; i < static_cast<int>(m_Data.size()); ++i)
	{
		LVITEM item;
		item.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iImage = GetImageIndex(m_Data[i].m_Link);
		item.lParam = i;
		m_ctrlLinks.InsertItem(&item);
	}
	for (i = 0; i < nColLinkInfo; ++i)
		m_ctrlLinks.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pThis = this;
	si.pCols = &m_sortLinks;
	m_ctrlLinks.SortItems(CompareLinks, reinterpret_cast<LPARAM>(&si));
	SetColumnHeaders();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFindLinks::OnColumnclickList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	SORTINFO si;
	si.pThis = this;
	m_sortLinks.SetColumnOrder(pNMListView->iSubItem);
	SetColumnHeaders();
	si.pCols = &m_sortLinks;
	m_ctrlLinks.SortItems(CompareLinks, reinterpret_cast<LPARAM>(&si));
	m_sortLinks.Save();
	*pResult = 0;
}

void CDlgFindLinks::OnGetdispinfoList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	int index = static_cast<int>(pDispInfo->item.lParam);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		switch (pDispInfo->item.iSubItem)
		{
		case 0:
			str = m_Data[index].m_Link.c_str();
			break;
		case 1:
			str = m_Data[index].m_pDog->GetGenericName().c_str();
			break;
		case 2:
			str = m_Data[index].m_pTrial->GetGenericName().c_str();
			break;
		case 3:
			str = m_Data[index].m_pRun->GetGenericName().c_str();
			break;
		}
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}

void CDlgFindLinks::OnDblclkList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	OnEdit();
	*pResult = 0;
}

void CDlgFindLinks::OnKeydownList(
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

void CDlgFindLinks::OnItemchangedList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}

void CDlgFindLinks::OnCopy()
{
	if (0 < m_Data.size())
	{
		if (!AfxGetMainWnd()->OpenClipboard())
			return;
		if (!EmptyClipboard())
		{
			CloseClipboard();
			return;
		}

		ARBString data;
		for (size_t i = 0; i < m_Data.size(); ++i)
		{
			data += m_Data[i].m_OldLink;
			data += _T("\r\n");
		}

		// alloc mem block & copy text in
		HGLOBAL temp = GlobalAlloc(GHND, data.length()+1);
		if (NULL != temp)
		{
			LPTSTR str = reinterpret_cast<LPTSTR>(GlobalLock(temp));
			lstrcpy(str, data.c_str());
			GlobalUnlock(reinterpret_cast<void*>(temp));
			// send data to clipbard
			SetClipboardData(CF_TEXT, temp);
		}

		CloseClipboard();
	}
}

void CDlgFindLinks::OnEdit()
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		int index = static_cast<int>(m_ctrlLinks.GetItemData(nItem));
		CDlgSelectURL dlg(m_Data[index].m_Link.c_str(), this);
		if (IDOK == dlg.DoModal())
		{
			ARBString newName = dlg.GetName();
			if (m_Data[index].m_Link != newName)
			{
				m_Data[index].m_Link = newName;
				CRect r;
				m_ctrlLinks.GetItemRect(nItem, r, LVIR_BOUNDS);
				m_ctrlLinks.InvalidateRect(r);
				LVITEM item;
				item.mask = LVIF_IMAGE;
				item.iItem = index;
				item.iImage = GetImageIndex(m_Data[index].m_Link);
				m_ctrlLinks.SetItem(&item);
			}
		}
	}
}

void CDlgFindLinks::OnOpen()
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		int index = static_cast<int>(m_ctrlLinks.GetItemData(nItem));
		RunCommand(m_Data[index].m_Link.c_str());
	}
}

void CDlgFindLinks::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	for (size_t idx = 0; idx < m_Data.size(); ++idx)
	{
		if (m_Data[idx].m_Link != m_Data[idx].m_OldLink)
		{
			m_Data[idx].m_pRun->RemoveLink(m_Data[idx].m_OldLink);
			if (0 < m_Data[idx].m_Link.length())
				m_Data[idx].m_pRun->AddLink(m_Data[idx].m_Link);
		}
	}
	CDlgBaseDialog::OnOK();
}
