/*
 * Copyright © 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgRunReference class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunReference.h"

#include "AgilityBookDoc.h"
#include "ARBDogRun.h"
#include "DlgReferenceRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	UINT idText;
	UINT fmt;
} const scColumns[] = {
	{IDS_COL_Q, LVCFMT_CENTER},
	{IDS_COL_PLACE, LVCFMT_CENTER},
	{IDS_COL_SCORE, LVCFMT_CENTER},
	{IDS_COL_TIME, LVCFMT_LEFT},
	{IDS_COL_NAME, LVCFMT_LEFT},
	{IDS_COL_BREED, LVCFMT_LEFT},
	{IDS_COL_NOTE, LVCFMT_LEFT},
};
static int const scNumColumns = sizeof(scColumns) / sizeof(scColumns[0]);

typedef struct
{
	CDlgRunReference *pThis;
	CColumnOrder *pCols;
} SORTINFO;

int CALLBACK CompareRefRuns(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	ARBDogReferenceRun const* pTitle1 = reinterpret_cast<ARBDogReferenceRun const*>(lParam1);
	ARBDogReferenceRun const* pTitle2 = reinterpret_cast<ARBDogReferenceRun const*>(lParam2);
	SORTINFO *psi = reinterpret_cast<SORTINFO*>(lParam3);
	int rc = 0;
	for (int i = 0; i < psi->pCols->GetSize(); ++i)
	{
		int col = psi->pCols->GetColumnAt(i);
		switch (col)
		{
		default:
		case 3: // Time
			if (pTitle1->GetTime() < pTitle2->GetTime())
				rc = -1;
			else if (pTitle1->GetTime() > pTitle2->GetTime())
				rc = 1;
			break;
		case 0: // Q
			if (pTitle1->GetQ() < pTitle2->GetQ())
				rc = -1;
			else if (pTitle1->GetQ() > pTitle2->GetQ())
				rc = 1;
			break;
		case 1: // Place
			if (pTitle1->GetPlace() < pTitle2->GetPlace())
				rc = -1;
			else if (pTitle1->GetPlace() > pTitle2->GetPlace())
				rc = 1;
			break;
		case 2: // Score
			if (pTitle1->GetScore() < pTitle2->GetScore())
				rc = -1;
			else if (pTitle1->GetScore() > pTitle2->GetScore())
				rc = 1;
			break;
		case 4: // Name
			if (pTitle1->GetName() < pTitle2->GetName())
				rc = -1;
			else if (pTitle1->GetName() > pTitle2->GetName())
				rc = 1;
			break;
		case 5: // Breed
			if (pTitle1->GetBreed() < pTitle2->GetBreed())
				rc = -1;
			else if (pTitle1->GetBreed() > pTitle2->GetBreed())
				rc = 1;
			break;
		case 6: // Notes
			if (pTitle1->GetNote() < pTitle2->GetNote())
				rc = -1;
			else if (pTitle1->GetNote() > pTitle2->GetNote())
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
// CDlgRunReference dialog

CDlgRunReference::CDlgRunReference(CAgilityBookDoc* pDoc, ARBConfigVenue const* pVenue, ARBDogRun* pRun)
	: CDlgBasePropertyPage(CDlgRunReference::IDD)
	, m_pDoc(pDoc)
	, m_Venue(pVenue)
	, m_Run(pRun)
	, m_sortRefRuns("RefRuns")
{
	m_sortRefRuns.Initialize(scNumColumns);
	//{{AFX_DATA_INIT(CDlgRunReference)
	//}}AFX_DATA_INIT
}

CDlgRunReference::~CDlgRunReference()
{
}

void CDlgRunReference::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunReference)
	DDX_Control(pDX, IDC_RUNREF_REF_RUNS, m_ctrlRefRuns);
	DDX_Control(pDX, IDC_RUNREF_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_RUNREF_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_RUNREF_DELETE, m_ctrlDelete);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRunReference, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgRunReference)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_RUNREF_REF_RUNS, OnGetdispinfoRefRuns)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_RUNREF_REF_RUNS, OnColumnclickRefRuns)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RUNREF_REF_RUNS, OnItemchangedRefRuns)
	ON_NOTIFY(NM_DBLCLK, IDC_RUNREF_REF_RUNS, OnDblclkRefRuns)
	ON_BN_CLICKED(IDC_RUNREF_NEW, OnRefRunNew)
	ON_BN_CLICKED(IDC_RUNREF_EDIT, OnRefRunEdit)
	ON_BN_CLICKED(IDC_RUNREF_DELETE, OnRefRunDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgRunReference::UpdateButtons()
{
	if (0 <= m_ctrlRefRuns.GetSelection())
	{
		m_ctrlEdit.EnableWindow(TRUE);
		m_ctrlDelete.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlDelete.EnableWindow(FALSE);
	}
}

void CDlgRunReference::SetColumnHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < scNumColumns; ++i)
	{
		CString str;
		str.LoadString(scColumns[i].idText);
		CString order;
		order.Format(_T("%c %s (%d)"),
			m_sortRefRuns.IsDescending(i) ? '<' : '>',
			(LPCTSTR)str,
			m_sortRefRuns.FindColumnOrder(i) + 1);
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlRefRuns.SetColumn(i, &col);
		order.ReleaseBuffer();
	}
}

void CDlgRunReference::ListRuns()
{
	ARBDogReferenceRun const* pSelected = NULL;
	int index = m_ctrlRefRuns.GetSelection();
	if (0 <= index)
		pSelected = reinterpret_cast<ARBDogReferenceRun const*>(m_ctrlRefRuns.GetItemData(index));
	m_ctrlRefRuns.DeleteAllItems();

	index = 0;
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
	iterRef != m_Run->GetReferenceRuns().end();
	++index, ++iterRef)
	{
		ARBDogReferenceRun* pRef = (*iterRef);
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = index;
		item.iSubItem = 0;
		item.lParam = reinterpret_cast<LPARAM>(pRef);
		m_ctrlRefRuns.InsertItem(&item);
	}
	for (index = 0; index < scNumColumns; ++index)
		m_ctrlRefRuns.SetColumnWidth(index, LVSCW_AUTOSIZE_USEHEADER);
	SORTINFO si;
	si.pThis = this;
	si.pCols = &m_sortRefRuns;
	m_ctrlRefRuns.SortItems(CompareRefRuns, reinterpret_cast<LPARAM>(&si));

	if (pSelected)
	{
		for (index = 0; index < m_ctrlRefRuns.GetItemCount(); ++index)
		{
			ARBDogReferenceRun const* pRegRun = reinterpret_cast<ARBDogReferenceRun const*>(m_ctrlRefRuns.GetItemData(index));
			if (pRegRun == pSelected) // compare by ptr is fine.
			{
				m_ctrlRefRuns.SetSelection(index, true);
				break;
			}
		}
	}
	UpdateButtons();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunReference message handlers

BOOL CDlgRunReference::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlRefRuns.SetExtendedStyle(m_ctrlRefRuns.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	int index;
	for (index = 0; index < scNumColumns; ++index)
	{
		CString str;
		str.LoadString(scColumns[index].idText);
		col.fmt = scColumns[index].fmt;
		col.pszText = str.GetBuffer(0);
		col.iSubItem = index;
		m_ctrlRefRuns.InsertColumn(index, &col);
		str.ReleaseBuffer();
	}
	SetColumnHeaders();
	ListRuns();
	for (index = 0; index < scNumColumns; ++index)
		m_ctrlRefRuns.SetColumnWidth(index, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRunReference::OnGetdispinfoRefRuns(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		ARBDogReferenceRun *pRef = reinterpret_cast<ARBDogReferenceRun*>(pDispInfo->item.lParam);
		CString str("");
		switch (pDispInfo->item.iSubItem)
		{
		default:
			break;
		case 0: // Q
			str = pRef->GetQ().str().c_str();
			break;
		case 1: // Place
			str.Format("%hd", pRef->GetPlace());
			break;
		case 2: // Score
			str = pRef->GetScore().c_str();
			break;
		case 3: // Time
			str = pRef->GetTime().str().c_str();
			break;
		case 4: // Name
			str = pRef->GetName().c_str();
			break;
		case 5: // Breed
			str = pRef->GetBreed().c_str();
			break;
		case 6: // Note
			str = pRef->GetNote().c_str();
			str.Replace("\n", " ");
			break;
		}
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}

void CDlgRunReference::OnColumnclickRefRuns(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SORTINFO si;
	si.pThis = this;
	m_sortRefRuns.SetColumnOrder(pNMListView->iSubItem);
	SetColumnHeaders();
	si.pCols = &m_sortRefRuns;
	m_ctrlRefRuns.SortItems(CompareRefRuns, reinterpret_cast<LPARAM>(&si));
	m_sortRefRuns.Save();
	*pResult = 0;
}

void CDlgRunReference::OnItemchangedRefRuns(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgRunReference::OnDblclkRefRuns(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRefRunEdit();
	*pResult = 0;
}

void CDlgRunReference::OnRefRunNew() 
{
	ARBDogReferenceRun* ref = new ARBDogReferenceRun();
	if (ARBDogRunScoring::eTypeByTime == m_Run->GetScoring().GetType())
	{
		ARBConfigScoring const* pScoring = m_pDoc->GetConfig().GetVenues().FindEvent(
			m_Venue->GetName(),
			m_Run->GetEvent(),
			m_Run->GetDivision(),
			m_Run->GetLevel(),
			m_Run->GetDate());
		if (pScoring)
		{
			std::string nScore;
			switch (pScoring->GetScoringStyle())
			{
			default:
				nScore = "0";
				break;
			case ARBConfigScoring::eFaults100ThenTime:
				nScore = "100";
				break;
			case ARBConfigScoring::eFaults200ThenTime:
				nScore = "200";
				break;
			}
			ref->SetScore(nScore);
		}
	}
	CDlgReferenceRun dlg(ref, this);
	if (IDOK == dlg.DoModal())
	{
		ARBDogReferenceRun* pRef = m_Run->GetReferenceRuns().AddReferenceRun(ref);
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = m_ctrlRefRuns.GetItemCount();
		item.iSubItem = 0;
		item.lParam = reinterpret_cast<LPARAM>(pRef);
		int index = m_ctrlRefRuns.InsertItem(&item);
		m_ctrlRefRuns.SetSelection(index);
		ListRuns();
	}
	ref->Release();
}

void CDlgRunReference::OnRefRunEdit() 
{
	int nItem = m_ctrlRefRuns.GetSelection();
	if (0 <= nItem)
	{
		ARBDogReferenceRun *pRef = reinterpret_cast<ARBDogReferenceRun*>(m_ctrlRefRuns.GetItemData(nItem));
		CDlgReferenceRun dlg(pRef, this);
		if (IDOK == dlg.DoModal())
			ListRuns();
	}
}

void CDlgRunReference::OnRefRunDelete() 
{
	int nItem = m_ctrlRefRuns.GetSelection();
	if (0 <= nItem)
	{
		ARBDogReferenceRun *pRef = reinterpret_cast<ARBDogReferenceRun*>(m_ctrlRefRuns.GetItemData(nItem));
		if (m_Run->GetReferenceRuns().DeleteReferenceRun(pRef))
			m_ctrlRefRuns.DeleteItem(nItem);
		UpdateButtons();
	}
}
