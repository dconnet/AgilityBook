/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitle.h"
#include ".\dlgconfigtitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitle dialog

CDlgConfigTitle::CDlgConfigTitle(LPCTSTR name, LPCTSTR longname,
		LPCTSTR desc, short inMultiple,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigTitle::IDD, pParent)
	, m_Name(name)
	, m_LongName(longname)
	, m_Desc(desc)
	, m_AllowMany(1 < inMultiple ? TRUE : FALSE)
	, m_Multiple(inMultiple)
{
	m_Desc.Replace("\n", "\r\n");
	//{{AFX_DATA_INIT(CDlgConfigTitle)
	//}}AFX_DATA_INIT
}

CDlgConfigTitle::~CDlgConfigTitle()
{
}

void CDlgConfigTitle::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigTitle)
	DDX_Text(pDX, IDC_CONFIG_TITLE_NAME, m_Name);
	DDX_Text(pDX, IDC_CONFIG_TITLE_LONG_NAME, m_LongName);
	DDX_Text(pDX, IDC_CONFIG_TITLE_DESC, m_Desc);
	DDX_Check(pDX, IDC_CONFIG_TITLE_ALLOW_MULTIPLE, m_AllowMany);
	DDX_Control(pDX, IDC_CONFIG_TITLE_MULTIPLE, m_ctrlMultiple);
	DDX_Text(pDX, IDC_CONFIG_TITLE_MULTIPLE, m_Multiple);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigTitle, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitle)
	ON_BN_CLICKED(IDC_CONFIG_TITLE_ALLOW_MULTIPLE, OnAllowMultiple)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CDlgConfigTitle message handlers

BOOL CDlgConfigTitle::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlMultiple.EnableWindow(m_AllowMany);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigTitle::OnAllowMultiple()
{
	if (!UpdateData(TRUE))
		return;
	if (m_AllowMany && 1 > m_Multiple)
	{
		m_Multiple = 2;
		UpdateData(FALSE);
	}
	m_ctrlMultiple.EnableWindow(m_AllowMany);
}

void CDlgConfigTitle::OnOK()
{
	if (!UpdateData(TRUE))
		return;
#if _MSC_VER >= 1300
	m_Name.Trim();
	m_LongName.Trim();
	m_Desc.Trim();
#else
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_LongName.TrimRight();
	m_LongName.TrimLeft();
	m_Desc.TrimRight();
	m_Desc.TrimLeft();
#endif
	UpdateData(FALSE); // Stuff what we did back.
	if (m_Name.IsEmpty())
	{
		MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_TITLE_NAME));
		return;
	}
	if (m_AllowMany && 1 > m_Multiple)
	{
		MessageBeep(0);
		m_Multiple = 2;
		UpdateData(FALSE);
		GotoDlgCtrl(&m_ctrlMultiple);
		return;
	}
	else if (!m_AllowMany && 0 != m_Multiple)
	{
		m_Multiple = 0;
		UpdateData(FALSE);
	}
	CDlgBaseDialog::OnOK();
	// Get rid of the dialog first - either that or we have to UpdateData(FALSE)
	// since OnOK will UpdateData(TRUE).
	m_Desc.Replace("\r\n", "\n");
}
