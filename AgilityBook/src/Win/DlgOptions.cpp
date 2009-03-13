/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2005-08-18 DRC Separated options and filters into two dialogs.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "DlgOptions.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgOptionsProgram.h"
#include <wx/notebook.h>


BEGIN_EVENT_TABLE(CDlgOptions, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgOptions::OnOk)
END_EVENT_TABLE()


CDlgOptions::CDlgOptions(
		CAgilityBookDoc* pDoc,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog(pParent, wxID_ANY, _("IDS_VIEWING_OPTIONS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
	, m_pDoc(pDoc)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	m_pageProgram = new CDlgOptionsProgram(notebook);

	notebook->AddPage(m_pageProgram, _("IDD_VIEW_OPTIONS_PROGRAM"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	/*

	// Filters
	// -handled thru CFilterOptions

	// Calendar
	m_pageCal.m_nOpeningNear = CAgilityBookOptions::CalendarOpeningNear();
	if (0 > m_pageCal.m_nOpeningNear)
	{
		m_pageCal.m_bOpeningNear = FALSE;
		m_pageCal.m_nOpeningNear = 0;
	}
	else
		m_pageCal.m_bOpeningNear = TRUE;
	m_pageCal.m_nClosingNear = CAgilityBookOptions::CalendarClosingNear();
	if (0 > m_pageCal.m_nClosingNear)
	{
		m_pageCal.m_bClosingNear = FALSE;
		m_pageCal.m_nClosingNear = 0;
	}
	else
		m_pageCal.m_bClosingNear = TRUE;
	m_pageCal.m_DayOfWeek = static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek());
	m_pageCal.m_bAutoDelete = CAgilityBookOptions::AutoDeleteCalendarEntries() ? TRUE : FALSE;
	m_pageCal.m_bHideOld = CAgilityBookOptions::ViewAllCalendarEntries() ? FALSE : TRUE;
	m_pageCal.m_Days = CAgilityBookOptions::DaysTillEntryIsPast();
	m_pageCal.m_bHideOverlapping = CAgilityBookOptions::HideOverlappingCalendarEntries() ? TRUE : FALSE;
	m_pageCal.m_bOpening = CAgilityBookOptions::ViewAllCalendarOpening() ? TRUE : FALSE;
	m_pageCal.m_bClosing = CAgilityBookOptions::ViewAllCalendarClosing() ? TRUE : FALSE;
	CAgilityBookOptions::GetCalendarFontInfo(m_pageCal.m_fontCalViewInfo);

	// Printing
	CAgilityBookOptions::GetPrinterFontInfo(m_pagePrint.m_fontPrintInfo);
	CRect margins;
	CAgilityBookOptions::GetPrinterMargins(margins);
	m_pagePrint.m_Left = margins.left / 100.0;
	m_pagePrint.m_Right = margins.right / 100.0;
	m_pagePrint.m_Top = margins.top / 100.0;
	m_pagePrint.m_Bottom = margins.bottom / 100.0;

	AddPage(&m_pageProgram);
	assert(0 == GetProgramPage());
	AddPage(&m_pageFilter);
	assert(1 == GetFilterPage());
	AddPage(&m_pageCal);
	assert(2 == GetCalendarPage());
	AddPage(&m_pagePrint);
	assert(3 == GetPrintPage());
	*/
}


void CDlgOptions::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	wxBusyCursor wait;
	m_pageProgram->Save();

	/*

	// Calendar
	if (!m_pageCal.m_bOpeningNear)
		m_pageCal.m_nOpeningNear = -1;
	CAgilityBookOptions::SetCalendarOpeningNear(m_pageCal.m_nOpeningNear);
	if (!m_pageCal.m_bClosingNear)
		m_pageCal.m_nClosingNear = -1;
	CAgilityBookOptions::SetCalendarClosingNear(m_pageCal.m_nClosingNear);
	CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDate::DayOfWeek>(m_pageCal.m_DayOfWeek));
	CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_pageCal.m_bAutoDelete ? true : false);
	CAgilityBookOptions::SetViewAllCalendarEntries(m_pageCal.m_bHideOld ? false : true);
	CAgilityBookOptions::SetDaysTillEntryIsPast(m_pageCal.m_Days);
	CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_pageCal.m_bHideOverlapping ? true : false);
	CAgilityBookOptions::SetViewAllCalendarOpening(m_pageCal.m_bOpening ? true : false);
	CAgilityBookOptions::SetViewAllCalendarClosing(m_pageCal.m_bClosing ? true : false);
	CAgilityBookOptions::SetCalendarFontInfo(m_pageCal.m_fontCalViewInfo);

	// Printing
	CAgilityBookOptions::SetPrinterFontInfo(m_pagePrint.m_fontPrintInfo);
	CRect margins;
	margins.left = static_cast<int>(m_pagePrint.m_Left * 100);
	margins.right = static_cast<int>(m_pagePrint.m_Right * 100);
	margins.top = static_cast<int>(m_pagePrint.m_Top * 100);
	margins.bottom = static_cast<int>(m_pagePrint.m_Bottom * 100);
	CAgilityBookOptions::SetPrinterMargins(margins);

	// Filters
	// Commit to the registry
	m_pageFilter.m_FilterOptions.Save();
	// Now load into the default object
	CFilterOptions::Options().Load();

	// Update
	m_pDoc->ResetVisibility();
	*/

	if (m_pageProgram->ResetHtmlView())
		m_pDoc->ShowPointsAsHtml(CAgilityBookOptions::ShowHtmlPoints());

	EndDialog(wxID_OK);
}
