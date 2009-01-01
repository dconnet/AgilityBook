/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgDogProperties class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-05-30 DRC DOB/Deceased were not initialized in ctor.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDogProp.h"

#include "AgilityBookDoc.h"
#include "ARBDog.h"
#include "DlgRegNum.h"
#include "DlgTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDogProperties dialog

CDlgDogProperties::CDlgDogProperties(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog)
	: CDlgBasePropertyPage(CDlgDogProperties::IDD)
	, m_pDoc(pDoc)
	, m_pDog(pDog)
	, m_DOB(pDog->GetDOB())
	, m_Deceased(pDog->GetDeceased())
{
	//{{AFX_DATA_INIT(CDlgDogProperties)
	m_CallName = pDog->GetCallName().c_str();
	m_RegName = pDog->GetRegisteredName().c_str();
	m_Breed = pDog->GetBreed().c_str();
	m_Notes = pDog->GetNote().c_str();
	//}}AFX_DATA_INIT
	m_Notes.Replace(_T("\n"), _T("\r\n"));
}


CDlgDogProperties::~CDlgDogProperties()
{
}


void CDlgDogProperties::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDogProperties)
	DDX_Text(pDX, IDC_DOG_CALLNAME, m_CallName);
	DDX_Control(pDX, IDC_DOG_DOB, m_ctrlDateDOB);
	DDX_Control(pDX, IDC_DOG_IS_DECEASED, m_ctrlDeceased);
	DDX_Control(pDX, IDC_DOG_DECEASED, m_ctrlDateDeceased);
	DDX_Text(pDX, IDC_DOG_BREED, m_Breed);
	DDX_Control(pDX, IDC_DOG_AGE_TEXT, m_ctrlAge);
	DDX_Text(pDX, IDC_DOG_REG_NAME, m_RegName);
	DDX_Text(pDX, IDC_DOG_NOTES, m_Notes);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_CallName.TrimRight();
		m_CallName.TrimLeft();
		m_Breed.TrimRight();
		m_Breed.TrimLeft();
		m_RegName.TrimRight();
		m_RegName.TrimLeft();
		m_Notes.TrimRight();
		m_Notes.TrimLeft();

		pDX->PrepareCtrl(IDC_DOG_CALLNAME);
		if (m_CallName.IsEmpty())
		{
			AfxMessageBox(IDS_BLANK_CALLNAME);
			pDX->Fail();
			return;
		}

		CTime time;
		m_ctrlDateDOB.GetTime(time);
		m_DOB = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());

		if (BST_CHECKED == m_ctrlDeceased.GetCheck())
		{
			m_ctrlDateDeceased.GetTime(time);
			m_Deceased = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
		}
		else
			m_Deceased.clear();
	}
}


BEGIN_MESSAGE_MAP(CDlgDogProperties, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgDogProperties)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DOG_DOB, OnDatetimechange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DOG_DECEASED, OnDatetimechange)
	ON_BN_CLICKED(IDC_DOG_IS_DECEASED, OnIsDeceased)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgDogProperties::UpdateAge()
{
	CTime dob;
	m_ctrlDateDOB.GetTime(dob);
	CTime current = CTime::GetCurrentTime();
	if (BST_CHECKED == m_ctrlDeceased.GetCheck())
		m_ctrlDateDeceased.GetTime(current);
	CTimeSpan age = current - dob;
	m_ctrlAge.SetWindowText(ARBDouble::str(age.GetDays()/365.0, 1).c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDogProperties message handlers

BOOL CDlgDogProperties::OnInitDialog()
{
	CDlgBasePropertyPage::OnInitDialog();

	CTime dob;
	if (m_pDog->GetDOB().IsValid())
		dob = CTime(m_pDog->GetDOB().GetDate());
	else
		dob = CTime::GetCurrentTime();
	m_ctrlDateDOB.SetTime(&dob);
	int nCheck = BST_UNCHECKED;
	if (m_pDog->GetDeceased().IsValid())
	{
		nCheck = BST_CHECKED;
		dob = CTime(m_pDog->GetDeceased().GetDate());
	}
	else
		dob = CTime::GetCurrentTime();
	m_ctrlDeceased.SetCheck(nCheck);
	m_ctrlDateDeceased.SetTime(&dob);
	OnIsDeceased(); // Fixes visibility of date and calls UpdateAge()

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDogProperties::OnDatetimechange(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	UpdateAge();
	*pResult = 0;
}


void CDlgDogProperties::OnIsDeceased()
{
	if (BST_CHECKED == m_ctrlDeceased.GetCheck())
		m_ctrlDateDeceased.ShowWindow(SW_SHOW);
	else
		m_ctrlDateDeceased.ShowWindow(SW_HIDE);
	UpdateAge();
}
