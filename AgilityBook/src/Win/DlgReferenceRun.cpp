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
 * @brief implementation of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgReferenceRun.h"
#include <set>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBDogReferenceRun.h"
#include "ARBTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReferenceRun dialog

CDlgReferenceRun::CDlgReferenceRun(
		CAgilityBookDoc* pDoc,
		std::set<tstring> const& inHeights,
		std::set<tstring> const& inNames,
		std::set<tstring> const& inBreeds,
		ARBDogReferenceRunPtr ref,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgReferenceRun::IDD, pParent)
	, m_ctrlQ(false)
	, m_ctrlHeight(false)
	, m_ctrlName(false)
	, m_ctrlBreed(false)
	, m_pDoc(pDoc)
	, m_Heights(inHeights)
	, m_Names(inNames)
	, m_Breeds(inBreeds)
	, m_Ref(ref)
{
	ASSERT(m_Ref);
	//{{AFX_DATA_INIT(CDlgReferenceRun)
	m_Place = m_Ref->GetPlace();
	m_Points = m_Ref->GetScore().c_str();
	m_Time = m_Ref->GetTime();
	m_Name = m_Ref->GetName().c_str();
	m_Height = m_Ref->GetHeight().c_str();
	m_Breed = m_Ref->GetBreed().c_str();
	m_Notes = m_Ref->GetNote().c_str();
	//}}AFX_DATA_INIT
	if (m_Points.IsEmpty())
		m_Points = _T("0");
	m_Notes.Replace(_T("\n"), _T("\r\n"));
	if (m_Height.IsEmpty())
		m_Height = CAgilityBookOptions::GetLastEnteredRefHeight();
}


void CDlgReferenceRun::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReferenceRun)
	DDX_Text(pDX, IDC_REFRUN_PLACE, m_Place);
	DDX_Control(pDX, IDC_REFRUN_Q, m_ctrlQ);
	DDX_Text(pDX, IDC_REFRUN_POINTS, m_Points);
	DDX_Text(pDX, IDC_REFRUN_TIME, m_Time);
	DDX_Control(pDX, IDC_REFRUN_HEIGHT, m_ctrlHeight);
	DDX_CBString(pDX, IDC_REFRUN_HEIGHT, m_Height);
	DDX_Control(pDX, IDC_REFRUN_NAME, m_ctrlName);
	DDX_CBString(pDX, IDC_REFRUN_NAME, m_Name);
	DDX_Control(pDX, IDC_REFRUN_BREED, m_ctrlBreed);
	DDX_CBString(pDX, IDC_REFRUN_BREED, m_Breed);
	DDX_Text(pDX, IDC_REFRUN_NOTES, m_Notes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReferenceRun, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgReferenceRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReferenceRun message handlers

BOOL CDlgReferenceRun::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	int nQs = ARB_Q::GetNumValidTypes();
	for (int index = 0; index < nQs; ++index)
	{
		ARB_Q q = ARB_Q::GetValidType(index);
		int idx = m_ctrlQ.AddString(q.str().c_str());
		m_ctrlQ.SetItemData(idx, index);
		if (m_Ref->GetQ() == q)
			m_ctrlQ.SetCurSel(idx);
	}

	std::set<tstring>::const_iterator iter;

	for (iter = m_Heights.begin(); iter != m_Heights.end(); ++iter)
	{
		m_ctrlHeight.AddString((*iter).c_str());
	}
	for (iter = m_Names.begin(); iter != m_Names.end(); ++iter)
	{
		m_ctrlName.AddString((*iter).c_str());
	}
	for (iter = m_Breeds.begin(); iter != m_Breeds.end(); ++iter)
	{
		m_ctrlBreed.AddString((*iter).c_str());
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
	m_Height.TrimRight();
	m_Height.TrimLeft();
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

	CAgilityBookOptions::SetLastEnteredRefHeight(m_Height);

	m_Ref->SetQ(q);
	m_Ref->SetPlace(m_Place);
	m_Ref->SetScore((LPCTSTR)m_Points);
	m_Ref->SetTime(m_Time); // Letting the prec default to 2 is fine.
	m_Ref->SetName((LPCTSTR)m_Name);
	m_Ref->SetHeight((LPCTSTR)m_Height);
	m_Ref->SetBreed((LPCTSTR)m_Breed);
	m_Notes.Replace(_T("\r\n"), _T("\n"));
	m_Ref->SetNote((LPCTSTR)m_Notes);

	CDlgBaseDialog::OnOK();
}
