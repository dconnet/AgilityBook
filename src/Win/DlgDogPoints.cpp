/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgDogPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-02-03 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDogPoints.h"

#include "AgilityBookDoc.h"
#include "ARBDate.h"
#include "ARBDog.h"
#include "DlgExistingPoints.h"

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
} const colExistingPointsInfo[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_DATE},
	{LVCFMT_LEFT, 50, IDS_COL_TYPE},
	{LVCFMT_LEFT, 50, IDS_COL_POINTS},
	{LVCFMT_LEFT, 50, IDS_OTHERPOINTS},
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
	{LVCFMT_LEFT, 50, IDS_COL_MULTIQ},
	{LVCFMT_LEFT, 50, IDS_COL_DIVISION},
	{LVCFMT_LEFT, 50, IDS_COL_LEVEL},
	{LVCFMT_LEFT, 50, IDS_COL_EVENT},
	{LVCFMT_LEFT, 50, IDS_COL_SUBNAME},
	{LVCFMT_LEFT, 50, IDS_COL_COMMENTS},
};
static int const nColExistingPointsInfo = sizeof(colExistingPointsInfo) / sizeof(colExistingPointsInfo[0]);

typedef struct
{
	CDlgDogPoints *pThis;
	CColumnOrder *pCols;
} SORTINFO;

/////////////////////////////////////////////////////////////////////////////

int CALLBACK ComparePoints(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	ARBDogExistingPoints* pExistingPoints1 = reinterpret_cast<ARBDogExistingPoints*>(lParam1);
	ARBDogExistingPoints* pExistingPoints2 = reinterpret_cast<ARBDogExistingPoints*>(lParam2);
	SORTINFO *psi = reinterpret_cast<SORTINFO*>(lParam3);
	int rc = 0;
	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{

		default:
		case 0: // date
			if (pExistingPoints1->GetDate() < pExistingPoints2->GetDate())
				rc = -1;
			else if (pExistingPoints1->GetDate() > pExistingPoints2->GetDate())
				rc = 1;
			break;
		case 1: // Type
			if (pExistingPoints1->GetType() < pExistingPoints2->GetType())
				rc = -1;
			else if (pExistingPoints1->GetType() > pExistingPoints2->GetType())
				rc = 1;
			break;
		case 2: // Points
			if (pExistingPoints1->GetPoints() < pExistingPoints2->GetPoints())
				rc = -1;
			else if (pExistingPoints1->GetPoints() > pExistingPoints2->GetPoints())
				rc = 1;
			break;
		case 3: // Other Points
			if (pExistingPoints1->GetOtherPoints() < pExistingPoints2->GetOtherPoints())
				rc = -1;
			else if (pExistingPoints1->GetOtherPoints() > pExistingPoints2->GetOtherPoints())
				rc = 1;
			break;
		case 4: // Venue
			if (pExistingPoints1->GetVenue() < pExistingPoints2->GetVenue())
				rc = -1;
			else if (pExistingPoints1->GetVenue() > pExistingPoints2->GetVenue())
				rc = 1;
			break;
		case 5: // MultiQ
			if (pExistingPoints1->GetMultiQ() < pExistingPoints2->GetMultiQ())
				rc = -1;
			else if (pExistingPoints1->GetMultiQ() > pExistingPoints2->GetMultiQ())
				rc = 1;
			break;
		case 6: // Division
			if (pExistingPoints1->GetDivision() < pExistingPoints2->GetDivision())
				rc = -1;
			else if (pExistingPoints1->GetDivision() > pExistingPoints2->GetDivision())
				rc = 1;
			break;
		case 7: // Level
			if (pExistingPoints1->GetLevel() < pExistingPoints2->GetLevel())
				rc = -1;
			else if (pExistingPoints1->GetLevel() > pExistingPoints2->GetLevel())
				rc = 1;
			break;
		case 8: // Event
			if (pExistingPoints1->GetEvent() < pExistingPoints2->GetEvent())
				rc = -1;
			else if (pExistingPoints1->GetEvent() > pExistingPoints2->GetEvent())
				rc = 1;
			break;
		case 9: // Subname
			if (pExistingPoints1->GetSubName() < pExistingPoints2->GetSubName())
				rc = -1;
			else if (pExistingPoints1->GetSubName() > pExistingPoints2->GetSubName())
				rc = 1;
			break;
		case 10: // Comment
			if (pExistingPoints1->GetComment() < pExistingPoints2->GetComment())
				rc = -1;
			else if (pExistingPoints1->GetComment() > pExistingPoints2->GetComment())
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
// CDlgDogPoints dialog

CDlgDogPoints::CDlgDogPoints(
		CAgilityBookDoc* pDoc,
		ARBDogExistingPointsList const& points)
	: CDlgBasePropertyPage(CDlgDogPoints::IDD)
	, m_pDoc(pDoc)
	, m_sortPoints("ExistingPoints")
	, m_ExistingPoints(points)
{
	m_sortPoints.Initialize(nColExistingPointsInfo);
	//{{AFX_DATA_INIT(CDlgDogPoints)
	//}}AFX_DATA_INIT
}

void CDlgDogPoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDogPoints)
	DDX_Control(pDX, IDC_DOGPTS_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_DOGPTS_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_DOGPTS_POINTS, m_ctrlPoints);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDogPoints, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgDogPoints)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DOGPTS_POINTS, OnColumnclickExistingPoints)
	ON_NOTIFY(NM_DBLCLK, IDC_DOGPTS_POINTS, OnDblclkExistingPoints)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_DOGPTS_POINTS, OnGetdispinfoExistingPoints)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOGPTS_POINTS, OnItemchangedExistingPoints)
	ON_BN_CLICKED(IDC_DOGPTS_NEW, OnNew)
	ON_BN_CLICKED(IDC_DOGPTS_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DOGPTS_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgDogPoints::SetColumnHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < nColExistingPointsInfo; ++i)
	{
		CString str;
		str.LoadString(colExistingPointsInfo[i].idText);
		CString order;
		order.Format(_T("%s (%d)"),
			(LPCTSTR)str,
			m_sortPoints.FindColumnOrder(i) + 1);
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlPoints.SetColumn(i, &col);
		m_ctrlPoints.HeaderSort(i, m_sortPoints.IsDescending(i) ? CHeaderCtrl2::eDescending : CHeaderCtrl2::eAscending);
		order.ReleaseBuffer();
	}
}

void CDlgDogPoints::ListExistingPoints()
{
	ARBDogExistingPoints const* pSelected = NULL;
	int i = m_ctrlPoints.GetSelection();
	if (0 <= i)
		pSelected = reinterpret_cast<ARBDogExistingPoints const*>(m_ctrlPoints.GetItemData(i));
	m_ctrlPoints.DeleteAllItems();
	i = 0;
	for (ARBDogExistingPointsList::const_iterator iterExistingPoints = m_ExistingPoints.begin();
		iterExistingPoints != m_ExistingPoints.end();
		++i, ++iterExistingPoints)
	{
		ARBDogExistingPoints const* pExistingPoints = (*iterExistingPoints);
		m_ctrlPoints.InsertItem(LVIF_TEXT | LVIF_PARAM, m_ctrlPoints.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(pExistingPoints));
	}
	for (i = 0; i < nColExistingPointsInfo; ++i)
		m_ctrlPoints.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pThis = this;
	si.pCols = &m_sortPoints;
	m_ctrlPoints.SortItems(ComparePoints, reinterpret_cast<LPARAM>(&si));
	if (pSelected)
	{
		for (i = 0; i < m_ctrlPoints.GetItemCount(); ++i)
		{
			ARBDogExistingPoints const* pExistingPoints = reinterpret_cast<ARBDogExistingPoints const*>(m_ctrlPoints.GetItemData(i));
			if (pExistingPoints == pSelected) // compare by ptr is fine.
			{
				m_ctrlPoints.SetSelection(i, true);
				break;
			}
		}
	}
}

void CDlgDogPoints::UpdateButtons()
{
	UINT selected = m_ctrlPoints.GetSelectedCount();
	ASSERT(1 >= selected);
	if (0 == selected)
	{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlDelete.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlEdit.EnableWindow(TRUE);
		m_ctrlDelete.EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDogPoints message handlers

BOOL CDlgDogPoints::OnInitDialog()
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlPoints.SetExtendedStyle(m_ctrlPoints.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int i;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for (i = 0; i < nColExistingPointsInfo; ++i)
	{
		col.fmt = colExistingPointsInfo[i].fmt;
		col.cx = colExistingPointsInfo[i].cx;
		col.pszText = _T("");
		col.iSubItem = i;
		m_ctrlPoints.InsertColumn(i, &col);
	}
	SetColumnHeaders();

	ListExistingPoints();

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDogPoints::OnColumnclickExistingPoints(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	SORTINFO si;
	si.pThis = this;
	m_sortPoints.SetColumnOrder(pNMListView->iSubItem);
	SetColumnHeaders();
	si.pCols = &m_sortPoints;
	m_ctrlPoints.SortItems(ComparePoints, reinterpret_cast<LPARAM>(&si));
	m_sortPoints.Save();
	*pResult = 0;
}

void CDlgDogPoints::OnDblclkExistingPoints(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgDogPoints::OnGetdispinfoExistingPoints(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		ARBDogExistingPoints const* pData = reinterpret_cast<ARBDogExistingPoints const*>(pDispInfo->item.lParam);
		CString str;
		switch (pDispInfo->item.iSubItem)
		{
		case 0:
			str = pData->GetDate().GetString(ARBDate::eDashYMD).c_str();
			break;
		case 1: // Type
			str = ARBDogExistingPoints::GetPointTypeName(pData->GetType()).c_str();
			break;
		case 2: // Points
			str.Format("%hd", pData->GetPoints());
			break;
		case 3: // Other Points
			str = pData->GetOtherPoints().c_str();
			break;
		case 4: // Venue
			str = pData->GetVenue().c_str();
			break;
		case 5: // MultiQ
			str = pData->GetMultiQ().c_str();
			break;
		case 6: // Division
			str = pData->GetDivision().c_str();
			break;
		case 7: // Level
			str = pData->GetLevel().c_str();
			break;
		case 8: // Event
			str = pData->GetEvent().c_str();
			break;
		case 9: // SubName
			str = pData->GetSubName().c_str();
			break;
		case 10: // Comment
			str = pData->GetComment().c_str();
			str.Replace("\n", " ");
			break;
		}
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}

void CDlgDogPoints::OnItemchangedExistingPoints(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}

void CDlgDogPoints::OnNew()
{
	CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, NULL, this);
	if (IDOK == dlg.DoModal())
		ListExistingPoints();
}

void CDlgDogPoints::OnEdit()
{
	int i = m_ctrlPoints.GetSelection();
	if (0 <= i)
	{
		ARBDogExistingPoints* pExistingPoints = reinterpret_cast<ARBDogExistingPoints*>(m_ctrlPoints.GetItemData(i));
		CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, pExistingPoints, this);
		if (IDOK == dlg.DoModal())
			ListExistingPoints();
	}
}

void CDlgDogPoints::OnDelete()
{
	int i = m_ctrlPoints.GetSelection();
	if (0 <= i)
	{
		ARBDogExistingPoints const* pExistingPoints = reinterpret_cast<ARBDogExistingPoints const*>(m_ctrlPoints.GetItemData(i));
		m_ExistingPoints.DeleteExistingPoints(pExistingPoints);
		m_ctrlPoints.DeleteItem(i);
	}
}
