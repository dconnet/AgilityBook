/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @brief Assign columns for import/export.
 * @author David Connet

Dog:
  Registered Name
  Call Name
  Breed
  DOB
  DOD
  Age
  Titles: Date,Venue,Name,Received
  Numbers: Venue,Number,Height,Received
  Notes
Trials:
  Location
  Notes
  Club,Venue
Runs Available fields:
  Date
  Venue
  Trial
  Location
  Division
  Level
  Event
  Height
  Judge
  Handler
  Conditions
  Course Faults
  Time
    Yards         Required Opening  Required Points
    YPS           Required Closing  Points
    SCT           Opening
    Total Faults  Closing
  Place/of
  DogsQd
  Q?
  Score
  Title Points
  Comments
  Faults
  Reference Runs: Q, Place, Score, Time, Name, Breed, Note
  CRCD

Calendar
  Start Date
  End Date
  Tentative?
  Not Entered,Planning,Entered
  Location
  Club
  Venue
  Opens
  Closes
  Notes
Training Log:
  Date
  Name
  Notes


 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgAssignColumns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns dialog

CDlgAssignColumns::CDlgAssignColumns(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAssignColumns::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAssignColumns)
	//}}AFX_DATA_INIT
}

void CDlgAssignColumns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAssignColumns)
	DDX_Control(pDX, IDC_AVAILABLE, m_ctrlAvailable);
	DDX_Control(pDX, IDC_COLUMNS, m_ctrlColumns);
	DDX_Control(pDX, IDC_ADD, m_ctrlAdd);
	DDX_Control(pDX, IDC_DELETE, m_ctrlRemove);
	DDX_Control(pDX, IDC_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_ctrlDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAssignColumns, CDialog)
	//{{AFX_MSG_MAP(CDlgAssignColumns)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnRemove)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgAssignColumns::UpdateButtons()
{
	m_ctrlAdd.EnableWindow(FALSE);
	m_ctrlRemove.EnableWindow(FALSE);
	m_ctrlUp.EnableWindow(FALSE);
	m_ctrlDown.EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns message handlers

BOOL CDlgAssignColumns::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAssignColumns::OnAdd() 
{
}

void CDlgAssignColumns::OnRemove() 
{
}

void CDlgAssignColumns::OnMoveUp() 
{
}

void CDlgAssignColumns::OnMoveDown() 
{
}

void CDlgAssignColumns::OnOK() 
{
	CDialog::OnOK();
}
