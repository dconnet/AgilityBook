/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgHelpPage2 class
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgHelpPage2.h"

#include "DlgARBHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage2 property page

IMPLEMENT_DYNAMIC(CDlgHelpPage2, CPropertyPage)

CDlgHelpPage2::CDlgHelpPage2(CDlgARBHelp* pParent)
	: CPropertyPage(CDlgHelpPage2::IDD)
	, m_Parent(pParent)
{
	//{{AFX_DATA_INIT(CDlgHelpPage2)
	//}}AFX_DATA_INIT
}

CDlgHelpPage2::~CDlgHelpPage2()
{
}

void CDlgHelpPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelpPage2)
	DDX_Control(pDX, IDC_TEXT, m_ctrlText);
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgHelpPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgHelpPage2)
	ON_CLBN_CHKCHANGE(IDC_LIST, OnCheckChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgHelpPage2::UpdateText()
{
	int nChecked = 0;
	for (int i = 0; i < m_ctrlList.GetCount(); ++i)
	{
		if (1 == m_ctrlList.GetCheck(i))
			++nChecked;
	}
	CString txt;
	txt.FormatMessage(m_Text, nChecked);
	m_ctrlText.SetWindowText(txt);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage2 message handlers

BOOL CDlgHelpPage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_ctrlText.GetWindowText(m_Text);
	for (CDlgARBHelp::FileMap::const_iterator i = m_Parent->GetARBFiles().begin();
		i != m_Parent->GetARBFiles().end();
		++i)
	{
		int idx = m_ctrlList.AddString((*i).first);
		m_ctrlList.SetCheck(idx, (*i).second ? 1 : 0);
	}
	UpdateText();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHelpPage2::OnCheckChange()
{
	UpdateText();
}

BOOL CDlgHelpPage2::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return __super::OnSetActive();
}

LRESULT CDlgHelpPage2::OnWizardNext()
{
	for (int i = 0; i < m_ctrlList.GetCount(); ++i)
	{
		CString filename;
		m_ctrlList.GetText(i, filename);
		m_Parent->SetARBFileStatus(filename, (1 == m_ctrlList.GetCheck(i)));
	}
	return 0;
}
