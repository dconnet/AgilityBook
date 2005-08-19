/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgFilter class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Separated options and filters into two dialogs.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgFilter.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFilter

IMPLEMENT_DYNAMIC(CDlgFilter, CDlgBaseSheet)

CDlgFilter::CDlgFilter(
		CAgilityBookDoc* pDoc,
		CWnd* pParentWnd,
		UINT iSelectPage)
	: CDlgBaseSheet(IDS_VIEWING_FILTERS, pParentWnd, iSelectPage)
	, m_pDoc(pDoc)
	, m_pageRuns(pDoc->GetConfig())
	, m_pageTraining(pDoc)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Date
	m_pageDate.m_ViewDates = CAgilityBookOptions::GetViewAllDates() ? 0 : 1;
	m_pageDate.m_timeStart = CAgilityBookOptions::GetStartFilterDate().GetDate();
	m_pageDate.m_bDateStart = CAgilityBookOptions::GetStartFilterDateSet();
	m_pageDate.m_timeEnd = CAgilityBookOptions::GetEndFilterDate().GetDate();
	m_pageDate.m_bDateEnd = CAgilityBookOptions::GetEndFilterDateSet();

	// Filter
	m_pageRuns.m_ViewVenues = CAgilityBookOptions::GetViewAllVenues() ? 0 : 1;
	CAgilityBookOptions::GetFilterVenue(m_pageRuns.m_VenueFilter);
	m_pageRuns.m_ViewQs = CAgilityBookOptions::GetViewAllRuns() ? 0
		: CAgilityBookOptions::GetViewQRuns() ? 1 : 2;

	// Training
	m_pageTraining.m_ViewNames = CAgilityBookOptions::GetTrainingViewAllNames() ? 0 : 1;
	CAgilityBookOptions::GetTrainingFilterNames(m_pageTraining.m_filterNames);

	AddPage(&m_pageDate);
	ASSERT(0 == GetDatePage());
	AddPage(&m_pageRuns);
	ASSERT(1 == GetRunsPage());
	AddPage(&m_pageTraining);
	ASSERT(2 == GetTrainingPage());
}

CDlgFilter::~CDlgFilter()
{
}

BOOL CDlgFilter::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDlgBaseSheet::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CDlgFilter, CDlgBaseSheet)
	//{{AFX_MSG_MAP(CDlgFilter)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFilter message handlers

void CDlgFilter::OnOK()
{
	if (GetActivePage()->UpdateData(TRUE))
	{
		CWaitCursor wait;
		bool bOldNewest = CAgilityBookOptions::GetNewestDatesFirst();

		// Date
		CAgilityBookOptions::SetViewAllDates(m_pageDate.m_ViewDates == 0);
		CAgilityBookOptions::SetStartFilterDate(m_pageDate.m_timeStart.GetTime());
		CAgilityBookOptions::SetStartFilterDateSet(m_pageDate.m_bDateStart ? true : false);
		CAgilityBookOptions::SetEndFilterDate(m_pageDate.m_timeEnd.GetTime());
		CAgilityBookOptions::SetEndFilterDateSet(m_pageDate.m_bDateEnd ? true : false);
		if (m_pageDate.m_ViewDates != 0
		&& !m_pageDate.m_bDateStart 
		&& !m_pageDate.m_bDateEnd)
		{
			CAgilityBookOptions::SetViewAllDates(true);
		}

		// Filter
		CAgilityBookOptions::SetViewAllVenues(m_pageRuns.m_ViewVenues == 0);
		CAgilityBookOptions::SetFilterVenue(m_pageRuns.m_VenueFilter);
		if (m_pageRuns.m_ViewVenues != 0
		&& 0 == m_pageRuns.m_VenueFilter.size())
		{
			CAgilityBookOptions::SetViewAllVenues(true);
		}
		switch (m_pageRuns.m_ViewQs)
		{
		default:
		case 0:
			CAgilityBookOptions::SetViewAllRuns(true);
			CAgilityBookOptions::SetViewQRuns(true);
			break;
		case 1:
			CAgilityBookOptions::SetViewAllRuns(false);
			CAgilityBookOptions::SetViewQRuns(true);
			break;
		case 2:
			CAgilityBookOptions::SetViewAllRuns(false);
			CAgilityBookOptions::SetViewQRuns(false);
			break;
		}
		if (bOldNewest != CAgilityBookOptions::GetNewestDatesFirst())
			m_pDoc->SortDates();

		// Training
		CAgilityBookOptions::SetTrainingViewAllNames(m_pageTraining.m_ViewNames == 0);
		CAgilityBookOptions::SetTrainingFilterNames(m_pageTraining.m_filterNames);
		if (m_pageTraining.m_ViewNames != 0
		&& 0 == m_pageTraining.m_filterNames.size())
		{
			CAgilityBookOptions::SetTrainingViewAllNames(true);
		}

		// Update
		m_pDoc->ResetVisibility();
		EndDialog(IDOK);
	}
}
