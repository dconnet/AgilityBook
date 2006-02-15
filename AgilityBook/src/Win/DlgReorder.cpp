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
 * @brief implementation of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "resource.h"
#include "DlgReorder.h"

#include "ARBBase.h"
#include "DlgConfigureData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReorder dialog

CDlgReorder::CDlgReorder(
		std::vector<ARBBasePtr>& items,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgReorder::IDD, pParent)
	, m_ctrlList(false)
	, m_Items(items)
{
	//{{AFX_DATA_INIT(CDlgReorder)
	//}}AFX_DATA_INIT
}

void CDlgReorder::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReorder)
	DDX_Control(pDX, IDC_REORDER_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_REORDER_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_REORDER_MOVE_DOWN, m_ctrlDown);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgReorder, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgReorder)
	ON_LBN_SELCHANGE(IDC_REORDER_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_REORDER_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_REORDER_MOVE_DOWN, OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgReorder::LoadData()
{
	m_ctrlList.ResetContent();
	for (size_t i = 0; i < m_Items.size(); ++i)
		m_ctrlList.AddString(m_Items[i]->GetGenericName().c_str());
	EnableControls();
}

void CDlgReorder::EnableControls()
{
	BOOL bEnableUp = FALSE;
	BOOL bEnableDown = FALSE;
	int index = m_ctrlList.GetCurSel();
	if (LB_ERR != index)
	{
		bEnableUp = bEnableDown = TRUE;
		if (0 == index)
			bEnableUp = FALSE;
		if (index == m_ctrlList.GetCount() - 1)
			bEnableDown = FALSE;
	}
	m_ctrlUp.EnableWindow(bEnableUp);
	m_ctrlDown.EnableWindow(bEnableDown);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgReorder message handlers

BOOL CDlgReorder::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();

	LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReorder::OnSelchangeList() 
{
	EnableControls();	
}

void CDlgReorder::OnMoveUp() 
{
	int index = m_ctrlList.GetCurSel();
	if (LB_ERR != index)
	{
		int newIndex = index - 1;
		if (0 <= newIndex)
		{
			ARBBasePtr pTmp = m_Items[index];
			m_Items[index] = m_Items[newIndex];
			m_Items[newIndex] = pTmp;
			LoadData();
			m_ctrlList.SetCurSel(newIndex);
			EnableControls();
		}
	}
}

void CDlgReorder::OnMoveDown() 
{
	int index = m_ctrlList.GetCurSel();
	if (LB_ERR != index)
	{
		int newIndex = index + 1;
		if (newIndex < m_ctrlList.GetCount())
		{
			ARBBasePtr pTmp = m_Items[index];
			m_Items[index] = m_Items[newIndex];
			m_Items[newIndex] = pTmp;
			LoadData();
			m_ctrlList.SetCurSel(newIndex);
			EnableControls();
		}
	}
}

void CDlgReorder::OnOK() 
{
	CDlgBaseDialog::OnOK();
}
