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
 * @brief implementation of the CDlgDogTitles class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDogTitles.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "DlgTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// Overlay indices
#define IDX_HIDDEN	1

static struct
{
	int fmt;
	int cx;
	UINT idText;
} const colTitleInfo[] =
{
	{LVCFMT_LEFT, 10, 0},
	{LVCFMT_LEFT, 50, IDS_COL_DATE},
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
	{LVCFMT_LEFT, 50, IDS_COL_TITLE},
	{LVCFMT_LEFT, 50, IDS_COL_NAME},
};
static int const nColTitleInfo = sizeof(colTitleInfo) / sizeof(colTitleInfo[0]);


typedef struct
{
	CAgilityBookDoc* pDoc;
	CColumnOrder* pCols;
} SORTINFO;

/////////////////////////////////////////////////////////////////////////////

int CALLBACK CompareTitles(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	CListData* pRawTitle1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawTitle2 = reinterpret_cast<CListData*>(lParam2);
	ARBDogTitlePtr pTitle1 = dynamic_cast<CListPtrData<ARBDogTitlePtr>*>(pRawTitle1)->GetData();
	ARBDogTitlePtr pTitle2 = dynamic_cast<CListPtrData<ARBDogTitlePtr>*>(pRawTitle2)->GetData();
	SORTINFO* psi = reinterpret_cast<SORTINFO*>(lParam3);
	int rc = 0;
	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{
		case 0: // Received and hidden
			if (!pTitle1->GetReceived() && pTitle2->GetReceived())
				rc = -1;
			else if (pTitle1->GetReceived() && !pTitle2->GetReceived())
				rc = 1;
			else if (!pTitle1->IsHidden() && pTitle2->IsHidden())
				rc = -1;
			else if (pTitle1->IsHidden() && !pTitle2->IsHidden())
				rc = 1;
			break;
		case 2: // venue
			if (pTitle1->GetVenue() < pTitle2->GetVenue())
				rc = -1;
			else if (pTitle1->GetVenue() > pTitle2->GetVenue())
				rc = 1;
			break;
		case 1: // date
			if (pTitle1->GetDate() < pTitle2->GetDate())
				rc = -1;
			else if (pTitle1->GetDate() > pTitle2->GetDate())
				rc = 1;
			break;
		case 3: // name
			{
				tstring n1 = pTitle1->GetGenericName();
				tstring n2 = pTitle2->GetGenericName();
				if (n1 < n2)
					rc = -1;
				else if (n1 > n2)
					rc = 1;
			}
			break;
		case 4: // nice name
			{
				tstring name1 = psi->pDoc->GetConfig().GetTitleNiceName(pTitle1->GetVenue(), pTitle1->GetRawName());
				tstring name2 = psi->pDoc->GetConfig().GetTitleNiceName(pTitle2->GetVenue(), pTitle2->GetRawName());
				if (name1 < name2)
					rc = -1;
				else if (name1 > name2)
					rc = 1;
			}
			break;
		}
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
// CDlgDogTitles dialog

CDlgDogTitles::CDlgDogTitles(
		CAgilityBookDoc* pDoc,
		ARBDogTitleList const& titles)
	: CDlgBasePropertyPage(CDlgDogTitles::IDD)
	, m_ctrlTitles(true)
	, m_pDoc(pDoc)
	, m_sortTitles(_T("Titles"))
	, m_Titles()
{
	titles.Clone(m_Titles);
	m_sortTitles.Initialize(nColTitleInfo);
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 3, 0);
	m_imgEmpty = m_ImageList.Add(theApp.LoadIcon(IDI_EMPTY));
	m_imgTitled = m_ImageList.Add(theApp.LoadIcon(IDI_TITLE));
	m_imgTitledReceived = m_ImageList.Add(theApp.LoadIcon(IDI_TITLE2));
	int imgHidden = m_ImageList.Add(theApp.LoadIcon(IDI_HIDDEN));
	m_ImageList.SetOverlayImage(imgHidden,IDX_HIDDEN);
	//{{AFX_DATA_INIT(CDlgDogTitles)
	//}}AFX_DATA_INIT
}


void CDlgDogTitles::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDogTitles)
	DDX_Control(pDX, IDC_DOG_TITLE_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_DOG_TITLE_EDIT, m_ctrlTitleEdit);
	DDX_Control(pDX, IDC_DOG_TITLE_DELETE, m_ctrlTitleDelete);
	DDX_Control(pDX, IDC_DOG_TITLE_HIDDEN, m_ctrlHidden);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDogTitles, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgDogTitles)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DOG_TITLE_TITLES, OnColumnclickTitles)
	ON_NOTIFY(NM_DBLCLK, IDC_DOG_TITLE_TITLES, OnDblclkTitles)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DOG_TITLE_TITLES, OnKeydownTitles)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOG_TITLE_TITLES, OnItemchangedTitles)
	ON_BN_CLICKED(IDC_DOG_TITLE_NEW, OnTitleNew)
	ON_BN_CLICKED(IDC_DOG_TITLE_EDIT, OnTitleEdit)
	ON_BN_CLICKED(IDC_DOG_TITLE_DELETE, OnTitleDelete)
	ON_BN_CLICKED(IDC_DOG_TITLE_HIDDEN, OnBnClickedHidden)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CListPtrData<ARBDogTitlePtr>* CDlgDogTitles::GetTitleData(int index) const
{
	CListData* pData = m_ctrlTitles.GetData(index);
	return dynamic_cast<CListPtrData<ARBDogTitlePtr>*>(pData);
}


void CDlgDogTitles::SetColumnTitleHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColTitleInfo; ++i)
	{
		CString str;
		if (0 < colTitleInfo[i].idText)
		{
			str.LoadString(colTitleInfo[i].idText);
			str += ' ';
		}
		otstringstream tmp;
		tmp << (LPCTSTR)str << '(' << m_sortTitles.FindColumnOrder(i) + 1 << ')';
		CString order(tmp.str().c_str());
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlTitles.SetColumn(i, &col);
		m_ctrlTitles.HeaderSort(i, m_sortTitles.IsDescending(i) ? CHeaderCtrl2::eDescending : CHeaderCtrl2::eAscending);
		order.ReleaseBuffer();
	}
}


void CDlgDogTitles::ListTitles()
{
	ARBDogTitlePtr pSelected;
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogTitlePtr>* pData = GetTitleData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlTitles.DeleteAllItems();
	i = 0;
	for (ARBDogTitleList::const_iterator iterTitle = m_Titles.begin();
		iterTitle != m_Titles.end();
		++iterTitle)
	{
		ARBDogTitlePtr pTitle = (*iterTitle);
		if (!CAgilityBookOptions::GetViewHiddenTitles()
		&& pTitle->IsHidden())
		{
			continue;
		}
		LVITEM item;
		item.mask = LVIF_IMAGE | LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = 0;
		if (pTitle->IsHidden())
		{
			item.mask |= LVIF_STATE;
			item.state = INDEXTOOVERLAYMASK(IDX_HIDDEN);
			item.stateMask = LVIS_OVERLAYMASK;
		}
		if (pTitle->GetDate().IsValid())
		{
			item.iImage = m_imgTitled;
			if (pTitle->GetReceived())
				item.iImage = m_imgTitledReceived;
		}
		else
			item.iImage = m_imgEmpty;
		item.lParam = reinterpret_cast<LPARAM>(
			static_cast<CListData*>(
				new CListPtrData<ARBDogTitlePtr>(pTitle)));
		int nItem = m_ctrlTitles.InsertItem(&item);
		if (pTitle->GetDate().IsValid())
			m_ctrlTitles.SetItemText(nItem, 1, pTitle->GetDate().GetString(ARBDate::eDashYYYYMMDD).c_str());
		else
		{
			CString str;
			str.LoadString(IDS_UNEARNED);
			m_ctrlTitles.SetItemText(nItem, 1, str);
		}
		m_ctrlTitles.SetItemText(nItem, 2, pTitle->GetVenue().c_str());
		m_ctrlTitles.SetItemText(nItem, 3, pTitle->GetGenericName().c_str());
		m_ctrlTitles.SetItemText(nItem, 4, m_pDoc->GetConfig().GetTitleNiceName(pTitle->GetVenue(), pTitle->GetRawName()).c_str());
		++i;
	}
	for (i = 0; i < nColTitleInfo; ++i)
		m_ctrlTitles.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pDoc = m_pDoc;
	si.pCols = &m_sortTitles;
	m_ctrlTitles.SortItems(CompareTitles, reinterpret_cast<LPARAM>(&si));
	if (pSelected)
	{
		for (i = 0; i < m_ctrlTitles.GetItemCount(); ++i)
		{
			CListPtrData<ARBDogTitlePtr>* pTitle = GetTitleData(i);
			if (pTitle && pTitle->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlTitles.SetSelection(i, true);
				break;
			}
		}
	}
}


void CDlgDogTitles::UpdateButtons()
{
	UINT selected = m_ctrlTitles.GetSelectedCount();
	ASSERT(1 >= selected);
	if (0 == selected)
	{
		m_ctrlTitleEdit.EnableWindow(FALSE);
		m_ctrlTitleDelete.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlTitleEdit.EnableWindow(TRUE);
		m_ctrlTitleDelete.EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDogTitles message handlers

BOOL CDlgDogTitles::OnInitDialog()
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlTitles.SetImageList(&m_ImageList, LVSIL_SMALL);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int i;
	for (i = 0; i < nColTitleInfo; ++i)
	{
		col.fmt = colTitleInfo[i].fmt;
		col.cx = colTitleInfo[i].cx;
		col.pszText = _T("");
		col.iSubItem = i;
		m_ctrlTitles.InsertColumn(i, &col);
	}
	SetColumnTitleHeaders();

	m_ctrlHidden.SetCheck(CAgilityBookOptions::GetViewHiddenTitles() ? 1 : 0);

	ListTitles();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDogTitles::OnColumnclickTitles(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	SORTINFO si;
	si.pDoc = m_pDoc;
	m_sortTitles.SetColumnOrder(pNMListView->iSubItem);
	SetColumnTitleHeaders();
	si.pCols = &m_sortTitles;
	m_ctrlTitles.SortItems(CompareTitles, reinterpret_cast<LPARAM>(&si));
	m_sortTitles.Save();
	*pResult = 0;
}


void CDlgDogTitles::OnDblclkTitles(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnTitleEdit();
	*pResult = 0;
}


void CDlgDogTitles::OnKeydownTitles(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnTitleEdit();
		break;
	}
	*pResult = 0;
}


void CDlgDogTitles::OnItemchangedTitles(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}


void CDlgDogTitles::OnTitleNew()
{
	CDlgTitle dlg(m_pDoc->GetConfig(), m_Titles, ARBDogTitlePtr(), this);
	if (IDOK == dlg.DoModal())
		ListTitles();
}


void CDlgDogTitles::OnTitleEdit()
{
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogTitlePtr>* pTitle = GetTitleData(i);
		CDlgTitle dlg(m_pDoc->GetConfig(), m_Titles, pTitle->GetData(), this);
		if (IDOK == dlg.DoModal())
			ListTitles();
	}
}


void CDlgDogTitles::OnTitleDelete()
{
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogTitlePtr>* pTitle = GetTitleData(i);
		m_Titles.DeleteTitle(pTitle->GetData());
		m_ctrlTitles.DeleteItem(i);
	}
}


void CDlgDogTitles::OnBnClickedHidden()
{
	CAgilityBookOptions::SetViewHiddenTitles(m_ctrlHidden.GetCheck() == 1);
	ListTitles();
}
