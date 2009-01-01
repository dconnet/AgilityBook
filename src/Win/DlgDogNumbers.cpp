/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgDogNumbers class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDogNumbers.h"

#include "AgilityBookDoc.h"
#include "ARBDog.h"
#include "DlgRegNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	int fmt;
	int cx;
	UINT idText;
} const colRegNumInfo[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
	{LVCFMT_LEFT, 50, IDS_COL_NUMBER},
	{LVCFMT_LEFT, 50, IDS_COL_HEIGHT},
	{LVCFMT_LEFT, 50, IDS_COL_RECEIVED},
	{LVCFMT_LEFT, 50, IDS_COL_NOTE},
};
static int const nColRegNumInfo = sizeof(colRegNumInfo) / sizeof(colRegNumInfo[0]);


typedef struct
{
	CDlgDogNumbers* pThis;
	CColumnOrder* pCols;
} SORTINFO;

/////////////////////////////////////////////////////////////////////////////

int CALLBACK CompareRegNums(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	CListData* pRawRegNum1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawRegNum2 = reinterpret_cast<CListData*>(lParam2);
	ARBDogRegNumPtr pRegNum1 = dynamic_cast<CListPtrData<ARBDogRegNumPtr>*>(pRawRegNum1)->GetData();
	ARBDogRegNumPtr pRegNum2 = dynamic_cast<CListPtrData<ARBDogRegNumPtr>*>(pRawRegNum2)->GetData();
	SORTINFO* psi = reinterpret_cast<SORTINFO*>(lParam3);
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
		case 2: // height
			if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
				rc = -1;
			else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
				rc = 1;
			break;
		case 3: // received
			if (!pRegNum1->GetReceived() && pRegNum2->GetReceived())
				rc = -1;
			else if (pRegNum1->GetReceived() && !pRegNum2->GetReceived())
				rc = 1;
			break;
		case 4: // note
			if (pRegNum1->GetNote() < pRegNum2->GetNote())
				rc = -1;
			else if (pRegNum1->GetNote() > pRegNum2->GetNote())
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
// CDlgDogNumbers dialog

CDlgDogNumbers::CDlgDogNumbers(
		CAgilityBookDoc* pDoc,
		ARBDogRegNumList const& regnums)
	: CDlgBasePropertyPage(CDlgDogNumbers::IDD)
	, m_ctrlRegNums(true)
	, m_pDoc(pDoc)
	, m_sortRegNums(_T("RegNums"))
	, m_RegNums()
{
	regnums.Clone(m_RegNums);
	m_sortRegNums.Initialize(nColRegNumInfo);
	//{{AFX_DATA_INIT(CDlgDogNumbers)
	//}}AFX_DATA_INIT
}


void CDlgDogNumbers::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDogNumbers)
	DDX_Control(pDX, IDC_DOGNUM_EDIT, m_ctrlRegEdit);
	DDX_Control(pDX, IDC_DOGNUM_DELETE, m_ctrlRegDelete);
	DDX_Control(pDX, IDC_DOGNUM_REG_NUMS, m_ctrlRegNums);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDogNumbers, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgDogNumbers)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DOGNUM_REG_NUMS, OnColumnclickRegNums)
	ON_NOTIFY(NM_DBLCLK, IDC_DOGNUM_REG_NUMS, OnDblclkRegNums)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DOGNUM_REG_NUMS, OnKeydownRegNums)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOGNUM_REG_NUMS, OnItemchangedRegNums)
	ON_BN_CLICKED(IDC_DOGNUM_NEW, OnRegNew)
	ON_BN_CLICKED(IDC_DOGNUM_EDIT, OnRegEdit)
	ON_BN_CLICKED(IDC_DOGNUM_DELETE, OnRegDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CListPtrData<ARBDogRegNumPtr>* CDlgDogNumbers::GetRegNumData(int index) const
{
	CListData* pData = m_ctrlRegNums.GetData(index);
	return dynamic_cast<CListPtrData<ARBDogRegNumPtr>*>(pData);
}


void CDlgDogNumbers::SetColumnRegNumHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColRegNumInfo; ++i)
	{
		CString str;
		str.LoadString(colRegNumInfo[i].idText);
		otstringstream tmp;
		tmp << (LPCTSTR)str << _T(" (") << m_sortRegNums.FindColumnOrder(i) + 1 << ')';
		CString order(tmp.str().c_str());
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlRegNums.SetColumn(i, &col);
		m_ctrlRegNums.HeaderSort(i, m_sortRegNums.IsDescending(i) ? CHeaderCtrl2::eDescending : CHeaderCtrl2::eAscending);
		order.ReleaseBuffer();
	}
}


void CDlgDogNumbers::ListRegNums()
{
	ARBDogRegNumPtr pSelected;
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogRegNumPtr>* pData = GetRegNumData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlRegNums.DeleteAllItems();
	i = 0;
	for (ARBDogRegNumList::const_iterator iterRegNum = m_RegNums.begin();
		iterRegNum != m_RegNums.end();
		++i, ++iterRegNum)
	{
		ARBDogRegNumPtr pRegNum = (*iterRegNum);
		int nItem = m_ctrlRegNums.InsertItem(i, pRegNum->GetVenue().c_str());
		m_ctrlRegNums.SetItemText(nItem, 1, pRegNum->GetNumber().c_str());
		m_ctrlRegNums.SetItemText(nItem, 2, pRegNum->GetHeight().c_str());
		m_ctrlRegNums.SetItemText(nItem, 3, pRegNum->GetReceived() ? _T("x") : _T(""));
		CString str(pRegNum->GetNote().c_str());
		str.Replace(_T("\n"), _T(" "));
		m_ctrlRegNums.SetItemText(nItem, 4, str);
		m_ctrlRegNums.SetItemData(nItem,
			reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CListPtrData<ARBDogRegNumPtr>(pRegNum))));
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
			CListPtrData<ARBDogRegNumPtr>* pData = GetRegNumData(i);
			if (pData && pData->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlRegNums.SetSelection(i, true);
				break;
			}
		}
	}
}


void CDlgDogNumbers::UpdateButtons()
{
	UINT selected = m_ctrlRegNums.GetSelectedCount();
	assert(1 >= selected);
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

/////////////////////////////////////////////////////////////////////////////
// CDlgDogNumbers message handlers

BOOL CDlgDogNumbers::OnInitDialog()
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlRegNums.SetExtendedStyle(m_ctrlRegNums.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	m_ctrlRegNums.SuppressTooltipFixing(true);
	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColRegNumInfo; ++i)
	{
		col.fmt = colRegNumInfo[i].fmt;
		col.cx = colRegNumInfo[i].cx;
		col.pszText = _T("");
		col.iSubItem = i;
		m_ctrlRegNums.InsertColumn(i, &col);
	}
	m_ctrlRegNums.SuppressTooltipFixing(false);
	SetColumnRegNumHeaders();

	ListRegNums();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDogNumbers::OnColumnclickRegNums(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	SORTINFO si;
	si.pThis = this;
	m_sortRegNums.SetColumnOrder(pNMListView->iSubItem);
	SetColumnRegNumHeaders();
	si.pCols = &m_sortRegNums;
	m_ctrlRegNums.SortItems(CompareRegNums, reinterpret_cast<LPARAM>(&si));
	m_sortRegNums.Save();
	*pResult = 0;
}


void CDlgDogNumbers::OnDblclkRegNums(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnRegEdit();
	*pResult = 0;
}


void CDlgDogNumbers::OnKeydownRegNums(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnRegEdit();
		break;
	}
	*pResult = 0;
}


void CDlgDogNumbers::OnItemchangedRegNums(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}


void CDlgDogNumbers::OnRegNew()
{
	CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, ARBDogRegNumPtr(), this);
	if (IDOK == dlg.DoModal())
		ListRegNums();
}


void CDlgDogNumbers::OnRegEdit()
{
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogRegNumPtr>* pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, pRegNum, this);
		if (IDOK == dlg.DoModal())
			ListRegNums();
	}
}


void CDlgDogNumbers::OnRegDelete()
{
	int i = m_ctrlRegNums.GetSelection();
	if (0 <= i)
	{
		CListPtrData<ARBDogRegNumPtr>* pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		m_RegNums.DeleteRegNum(pRegNum->GetVenue(), pRegNum->GetNumber());
		m_ctrlRegNums.DeleteItem(i);
	}
}
