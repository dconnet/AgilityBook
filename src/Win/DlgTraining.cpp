/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2003-09-21 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTraining.h"

#include "AgilityBookDoc.h"
#include "ARBConfig.h"
#include "ARBTraining.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTraining dialog

CDlgTraining::CDlgTraining(ARBTraining* pTraining, CAgilityBookDoc* pDoc, CWnd* pParent)
	: CDlgBaseDialog(CDlgTraining::IDD, pParent)
	, m_pTraining(pTraining)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CDlgTraining)
	m_date = CTime::GetCurrentTime();
	m_Name = pTraining->GetName().c_str();
	m_Notes = pTraining->GetNote().c_str();
	//}}AFX_DATA_INIT
	if (pTraining->GetDate().IsValid())
		m_date = CTime(pTraining->GetDate().GetDate());
	m_Notes.Replace("\n", "\r\n");
}

void CDlgTraining::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTraining)
	DDX_DateTimeCtrl(pDX, IDC_TRAINING_DATE, m_date);
	DDX_Control(pDX, IDC_TRAINING_NAME, m_ctrlNames);
	DDX_CBString(pDX, IDC_TRAINING_NAME, m_Name);
	DDX_Text(pDX, IDC_TRAINING_NOTES, m_Notes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTraining, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgTraining)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTraining message handlers

BOOL CDlgTraining::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();

	std::set<std::string> names;
	m_pDoc->GetTraining().GetAllNames(names);
	for (std::set<std::string>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		int index = m_ctrlNames.AddString((*iter).c_str());
		if ((*iter) == m_pTraining->GetName())
			m_ctrlNames.SetCurSel(index);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTraining::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	ARBDate date(m_date.GetYear(), m_date.GetMonth(), m_date.GetDay());

	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_Notes.Replace("\r\n", "\n");

	m_pTraining->SetDate(date);
	m_pTraining->SetName((LPCTSTR)m_Name);
	m_pTraining->SetNote((LPCTSTR)m_Notes);

	CDlgBaseDialog::OnOK();
}
