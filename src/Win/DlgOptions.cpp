/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOptions class
 * @author David Connet
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2009-02-09 Ported to wxWidgets.
 * 2005-08-18 Separated options and filters into two dialogs.
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2004-08-31 Added option to disable splash screen.
 * 2004-07-23 Auto-check the config version on document open.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
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

#include "LibARBWin/DlgPadding.h"
#include <wx/notebook.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{

wxBEGIN_EVENT_TABLE(CDlgOptions, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgOptions::OnOk)
wxEND_EVENT_TABLE()


CDlgOptions::CDlgOptions(CAgilityBookDoc* pDoc, wxWindow* pParent, size_t iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_trace("CDlgOptions")
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDS_VIEWING_OPTIONS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, &CDlgOptions::OnPageChanging, this);

	m_pageProgram = new CDlgOptionsProgram(notebook);
	m_pageFilter = new CDlgOptionsFilter(notebook, m_pDoc);
	m_pageCal = new CDlgOptionsCalendar(notebook);
	m_pagePrint = new CDlgOptionsPrint(notebook);

	notebook->AddPage(m_pageProgram, _("IDD_VIEW_OPTIONS_PROGRAM"), false);
	notebook->AddPage(m_pageFilter, _("IDD_VIEW_OPTIONS_FILTER"), false);
	notebook->AddPage(m_pageCal, _("IDD_VIEW_OPTIONS_CALENDAR"), false);
	notebook->AddPage(m_pagePrint, _("IDD_VIEW_OPTIONS_PRINTING"), false);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, padding.ButtonSizer());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	notebook->ChangeSelection(iSelectPage);
}


void CDlgOptions::OnPageChanging(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
	{
		if (!Validate() || !TransferDataFromWindow())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
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

	CUpdateHint hint(UPDATE_OPTIONS);
	m_pDoc->UpdateAllViews(nullptr, &hint);

	EndDialog(wxID_OK);
}

} // namespace dconSoft
