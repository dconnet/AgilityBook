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
 * @brief implementation of the CDlgRunComments class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunComments.h"

#include "AgilityBookDoc.h"
#include "ARBDogRun.h"
#include "DlgListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunComments dialog

CDlgRunComments::CDlgRunComments(CAgilityBookDoc* pDoc, ARBDogRun* pRun)
	: CPropertyPage(CDlgRunComments::IDD)
	, m_pDoc(pDoc)
	, m_Run(pRun)
{
	//{{AFX_DATA_INIT(CDlgRunComments)
	m_Comments = m_Run->GetNote().c_str();
	//}}AFX_DATA_INIT
	m_Comments.Replace("\n", "\r\n");
}

CDlgRunComments::~CDlgRunComments()
{
}

void CDlgRunComments::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunComments)
	DDX_Control(pDX, IDC_COMMENTS_FAULTS, m_ctrlCommentsFaults);
	DDX_Control(pDX, IDC_FAULTS_LIST, m_ctrlFaultsList);
	DDX_Control(pDX, IDC_COMMENTS, m_ctrlComments);
	DDX_Text(pDX, IDC_COMMENTS, m_Comments);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_Comments.TrimRight();
		m_Comments.TrimLeft();
		CString tmp(m_Comments);
		tmp.Replace("\r\n", "\n");
		m_Run->SetNote((LPCSTR)tmp);
	}
}

BEGIN_MESSAGE_MAP(CDlgRunComments, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgRunComments)
	ON_BN_CLICKED(IDC_COMMENTS_FAULTS, OnCommentsFaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgRunComments::SetFaultsText()
{
	CString str;
	for (ARBDogFaultList::const_iterator iter = m_Run->GetFaults().begin(); iter != m_Run->GetFaults().end(); ++iter)
	{
		if (!str.IsEmpty())
			str += "\r\n";
		str += (*iter).c_str();
	}
	m_ctrlFaultsList.SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunComments message handlers

BOOL CDlgRunComments::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	SetFaultsText();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRunComments::OnCommentsFaults()
{
	CDlgListCtrl dlg(m_pDoc, m_Run, this);
	if (IDOK == dlg.DoModal())
		SetFaultsText();
}
