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
 * @brief implementation of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgReferenceRun.h"

#include "ARBDogReferenceRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReferenceRun dialog

CDlgReferenceRun::CDlgReferenceRun(ARBDogReferenceRun* ref, CWnd* pParent)
	: CDialog(CDlgReferenceRun::IDD, pParent)
	, m_Ref(ref)
{
	ASSERT(m_Ref);
	//{{AFX_DATA_INIT(CDlgReferenceRun)
	m_Place = m_Ref->GetPlace();
	m_Points = m_Ref->GetScore().c_str();
	m_Time = m_Ref->GetTime();
	m_Name = m_Ref->GetName().c_str();
	m_Breed = m_Ref->GetBreed().c_str();
	m_Notes = m_Ref->GetNote().c_str();
	//}}AFX_DATA_INIT
	if (m_Points.IsEmpty())
		m_Points = "0";
	m_Notes.Replace("\n", "\r\n");
}

void CDlgReferenceRun::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReferenceRun)
	DDX_Text(pDX, IDC_PLACE, m_Place);
	DDX_Control(pDX, IDC_Q, m_ctrlQ);
	DDX_Text(pDX, IDC_POINTS, m_Points);
	DDX_Text(pDX, IDC_TIME, m_Time);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_BREED, m_Breed);
	DDX_Text(pDX, IDC_NOTES, m_Notes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgReferenceRun, CDialog)
	//{{AFX_MSG_MAP(CDlgReferenceRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReferenceRun message handlers

BOOL CDlgReferenceRun::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int nQs = ARB_Q::GetNumValidTypes();
	for (int index = 0; index < nQs; ++index)
	{
		ARB_Q q = ARB_Q::GetValidType(index);
		int idx = m_ctrlQ.AddString(q.str().c_str());
		m_ctrlQ.SetItemData(idx, index);
		if (m_Ref->GetQ() == q)
			m_ctrlQ.SetCurSel(idx);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReferenceRun::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	m_Points.TrimRight();
	m_Points.TrimLeft();
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_Breed.TrimRight();
	m_Breed.TrimLeft();
	m_Notes.TrimRight();
	m_Notes.TrimLeft();

	int index = m_ctrlQ.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlQ);
		return;
	}
	ARB_Q q = ARB_Q::GetValidType(static_cast<int>(m_ctrlQ.GetItemData(index)));

	m_Ref->SetQ(q);
	m_Ref->SetPlace(m_Place);
	m_Ref->SetScore((LPCSTR)m_Points);
	m_Ref->SetTime(m_Time); // Letting the prec default to 2 is fine.
	m_Ref->SetName((LPCSTR)m_Name);
	m_Ref->SetBreed((LPCSTR)m_Breed);
	m_Notes.Replace("\r\n", "\n");
	m_Ref->SetNote((LPCSTR)m_Notes);

	CDialog::OnOK();
}
