/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2004-04-08 DRC Created
 */

#include "stdafx.h"
#include "DlgOptionsProgram.h"

#pragma message PRAGMA_MESSAGE("TODO: Implement CDlgOptionsProgram")
#if 0
#include "AgilityBook.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsProgram property page

IMPLEMENT_DYNCREATE(CDlgOptionsProgram, CDlgBasePropertyPage)


CDlgOptionsProgram::CDlgOptionsProgram()
	: CDlgBasePropertyPage(CDlgOptionsProgram::IDD)
	, m_bAutoCheck(TRUE)
	, m_Backups(0)
	, m_bAutoShow(TRUE)
	, m_bShowSplash(TRUE)
	, m_Splash(_T(""))
	, m_bShowHtml(FALSE)
	, m_IEInstalled(FALSE)
{
}


CDlgOptionsProgram::~CDlgOptionsProgram()
{
}


void CDlgOptionsProgram::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsProgram)
	DDX_Check(pDX, IDC_OPT_PGM_AUTO_CHECK, m_bAutoCheck);
	DDX_Text(pDX, IDC_OPT_PGM_EDIT, m_Backups);
	DDX_Check(pDX, IDC_OPT_PGM_AUTOSHOW, m_bAutoShow);
	DDX_Check(pDX, IDC_OPT_PGM_SHOW_SPLASH, m_bShowSplash);
	DDX_Text(pDX, IDC_OPT_PGM_FILENAME, m_Splash);
	DDX_Check(pDX, IDC_OPT_PGM_SHOWHTML, m_bShowHtml);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOptionsProgram, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsProgram)
	ON_BN_CLICKED(IDC_OPT_PGM_SHOW_SPLASH, OnShowSplash)
	ON_BN_CLICKED(IDC_OPT_PGM_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgOptionsProgram::UpdateButtons()
{
	GetDlgItem(IDC_OPT_PGM_FILENAME)->EnableWindow(m_bShowSplash);
	GetDlgItem(IDC_OPT_PGM_BROWSE)->EnableWindow(m_bShowSplash);
	if (!m_IEInstalled)
		GetDlgItem(IDC_OPT_PGM_SHOWHTML)->EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsProgram message handlers

BOOL CDlgOptionsProgram::OnInitDialog()
{
	CDlgBasePropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOptionsProgram::OnShowSplash()
{
	UpdateData(TRUE);
	UpdateButtons();
}


void CDlgOptionsProgram::OnBrowse()
{
	UpdateData(TRUE);
	CString bmpFilter;
	bmpFilter.LoadString(IDS_FILEEXT_FILTER_BMP);
	CFileDialog dlg(TRUE, NULL, m_Splash, OFN_FILEMUSTEXIST, bmpFilter, this);
	if (IDOK == dlg.DoModal())
	{
		m_Splash = dlg.GetPathName();
		UpdateData(FALSE);
	}
}
#endif
