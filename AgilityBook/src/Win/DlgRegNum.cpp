/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief implementation of the CDlgRegNum class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRegNum.h"

#include "ARBConfig.h"
#include "ARBDogRegNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRegNum dialog

// If pRegNum is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgRegNum::CDlgRegNum(const ARBConfig& config, ARBDogRegNumList& regnums, ARBDogRegNum* pRegNum, CWnd* pParent)
	: CDialog(CDlgRegNum::IDD, pParent)
	, m_Config(config)
	, m_RegNums(regnums)
	, m_pRegNum(pRegNum)
{
	//{{AFX_DATA_INIT(CDlgRegNum)
	m_RegNum = _T("");
	m_Height = _T("");
	//}}AFX_DATA_INIT
}

void CDlgRegNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegNum)
	DDX_Control(pDX, IDC_VENUES, m_ctrlVenues);
	DDX_Text(pDX, IDC_REG_NUM, m_RegNum);
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRegNum, CDialog)
	//{{AFX_MSG_MAP(CDlgRegNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRegNum message handlers

BOOL CDlgRegNum::OnInitDialog()
{
	CDialog::OnInitDialog();
	for (ARBConfigVenueList::const_iterator iter = m_Config.GetVenues().begin();
		iter != m_Config.GetVenues().end();
		++iter)
	{
		ARBConfigVenue* pVenue = (*iter);
		m_ctrlVenues.AddString(pVenue->GetName().c_str());
	}
	if (m_pRegNum)
	{
		m_RegNum = m_pRegNum->GetNumber().c_str();
		int index = m_ctrlVenues.FindStringExact(0, m_pRegNum->GetVenue().c_str());
		if (0 <= index)
			m_ctrlVenues.SetCurSel(index);
		m_Height = m_pRegNum->GetHeight().c_str();
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRegNum::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_RegNum.TrimRight();
	m_RegNum.TrimLeft();
	m_Height.TrimRight();
	m_Height.TrimLeft();

	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlVenues);
		return;
	}
	CString venue;
	m_ctrlVenues.GetLBText(index, venue);
	if (m_RegNum.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_REG_NUM));
		return;
	}
	if (m_pRegNum)
	{
		m_pRegNum->SetNumber((LPCSTR)m_RegNum);
		m_pRegNum->SetVenue((LPCSTR)venue);
		m_pRegNum->SetHeight((LPCSTR)m_Height);
	}
	else
		m_RegNums.AddRegNum((LPCSTR)venue, (LPCSTR)m_RegNum);
	CDialog::OnOK();
}
