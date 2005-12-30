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
 * @brief implementation of the CDlgOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Separated options and filters into two dialogs.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
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

CDlgOptions::CDlgOptions(
		CAgilityBookDoc* pDoc,
		CWnd* pParentWnd,
		UINT iSelectPage)
	: CDlgBaseSheet(IDS_VIEWING_OPTIONS, pParentWnd, iSelectPage)
	, m_pDoc(pDoc)
	, m_pageRuns(pDoc->GetConfig())
	, m_pageTraining(pDoc)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Program options
	m_pageProgram.m_bAutoCheck = CAgilityBookOptions::GetAutoUpdateCheck() ? TRUE : FALSE;
	m_pageProgram.m_Backups = CAgilityBookOptions::GetNumBackupFiles();
	m_pageProgram.m_bAutoShow = CAgilityBookOptions::AutoShowPropertiesOnNewTitle() ? TRUE : FALSE;
	m_pageProgram.m_bShowSplash = CAgilityBookOptions::AutoShowSplashScreen() ? TRUE : FALSE;
	m_pageProgram.m_Splash = CAgilityBookOptions::GetSplashImage();

	// Fonts
	CAgilityBookOptions::GetPrinterFontInfo(m_pageFonts.m_fontGeneralPrintInfo);
	CAgilityBookOptions::GetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[0], false);
	CAgilityBookOptions::GetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[0], false);
	CAgilityBookOptions::GetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[1], true);
	CAgilityBookOptions::GetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[1], true);

	// Date
	m_pageDate.m_ViewDates = CAgilityBookOptions::GetViewAllDates() ? 0 : 1;
	m_pageDate.m_timeStart = CAgilityBookOptions::GetStartFilterDate().GetDate();
	m_pageDate.m_bDateStart = CAgilityBookOptions::GetStartFilterDateSet();
	m_pageDate.m_timeEnd = CAgilityBookOptions::GetEndFilterDate().GetDate();
	m_pageDate.m_bDateEnd = CAgilityBookOptions::GetEndFilterDateSet();

	// Runs
	m_pageRuns.m_ViewVenues = CAgilityBookOptions::GetViewAllVenues() ? 0 : 1;
	CAgilityBookOptions::GetFilterVenue(m_pageRuns.m_VenueFilter);
	m_pageRuns.m_ViewQs = CAgilityBookOptions::GetViewAllRuns() ? 0
		: CAgilityBookOptions::GetViewQRuns() ? 1 : 2;

	// Calendar
	m_pageCalendar.m_DayOfWeek = static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek());
	m_pageCalendar.m_bAutoDelete = CAgilityBookOptions::AutoDeleteCalendarEntries() ? TRUE : FALSE;
	m_pageCalendar.m_bHideOld = CAgilityBookOptions::ViewAllCalendarEntries() ? FALSE : TRUE;
	m_pageCalendar.m_Days = CAgilityBookOptions::DaysTillEntryIsPast();
	m_pageCalendar.m_bHideOverlapping = CAgilityBookOptions::HideOverlappingCalendarEntries() ? TRUE : FALSE;
	m_pageCalendar.m_sizeX = CAgilityBookOptions::GetCalendarEntrySize().cx;
	m_pageCalendar.m_sizeY = CAgilityBookOptions::GetCalendarEntrySize().cy;
	CCalendarViewFilter filter = CAgilityBookOptions::FilterCalendarView();
	m_pageCalendar.m_bOpeningNear = CAgilityBookOptions::CalendarOpeningNear() >= 0;
	m_pageCalendar.m_nOpeningNear = CAgilityBookOptions::CalendarOpeningNear();
	if (0 > m_pageCalendar.m_nOpeningNear)
		m_pageCalendar.m_nOpeningNear = 0;
	m_pageCalendar.m_bClosingNear = CAgilityBookOptions::CalendarClosingNear() >= 0;
	m_pageCalendar.m_nClosingNear = CAgilityBookOptions::CalendarClosingNear();
	if (0 > m_pageCalendar.m_nClosingNear)
		m_pageCalendar.m_nClosingNear = 0;
	m_pageCalendar.m_bNotEntered = filter.ViewNotEntered();
	m_pageCalendar.m_bPlanning = filter.ViewPlanning();
	m_pageCalendar.m_bEntered = filter.ViewEntered();
	m_pageCalendar.m_bOpening = filter.ViewOpening();
	m_pageCalendar.m_bClosing = filter.ViewClosing();

	// Training
	m_pageTraining.m_ViewNames = CAgilityBookOptions::GetTrainingViewAllNames() ? 0 : 1;
	CAgilityBookOptions::GetTrainingFilterNames(m_pageTraining.m_filterNames);

	AddPage(&m_pageProgram);
	ASSERT(0 == GetProgramPage());
	AddPage(&m_pageFonts);
	ASSERT(1 == GetFontPage());
	AddPage(&m_pageDate);
	ASSERT(2 == GetDatePage());
	AddPage(&m_pageRuns);
	ASSERT(3 == GetRunsPage());
	AddPage(&m_pageCalendar);
	ASSERT(4 == GetCalendarPage());
	AddPage(&m_pageTraining);
	ASSERT(5 == GetTrainingPage());
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
		CWaitCursor wait;
		bool bOldNewest = CAgilityBookOptions::GetNewestDatesFirst();

		// Program options
		CAgilityBookOptions::SetAutoUpdateCheck(m_pageProgram.m_bAutoCheck ? true : false);
		CAgilityBookOptions::SetNumBackupFiles(m_pageProgram.m_Backups);
		CAgilityBookOptions::AutoShowPropertiesOnNewTitle(m_pageProgram.m_bAutoShow ? true : false);
		CAgilityBookOptions::AutoShowSplashScreen(m_pageProgram.m_bShowSplash ? true : false);
		CAgilityBookOptions::SetSplashImage(m_pageProgram.m_Splash);

		// Fonts
		CAgilityBookOptions::SetPrinterFontInfo(m_pageFonts.m_fontGeneralPrintInfo);
		CAgilityBookOptions::SetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[0], false);
		CAgilityBookOptions::SetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[0], false);
		CAgilityBookOptions::SetCalendarDateFontInfo(m_pageFonts.m_fontDateInfo[1], true);
		CAgilityBookOptions::SetCalendarTextFontInfo(m_pageFonts.m_fontTextInfo[1], true);

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

		// Runs
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

		// Calendar
		CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDate::DayOfWeek>(m_pageCalendar.m_DayOfWeek));
		CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_pageCalendar.m_bAutoDelete ? true : false);
		CAgilityBookOptions::SetViewAllCalendarEntries(m_pageCalendar.m_bHideOld ? false : true);
		CAgilityBookOptions::SetDaysTillEntryIsPast(m_pageCalendar.m_Days);
		CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_pageCalendar.m_bHideOverlapping ? true : false);
		CAgilityBookOptions::SetCalendarEntrySize(CSize(m_pageCalendar.m_sizeX, m_pageCalendar.m_sizeY));
		if (!m_pageCalendar.m_bOpeningNear)
			m_pageCalendar.m_nOpeningNear = -1;
		CAgilityBookOptions::SetCalendarOpeningNear(m_pageCalendar.m_nOpeningNear);
		if (!m_pageCalendar.m_bClosingNear)
			m_pageCalendar.m_nClosingNear = -1;
		CAgilityBookOptions::SetCalendarClosingNear(m_pageCalendar.m_nClosingNear);
		CCalendarViewFilter filter;
		if (m_pageCalendar.m_bNotEntered)
			filter.AddNotEntered();
		if (m_pageCalendar.m_bPlanning)
			filter.AddPlanning();
		if (m_pageCalendar.m_bEntered)
			filter.AddEntered();
		if (m_pageCalendar.m_bOpening)
			filter.AddOpening();
		if (m_pageCalendar.m_bClosing)
			filter.AddClosing();
		CAgilityBookOptions::SetFilterCalendarView(filter);

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
