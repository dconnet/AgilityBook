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
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRun.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBDogRun.h"
#include "DlgRunScore.h"
#include "DlgRunComments.h"
#include "DlgRunReference.h"
#include "DlgRunCRCD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRun

IMPLEMENT_DYNAMIC(CDlgRun, CPropertySheet)

CDlgRun::CDlgRun(CAgilityBookDoc* pDoc, ARBDogTrial* pTrial, ARBDogRun* pRun, CWnd* pParent, UINT iSelectPage)
	: CPropertySheet(IDS_RUN_PROPERTIES, pParent, iSelectPage)
	, m_pRealRun(pRun)
	, m_Run(new ARBDogRun(*pRun))
	, m_pageScore(NULL)
	, m_pageComments(NULL)
	, m_pageReference(NULL)
	, m_pageCRCD(NULL)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	const ARBDogClub* pClub = pTrial->GetClubs().GetPrimaryClub();
	ASSERT(NULL != pClub);
	const ARBConfigVenue* pVenue = pDoc->GetConfig().GetVenues().FindVenue(pClub->GetVenue());
	ASSERT(NULL != pVenue);

	m_pageScore = new CDlgRunScore(pDoc, pVenue, pTrial, m_pRealRun, m_Run);
	m_pageComments = new CDlgRunComments(pDoc, m_Run);
	m_pageReference = new CDlgRunReference(m_Run);
	m_pageCRCD = new CDlgRunCRCD(m_Run);
	AddPage(m_pageScore);
	AddPage(m_pageComments);
	AddPage(m_pageReference);
	AddPage(m_pageCRCD);
}

CDlgRun::~CDlgRun()
{
	m_Run->Release();
	m_Run = NULL;
	delete m_pageScore;
	delete m_pageComments;
	delete m_pageReference;
	delete m_pageCRCD;
}

BEGIN_MESSAGE_MAP(CDlgRun, CPropertySheet)
	//{{AFX_MSG_MAP(CDlgRun)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRun message handlers

void CDlgRun::OnOK()
{
	if (GetActivePage()->UpdateData(TRUE))
	{
		*m_pRealRun = *m_Run;
		CAgilityBookOptions::SetLastEnteredDivision(m_Run->GetDivision().c_str());
		CAgilityBookOptions::SetLastEnteredLevel(m_Run->GetLevel().c_str());
		CAgilityBookOptions::SetLastEnteredHeight(m_Run->GetHeight().c_str());
		CAgilityBookOptions::SetLastEnteredJudge(m_Run->GetJudge().c_str());
		CAgilityBookOptions::SetLastEnteredHandler(m_Run->GetHandler().c_str());
		EndDialog(IDOK);
	}
}
