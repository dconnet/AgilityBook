/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgName class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgName dialog

CDlgName::CDlgName(
		LPCTSTR name,
		UINT caption,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgName::IDD, pParent)
	, m_Name(name)
{
	if (0 != caption)
		m_Caption.LoadString(caption);
	//{{AFX_DATA_INIT(CDlgName)
	//}}AFX_DATA_INIT
}


CDlgName::CDlgName(
		LPCTSTR name,
		LPCTSTR pCaption,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgName::IDD, pParent)
	, m_Name(name)
	, m_Caption(pCaption)
{
}


CDlgName::~CDlgName()
{
}


void CDlgName::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgName)
	DDX_Text(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgName, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgName message handlers

BOOL CDlgName::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	if (!m_Caption.IsEmpty())
		SetWindowText(m_Caption);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgName::OnOK()
{
	if (!UpdateData(TRUE))
		return;
#if _MSC_VER >= 1300
	m_Name.Trim();
#else
	m_Name.TrimRight();
	m_Name.TrimLeft();
#endif
	UpdateData(FALSE);
	if (m_Name.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_NAME));
		return;
	}
	CDlgBaseDialog::OnOK();
}
