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
 * @brief implementation of the CDlgRunReference class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-09-20 DRC Added yourself was not getting up-to-date scoring info.
 * @li 2005-07-10 DRC Add button to add yourself to ref-runs.
 *                    Make default ref-run a 'Q'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Accumulate all heights for refrun dlg.
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
	{IDS_COL_HEIGHT, LVCFMT_LEFT},
	{IDS_COL_BREED, LVCFMT_LEFT},
	{IDS_COL_NOTE, LVCFMT_LEFT},
};
static int const scNumColumns = sizeof(scColumns) / sizeof(scColumns[0]);


typedef struct
{
	CDlgRunReference* pThis;
	CColumnOrder* pCols;
} SORTINFO;


int CALLBACK CompareRefRuns(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	CListData* pRawTitle1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawTitle2 = reinterpret_cast<CListData*>(lParam2);
	ARBDogReferenceRunPtr pTitle1 = dynamic_cast<CListPtrData<ARBDogReferenceRunPtr>*>(pRawTitle1)->GetData();
	ARBDogReferenceRunPtr pTitle2 = dynamic_cast<CListPtrData<ARBDogReferenceRunPtr>*>(pRawTitle2)->GetData();
	SORTINFO* psi = reinterpret_cast<SORTINFO*>(lParam3);
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
		case 5: // Height
			if (pTitle1->GetHeight() < pTitle2->GetHeight())
				rc = -1;
			else if (pTitle1->GetHeight() > pTitle2->GetHeight())
				rc = 1;
			break;
		case 6: // Breed
			if (pTitle1->GetBreed() < pTitle2->GetBreed())
				rc = -1;
			else if (pTitle1->GetBreed() > pTitle2->GetBreed())
				rc = 1;
			break;
		case 7: // Notes
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

CDlgRunReference::CDlgRunReference(
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr pVenue,
		ARBDogRunPtr pRun)
	: CDlgBasePropertyPage(CDlgRunReference::IDD)
	, m_ctrlRefRuns(true)
	, m_pDoc(pDoc)
	, m_Venue(pVenue)
	, m_Run(pRun)
	, m_pRefRunMe()
	, m_sortRefRuns(_T("RefRuns"))
{
	ASSERT(NULL != m_Venue.get());
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
	DDX_Control(pDX, IDC_RUNREF_ADDDOG, m_ctrlAdd);
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
	ON_NOTIFY(LVN_KEYDOWN, IDC_RUNREF_REF_RUNS, OnKeydownRefRuns)
	ON_BN_CLICKED(IDC_RUNREF_ADDDOG, OnRefRunAdd)
	ON_BN_CLICKED(IDC_RUNREF_NEW, OnRefRunNew)
	ON_BN_CLICKED(IDC_RUNREF_EDIT, OnRefRunEdit)
	ON_BN_CLICKED(IDC_RUNREF_DELETE, OnRefRunDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CListPtrData<ARBDogReferenceRunPtr>* CDlgRunReference::GetReferenceData(int index) const
{
	CListData* pData = m_ctrlRefRuns.GetData(index);
	return dynamic_cast<CListPtrData<ARBDogReferenceRunPtr>*>(pData);
}


bool CDlgRunReference::IsRefRunMe()
{
	if (m_pRefRunMe)
	{
		for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
			iterRef != m_Run->GetReferenceRuns().end();
			++iterRef)
		{
			if (*iterRef == m_pRefRunMe)
			{
				return true;
			}
		}
	}
	return false;
}


void CDlgRunReference::CreateRefRunMe()
{
	// Create the 'me' reference run.
	m_pRefRunMe = ARBDogReferenceRunPtr(ARBDogReferenceRun::New());
	m_pRefRunMe->SetQ(m_Run->GetQ());
	m_pRefRunMe->SetPlace(m_Run->GetPlace());
	m_pRefRunMe->SetName(m_pDoc->GetCurrentDog()->GetCallName());
	m_pRefRunMe->SetHeight(m_Run->GetHeight());
	m_pRefRunMe->SetBreed(m_pDoc->GetCurrentDog()->GetBreed());
	m_pRefRunMe->SetTime(m_Run->GetScoring().GetTime());
	ARBConfigScoringPtr pScoring;
	if (m_pDoc->GetConfig().GetVenues().FindEvent(
		m_Venue->GetName(),
		m_Run->GetEvent(),
		m_Run->GetDivision(),
		m_Run->GetLevel(),
		m_Run->GetDate(),
		NULL,
		&pScoring))
	{
		m_pRefRunMe->SetScore(ARBDouble::str(m_Run->GetScore(pScoring)));
	}

	// Now see if I'm already in there.
	// Only compare: Q/Place/Name/Time/Score.
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
		iterRef != m_Run->GetReferenceRuns().end();
		++iterRef)
	{
		ARBDogReferenceRunPtr pRef = *iterRef;
		if (pRef->GetQ() == m_pRefRunMe->GetQ()
		&& pRef->GetPlace() == m_pRefRunMe->GetPlace()
		&& pRef->GetName() == m_pRefRunMe->GetName()
		&& ARBDouble::equal(pRef->GetTime(), m_pRefRunMe->GetTime())
		&& pRef->GetScore() == m_pRefRunMe->GetScore())
		{
			m_pRefRunMe = pRef;
			break;
		}
	}
}


void CDlgRunReference::UpdateButtons()
{
	m_ctrlAdd.EnableWindow(IsRefRunMe() ? FALSE : TRUE);
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
		otstringstream tmp;
		tmp << (LPCTSTR)str << _T(" (" << m_sortRefRuns.FindColumnOrder(i) + 1) << ')';
		CString order(tmp.str().c_str());
		col.pszText = order.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlRefRuns.SetColumn(i, &col);
		m_ctrlRefRuns.HeaderSort(i, m_sortRefRuns.IsDescending(i) ? CHeaderCtrl2::eDescending : CHeaderCtrl2::eAscending);
		order.ReleaseBuffer();
	}
}


void CDlgRunReference::ListRuns()
{
	ARBDogReferenceRunPtr pSelected;
	int index = m_ctrlRefRuns.GetSelection();
	if (0 <= index)
	{
		CListPtrData<ARBDogReferenceRunPtr>* pData = GetReferenceData(index);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlRefRuns.DeleteAllItems();

	index = 0;
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
	iterRef != m_Run->GetReferenceRuns().end();
	++index, ++iterRef)
	{
		ARBDogReferenceRunPtr pRef = (*iterRef);
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iItem = index;
		item.iSubItem = 0;
		item.lParam = reinterpret_cast<LPARAM>(
			static_cast<CListData*>(
				new CListPtrData<ARBDogReferenceRunPtr>(pRef)));
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
			ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
			if (pRefRun == pSelected) // compare by ptr is fine.
			{
				m_ctrlRefRuns.SetSelection(index, true);
				break;
			}
		}
	}
	UpdateButtons();
}


void CDlgRunReference::GetAllHeights(std::set<tstring>& outNames)
{
	m_pDoc->GetAllHeights(outNames);
	for (int index = 0; index < m_ctrlRefRuns.GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetHeight();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRunReference::GetAllCallNames(std::set<tstring>& outNames)
{
	m_pDoc->GetAllCallNames(outNames);
	for (int index = 0; index < m_ctrlRefRuns.GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetName();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRunReference::GetAllBreeds(std::set<tstring>& outNames)
{
	m_pDoc->GetAllBreeds(outNames);
	for (int index = 0; index < m_ctrlRefRuns.GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetBreed();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunReference message handlers

BOOL CDlgRunReference::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlRefRuns.SetExtendedStyle(m_ctrlRefRuns.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

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


BOOL CDlgRunReference::OnSetActive()
{
	CreateRefRunMe();
	UpdateButtons();
	return CDlgBasePropertyPage::OnSetActive();
}


void CDlgRunReference::OnGetdispinfoRefRuns(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CListPtrData<ARBDogReferenceRunPtr>* pData = reinterpret_cast<CListPtrData<ARBDogReferenceRunPtr>*>(pDispInfo->item.lParam);
		ARBDogReferenceRunPtr pRef = pData->GetData();
		CString str(_T(""));
		switch (pDispInfo->item.iSubItem)
		{
		default:
			break;
		case 0: // Q
			str = pRef->GetQ().str().c_str();
			break;
		case 1: // Place
			{
				otstringstream tmp;
				tmp << pRef->GetPlace();
				str = tmp.str().c_str();
			}
			break;
		case 2: // Score
			str = pRef->GetScore().c_str();
			break;
		case 3: // Time
			str = ARBDouble::str(pRef->GetTime()).c_str();
			break;
		case 4: // Name
			str = pRef->GetName().c_str();
			break;
		case 5: // Height
			str = pRef->GetHeight().c_str();
			break;
		case 6: // Breed
			str = pRef->GetBreed().c_str();
			break;
		case 7: // Note
			str = pRef->GetNote().c_str();
			str.Replace(_T("\n"), _T(" "));
			break;
		}
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	*pResult = 0;
}


void CDlgRunReference::OnColumnclickRefRuns(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	SORTINFO si;
	si.pThis = this;
	m_sortRefRuns.SetColumnOrder(pNMListView->iSubItem);
	SetColumnHeaders();
	si.pCols = &m_sortRefRuns;
	m_ctrlRefRuns.SortItems(CompareRefRuns, reinterpret_cast<LPARAM>(&si));
	m_sortRefRuns.Save();
	*pResult = 0;
}


void CDlgRunReference::OnItemchangedRefRuns(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}


void CDlgRunReference::OnDblclkRefRuns(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnRefRunEdit();
	*pResult = 0;
}


void CDlgRunReference::OnKeydownRefRuns(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnRefRunEdit();
		break;
	}
	*pResult = 0;
}


void CDlgRunReference::OnRefRunAdd()
{
	if (!IsRefRunMe())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(m_pRefRunMe))
		{
			LV_ITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.iItem = m_ctrlRefRuns.GetItemCount();
			item.iSubItem = 0;
			item.lParam = reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CListPtrData<ARBDogReferenceRunPtr>(m_pRefRunMe)));
			int index = m_ctrlRefRuns.InsertItem(&item);
			m_ctrlRefRuns.SetSelection(index);
			ListRuns();
		}
	}
}


void CDlgRunReference::OnRefRunNew() 
{
	ARBDogReferenceRunPtr ref(ARBDogReferenceRun::New());
	if (ARBDogRunScoring::eTypeByTime == m_Run->GetScoring().GetType())
	{
		ARBConfigScoringPtr pScoring;
		if (m_pDoc->GetConfig().GetVenues().FindEvent(
			m_Venue->GetName(),
			m_Run->GetEvent(),
			m_Run->GetDivision(),
			m_Run->GetLevel(),
			m_Run->GetDate(),
			NULL,
			&pScoring))
		{
			tstring nScore;
			switch (pScoring->GetScoringStyle())
			{
			default:
				nScore = _T("0");
				break;
			case ARBConfigScoring::eFaults100ThenTime:
				nScore = _T("100");
				break;
			case ARBConfigScoring::eFaults200ThenTime:
				nScore = _T("200");
				break;
			}
			ref->SetScore(nScore);
		}
	}
	ref->SetQ(ARB_Q::eQ_Q);
	std::set<tstring> heights, names, breeds;
	GetAllHeights(heights);
	GetAllCallNames(names);
	GetAllBreeds(breeds);
	CDlgReferenceRun dlg(m_pDoc, heights, names, breeds, ref, this);
	if (IDOK == dlg.DoModal())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(ref))
		{
			LV_ITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.iItem = m_ctrlRefRuns.GetItemCount();
			item.iSubItem = 0;
			item.lParam = reinterpret_cast<LPARAM>(
				static_cast<CListData*>(
					new CListPtrData<ARBDogReferenceRunPtr>(ref)));
			int index = m_ctrlRefRuns.InsertItem(&item);
			m_ctrlRefRuns.SetSelection(index);
			ListRuns();
		}
	}
}


void CDlgRunReference::OnRefRunEdit() 
{
	int nItem = m_ctrlRefRuns.GetSelection();
	if (0 <= nItem)
	{
		std::set<tstring> heights, names, breeds;
		GetAllHeights(heights);
		GetAllCallNames(names);
		GetAllBreeds(breeds);
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		CDlgReferenceRun dlg(m_pDoc, heights, names, breeds, pRef, this);
		if (IDOK == dlg.DoModal())
			ListRuns();
	}
}


void CDlgRunReference::OnRefRunDelete() 
{
	int nItem = m_ctrlRefRuns.GetSelection();
	if (0 <= nItem)
	{
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		if (pRef)
		{
			if (m_Run->GetReferenceRuns().DeleteReferenceRun(pRef))
				m_ctrlRefRuns.DeleteItem(nItem);
			if (m_pRefRunMe && m_pRefRunMe == pRef)
				CreateRefRunMe();
		}
		UpdateButtons();
	}
}
