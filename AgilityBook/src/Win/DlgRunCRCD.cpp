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
 * @brief implementation of the CDlgRunCRCD class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunCRCD.h"

#include "ARBDogRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD dialog

CDlgRunCRCD::CDlgRunCRCD(ARBDogRun* pRun)
	: CPropertyPage(CDlgRunCRCD::IDD)
	, m_Run(pRun)
{
	//{{AFX_DATA_INIT(CDlgRunCRCD)
	m_CRCD = m_Run->GetCRCD().c_str();
	//}}AFX_DATA_INIT
	m_CRCD.Replace("\n", "\r\n");
}

CDlgRunCRCD::~CDlgRunCRCD()
{
}

void CDlgRunCRCD::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunCRCD)
	DDX_Text(pDX, IDC_CRCD, m_CRCD);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_CRCD.TrimRight();
		m_CRCD.TrimLeft();
		CString tmp(m_CRCD);
		tmp.Replace("\r\n", "\n");
		m_Run->SetCRCD((LPCSTR)tmp);
	}
}

BEGIN_MESSAGE_MAP(CDlgRunCRCD, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgRunCRCD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD message handlers

BOOL CDlgRunCRCD::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
