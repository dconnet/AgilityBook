/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDog.h"

#include "ARBDog.h"
#include "DlgDogNumbers.h"
#include "DlgDogPoints.h"
#include "DlgDogProp.h"
#include "DlgDogTitles.h"
#include "DlgRegNum.h"
#include "DlgTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDog dialog

IMPLEMENT_DYNAMIC(CDlgDog, CPropertySheet)

CDlgDog::CDlgDog(ARBConfig& config, ARBDog* pDog, CWnd* pParent, UINT iSelectPage)
	: CPropertySheet(IDS_COL_DOG, pParent, iSelectPage)
	, m_Config(config)
	, m_pDog(pDog)
	, m_pageProp(NULL)
	, m_pageTitles(NULL)
	, m_pageRegNums(NULL)
	, m_pagePoints(NULL)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_pageProp = new CDlgDogProperties(config, pDog);
	m_pageTitles = new CDlgDogTitles(config, pDog->GetTitles());
	m_pageRegNums = new CDlgDogNumbers(config, pDog->GetRegNums());
	m_pagePoints = new CDlgDogPoints(config, pDog->GetExistingPoints());
	AddPage(m_pageProp);
	AddPage(m_pageTitles);
	AddPage(m_pageRegNums);
	AddPage(m_pagePoints);
}

CDlgDog::~CDlgDog()
{
	delete m_pageProp;
	delete m_pageTitles;
	delete m_pageRegNums;
	delete m_pagePoints;
}

BEGIN_MESSAGE_MAP(CDlgDog, CPropertySheet)
	//{{AFX_MSG_MAP(CDlgDog)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDog message handlers

void CDlgDog::OnOK()
{
	if (!GetActivePage()->UpdateData(TRUE))
		return;

	m_pDog->SetCallName((LPCSTR)(m_pageProp->m_CallName));
	m_pDog->SetDOB(m_pageProp->m_DOB);
	m_pDog->SetDeceased(m_pageProp->m_Deceased);
	m_pDog->SetBreed((LPCSTR)(m_pageProp->m_Breed));
	m_pDog->SetRegisteredName((LPCSTR)(m_pageProp->m_RegName));
	m_pageProp->m_Notes.Replace("\r\n", "\n");
	m_pDog->SetNote((LPCSTR)(m_pageProp->m_Notes));
	m_pDog->GetTitles() = m_pageTitles->m_Titles;
	m_pDog->GetRegNums() = m_pageRegNums->m_RegNums;
	m_pDog->GetExistingPoints() = m_pagePoints->m_ExistingPoints;

	EndDialog(IDOK);
}
