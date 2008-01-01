/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgHelpPage3 class
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgHelpPage3.h"

#include "DlgARBHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage3 property page

IMPLEMENT_DYNAMIC(CDlgHelpPage3, CPropertyPage)

CDlgHelpPage3::CDlgHelpPage3(CDlgARBHelp* pParent)
	: CPropertyPage(CDlgHelpPage3::IDD)
	, m_Parent(pParent)
{
	//{{AFX_DATA_INIT(CDlgHelpPage3)
	//}}AFX_DATA_INIT
}

CDlgHelpPage3::~CDlgHelpPage3()
{
}

void CDlgHelpPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelpPage3)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgHelpPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgHelpPage3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage3 message handlers

BOOL CDlgHelpPage3::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgHelpPage3::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return __super::OnSetActive();
}

LRESULT CDlgHelpPage3::OnWizardBack()
{
	if (0 == m_Parent->GetARBFiles().size())
		return IDD_HELP_PAGE1;
	else
		return 0;
}

BOOL CDlgHelpPage3::OnWizardFinish()
{
	CWaitCursor wait;
	CPropertySheet* psheet = (CPropertySheet*) GetParent();
	// SetWizardButtons does a postmessage. I need it updated now! (lparam is buttons)
	psheet->SendMessage(PSM_SETWIZBUTTONS, 0, 0);

	m_Parent->SendIt();

	return __super::OnWizardFinish();
}
