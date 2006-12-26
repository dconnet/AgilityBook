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
 * @brief implementation of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitle.h"

#include "ARBConfigTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitle dialog

CDlgConfigTitle::CDlgConfigTitle(
		ARBConfigTitlePtr inTitle,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigTitle::IDD, pParent)
	, m_Title(inTitle)
	, m_Name(inTitle->GetName().c_str())
	, m_LongName(inTitle->GetLongName().c_str())
	, m_Desc(inTitle->GetDescription().c_str())
	, m_Prefix(inTitle->GetPrefix() ? TRUE : FALSE)
	, m_AllowMany(1 < inTitle->GetMultiple() ? TRUE : FALSE)
	, m_Multiple(inTitle->GetMultiple())
	, m_DateFrom(inTitle->GetValidFrom().IsValid() ? TRUE : FALSE)
	, m_DateTo(inTitle->GetValidTo().IsValid() ? TRUE : FALSE)
{
	m_Desc.Replace(_T("\n"), _T("\r\n"));
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
	DDX_Check(pDX, IDC_CONFIG_TITLE_PREFIX, m_Prefix);
	DDX_Check(pDX, IDC_CONFIG_TITLE_ALLOW_MULTIPLE, m_AllowMany);
	DDX_Control(pDX, IDC_CONFIG_TITLE_MULTIPLE, m_ctrlMultiple);
	DDX_Text(pDX, IDC_CONFIG_TITLE_MULTIPLE, m_Multiple);
	DDX_Check(pDX, IDC_CONFIG_TITLE_VALID_FROM, m_DateFrom);
	DDX_Control(pDX, IDC_CONFIG_TITLE_VALID_FROM_DATE, m_ctrlDateFrom);
	DDX_Check(pDX, IDC_CONFIG_TITLE_VALID_TO, m_DateTo);
	DDX_Control(pDX, IDC_CONFIG_TITLE_VALID_TO_DATE, m_ctrlDateTo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigTitle, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitle)
	ON_BN_CLICKED(IDC_CONFIG_TITLE_ALLOW_MULTIPLE, OnAllowMultiple)
	ON_BN_CLICKED(IDC_CONFIG_TITLE_VALID_FROM, OnCheck)
	ON_BN_CLICKED(IDC_CONFIG_TITLE_VALID_TO, OnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgConfigTitle::UpdateButtons()
{
	m_ctrlDateFrom.EnableWindow(m_DateFrom);
	m_ctrlDateTo.EnableWindow(m_DateTo);
}

// CDlgConfigTitle message handlers

BOOL CDlgConfigTitle::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	m_ctrlMultiple.EnableWindow(m_AllowMany);

	CTime t;
	if (m_DateFrom)
		t = m_Title->GetValidFrom().GetDate();
	else
		t = CTime::GetCurrentTime();
	m_ctrlDateFrom.SetTime(&t);
	if (m_DateTo)
		t = m_Title->GetValidTo().GetDate();
	else
		t = CTime::GetCurrentTime();
	m_ctrlDateTo.SetTime(&t);

	UpdateButtons();

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

void CDlgConfigTitle::OnCheck()
{
	UpdateData(TRUE);
	UpdateButtons();
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
	if (m_Name.IsEmpty())
	{
		UpdateData(FALSE); // Stuff what we did back.
		MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_TITLE_NAME));
		return;
	}
	if (m_AllowMany && 1 > m_Multiple)
	{
		UpdateData(FALSE); // Stuff what we did back.
		MessageBeep(0);
		m_Multiple = 2;
		UpdateData(FALSE);
		GotoDlgCtrl(&m_ctrlMultiple);
		return;
	}
	else if (!m_AllowMany && 0 != m_Multiple)
	{
		m_Multiple = 0;
	}
	m_Desc.Replace(_T("\r\n"), _T("\n"));

	m_Title->SetName((LPCTSTR)m_Name);
	m_Title->SetLongName((LPCTSTR)m_LongName);
	m_Title->SetDescription((LPCTSTR)m_Desc);
	m_Title->SetPrefix(m_Prefix ? true : false);
	m_Title->SetMultiple(m_Multiple);
	ARBDate date;
	if (m_DateFrom)
	{
		CTime time;
		if (GDT_VALID == m_ctrlDateFrom.GetTime(time))
			date = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
	else
		date.clear();
	m_Title->SetValidFrom(date);
	if (m_DateTo)
	{
		CTime time;
		if (GDT_VALID == m_ctrlDateTo.GetTime(time))
			date = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
	else
		date.clear();
	m_Title->SetValidTo(date);

	CDlgBaseDialog::OnOK();
}
