/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardExport.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "DlgAssignColumns.h"
#include "Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardExport property page

IMPLEMENT_DYNAMIC(CWizardExport, CPropertyPage)

CWizardExport::CWizardExport(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: CPropertyPage(CWizardExport::IDD)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardExport)
	m_Delim = _T(":");
	//}}AFX_DATA_INIT
}

CWizardExport::~CWizardExport()
{
}

void CWizardExport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardExport)
	DDX_Text(pDX, IDC_WIZARD_DELIM, m_Delim);
	DDV_MaxChars(pDX, m_Delim, 1);
	DDX_Control(pDX, IDC_WIZARD_PREVIEW, m_ctrlPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardExport, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardExport)
	ON_BN_CLICKED(IDC_WIZARD_ASSIGN, OnExportAssign)
	ON_EN_CHANGE(IDC_WIZARD_DELIM, OnExportDelim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CWizardExport::UpdateButtons()
{
	DWORD dwWiz = PSWIZB_BACK;
	// Some test to make sure things are ready
	bool bOk = false;
	dwWiz |= (bOk ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH);
	m_pSheet->SetWizardButtons(dwWiz);
}

void CWizardExport::UpdatePreview()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWizardExport message handlers

BOOL CWizardExport::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_ctrlPreview.InsertColumn(0, "Todo");
	m_ctrlPreview.InsertItem(0, "This is still being developed.");
	m_ctrlPreview.InsertItem(1, "It doesn't work yet.");
	m_ctrlPreview.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardExport::OnSetActive() 
{
	UpdateButtons();
	return CPropertyPage::OnSetActive();
}

LRESULT CWizardExport::OnWizardBack()
{
	return IDD_WIZARD_START;
}

BOOL CWizardExport::OnWizardFinish() 
{
	return CPropertyPage::OnWizardFinish();
}

void CWizardExport::OnExportDelim() 
{
	UpdateData(TRUE);
	UpdateButtons();
	UpdatePreview();
}

void CWizardExport::OnExportAssign() 
{
	UpdateData(TRUE);
	CDlgAssignColumns dlg(this);
	if (IDOK == dlg.DoModal())
	{
		UpdateButtons();
		UpdatePreview();
	}
}
