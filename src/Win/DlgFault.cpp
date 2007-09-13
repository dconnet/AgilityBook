/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgFault class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include <set>
#include "AgilityBook.h"
#include "DlgFault.h"

#include "AgilityBookDoc.h"
#include "ARBConfig.h"
#include "ARBConfigFault.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFault dialog

CDlgFault::CDlgFault(
		std::set<tstring>& faults,
		LPCTSTR pFault,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgFault::IDD, pParent)
	, m_ctrlFaults(false)
	, m_setFaults(faults)
{
	//{{AFX_DATA_INIT(CDlgFault)
	m_Faults = _T("");
	//}}AFX_DATA_INIT
	if (pFault)
		m_Faults = pFault;
}


void CDlgFault::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFault)
	DDX_Control(pDX, IDC_FAULTS, m_ctrlFaults);
	DDX_CBString(pDX, IDC_FAULTS, m_Faults);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFault, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgFault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFault message handlers

BOOL CDlgFault::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	
	for (std::set<tstring>::const_iterator iter = m_setFaults.begin(); iter != m_setFaults.end(); ++iter)
	{
		int index = m_ctrlFaults.AddString((*iter).c_str());
		if (m_Faults == (*iter).c_str())
			m_ctrlFaults.SetCurSel(index);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgFault::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	m_Faults.TrimRight();
	m_Faults.TrimLeft();
	UpdateData(FALSE);
	if (m_Faults.IsEmpty())
		return;
	CDlgBaseDialog::OnOK();
}
