/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * @li 2004-04-08 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsProgram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsProgram property page

IMPLEMENT_DYNCREATE(CDlgOptionsProgram, CPropertyPage)

CDlgOptionsProgram::CDlgOptionsProgram()
	: CPropertyPage(CDlgOptionsProgram::IDD)
{
	//{{AFX_DATA_INIT(CDlgOptionsProgram)
	m_bAutoCheck = TRUE;
	m_Backups = 0;
	m_bAutoShow = TRUE;
	m_Splash = _T("");
	//}}AFX_DATA_INIT
}

CDlgOptionsProgram::~CDlgOptionsProgram()
{
}

void CDlgOptionsProgram::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsProgram)
	DDX_Check(pDX, IDC_AUTO_CHECK, m_bAutoCheck);
	DDX_Text(pDX, IDC_EDIT, m_Backups);
	DDX_Check(pDX, IDC_AUTOSHOW, m_bAutoShow);
	DDX_Text(pDX, IDC_FILENAME, m_Splash);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsProgram, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsProgram)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsProgram message handlers

BOOL CDlgOptionsProgram::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsProgram::OnBrowse()
{
	UpdateData(TRUE);
	CFileDialog dlg(TRUE, NULL, m_Splash, OFN_FILEMUSTEXIST, "Bitmap Files (*.bmp)|*.bmp||", this);
	if (IDOK == dlg.DoModal())
	{
		m_Splash = dlg.GetPathName();
		UpdateData(FALSE);
	}
}
