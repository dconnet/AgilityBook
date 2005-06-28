/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2004-10-06 DRC Removed ARB classes so it could be used to lifetime pts.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitlePoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints dialog

CDlgConfigTitlePoints::CDlgConfigTitlePoints(
		short inPoints,
		short inFaults,
		BOOL bLifetime,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigTitlePoints::IDD, pParent)
	, m_Points(inPoints)
	, m_Faults(inFaults)
	, m_LifeTime(bLifetime)
{
	//{{AFX_DATA_INIT(CDlgConfigTitlePoints)
	//}}AFX_DATA_INIT
}

void CDlgConfigTitlePoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigTitlePoints)
	DDX_Text(pDX, IDC_CONFIG_TITLEPTS_POINTS, m_Points);
	DDX_Text(pDX, IDC_CONFIG_TITLEPTS_FAULTS, m_Faults);
	DDX_Check(pDX, IDC_CONFIG_TITLEPTS_TITLE_POINTS, m_LifeTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigTitlePoints, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitlePoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitlePoints message handlers
