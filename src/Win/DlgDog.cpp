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
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDog.h"

#include "ARBDog.h"
#include "DlgRegNum.h"
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

static const struct
{
	int fmt;
	int cx;
	UINT idText;
} colRegNumInfo[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
	{LVCFMT_LEFT, 50, IDS_COL_NUMBER},
	{LVCFMT_LEFT, 50, IDS_COL_HEIGHT},
	{LVCFMT_LEFT, 50, IDS_COL_RECEIVED},
};
static const int nColRegNumInfo = sizeof(colRegNumInfo) / sizeof(colRegNumInfo[0]);

typedef struct
{
	CDlgDog *pThis;
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

int CALLBACK CompareRegNums(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	ARBDogRegNum* pRegNum1 = reinterpret_cast<ARBDogRegNum*>(lParam1);
	ARBDogRegNum* pRegNum2 = reinterpret_cast<ARBDogRegNum*>(lParam2);
	SORTINFO *psi = reinterpret_cast<SORTINFO*>(lParam3);
	int rc = 0;
	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{
		default:
		case 0: // venue
			if (pRegNum1->GetVenue() < pRegNum2->GetVenue())
				rc = -1;
			else if (pRegNum1->GetVenue() > pRegNum2->GetVenue())
				rc = 1;
			break;
		case 1: // number
			if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
				rc = -1;
			else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
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
// CDlgDog dialog

CDlgDog::CDlgDog(ARBConfig& config, ARBDog* pDog, CWnd* pParent)
	: CDialog(CDlgDog::IDD, pParent)
	, m_Config(config)
	, m_pDog(pDog)
	, m_sortTitles("Titles")
	, m_sortRegNums("RegNums")
{
	m_sortTitles.Initialize(nColTitleInfo);
	m_sortRegNums.Initialize(nColRegNumInfo);
	m_Titles = m_pDog->GetTitles();
	m_RegNums = m_pDog->GetRegNums();
	//{{AFX_DATA_INIT(CDlgDog)
	m_CallName = pDog->GetCallName().c_str();
	m_RegName = pDog->GetRegisteredName().c_str();
	m_Breed = pDog->GetBreed().c_str();
	m_Notes = pDog->GetNote().c_str();
	//}}AFX_DATA_INIT
	m_Notes.Replace("\n", "\r\n");
}

void CDlgDog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDog)
	DDX_Text(pDX, IDC_CALLNAME, m_CallName);
	DDX_Control(pDX, IDC_DOB, m_ctrlDateDOB);
	DDX_Control(pDX, IDC_IS_DECEASED, m_ctrlDeceased);
	DDX_Control(pDX, IDC_DECEASED, m_ctrlDateDeceased);
	DDX_Text(pDX, IDC_BREED, m_Breed);
	DDX_Control(pDX, IDC_AGE_TEXT, m_ctrlAge);
	DDX_Text(pDX, IDC_REG_NAME, m_RegName);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_ctrlTitleEdit);
	DDX_Control(pDX, IDC_TITLE_DELETE, m_ctrlTitleDelete);
	DDX_Control(pDX, IDC_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_REG_EDIT, m_ctrlRegEdit);
	DDX_Control(pDX, IDC_REG_DELETE, m_ctrlRegDelete);
	DDX_Control(pDX, IDC_REG_NUMS, m_ctrlRegNums);
	DDX_Text(pDX, IDC_NOTES, m_Notes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDog, CDialog)
	//{{AFX_MSG_MAP(CDlgDog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DOB, OnDatetimechange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DECEASED, OnDatetimechange)
	ON_BN_CLICKED(IDC_IS_DECEASED, OnIsDeceased)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_TITLES, OnColumnclickTitles)
	ON_NOTIFY(NM_DBLCLK, IDC_TITLES, OnDblclkTitles)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TITLES, OnItemchangedTitles)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_REG_NUMS, OnColumnclickRegNums)
	ON_NOTIFY(NM_DBLCLK, IDC_REG_NUMS, OnDblclkRegNums)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REG_NUMS, OnItemchangedRegNums)
	ON_BN_CLICKED(IDC_TITLE_NEW, OnTitleNew)
	ON_BN_CLICKED(IDC_TITLE_EDIT, OnTitleEdit)
	ON_BN_CLICKED(IDC_TITLE_DELETE, OnTitleDelete)
	ON_BN_CLICKED(IDC_REG_NEW, OnRegNew)
	ON_BN_CLICKED(IDC_REG_EDIT, OnRegEdit)
	ON_BN_CLICKED(IDC_REG_DELETE, OnRegDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgDog::SetColumnTitleHeaders()
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

void CDlgDog::SetColumnRegNumHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColRegNumInfo; ++i)
	{
		CString str;
		str.LoadString(colRegNumInfo[i].idText);
		CString order;
		order.Format(_T("%c %s (%d)"),
			m_sortRegNums.IsDescending(i) ? '<' : '>',
			(LPCTSTR)str,
			m_sortRegNums.FindColumnOrder(i) + 1);
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlRegNums.SetColumn(i, &col);
		order.ReleaseBuffer();
	}
}

void CDlgDog::ListTitles()
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

void CDlgDog::ListRegNums()
{
	const ARBDogRegNum* pSelected = NULL;
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
		pSelected = reinterpret_cast<const ARBDogRegNum*>(m_ctrlRegNums.GetItemData(i));
	m_ctrlRegNums.DeleteAllItems();
	i = 0;
	for (ARBDogRegNumList::const_iterator iterRegNum = m_RegNums.begin();
		iterRegNum != m_RegNums.end();
		++i, ++iterRegNum)
	{
		const ARBDogRegNum* pRegNum = (*iterRegNum);
		int nItem = m_ctrlRegNums.InsertItem(i, pRegNum->GetVenue().c_str());
		m_ctrlRegNums.SetItemText(nItem, 1, pRegNum->GetNumber().c_str());
		m_ctrlRegNums.SetItemText(nItem, 2, pRegNum->GetHeight().c_str());
		m_ctrlRegNums.SetItemText(nItem, 3, pRegNum->GetReceived() ? "x" : "");
		m_ctrlRegNums.SetItemData(nItem, reinterpret_cast<LPARAM>(pRegNum));
	}
	for (i = 0; i < nColRegNumInfo; ++i)
		m_ctrlRegNums.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pThis = this;
	si.pCols = &m_sortRegNums;
	m_ctrlRegNums.SortItems(CompareRegNums, reinterpret_cast<LPARAM>(&si));
	if (pSelected)
	{
		for (i = 0; i < m_ctrlRegNums.GetItemCount(); ++i)
		{
			const ARBDogRegNum* pRegNum = reinterpret_cast<const ARBDogRegNum*>(m_ctrlRegNums.GetItemData(i));
			if (pRegNum == pSelected) // compare by ptr is fine.
			{
				m_ctrlRegNums.SetSelection(i, true);
				break;
			}
		}
	}
}

void CDlgDog::UpdateButtons()
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
	selected = m_ctrlRegNums.GetSelectedCount();
	ASSERT(1 >= selected);
	if (0 == selected)
	{
		m_ctrlRegEdit.EnableWindow(FALSE);
		m_ctrlRegDelete.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlRegEdit.EnableWindow(TRUE);
		m_ctrlRegDelete.EnableWindow(TRUE);
	}
}

void CDlgDog::UpdateAge()
{
	CTime dob;
	m_ctrlDateDOB.GetTime(dob);
	CTime current = CTime::GetCurrentTime();
	if (BST_CHECKED == m_ctrlDeceased.GetCheck())
		m_ctrlDateDeceased.GetTime(current);
	CTimeSpan age = current - dob;
	CString str;
	str.Format("%.1f", age.GetDays()/365.0);
	m_ctrlAge.SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDog message handlers

BOOL CDlgDog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlTitles.SetExtendedStyle(m_ctrlTitles.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlRegNums.SetExtendedStyle(m_ctrlRegNums.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CTime dob;
	if (m_pDog->GetDOB().IsValid())
		dob = CTime(m_pDog->GetDOB().GetDate());
	else
		dob = CTime::GetCurrentTime();
	m_ctrlDateDOB.SetTime(&dob);
	int nCheck = BST_UNCHECKED;
	if (m_pDog->GetDeceased().IsValid())
	{
		nCheck = BST_CHECKED;
		dob = CTime(m_pDog->GetDeceased().GetDate());
	}
	else
		dob = CTime::GetCurrentTime();
	m_ctrlDeceased.SetCheck(nCheck);
	m_ctrlDateDeceased.SetTime(&dob);
	OnIsDeceased(); // Fixes visibility of date and calls UpdateAge()

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

	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for (i = 0; i < nColRegNumInfo; ++i)
	{
		col.fmt = colRegNumInfo[i].fmt;
		col.cx = colRegNumInfo[i].cx;
		col.pszText = _T("");
		col.iSubItem = i;
		m_ctrlRegNums.InsertColumn(i, &col);
	}
	SetColumnRegNumHeaders();

	ListTitles();
	ListRegNums();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDog::OnDatetimechange(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateAge();
	*pResult = 0;
}

void CDlgDog::OnIsDeceased()
{
	if (BST_CHECKED == m_ctrlDeceased.GetCheck())
		m_ctrlDateDeceased.ShowWindow(SW_SHOW);
	else
		m_ctrlDateDeceased.ShowWindow(SW_HIDE);
	UpdateAge();
}

void CDlgDog::OnColumnclickTitles(NMHDR* pNMHDR, LRESULT* pResult)
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

void CDlgDog::OnDblclkTitles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTitleEdit();
	*pResult = 0;
}

void CDlgDog::OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgDog::OnColumnclickRegNums(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SORTINFO si;
	si.pThis = this;
	m_sortRegNums.SetColumnOrder(pNMListView->iSubItem);
	SetColumnRegNumHeaders();
	si.pCols = &m_sortRegNums;
	m_ctrlRegNums.SortItems(CompareRegNums, reinterpret_cast<LPARAM>(&si));
	*pResult = 0;
}

void CDlgDog::OnDblclkRegNums(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRegEdit();
	*pResult = 0;
}

void CDlgDog::OnItemchangedRegNums(NMHDR* pNMHDR, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgDog::OnTitleNew()
{
	CDlgTitle dlg(m_Config, m_Titles, NULL, this);
	if (IDOK == dlg.DoModal())
		ListTitles();
}

void CDlgDog::OnTitleEdit()
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

void CDlgDog::OnTitleDelete()
{
	int i = m_ctrlTitles.GetSelection();
	if (0 <= i)
	{
		const ARBDogTitle* pTitle = reinterpret_cast<const ARBDogTitle*>(m_ctrlTitles.GetItemData(i));
		m_Titles.DeleteTitle(pTitle->GetVenue(), pTitle->GetName());
		m_ctrlTitles.DeleteItem(i);
	}
}

void CDlgDog::OnRegNew()
{
	CDlgRegNum dlg(m_Config, m_RegNums, NULL, this);
	if (IDOK == dlg.DoModal())
		ListRegNums();
}

void CDlgDog::OnRegEdit()
{
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
	{
		ARBDogRegNum* pRegNum = reinterpret_cast<ARBDogRegNum*>(m_ctrlRegNums.GetItemData(i));
		CDlgRegNum dlg(m_Config, m_RegNums, pRegNum, this);
		if (IDOK == dlg.DoModal())
			ListRegNums();
	}
}

void CDlgDog::OnRegDelete()
{
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
	{
		const ARBDogRegNum* pRegNum = reinterpret_cast<const ARBDogRegNum*>(m_ctrlRegNums.GetItemData(i));
		m_RegNums.DeleteRegNum(pRegNum->GetVenue(), pRegNum->GetNumber());
		m_ctrlRegNums.DeleteItem(i);
	}
}

void CDlgDog::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_CallName.TrimRight();
	m_CallName.TrimLeft();
	m_Breed.TrimRight();
	m_Breed.TrimLeft();
	m_RegName.TrimRight();
	m_RegName.TrimLeft();
	m_Notes.TrimRight();
	m_Notes.TrimLeft();

	if (m_CallName.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_CALLNAME));
		return;
	}

	CTime time;
	m_ctrlDateDOB.GetTime(time);
	ARBDate date(time.GetYear(), time.GetMonth(), time.GetDay());

	ARBDate deceased;
	if (BST_CHECKED == m_ctrlDeceased.GetCheck())
	{
		m_ctrlDateDeceased.GetTime(time);
		deceased = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}

	m_pDog->SetCallName((LPCSTR)m_CallName);
	m_pDog->SetDOB(date);
	m_pDog->SetDeceased(deceased);
	m_pDog->SetBreed((LPCSTR)m_Breed);
	m_pDog->SetRegisteredName((LPCSTR)m_RegName);
	m_pDog->GetTitles() = m_Titles;
	m_pDog->GetRegNums() = m_RegNums;
	m_Notes.Replace("\r\n", "\n");
	m_pDog->SetNote((LPCSTR)m_Notes);

	m_sortTitles.Save();
	m_sortRegNums.Save();

	CDialog::OnOK();
}

void CDlgDog::OnCancel()
{
	CDialog::OnCancel();
}
