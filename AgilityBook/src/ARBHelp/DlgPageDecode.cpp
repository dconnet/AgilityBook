/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgPageDecode class
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageDecode.h"

#include <set>
#include <sstream>
#include "ARBHelp.h"
#include "ARBTypes.h"
#include "DlgARBHelp.h"
#include "..\Win\VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPageDecode property page

IMPLEMENT_DYNAMIC(CDlgPageDecode, CPropertyPage)

CDlgPageDecode::CDlgPageDecode()
	: CPropertyPage(CDlgPageDecode::IDD)
{
	//{{AFX_DATA_INIT(CDlgPageDecode)
	//}}AFX_DATA_INIT
}

CDlgPageDecode::~CDlgPageDecode()
{
}

void CDlgPageDecode::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPageDecode)
	DDX_Control(pDX, IDC_TEXT, m_ctrlText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgPageDecode, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgPageDecode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPageDecode message handlers

BOOL CDlgPageDecode::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString msg;
	msg.FormatMessage(IDS_DECODE, STREAM_DATA_BEGIN, STREAM_DATA_END);
	m_ctrlText.SetWindowText(msg);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgPageDecode::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_NEXT);
	return __super::OnSetActive();
}
