/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigOtherPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-05-22 DRC Fixed CRNL sequences.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigOtherPoints.h"

#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	UINT idDesc;
	ARBConfigOtherPoints::eOtherPointsTally tally;
} const sc_Tally[] =
{
	{IDS_OTHERPTS_ALL, ARBConfigOtherPoints::eTallyAll},
	{IDS_OTHERPTS_EVENT, ARBConfigOtherPoints::eTallyAllByEvent},
	{IDS_OTHERPTS_LEVEL, ARBConfigOtherPoints::eTallyLevel},
	{IDS_OTHERPTS_EVENT_LEVEL, ARBConfigOtherPoints::eTallyLevelByEvent}
};
static int const sc_numTally = sizeof(sc_Tally) / sizeof(sc_Tally[0]);

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigOtherPoints dialog

CDlgConfigOtherPoints::CDlgConfigOtherPoints(
		ARBConfig& config,
		ARBConfigOtherPointsPtr pOther,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigOtherPoints::IDD, pParent)
	, m_ctrlTally(false)
	, m_Config(config)
	, m_pOther(pOther)
{
	ASSERT(m_pOther);
	m_Name = m_pOther->GetName().c_str();
	m_Desc = m_pOther->GetDescription().c_str();
	m_Desc.Replace(_T("\n"), _T("\r\n"));
	//{{AFX_DATA_INIT(CDlgConfigOtherPoints)
	//}}AFX_DATA_INIT
}

CDlgConfigOtherPoints::~CDlgConfigOtherPoints()
{
}

void CDlgConfigOtherPoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigOtherPoints)
	DDX_Text(pDX, IDC_CONFIG_OTHER_NAME, m_Name);
	DDX_Control(pDX, IDC_CONFIG_OTHER_TALLY, m_ctrlTally);
	DDX_Text(pDX, IDC_CONFIG_OTHER_DESC, m_Desc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigOtherPoints, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigOtherPoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CDlgConfigOtherPoints message handlers

BOOL CDlgConfigOtherPoints::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	for (int index = 0; index < sc_numTally; ++index)
	{
		CString desc;
		desc.LoadString(sc_Tally[index].idDesc);
		int id = m_ctrlTally.AddString(desc);
		m_ctrlTally.SetItemData(id, sc_Tally[index].tally);
		if (m_pOther->GetTally() == sc_Tally[index].tally)
			m_ctrlTally.SetCurSel(id);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigOtherPoints::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_Desc.TrimRight();
	m_Desc.TrimLeft();
	m_Desc.Replace(_T("\r\n"), _T("\n"));
	if (m_Name.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_OTHER_NAME));
		return;
	}
	if (m_pOther->GetName() != (LPCTSTR)m_Name)
	{
		if (m_Config.GetOtherPoints().FindOtherPoints((LPCTSTR)m_Name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_OTHER_NAME));
			return;
		}
		m_pOther->SetName((LPCTSTR)m_Name);
	}
	int index = m_ctrlTally.GetCurSel();
	if (index != CB_ERR)
	{
		m_pOther->SetTally(static_cast<ARBConfigOtherPoints::eOtherPointsTally>(m_ctrlTally.GetItemData(index)));
	}
	m_pOther->SetDescription((LPCTSTR)m_Desc);
	CDlgBaseDialog::OnOK();
}
