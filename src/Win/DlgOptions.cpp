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
 * @brief implementation of the CDlgOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptions.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions

IMPLEMENT_DYNAMIC(CDlgOptions, CDlgBaseSheet)

CDlgOptions::CDlgOptions(CAgilityBookDoc* pDoc, CWnd* pParentWnd, UINT iSelectPage)
	: CDlgBaseSheet(IDS_VIEWING_OPTIONS, pParentWnd, iSelectPage)
	, m_pDoc(pDoc)
	, m_pageFilter(pDoc->GetConfig())
	, m_pageTraining(pDoc)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Calendar
	m_pageCalendar.m_DayOfWeek = (int)CAgilityBookOptions::GetFirstDayOfWeek();
	m_pageCalendar.m_bAutoDelete = CAgilityBookOptions::AutoDeleteCalendarEntries() ? TRUE : FALSE;
	m_pageCalendar.m_bHideOld = CAgilityBookOptions::ViewAllCalendarEntries() ? FALSE : TRUE;
	m_pageCalendar.m_Days = CAgilityBookOptions::DaysTillEntryIsPast();
	m_pageCalendar.m_bHideOverlapping = CAgilityBookOptions::HideOverlappingCalendarEntries() ? TRUE : FALSE;
	m_pageCalendar.m_sizeX = CAgilityBookOptions::GetCalendarEntrySize().cx;
	m_pageCalendar.m_sizeY = CAgilityBookOptions::GetCalendarEntrySize().cy;
	// Filter
	m_pageFilter.m_ViewDates = CAgilityBookOptions::GetViewAllDates() ? 0 : 1;
	m_pageFilter.m_timeStart = CAgilityBookOptions::GetStartFilterDate().GetDate();
	m_pageFilter.m_bDateStart = CAgilityBookOptions::GetStartFilterDateSet();
	m_pageFilter.m_timeEnd = CAgilityBookOptions::GetEndFilterDate().GetDate();
	m_pageFilter.m_bDateEnd = CAgilityBookOptions::GetEndFilterDateSet();
	m_pageFilter.m_ViewVenues = CAgilityBookOptions::GetViewAllVenues() ? 0 : 1;
	CAgilityBookOptions::GetFilterVenue(m_pageFilter.m_VenueFilter);
	m_pageFilter.m_ViewQs = CAgilityBookOptions::GetViewAllRuns() ? 0
		: CAgilityBookOptions::GetViewQRuns() ? 1 : 2;
	// Training
	m_pageTraining.m_ViewDates = CAgilityBookOptions::GetTrainingViewAllDates() ? 0 : 1;
	m_pageTraining.m_timeStart = CAgilityBookOptions::GetTrainingStartFilterDate().GetDate();
	m_pageTraining.m_bDateStart = CAgilityBookOptions::GetTrainingStartFilterDateSet();
	m_pageTraining.m_timeEnd = CAgilityBookOptions::GetTrainingEndFilterDate().GetDate();
	m_pageTraining.m_bDateEnd = CAgilityBookOptions::GetTrainingEndFilterDateSet();
	m_pageTraining.m_ViewNames = CAgilityBookOptions::GetTrainingViewAllNames() ? 0 : 1;
	CAgilityBookOptions::GetTrainingFilterNames(m_pageTraining.m_filterNames);
	// Fonts
	CAgilityBookOptions::GetPrinterFontInfo(m_pageFonts.m_fontGeneralPrintInfo);
	CAgilityBookOptions::GetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[0], false);
	CAgilityBookOptions::GetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[0], false);
	CAgilityBookOptions::GetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[1], true);
	CAgilityBookOptions::GetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[1], true);
	// Program options
	m_pageProgram.m_bAutoCheck = CAgilityBookOptions::GetAutoUpdateCheck() ? TRUE : FALSE;
	m_pageProgram.m_Backups = CAgilityBookOptions::GetNumBackupFiles();
	m_pageProgram.m_bAutoShow = CAgilityBookOptions::AutoShowPropertiesOnNewTitle() ? TRUE : FALSE;
	m_pageProgram.m_Splash = CAgilityBookOptions::GetSplashImage();

	AddPage(&m_pageFilter);
	AddPage(&m_pageCalendar);
	AddPage(&m_pageTraining);
	AddPage(&m_pageFonts);
	AddPage(&m_pageProgram);
}

CDlgOptions::~CDlgOptions()
{
}

BOOL CDlgOptions::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDlgBaseSheet::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CDlgOptions, CDlgBaseSheet)
	//{{AFX_MSG_MAP(CDlgOptions)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions message handlers

void CDlgOptions::OnOK()
{
	if (GetActivePage()->UpdateData(TRUE))
	{
		bool bOldNewest = CAgilityBookOptions::GetNewestDatesFirst();
		// Calendar
		CAgilityBookOptions::SetFirstDayOfWeek((ARBDate::DayOfWeek)m_pageCalendar.m_DayOfWeek);
		CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_pageCalendar.m_bAutoDelete ? true : false);
		CAgilityBookOptions::SetViewAllCalendarEntries(m_pageCalendar.m_bHideOld ? false : true);
		CAgilityBookOptions::SetDaysTillEntryIsPast(m_pageCalendar.m_Days);
		CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_pageCalendar.m_bHideOverlapping ? true : false);
		CAgilityBookOptions::SetCalendarEntrySize(CSize(m_pageCalendar.m_sizeX, m_pageCalendar.m_sizeY));
		// Runs
		CAgilityBookOptions::SetViewAllDates(m_pageFilter.m_ViewDates == 0);
		CAgilityBookOptions::SetStartFilterDate(m_pageFilter.m_timeStart.GetTime());
		CAgilityBookOptions::SetStartFilterDateSet(m_pageFilter.m_bDateStart ? true : false);
		CAgilityBookOptions::SetEndFilterDate(m_pageFilter.m_timeEnd.GetTime());
		CAgilityBookOptions::SetEndFilterDateSet(m_pageFilter.m_bDateEnd ? true : false);
		CAgilityBookOptions::SetViewAllVenues(m_pageFilter.m_ViewVenues == 0);
		CAgilityBookOptions::SetFilterVenue(m_pageFilter.m_VenueFilter);
		if (m_pageFilter.m_ViewDates != 0
		&& !m_pageFilter.m_bDateStart 
		&& !m_pageFilter.m_bDateEnd)
		{
			CAgilityBookOptions::SetViewAllDates(true);
		}
		if (m_pageFilter.m_ViewVenues != 0
		&& 0 == m_pageFilter.m_VenueFilter.size())
		{
			CAgilityBookOptions::SetViewAllVenues(true);
		}
		switch (m_pageFilter.m_ViewQs)
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
		CAgilityBookOptions::SetTrainingViewAllDates(m_pageTraining.m_ViewDates == 0);
		CAgilityBookOptions::SetTrainingStartFilterDate(m_pageTraining.m_timeStart.GetTime());
		CAgilityBookOptions::SetTrainingStartFilterDateSet(m_pageTraining.m_bDateStart ? true : false);
		CAgilityBookOptions::SetTrainingEndFilterDate(m_pageTraining.m_timeEnd.GetTime());
		CAgilityBookOptions::SetTrainingEndFilterDateSet(m_pageTraining.m_bDateEnd ? true : false);
		CAgilityBookOptions::SetTrainingViewAllNames(m_pageTraining.m_ViewNames == 0);
		CAgilityBookOptions::SetTrainingFilterNames(m_pageTraining.m_filterNames);
		if (m_pageTraining.m_ViewDates != 0
		&& !m_pageTraining.m_bDateStart 
		&& !m_pageTraining.m_bDateEnd)
		{
			CAgilityBookOptions::SetTrainingViewAllDates(true);
		}
		if (m_pageTraining.m_ViewNames != 0
		&& 0 == m_pageTraining.m_filterNames.size())
		{
			CAgilityBookOptions::SetTrainingViewAllNames(true);
		}
		// Fonts
		CAgilityBookOptions::SetPrinterFontInfo(m_pageFonts.m_fontGeneralPrintInfo);
		CAgilityBookOptions::SetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[0], false);
		CAgilityBookOptions::SetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[0], false);
		CAgilityBookOptions::SetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[1], true);
		CAgilityBookOptions::SetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[1], true);
		// Program options
		CAgilityBookOptions::SetAutoUpdateCheck(m_pageProgram.m_bAutoCheck ? true : false);
		CAgilityBookOptions::SetNumBackupFiles(m_pageProgram.m_Backups);
		CAgilityBookOptions::AutoShowPropertiesOnNewTitle(m_pageProgram.m_bAutoShow ? true : false);
		CAgilityBookOptions::SetSplashImage(m_pageProgram.m_Splash);
		// Update
		m_pDoc->ResetVisibility();
		EndDialog(IDOK);
	}
}
