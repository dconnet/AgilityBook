/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDogTitles.h"

#include "ARBDog.h"
#include "DlgTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static const struct
{
	int fmt;
	int cx;
	UINT idText;
} colTitleInfo[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_DATE},
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
	{LVCFMT_LEFT, 50, IDS_COL_NAME},
	{LVCFMT_LEFT, 50, IDS_COL_RECEIVED},
};
static const int nColTitleInfo = sizeof(colTitleInfo) / sizeof(colTitleInfo[0]);

typedef struct
{
	CDlgDogTitles *pThis;
	CColumnOrder *pCols;
} SORTINFO;

/////////////////////////////////////////////////////////////////////////////

int CALLBACK CompareTitles(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	const ARBDogTitle* pTitle1 = reinterpret_cast<const ARBDogTitle*>(lParam1);
	const ARBDogTitle* pTitle2 = reinterpret_cast<const ARBDogTitle*>(lParam2);
	SORTINFO *psi = reinterpret_cast<SORTINFO*>(lParam3);
	int rc = 0;
	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{
		default:
		case 1: // venue
			if (pTitle1->GetVenue() < pTitle2->GetVenue())
				rc = -1;
			else if (pTitle1->GetVenue() > pTitle2->GetVenue())
				rc = 1;
			break;
		case 0: // date
			if (pTitle1->GetDate() < pTitle2->GetDate())
				rc = -1;
			else if (pTitle1->GetDate() > pTitle2->GetDate())
				rc = 1;
			break;
		case 2: // name
			if (pTitle1->GetName() < pTitle2->GetName())
				rc = -1;
			else if (pTitle1->GetName() > pTitle2->GetName())
				rc = 1;
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

CDlgDogTitles::CDlgDogTitles(ARBConfig& config, const ARBDogTitleList& titles)
	: CPropertyPage(CDlgDogTitles::IDD)
	, m_Config(config)
	, m_sortTitles("Titles")
	, m_Titles(titles)
{
	m_sortTitles.Initialize(nColTitleInfo);
	//{{AFX_DATA_INIT(CDlgDogTitles)
	//}}AFX_DATA_INIT
}

void CDlgDogTitles::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDogTitles)
	DDX_Control(pDX, IDC_TITLE_EDIT, m_ctrlTitleEdit);
	DDX_Control(pDX, IDC_TITLE_DELETE, m_ctrlTitleDelete);
	DDX_Control(pDX, IDC_TITLES, m_ctrlTitles);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_sortTitles.Save();
	}
}

BEGIN_MESSAGE_MAP(CDlgDogTitles, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgDogTitles)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_TITLES, OnColumnclickTitles)
	ON_NOTIFY(NM_DBLCLK, IDC_TITLES, OnDblclkTitles)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TITLES, OnItemchangedTitles)
	ON_BN_CLICKED(IDC_TITLE_NEW, OnTitleNew)
	ON_BN_CLICKED(IDC_TITLE_EDIT, OnTitleEdit)
	ON_BN_CLICKED(IDC_TITLE_DELETE, OnTitleDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgDogTitles::SetColumnTitleHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColTitleInfo; ++i)
	{
		CString str;
		str.LoadString(colTitleInfo[i].idText);
		CString order;
		order.Format(_T("%c %s (%d)"),
			m_sortTitles.IsDescending(i) ? '<' : '>',
			(LPCTSTR)str,
			m_sortTitles.FindColumnOrder(i) + 1);
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlTitles.SetColumn(i, &col);
		order.ReleaseBuffer();
	}
}

void CDlgDogTitles::ListTitles()
{
	const ARBDogTitle* pSelected = NULL;
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
		pSelected = reinterpret_cast<const ARBDogTitle*>(m_ctrlTitles.GetItemData(i));
	m_ctrlTitles.DeleteAllItems();
	i = 0;
	for (ARBDogTitleList::const_iterator iterTitle = m_Titles.begin();
		iterTitle != m_Titles.end();
		++i, ++iterTitle)
	{
		const ARBDogTitle* pTitle = (*iterTitle);
		int nItem = m_ctrlTitles.InsertItem(i, pTitle->GetDate().GetString(true, ARBDate::eDashYYYYMMDD).c_str());
		m_ctrlTitles.SetItemText(nItem, 1, pTitle->GetVenue().c_str());
		m_ctrlTitles.SetItemText(nItem, 2, pTitle->GetName().c_str());
		m_ctrlTitles.SetItemText(nItem, 3, pTitle->GetReceived() ? "x" : "");
		m_ctrlTitles.SetItemData(nItem, reinterpret_cast<LPARAM>(pTitle));
	}
	for (i = 0; i < nColTitleInfo; ++i)
		m_ctrlTitles.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pThis = this;
	si.pCols = &m_sortTitles;
	m_ctrlTitles.SortItems(CompareTitles, reinterpret_cast<LPARAM>(&si));
	if (pSelected)
	{
		for (i = 0; i < m_ctrlTitles.GetItemCount(); ++i)
		{
			const ARBDogTitle* pTitle = reinterpret_cast<const ARBDogTitle*>(m_ctrlTitles.GetItemData(i));
			if (pTitle == pSelected) // compare by ptr is fine.
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
	CPropertyPage::OnInitDialog();
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

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

	ListTitles();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDogTitles::OnColumnclickTitles(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SORTINFO si;
	si.pThis = this;
	m_sortTitles.SetColumnOrder(pNMListView->iSubItem);
	SetColumnTitleHeaders();
	si.pCols = &m_sortTitles;
	m_ctrlTitles.SortItems(CompareTitles, reinterpret_cast<LPARAM>(&si));
	*pResult = 0;
}

void CDlgDogTitles::OnDblclkTitles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTitleEdit();
	*pResult = 0;
}

void CDlgDogTitles::OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgDogTitles::OnTitleNew()
{
	CDlgTitle dlg(m_Config, m_Titles, NULL, this);
	if (IDOK == dlg.DoModal())
		ListTitles();
}

void CDlgDogTitles::OnTitleEdit()
{
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		ARBDogTitle* pTitle = reinterpret_cast<ARBDogTitle*>(m_ctrlTitles.GetItemData(i));
		CDlgTitle dlg(m_Config, m_Titles, pTitle, this);
		if (IDOK == dlg.DoModal())
			ListTitles();
	}
}

void CDlgDogTitles::OnTitleDelete()
{
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		const ARBDogTitle* pTitle = reinterpret_cast<const ARBDogTitle*>(m_ctrlTitles.GetItemData(i));
		m_Titles.DeleteTitle(pTitle->GetVenue(), pTitle->GetName());
		m_ctrlTitles.DeleteItem(i);
	}
}
