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
 * @brief implementation of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOtherPoint.h"

#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBDogRunOtherPoints.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOtherPoint dialog

CDlgOtherPoint::CDlgOtherPoint(
		ARBConfig const& config,
		ARBDogRunOtherPointsPtr pOther,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgOtherPoint::IDD, pParent)
	, m_ctrlOtherPoints(true)
	, m_Config(config)
	, m_pOther(pOther)
{
	//{{AFX_DATA_INIT(CDlgOtherPoint)
	m_Points = m_pOther->GetPoints();
	//}}AFX_DATA_INIT
}

void CDlgOtherPoint::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOtherPoint)
	DDX_Control(pDX, IDC_OTHER_OTHERPOINTS, m_ctrlOtherPoints);
	DDX_Control(pDX, IDC_OTHER_DESC, m_ctrlDesc);
	DDX_Text(pDX, IDC_OTHER_POINTS, m_Points);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOtherPoint, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgOtherPoint)
	ON_CBN_SELCHANGE(IDC_OTHER_OTHERPOINTS, OnSelchangeOtherpoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOtherPoint message handlers

BOOL CDlgOtherPoint::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();

	m_ctrlDesc.SetWindowText(_T(""));
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_Config.GetOtherPoints().begin();
	iterOther != m_Config.GetOtherPoints().end();
	++iterOther)
	{
		ARBConfigOtherPointsPtr pOther = (*iterOther);
		int index = m_ctrlOtherPoints.AddString(pOther->GetName().c_str());
		m_ctrlOtherPoints.SetData(index,
			new CListPtrData<ARBConfigOtherPointsPtr>(pOther));
		if (pOther->GetName() == m_pOther->GetName())
		{
			m_ctrlOtherPoints.SetCurSel(index);
			CString str(pOther->GetDescription().c_str());
			str.Replace(_T("\n"), _T("\r\n"));
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
		CListData* pData = m_ctrlOtherPoints.GetData(index);
		ARBConfigOtherPointsPtr pOther = dynamic_cast<CListPtrData<ARBConfigOtherPointsPtr>*>(pData)->GetData();
		CString str(pOther->GetDescription().c_str());
		str.Replace(_T("\n"), _T("\r\n"));
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
	CListData* pData = m_ctrlOtherPoints.GetData(index);
	ARBConfigOtherPointsPtr pOther = dynamic_cast<CListPtrData<ARBConfigOtherPointsPtr>*>(pData)->GetData();
	m_pOther->SetName(pOther->GetName());
	m_pOther->SetPoints(m_Points);
	CDlgBaseDialog::OnOK();
}
