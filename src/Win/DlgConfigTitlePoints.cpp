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
 * @brief implementation of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitlePoints.h"

#include "ARBConfigTitlePoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints dialog

// If pRegNum is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgConfigTitlePoints::CDlgConfigTitlePoints(
	ARBConfigTitlePointsList& titlePoints,
	ARBConfigTitlePoints* pTitle,
	CWnd* pParent)
	: CDialog(CDlgConfigTitlePoints::IDD, pParent)
	, m_TitlePoints(titlePoints)
	, m_pTitle(pTitle)
{
	//{{AFX_DATA_INIT(CDlgConfigTitlePoints)
	m_Points = 0;
	m_Faults = 0;
	//}}AFX_DATA_INIT
}

void CDlgConfigTitlePoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigTitlePoints)
	DDX_Text(pDX, IDC_POINTS, m_Points);
	DDX_Text(pDX, IDC_FAULTS, m_Faults);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigTitlePoints, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitlePoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints message handlers

BOOL CDlgConfigTitlePoints::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pTitle)
	{
		m_Points = m_pTitle->GetPoints();
		m_Faults = m_pTitle->GetFaults();
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigTitlePoints::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	if (m_pTitle)
	{
		m_pTitle->SetPoints(m_Points);
		m_pTitle->SetFaults(m_Faults);
		m_TitlePoints.sort();
	}
	else
	{
		// The only reason this fails is if the faults entry exists.
		if (!m_TitlePoints.AddTitlePoints(m_Points, m_Faults))
		{
			AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
			return;
		}
	}

	CDialog::OnOK();
}
