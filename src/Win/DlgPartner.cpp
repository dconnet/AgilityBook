/*
 * Copyright � 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgPartner class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgPartner.h"

#include "ARBDogRunPartner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPartner dialog

CDlgPartner::CDlgPartner(ARBDogRunPartner* partner, CWnd* pParent)
	: CDlgBaseDialog(CDlgPartner::IDD, pParent)
	, m_Partner(partner)
{
	ASSERT(m_Partner);
	//{{AFX_DATA_INIT(CDlgPartner)
	m_Handler = m_Partner->GetHandler().c_str();
	m_Dog = m_Partner->GetDog().c_str();
	m_RegNum = m_Partner->GetRegNum().c_str();
	//}}AFX_DATA_INIT
}

void CDlgPartner::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPartner)
	DDX_Text(pDX, IDC_PARTNER_NAME, m_Handler);
	DDX_Text(pDX, IDC_PARTNER_CALLNAME, m_Dog);
	DDX_Text(pDX, IDC_PARTNER_REG_NUM, m_RegNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgPartner, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgPartner)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPartner message handlers

BOOL CDlgPartner::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPartner::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	m_Handler.TrimRight();
	m_Handler.TrimLeft();
	m_Dog.TrimRight();
	m_Dog.TrimLeft();
	m_RegNum.TrimRight();
	m_RegNum.TrimLeft();

	m_Partner->SetHandler((LPCSTR)m_Handler);
	m_Partner->SetDog((LPCSTR)m_Dog);
	m_Partner->SetRegNum((LPCSTR)m_RegNum);
	CDlgBaseDialog::OnOK();
}
