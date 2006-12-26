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
 * @brief implementation of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-19 DRC Added Location/Club note information.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTrial.h"

#include "AgilityBookDoc.h"
#include "ARBDogTrial.h"
#include "DlgClub.h"
#include "DlgInfoJudge.h"
#include "ListData.h"

using namespace std;

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
} const colInfo1[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_CLUB},
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
};
static int const nColInfo1 = sizeof(colInfo1) / sizeof(colInfo1[0]);

/////////////////////////////////////////////////////////////////////////////
// CDlgTrial dialog

CDlgTrial::CDlgTrial(
		CAgilityBookDoc* pDoc,
		ARBDogTrialPtr pTrial,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgTrial::IDD, pParent)
	, m_ctrlLocation(false)
	, m_ctrlClubs(true)
	, m_pDoc(pDoc)
	, m_pTrial(pTrial)
	, m_Clubs()
	, m_bRunsDeleted(false)
{
	pTrial->GetClubs().Clone(m_Clubs);
	//{{AFX_DATA_INIT(CDlgTrial)
	m_Verified = pTrial->IsVerified() ? TRUE : FALSE;
	m_Location = pTrial->GetLocation().c_str();
	m_Notes = pTrial->GetNote().c_str();
	//}}AFX_DATA_INIT
	m_Notes.Replace(_T("\n"), _T("\r\n"));
}

void CDlgTrial::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrial)
	DDX_Check(pDX, IDC_TRIAL_VERIFIED, m_Verified);
	DDX_CBString(pDX, IDC_TRIAL_LOCATION, m_Location);
	DDX_Control(pDX, IDC_TRIAL_LOCATION, m_ctrlLocation);
	DDX_Text(pDX, IDC_TRIAL_NOTES, m_Notes);
	DDX_Control(pDX, IDC_TRIAL_LOCATION_NOTES, m_ctrlLocationNotes);
	DDX_Control(pDX, IDC_TRIAL_LOCATION_INFO, m_ctrlLocationInfo);
	DDX_Control(pDX, IDC_TRIAL_CLUB_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_TRIAL_CLUB_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_TRIAL_CLUBS, m_ctrlClubs);
	DDX_Control(pDX, IDC_TRIAL_CLUB_NOTES, m_ctrlClubNotes);
	DDX_Control(pDX, IDC_TRIAL_CLUB_INFO, m_ctrlClubInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTrial, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgTrial)
	ON_CBN_SELCHANGE(IDC_TRIAL_LOCATION, OnSelchangeLocation)
	ON_CBN_KILLFOCUS(IDC_TRIAL_LOCATION, OnKillfocusLocation)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRIAL_CLUBS, OnItemchangedClubs)
	ON_NOTIFY(NM_DBLCLK, IDC_TRIAL_CLUBS, OnDblclkClubs)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TRIAL_CLUBS, OnKeydownClubs)
	ON_BN_CLICKED(IDC_TRIAL_LOCATION_NOTES, OnLocationNotes)
	ON_BN_CLICKED(IDC_TRIAL_CLUB_NOTES, OnClubNotes)
	ON_BN_CLICKED(IDC_TRIAL_CLUB_NEW, OnClubNew)
	ON_BN_CLICKED(IDC_TRIAL_CLUB_EDIT, OnClubEdit)
	ON_BN_CLICKED(IDC_TRIAL_CLUB_DELETE, OnClubDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

ARBDogClubPtr CDlgTrial::GetClubData(int index) const
{
	CListData* pData = m_ctrlClubs.GetData(index);
	return dynamic_cast<CListPtrData<ARBDogClubPtr>*>(pData)->GetData();
}

void CDlgTrial::UpdateNotes(
		bool bLocation,
		bool bClub)
{
	if (bLocation)
	{
		CString str;
		ARBInfoItemPtr pItem;
		if (m_pDoc->GetInfo().GetInfo(ARBInfo::eLocationInfo).FindItem((LPCTSTR)m_Location, &pItem))
		{
			str = pItem->GetComment().c_str();
			str.Replace(_T("\n"), _T("\r\n"));
		}
		m_ctrlLocationInfo.SetWindowText(str);
	}
	if (bClub)
	{
		CString str;
		int index = m_ctrlClubs.GetSelection();
		if (0 <= index)
		{
			ARBDogClubPtr pClub = GetClubData(index);
			if (pClub)
			{
				ARBInfoItemPtr pItem;
				if (m_pDoc->GetInfo().GetInfo(ARBInfo::eClubInfo).FindItem(pClub->GetName(), &pItem))
				{
					str = pItem->GetComment().c_str();
					str.Replace(_T("\n"), _T("\r\n"));
				}
			}
		}
		m_ctrlClubInfo.SetWindowText(str);
	}
}

void CDlgTrial::ListClubs()
{
	m_ctrlClubs.DeleteAllItems();
	int i = 0;
	for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); iter != m_Clubs.end(); ++i, ++iter)
	{
		ARBDogClubPtr pClub = (*iter);
		int nItem = m_ctrlClubs.InsertItem(i, pClub->GetName().c_str());
		m_ctrlClubs.SetItemText(nItem, 1, pClub->GetVenue().c_str());
		m_ctrlClubs.SetData(nItem,
			new CListPtrData<ARBDogClubPtr>(pClub));
	}
	for (i = 0; i < nColInfo1 ; ++i)
		m_ctrlClubs.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgTrial message handlers

BOOL CDlgTrial::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlClubs.SetExtendedStyle(m_ctrlClubs.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int i;
	for (i = 0; i < nColInfo1; ++i)
	{
		CString str;
		str.LoadString(colInfo1[i].idText);
		col.fmt = colInfo1[i].fmt;
		col.cx = colInfo1[i].cx;
		col.pszText = str.GetBuffer(0);
		col.iSubItem = i;
		m_ctrlClubs.InsertColumn(i, &col);
		str.ReleaseBuffer();
	}
	m_ctrlEdit.EnableWindow(FALSE);
	m_ctrlClubNotes.EnableWindow(FALSE);
	m_ctrlDelete.EnableWindow(FALSE);

	set<ARBString> locations;
	m_pDoc->GetAllTrialLocations(locations);
	for (set<ARBString>::const_iterator iter = locations.begin(); iter != locations.end(); ++iter)
	{
		int index = m_ctrlLocation.AddString((*iter).c_str());
		if ((*iter) == m_pTrial->GetLocation())
			m_ctrlLocation.SetCurSel(index);
	}
	ListClubs();
	UpdateNotes(true, true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTrial::OnSelchangeLocation()
{
	// This message is sent when the combo is about to change.
	int idx = m_ctrlLocation.GetCurSel();
	if (CB_ERR != idx)
		m_ctrlLocation.GetLBText(idx, m_Location);
	else
		m_Location.Empty();
	UpdateNotes(true, false);
}

void CDlgTrial::OnKillfocusLocation()
{
	UpdateData(TRUE);
	UpdateNotes(true, false);
}

void CDlgTrial::OnItemchangedClubs(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	//NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UINT selected = m_ctrlClubs.GetSelectedCount();
	ASSERT(1 >= selected);
	if (0 == selected)
	{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlClubNotes.EnableWindow(FALSE);
		m_ctrlDelete.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlEdit.EnableWindow(TRUE);
		m_ctrlClubNotes.EnableWindow(TRUE);
		m_ctrlDelete.EnableWindow(TRUE);
	}
	UpdateNotes(false, true);
	*pResult = 0;
}

void CDlgTrial::OnDblclkClubs(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	OnClubEdit();
	*pResult = 0;
}

void CDlgTrial::OnKeydownClubs(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnClubEdit();
		break;
	}
	*pResult = 0;
}

void CDlgTrial::OnLocationNotes()
{
	UpdateData(TRUE);
	m_Location.TrimRight();
	m_Location.TrimLeft();
	CDlgInfoJudge dlg(m_pDoc, ARBInfo::eLocationInfo, (LPCTSTR)m_Location, this);
	if (IDOK == dlg.DoModal())
	{
		UpdateNotes(true, false);
	}
}

void CDlgTrial::OnClubNotes()
{
	UpdateData(TRUE);
	int index = m_ctrlClubs.GetSelection();
	if (0 <= index)
	{
		ARBDogClubPtr pClub = GetClubData(index);
		CDlgInfoJudge dlg(m_pDoc, ARBInfo::eClubInfo, pClub->GetName(), this);
		if (IDOK == dlg.DoModal())
		{
			UpdateNotes(false, true);
		}
	}
}

void CDlgTrial::OnClubNew()
{
	CDlgClub dlg(m_pDoc, m_Clubs, ARBDogClubPtr(), this);
	if (IDOK == dlg.DoModal())
		ListClubs();
}

void CDlgTrial::OnClubEdit()
{
	int index = m_ctrlClubs.GetSelection();
	if (0 <= index)
	{
		ARBDogClubPtr pClub = GetClubData(index);
		CDlgClub dlg(m_pDoc, m_Clubs, pClub, this);
		if (IDOK == dlg.DoModal())
			ListClubs();
	}
}

void CDlgTrial::OnClubDelete()
{
	int index = m_ctrlClubs.GetSelection();
	if (0 <= index)
	{
		ARBDogClubPtr pClub = GetClubData(index);
		m_Clubs.DeleteClub(pClub->GetName(), pClub->GetVenue());
		m_ctrlClubs.DeleteItem(index);
	}
}

void CDlgTrial::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_Location.TrimRight();
	m_Location.TrimLeft();
	m_Notes.TrimRight();
	m_Notes.TrimLeft();

	if (m_Location.IsEmpty())
	{
		GotoDlgCtrl(&m_ctrlLocation);
		return;
	}

	// Before the final commit, verify the trial still makes sense!
	// A user could have changed the venue of a club, this will make
	// any existing runs possibly invalid.
	m_bRunsDeleted = false;
	if (0 < m_pTrial->GetRuns().size())
	{
		std::set<ARBString> oldVenues;
		std::set<ARBString> newVenues;
		ARBDogClubList::iterator iterClub;
		for (iterClub = m_pTrial->GetClubs().begin(); iterClub != m_pTrial->GetClubs().end(); ++iterClub)
		{
			ARBDogClubPtr pClub = *iterClub;
			oldVenues.insert(pClub->GetVenue());
		}
		for (iterClub = m_Clubs.begin(); iterClub != m_Clubs.end(); ++iterClub)
		{
			ARBDogClubPtr pClub = *iterClub;
			newVenues.insert(pClub->GetVenue());
		}
		bool bAllThere = true;
		for (std::set<ARBString>::iterator iterVenues = oldVenues.begin(); iterVenues != oldVenues.end(); ++iterVenues)
		{
			if (newVenues.end() == newVenues.find((*iterVenues)))
			{
				bAllThere = false;
				break;
			}
		}
		if (!bAllThere)
		{
			int nDelete = 0;
			for (ARBDogRunList::iterator iterRun = m_pTrial->GetRuns().begin();
			iterRun != m_pTrial->GetRuns().end();
			++iterRun)
			{
				ARBDogRunPtr pRun = *iterRun;
				bool bFound = false;
				for (std::set<ARBString>::iterator iterVenues = newVenues.begin(); iterVenues != newVenues.end(); ++iterVenues)
				{
					if (m_pDoc->GetConfig().GetVenues().FindEvent(
						(*iterVenues),
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel(),
						pRun->GetDate()))
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					++nDelete;
				}
			}
			if (0 < nDelete)
			{
				CString msg;
				msg.FormatMessage(IDS_CONFIG_DELETE_RUNS, m_pTrial->GetRuns().size());
				if (IDYES != AfxMessageBox(msg, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
					return;
				m_pTrial->GetRuns().clear();
				m_bRunsDeleted = true;
			}
		}
	}

	m_pTrial->SetLocation((LPCTSTR)m_Location);
	m_Notes.Replace(_T("\r\n"), _T("\n"));
	m_pTrial->SetNote((LPCTSTR)m_Notes);
	m_pTrial->SetVerified(m_Verified ? true : false);
	m_pTrial->GetClubs() = m_Clubs;
	CDlgBaseDialog::OnOK();
}
