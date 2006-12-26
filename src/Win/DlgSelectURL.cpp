/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgSelectURL class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-30 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgSelectURL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectURL dialog

CDlgSelectURL::CDlgSelectURL(
		LPCTSTR name,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgSelectURL::IDD, pParent)
	, m_Name(name)
{
	//{{AFX_DATA_INIT(CDlgSelectURL)
	//}}AFX_DATA_INIT
}

CDlgSelectURL::~CDlgSelectURL()
{
}

void CDlgSelectURL::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectURL)
	DDX_Text(pDX, IDC_SELECTURL_NAME, m_Name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSelectURL, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgSelectURL)
	ON_BN_CLICKED(IDC_SELECTURL_FILENAME, OnBnClickedFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CDlgSelectURL message handlers

BOOL CDlgSelectURL::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectURL::OnBnClickedFilename()
{
	UpdateData(TRUE);
	CString filter;
	filter.LoadString(IDS_FILEEXT_FILTER_ALL);
	CFileDialog dlg(TRUE, NULL, m_Name, OFN_FILEMUSTEXIST, filter, this);
	if (IDOK == dlg.DoModal())
	{
		m_Name = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CDlgSelectURL::OnOK()
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
	CDlgBaseDialog::OnOK();
}
