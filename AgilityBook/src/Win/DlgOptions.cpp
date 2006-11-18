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
#include "MainFrm.h"

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
	, m_pageFilter(pDoc)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Program options
	m_pageProgram.m_bAutoCheck = CAgilityBookOptions::GetAutoUpdateCheck() ? TRUE : FALSE;
	m_pageProgram.m_Backups = CAgilityBookOptions::GetNumBackupFiles();
	m_pageProgram.m_bAutoShow = CAgilityBookOptions::AutoShowPropertiesOnNewTitle() ? TRUE : FALSE;
	m_pageProgram.m_bShowSplash = CAgilityBookOptions::AutoShowSplashScreen() ? TRUE : FALSE;
	m_pageProgram.m_Splash = CAgilityBookOptions::GetSplashImage();
	m_pageProgram.m_bShowHtml = CAgilityBookOptions::ShowHtmlPoints(&m_pageProgram.m_IEInstalled);

	// Filters
	// -handled thru CFilterOptions

	// Views
	m_pageView.m_nOpeningNear = CAgilityBookOptions::CalendarOpeningNear();
	if (0 > m_pageView.m_nOpeningNear)
	{
		m_pageView.m_bOpeningNear = FALSE;
		m_pageView.m_nOpeningNear = 0;
	}
	else
		m_pageView.m_bOpeningNear = TRUE;
	m_pageView.m_nClosingNear = CAgilityBookOptions::CalendarClosingNear();
	if (0 > m_pageView.m_nClosingNear)
	{
		m_pageView.m_bClosingNear = FALSE;
		m_pageView.m_nClosingNear = 0;
	}
	else
		m_pageView.m_bClosingNear = TRUE;
	m_pageView.m_DayOfWeek = static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek());
	m_pageView.m_sizeX = CAgilityBookOptions::GetCalendarEntrySize().cx;
	m_pageView.m_sizeY = CAgilityBookOptions::GetCalendarEntrySize().cy;
	m_pageView.m_bAutoDelete = CAgilityBookOptions::AutoDeleteCalendarEntries() ? TRUE : FALSE;
	m_pageView.m_bHideOld = CAgilityBookOptions::ViewAllCalendarEntries() ? FALSE : TRUE;
	m_pageView.m_Days = CAgilityBookOptions::DaysTillEntryIsPast();
	m_pageView.m_bHideOverlapping = CAgilityBookOptions::HideOverlappingCalendarEntries() ? TRUE : FALSE;
	m_pageView.m_bOpening = CAgilityBookOptions::ViewAllCalendarOpening() ? TRUE : FALSE;
	m_pageView.m_bClosing = CAgilityBookOptions::ViewAllCalendarClosing() ? TRUE : FALSE;
	CAgilityBookOptions::GetPrinterFontInfo(m_pageView.m_fontPrintInfo);
	CAgilityBookOptions::GetCalendarFontInfo(m_pageView.m_fontCalViewInfo);

	AddPage(&m_pageProgram);
	ASSERT(0 == GetProgramPage());
	AddPage(&m_pageFilter);
	ASSERT(1 == GetFilterPage());
	AddPage(&m_pageView);
	ASSERT(2 == GetViewPage());
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
		bool bResetHtmlView = false;

		// Program options
		CAgilityBookOptions::SetAutoUpdateCheck(m_pageProgram.m_bAutoCheck ? true : false);
		CAgilityBookOptions::SetNumBackupFiles(m_pageProgram.m_Backups);
		CAgilityBookOptions::AutoShowPropertiesOnNewTitle(m_pageProgram.m_bAutoShow ? true : false);
		CAgilityBookOptions::AutoShowSplashScreen(m_pageProgram.m_bShowSplash ? true : false);
		CAgilityBookOptions::SetSplashImage(m_pageProgram.m_Splash);
		if (m_pageProgram.m_IEInstalled)
		{
			bool bShow = m_pageProgram.m_bShowHtml ? true : false;
			if (CAgilityBookOptions::ShowHtmlPoints() != bShow)
			{
				bResetHtmlView = true;
				CAgilityBookOptions::SetShowHtmlPoints(bShow);
			}
		}

		// Views
		if (!m_pageView.m_bOpeningNear)
			m_pageView.m_nOpeningNear = -1;
		CAgilityBookOptions::SetCalendarOpeningNear(m_pageView.m_nOpeningNear);
		if (!m_pageView.m_bClosingNear)
			m_pageView.m_nClosingNear = -1;
		CAgilityBookOptions::SetCalendarClosingNear(m_pageView.m_nClosingNear);
		CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDate::DayOfWeek>(m_pageView.m_DayOfWeek));
		CAgilityBookOptions::SetCalendarEntrySize(CSize(m_pageView.m_sizeX, m_pageView.m_sizeY));
		CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_pageView.m_bAutoDelete ? true : false);
		CAgilityBookOptions::SetViewAllCalendarEntries(m_pageView.m_bHideOld ? false : true);
		CAgilityBookOptions::SetDaysTillEntryIsPast(m_pageView.m_Days);
		CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_pageView.m_bHideOverlapping ? true : false);
		CAgilityBookOptions::SetViewAllCalendarOpening(m_pageView.m_bOpening ? true : false);
		CAgilityBookOptions::SetViewAllCalendarClosing(m_pageView.m_bClosing ? true : false);
		CAgilityBookOptions::SetPrinterFontInfo(m_pageView.m_fontPrintInfo);
		CAgilityBookOptions::SetCalendarFontInfo(m_pageView.m_fontCalViewInfo);

		// Filters
		// Commit to the registry
		m_pageFilter.m_FilterOptions.Save();
		// Now load into the default object
		CFilterOptions::Options().Load();

		// Update
		m_pDoc->ResetVisibility();

		if (bResetHtmlView)
		{
			CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
			pFrame->ShowPointsAs(CAgilityBookOptions::ShowHtmlPoints());
		}

		EndDialog(IDOK);
	}
}
