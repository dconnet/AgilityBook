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
 * @brief implementation of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-10-06 DRC Removed ARB classes so it could be used to lifetime pts.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitlePoints.h"

#include "ARBConfigVenue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints dialog

CDlgConfigTitlePoints::CDlgConfigTitlePoints(
		ARBConfigVenuePtr inVenue,
		double inValue, // Faults or Place
		double inPoints,
		ETitlePointType inType,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigTitlePoints::IDD, pParent)
	, m_Venue(inVenue)
	, m_Value(inValue)
	, m_Points(inPoints)
	, m_Type(inType)
{
	//{{AFX_DATA_INIT(CDlgConfigTitlePoints)
	//}}AFX_DATA_INIT
}


void CDlgConfigTitlePoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigTitlePoints)
	DDX_Control(pDX, IDC_CONFIG_TITLEPTS_VALUE_TEXT, m_ctrlValueText);
	DDX_Text(pDX, IDC_CONFIG_TITLEPTS_VALUE, m_Value);
	DDX_Text(pDX, IDC_CONFIG_TITLEPTS_POINTS, m_Points);
	DDX_Control(pDX, IDC_CONFIG_TITLEPTS_TITLE_POINTS, m_ctrlType);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		int idx = m_ctrlType.GetCurSel();
		ASSERT(0 <= idx);
		m_Type = (ETitlePointType)m_ctrlType.GetItemData(idx);
	}
}


BEGIN_MESSAGE_MAP(CDlgConfigTitlePoints, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitlePoints)
	ON_CBN_SELCHANGE(IDC_CONFIG_TITLEPTS_TITLE_POINTS, OnCbnSelchangeTitlePoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigTitlePoints::UpdateText()
{
	if (eTitlePlacement == m_Type)
		m_ctrlValueText.SetWindowText(m_PlaceText);
	else
		m_ctrlValueText.SetWindowText(m_FaultText);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints message handlers

BOOL CDlgConfigTitlePoints::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	static struct
	{
		UINT iText;
		ETitlePointType eType;
	} sc_Types[] = {
		{ IDS_TITLEPOINT_NORMAL, eTitleNormal},
		{ IDS_TITLEPOINT_LIFETIME, eTitleLifetime},
		{ IDS_TITLEPOINT_PLACEMENT, eTitlePlacement}
	};
	for (int index = 0; index < 3; ++index)
	{
		CString str;
		if (IDS_TITLEPOINT_LIFETIME == sc_Types[index].iText && m_Venue->HasLifetimeName())
			str = m_Venue->GetLifetimeName().c_str();
		else
			str.LoadString(sc_Types[index].iText);
		int idx = m_ctrlType.AddString(str);
		m_ctrlType.SetItemData(idx, sc_Types[index].eType);
		if (m_Type == sc_Types[index].eType)
			m_ctrlType.SetCurSel(index);
	}

	CString str;
	m_ctrlValueText.GetWindowText(str);
	int idx = str.Find('\n');
	ASSERT(0 < idx);
	m_FaultText = str.Left(idx);
	m_PlaceText = str.Mid(idx+1);

	UpdateText();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgConfigTitlePoints::OnCbnSelchangeTitlePoints()
{
	UpdateData(TRUE);
	UpdateText();
}
