/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgClub class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgClub.h"

#include "AgilityBookDoc.h"
#include "ARBDogClub.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgClub dialog

// If pClub is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgClub::CDlgClub(CAgilityBookDoc* pDoc, ARBDogClubList& clubs, ARBDogClub* pClub, CWnd* pParent)
	: CDialog(CDlgClub::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Clubs(clubs)
	, m_pClub(pClub)
{
	//{{AFX_DATA_INIT(CDlgClub)
	m_Club = _T("");
	//}}AFX_DATA_INIT
}

void CDlgClub::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgClub)
	DDX_Control(pDX, IDC_CLUBS, m_ctrlClubs);
	DDX_CBString(pDX, IDC_CLUBS, m_Club);
	DDX_Control(pDX, IDC_VENUES, m_ctrlVenues);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgClub, CDialog)
	//{{AFX_MSG_MAP(CDlgClub)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgClub message handlers

BOOL CDlgClub::OnInitDialog()
{
	CDialog::OnInitDialog();

	set<string> clubs;
	m_pDoc->GetAllClubNames(clubs);
	for (set<string>::const_iterator iter = clubs.begin(); iter != clubs.end(); ++iter)
	{
		m_ctrlClubs.AddString((*iter).c_str());
	}
	m_ctrlClubs.SetCurSel(-1);

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->GetConfig().GetVenues().begin();
	iterVenue != m_pDoc->GetConfig().GetVenues().end();
	++iterVenue)
	{
		m_ctrlVenues.AddString((*iterVenue)->GetName().c_str());
	}
	m_ctrlVenues.SetCurSel(-1);
	if (m_pClub)
	{
		m_Club = m_pClub->GetName().c_str();
		int index = m_ctrlVenues.FindStringExact(0, m_pClub->GetVenue().c_str());
		if (0 <= index)
			m_ctrlVenues.SetCurSel(index);
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgClub::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	m_Club.TrimRight();
	m_Club.TrimLeft();
	if (m_Club.IsEmpty())
	{
		GotoDlgCtrl(&m_ctrlClubs);
		return;
	}

	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlVenues);
		return;
	}
	CString venue;
	m_ctrlVenues.GetLBText(index, venue);

	if (m_pClub)
	{
		m_pClub->SetName((LPCSTR)m_Club);
		m_pClub->SetVenue((LPCSTR)venue);
	}
	else
		m_Clubs.AddClub((LPCSTR)m_Club, (LPCSTR)venue);
	CDialog::OnOK();
}
