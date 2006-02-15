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
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2005-10-17 DRC Points view didn't update 'View Hidden' after canceling.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgDog.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
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

IMPLEMENT_DYNAMIC(CDlgDog, CDlgBaseSheet)

CDlgDog::CDlgDog(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		CWnd* pParent,
		UINT iSelectPage)
	: CDlgBaseSheet(IDS_COL_DOG, pParent, iSelectPage)
	, m_pDoc(pDoc)
	, m_pDog(pDog)
	, m_pageProp(NULL)
	, m_pageTitles(NULL)
	, m_pageRegNums(NULL)
	, m_pagePoints(NULL)
	, m_viewHidden(CAgilityBookOptions::GetViewHiddenTitles())
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_pageProp = new CDlgDogProperties(m_pDoc, pDog);
	m_pageTitles = new CDlgDogTitles(m_pDoc, pDog->GetTitles());
	m_pageRegNums = new CDlgDogNumbers(m_pDoc, pDog->GetRegNums());
	m_pagePoints = new CDlgDogPoints(m_pDoc, pDog->GetExistingPoints());
	AddPage(m_pageProp);
	AddPage(m_pageTitles);
	AddPage(m_pageRegNums);
	AddPage(m_pagePoints);
}

CDlgDog::~CDlgDog()
{
	if (m_viewHidden != CAgilityBookOptions::GetViewHiddenTitles())
	{
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		for (ARBDogList::iterator iterDogs = m_pDoc->GetDogs().begin(); iterDogs != m_pDoc->GetDogs().end(); ++iterDogs)
			for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin(); iterTitle != (*iterDogs)->GetTitles().end(); ++iterTitle)
				m_pDoc->ResetVisibility(venues, *iterTitle);
		m_pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW);
	}

	delete m_pageProp;
	delete m_pageTitles;
	delete m_pageRegNums;
	delete m_pagePoints;
}

BEGIN_MESSAGE_MAP(CDlgDog, CDlgBaseSheet)
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

	m_pDog->SetCallName((LPCTSTR)(m_pageProp->m_CallName));
	m_pDog->SetDOB(m_pageProp->m_DOB);
	m_pDog->SetDeceased(m_pageProp->m_Deceased);
	m_pDog->SetBreed((LPCTSTR)(m_pageProp->m_Breed));
	m_pDog->SetRegisteredName((LPCTSTR)(m_pageProp->m_RegName));
	m_pageProp->m_Notes.Replace(_T("\r\n"), _T("\n"));
	m_pDog->SetNote((LPCTSTR)(m_pageProp->m_Notes));
	m_pageTitles->m_Titles.Clone(m_pDog->GetTitles());
	m_pageRegNums->m_RegNums.Clone(m_pDog->GetRegNums());
	m_pagePoints->m_ExistingPoints.Clone(m_pDog->GetExistingPoints());

	m_pDoc->ResetVisibility();

	EndDialog(IDOK);
}
