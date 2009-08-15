/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgOptionsCalendar.h"
#include "DlgOptionsFilter.h"
#include "DlgOptionsPrint.h"
#include "DlgOptionsProgram.h"
#include <wx/notebook.h>


BEGIN_EVENT_TABLE(CDlgOptions, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgOptions::OnOk)
END_EVENT_TABLE()


CDlgOptions::CDlgOptions(
		CAgilityBookDoc* pDoc,
		wxWindow* pParent,
		size_t iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDS_VIEWING_OPTIONS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	m_pageProgram = new CDlgOptionsProgram(notebook);
	m_pageFilter = new CDlgOptionsFilter(notebook, m_pDoc);
	m_pageCal = new CDlgOptionsCalendar(notebook);
	m_pagePrint = new CDlgOptionsPrint(notebook);

	notebook->AddPage(m_pageProgram, _("IDD_VIEW_OPTIONS_PROGRAM"), false);
	notebook->AddPage(m_pageFilter, _("IDD_VIEW_OPTIONS_FILTER"), false);
	notebook->AddPage(m_pageCal, _("IDD_VIEW_OPTIONS_CALENDAR"), false);
	notebook->AddPage(m_pagePrint, _("IDD_VIEW_OPTIONS_PRINTING"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxSizer* sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	notebook->SetSelection(iSelectPage);
}


void CDlgOptions::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	wxBusyCursor wait;
	m_pageProgram->Save();
	m_pageFilter->Save();
	m_pageCal->Save();
	m_pagePrint->Save();

	// Load into the default object
	CFilterOptions::Options().Load();

	// Update
	m_pDoc->ResetVisibility();

	if (m_pageProgram->ResetHtmlView())
		m_pDoc->ShowPointsAsHtml(CAgilityBookOptions::ShowHtmlPoints());

	CUpdateHint hint(UPDATE_OPTIONS);
	m_pDoc->UpdateAllViews(NULL, &hint);

	EndDialog(wxID_OK);
}
