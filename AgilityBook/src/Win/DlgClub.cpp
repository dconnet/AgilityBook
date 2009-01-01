/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2008-02-01 DRC Only modify existing club, don't add a new one (caller does).
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
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
CDlgClub::CDlgClub(
		CAgilityBookDoc* pDoc,
		ARBDogClubList& clubs,
		ARBDogClubPtr pClub,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgClub::IDD, pParent)
	, m_ctrlClubs(false)
	, m_ctrlVenues(false)
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
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgClub)
	DDX_Control(pDX, IDC_CLUB_CLUBS, m_ctrlClubs);
	DDX_CBString(pDX, IDC_CLUB_CLUBS, m_Club);
	DDX_Control(pDX, IDC_CLUB_VENUES, m_ctrlVenues);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgClub, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgClub)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgClub message handlers

BOOL CDlgClub::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	set<tstring> clubs;
	m_pDoc->Book().GetAllClubNames(clubs, true, true);
	if (m_pClub && !m_pClub->GetName().empty())
		clubs.insert(m_pClub->GetName());
	for (set<tstring>::const_iterator iter = clubs.begin(); iter != clubs.end(); ++iter)
	{
		m_ctrlClubs.AddString((*iter).c_str());
	}
	m_ctrlClubs.SetCurSel(-1);

	m_ctrlVenues.Initialize(m_pDoc->Book().GetConfig().GetVenues(), tstring());
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
	m_ctrlVenues.GetLBText(index, m_Venue);

	if (m_pClub)
	{
		m_pClub->SetName((LPCTSTR)m_Club);
		m_pClub->SetVenue((LPCTSTR)m_Venue);
	}
	CDlgBaseDialog::OnOK();
}
