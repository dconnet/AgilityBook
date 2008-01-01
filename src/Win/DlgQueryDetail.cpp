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
 * @brief implementation of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2007-12-24 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgQueryDetail.h"

#include "ARBConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryDetail dialog

CDlgQueryDetail::CDlgQueryDetail(
		ARBConfig const* inConfig,
		const TCHAR* inCode,
		const TCHAR* inName,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgQueryDetail::IDD, pParent)
	, m_strCode(inCode)
	, m_strName(inName)
	, m_Config(inConfig)
	, m_Code(inCode)
	, m_Name(inName)
{
}


void CDlgQueryDetail::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryDetail)
	DDX_Text(pDX, IDC_QUERYDETAIL_CODE, m_strCode);
	DDX_Control(pDX, IDC_QUERYDETAIL_NAME_TEXT, m_ctrlName);
	DDX_Text(pDX, IDC_QUERYDETAIL_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryDetail, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgQueryDetail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryDetail message handlers

BOOL CDlgQueryDetail::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	CString str;
	if (m_Config)
		str.LoadString(IDS_COL_VENUE);
	else
		str.LoadString(IDS_COL_LOCATION);
	m_ctrlName.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgQueryDetail::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	m_strCode.TrimRight();
	m_strCode.TrimLeft();
	if (m_strCode.IsEmpty())
	{
		MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_QUERYDETAIL_CODE));
		return;
	}

	m_strName.TrimRight();
	m_strName.TrimLeft();

	m_Code = (LPCTSTR)m_strCode;
	m_Name = (LPCTSTR)m_strName;
	if (m_Config)
	{
		if (m_Name.empty())
			m_Name = m_Code;
		if (m_Config->GetVenues().FindVenue(m_Code))
			m_Name.erase();
		else if (!m_Config->GetVenues().FindVenue(m_Name))
		{
			CString str;
			str.LoadString(IDS_INVALID_VENUE_NAME);
			str += m_Name.c_str();
			AfxMessageBox(str, MB_ICONEXCLAMATION);
			GotoDlgCtrl(GetDlgItem(IDC_QUERYDETAIL_NAME));
			return;
		}
		if (m_Name.empty())
			m_Name = m_Code;
	}

	CDlgBaseDialog::OnOK();
}
