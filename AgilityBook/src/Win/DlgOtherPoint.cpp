/*
 * Copyright © 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOtherPoint.h"

#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBDogRunOtherPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOtherPoint dialog

CDlgOtherPoint::CDlgOtherPoint(ARBConfig const& config, ARBDogRunOtherPoints* pOther, CWnd* pParent)
	: CDialog(CDlgOtherPoint::IDD, pParent)
	, m_Config(config)
	, m_pOther(pOther)
{
	//{{AFX_DATA_INIT(CDlgOtherPoint)
	m_Points = m_pOther->GetPoints();
	//}}AFX_DATA_INIT
}

void CDlgOtherPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOtherPoint)
	DDX_Control(pDX, IDC_OTHERPOINTS, m_ctrlOtherPoints);
	DDX_Control(pDX, IDC_DESC, m_ctrlDesc);
	DDX_Text(pDX, IDC_POINTS, m_Points);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOtherPoint, CDialog)
	//{{AFX_MSG_MAP(CDlgOtherPoint)
	ON_CBN_SELCHANGE(IDC_OTHERPOINTS, OnSelchangeOtherpoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOtherPoint message handlers

BOOL CDlgOtherPoint::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlDesc.SetWindowText("");
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_Config.GetOtherPoints().begin();
	iterOther != m_Config.GetOtherPoints().end();
	++iterOther)
	{
		ARBConfigOtherPoints* pOther = (*iterOther);
		int index = m_ctrlOtherPoints.AddString(pOther->GetName().c_str());
		m_ctrlOtherPoints.SetItemDataPtr(index, pOther);
		if (pOther->GetName() == m_pOther->GetName())
		{
			m_ctrlOtherPoints.SetCurSel(index);
			CString str(pOther->GetDescription().c_str());
			str.Replace("\n", "\r\n");
			m_ctrlDesc.SetWindowText(str);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOtherPoint::OnSelchangeOtherpoints() 
{
	int index = m_ctrlOtherPoints.GetCurSel();
	if (CB_ERR != index)
	{
		ARBConfigOtherPoints* pOther = reinterpret_cast<ARBConfigOtherPoints*>(m_ctrlOtherPoints.GetItemDataPtr(index));
		CString str(pOther->GetDescription().c_str());
		str.Replace("\n", "\r\n");
		m_ctrlDesc.SetWindowText(str);
	}
}

void CDlgOtherPoint::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	int index = m_ctrlOtherPoints.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlOtherPoints);
		return;
	}
	ARBConfigOtherPoints* pOther = reinterpret_cast<ARBConfigOtherPoints*>(m_ctrlOtherPoints.GetItemDataPtr(index));
	m_pOther->SetName(pOther->GetName());
	m_pOther->SetPoints(m_Points);
	CDialog::OnOK();
}
