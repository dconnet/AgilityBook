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
 * @brief implementation of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTrial.h"

#include "AgilityBookDoc.h"
#include "ARBDogTrial.h"
#include "DlgClub.h"

using namespace std;

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
} colInfo1[] =
{
	{LVCFMT_LEFT, 50, IDS_COL_CLUB},
	{LVCFMT_LEFT, 50, IDS_COL_VENUE},
};
static const int nColInfo1 = sizeof(colInfo1) / sizeof(colInfo1[0]);

/////////////////////////////////////////////////////////////////////////////
// CDlgTrial dialog

CDlgTrial::CDlgTrial(CAgilityBookDoc* pDoc, ARBDogTrial* pTrial, CWnd* pParent)
	: CDialog(CDlgTrial::IDD, pParent)
	, m_pDoc(pDoc)
	, m_pTrial(pTrial)
	, m_Clubs(pTrial->GetClubs())
	, m_bRunsDeleted(false)
{
	//{{AFX_DATA_INIT(CDlgTrial)
	m_Location = pTrial->GetLocation().c_str();
	m_Notes = pTrial->GetNote().c_str();
	m_Verified = pTrial->IsVerified() ? TRUE : FALSE;
	//}}AFX_DATA_INIT
	m_Notes.Replace("\n", "\r\n");
}

void CDlgTrial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrial)
	DDX_CBString(pDX, IDC_LOCATION, m_Location);
	DDX_Control(pDX, IDC_LOCATION, m_ctrlLocation);
	DDX_Text(pDX, IDC_NOTES, m_Notes);
	DDX_Control(pDX, IDC_CLUB_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CLUB_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CLUBS, m_ctrlClubs);
	DDX_Check(pDX, IDC_VERIFIED, m_Verified);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTrial, CDialog)
	//{{AFX_MSG_MAP(CDlgTrial)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CLUBS, OnItemchangedClubs)
	ON_NOTIFY(NM_DBLCLK, IDC_CLUBS, OnDblclkClubs)
	ON_BN_CLICKED(IDC_CLUB_NEW, OnClubNew)
	ON_BN_CLICKED(IDC_CLUB_EDIT, OnClubEdit)
	ON_BN_CLICKED(IDC_CLUB_DELETE, OnClubDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgTrial::UpdateButtons()
{
	UINT selected = m_ctrlClubs.GetSelectedCount();
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

void CDlgTrial::ListClubs()
{
	m_ctrlClubs.DeleteAllItems();
	int i = 0;
	for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); iter != m_Clubs.end(); ++i, ++iter)
	{
		const ARBDogClub* pClub = (*iter);
		int nItem = m_ctrlClubs.InsertItem(i, pClub->GetName().c_str());
		m_ctrlClubs.SetItemText(nItem, 1, pClub->GetVenue().c_str());
		m_ctrlClubs.SetItemData(nItem, reinterpret_cast<LPARAM>(pClub));
	}
	for (i = 0; i < nColInfo1 ; ++i)
		m_ctrlClubs.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgTrial message handlers

BOOL CDlgTrial::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlClubs.SetExtendedStyle(m_ctrlClubs.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

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

	set<string> locations;
	m_pDoc->GetAllTrialLocations(locations);
	for (set<string>::const_iterator iter = locations.begin(); iter != locations.end(); ++iter)
	{
		int index = m_ctrlLocation.AddString((*iter).c_str());
		if ((*iter) == m_pTrial->GetLocation())
			m_ctrlLocation.SetCurSel(index);
	}
	ListClubs();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTrial::OnItemchangedClubs(NMHDR* pNMHDR, LRESULT* pResult)
{
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgTrial::OnDblclkClubs(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnClubEdit();
	*pResult = 0;
}

void CDlgTrial::OnClubNew()
{
	CDlgClub dlg(m_pDoc, m_Clubs, NULL, this);
	if (IDOK == dlg.DoModal())
		ListClubs();
}

void CDlgTrial::OnClubEdit()
{
	int index = m_ctrlClubs.GetSelection();
	if (0 <= index)
	{
		ARBDogClub* pClub = reinterpret_cast<ARBDogClub*>(m_ctrlClubs.GetItemData(index));
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
		const ARBDogClub* pClub = reinterpret_cast<const ARBDogClub*>(m_ctrlClubs.GetItemData(index));
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
		std::set<std::string> oldVenues;
		std::set<std::string> newVenues;
		ARBDogClubList::iterator iterClub;
		for (iterClub = m_pTrial->GetClubs().begin(); iterClub != m_pTrial->GetClubs().end(); ++iterClub)
		{
			ARBDogClub* pClub = *iterClub;
			oldVenues.insert(pClub->GetVenue());
		}
		for (iterClub = m_Clubs.begin(); iterClub != m_Clubs.end(); ++iterClub)
		{
			ARBDogClub* pClub = *iterClub;
			newVenues.insert(pClub->GetVenue());
		}
		bool bAllThere = true;
		for (std::set<std::string>::iterator iterVenues = oldVenues.begin(); iterVenues != oldVenues.end(); ++iterVenues)
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
				const ARBDogRun* pRun = *iterRun;
				bool bFound = false;
				for (std::set<std::string>::iterator iterVenues = newVenues.begin(); iterVenues != newVenues.end(); ++iterVenues)
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

	m_pTrial->SetLocation((LPCSTR)m_Location);
	m_Notes.Replace("\r\n", "\n");
	m_pTrial->SetNote((LPCSTR)m_Notes);
	m_pTrial->SetVerified(m_Verified ? true : false);
	m_pTrial->GetClubs() = m_Clubs;
	CDialog::OnOK();
}
