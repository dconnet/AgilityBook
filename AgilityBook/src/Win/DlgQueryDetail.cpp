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
		const TCHAR* inCode,
		const TCHAR* inName,
		CWnd* pParent,
		ARBConfig const* inConfig)
	: CDlgBaseDialog(CDlgQueryDetail::IDD, pParent)
	, m_strCode(inCode)
	, m_strName(inName)
	, m_ctrlVenues(true)
	, m_strVenue(inName)
	, m_Config(inConfig)
	, m_Code(inCode)
	, m_Name(inName)
{
}


void CDlgQueryDetail::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryDetail)
	DDX_Control(pDX, IDC_QUERYDETAIL_CODE, m_ctrlCode);
	DDX_Text(pDX, IDC_QUERYDETAIL_CODE, m_strCode);
	DDX_Control(pDX, IDC_QUERYDETAIL_NAME_TEXT, m_ctrlNameText);
	DDX_Control(pDX, IDC_QUERYDETAIL_NAME, m_ctrlName);
	DDX_Text(pDX, IDC_QUERYDETAIL_NAME, m_strName);
	DDX_Control(pDX, IDC_QUERYDETAIL_COMBO, m_ctrlVenues);
	DDX_Text(pDX, IDC_QUERYDETAIL_COMBO, m_strVenue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryDetail, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgQueryDetail)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_QUERYDETAIL_COMBO, &CDlgQueryDetail::OnCbnSelchangeQuerydetailCombo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryDetail message handlers

BOOL CDlgQueryDetail::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	CString str;
	if (m_Config)
	{
		str.LoadString(IDS_COL_VENUE);
		m_ctrlName.ShowWindow(SW_HIDE);
		m_ctrlVenues.Initialize(m_Config->GetVenues(), m_Name.empty() ? m_Code : m_Name, true);
	}
	else
	{
		str.LoadString(IDS_COL_LOCATION);
		m_ctrlVenues.ShowWindow(SW_HIDE);
	}
	m_ctrlNameText.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgQueryDetail::OnCbnSelchangeQuerydetailCombo()
{
	int idx = m_ctrlVenues.GetCurSel();
	if (0 <= idx)
	{
		CListData* pRawData = m_ctrlVenues.GetData(idx);
		if (pRawData)
		{
			CListPtrData<ARBConfigVenuePtr>* pData = dynamic_cast<CListPtrData<ARBConfigVenuePtr>*>(pRawData);
			if (pData)
			{
				if (m_strCode != pData->GetData()->GetName().c_str())
				{
					// Yes, this will kill the existing code. In general, most
					// sites will probably use the same acronyms I am. So I'm
					// just not going to worry about this.
					m_strCode = pData->GetData()->GetName().c_str();
					UpdateData(FALSE);
				}
			}
		}
	}
}


void CDlgQueryDetail::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	m_strCode.TrimRight();
	m_strCode.TrimLeft();
	m_strName.TrimRight();
	m_strName.TrimLeft();
	m_strVenue.TrimRight();
	m_strVenue.TrimLeft();

	if (m_Config && m_strCode.IsEmpty())
		m_strCode = m_strVenue;
	if (m_strCode.IsEmpty())
	{
		MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_QUERYDETAIL_CODE));
		return;
	}

	m_Code = (LPCTSTR)m_strCode;
	if (m_Config)
		m_Name = (LPCTSTR)m_strVenue;
	else
		m_Name = (LPCTSTR)m_strName;
	if (m_Config)
	{
		// If there's no name, use the code.
		if (m_Name.empty())
			m_Name = m_Code;
		// If no code, use the name.
		if (m_Code.empty())
			m_Code = m_Name;
		// Do not verify against our venues. Originally, we set the name to
		// the code if the code was one of our venues - but it's possible that
		// the code someone uses may map to one of our codes, but actually be
		// a different venue.
	}

	CDlgBaseDialog::OnOK();
}
