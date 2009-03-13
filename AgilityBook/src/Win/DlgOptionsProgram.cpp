/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2004-04-08 DRC Created
 */

#include "stdafx.h"
#include "DlgOptionsProgram.h"

#include "AgilityBookOptions.h"
#include <wx/valgen.h>


CDlgOptionsProgram::CDlgOptionsProgram(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_bResetHtmlView(false)
	, m_bAutoCheck(CAgilityBookOptions::GetAutoUpdateCheck())
	, m_Backups(CAgilityBookOptions::GetNumBackupFiles())
	, m_bAutoShow(CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
	, m_bShowHtml(CAgilityBookOptions::ShowHtmlPoints())
{
	// Controls (these are done first to control tab order)

	wxCheckBox* ctrlUpdates = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTO_CHECK"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bAutoCheck));
	ctrlUpdates->SetHelpText(_("HIDC_OPT_PGM_AUTO_CHECK"));
	ctrlUpdates->SetToolTip(_("HIDC_OPT_PGM_AUTO_CHECK"));

	wxStaticText* textUpdates = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTO_CHECK_TEXT"),
		wxDefaultPosition, wxSize(400, -1), 0);
	textUpdates->Wrap(400);

	wxStaticText* textBackups = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBackups->Wrap(-1);

	wxTextCtrl* ctrlBackups = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		wxGenericValidator(&m_Backups));
	ctrlBackups->SetHelpText(_("HIDC_OPT_PGM_EDIT"));
	ctrlBackups->SetToolTip(_("HIDC_OPT_PGM_EDIT"));

	wxStaticText* textBackupHelp = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_EDIT_TEXT"),
		wxDefaultPosition, wxSize(250, -1), 0);
	textBackupHelp->Wrap(250);

	wxCheckBox* ctrlShowDog = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTOSHOW"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bAutoShow));
	ctrlShowDog->SetHelpText(_("HIDC_OPT_PGM_AUTOSHOW"));
	ctrlShowDog->SetToolTip(_("HIDC_OPT_PGM_AUTOSHOW"));

	wxCheckBox* ctrlHtml = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_SHOWHTML"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bShowHtml));
	ctrlHtml->SetHelpText(_("HIDC_OPT_PGM_SHOWHTML"));
	ctrlHtml->SetToolTip(_("HIDC_OPT_PGM_SHOWHTML"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerPgm = new wxBoxSizer(wxVERTICAL);
	sizerPgm->Add(ctrlUpdates, 0, wxALL, 5);
	sizerPgm->Add(textUpdates, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);

	wxBoxSizer* sizerBackups = new wxBoxSizer(wxHORIZONTAL);
	sizerBackups->Add(textBackups, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerBackups->Add(ctrlBackups, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerBackups->Add(textBackupHelp, 0, wxALL, 5);

	sizerPgm->Add(sizerBackups, 0, wxEXPAND, 5);
	sizerPgm->Add(ctrlShowDog, 0, wxALL, 5);
	sizerPgm->Add(ctrlHtml, 0, wxALL, 5);

	SetSizer(sizerPgm);
	Layout();
	sizerPgm->Fit(this);
}


void CDlgOptionsProgram::Save()
{
	CAgilityBookOptions::SetAutoUpdateCheck(m_bAutoCheck);
	CAgilityBookOptions::SetNumBackupFiles(m_Backups);
	CAgilityBookOptions::AutoShowPropertiesOnNewTitle(m_bAutoShow);
	if (CAgilityBookOptions::ShowHtmlPoints() != m_bShowHtml)
	{
		m_bResetHtmlView = true;
		CAgilityBookOptions::SetShowHtmlPoints(m_bShowHtml);
	}
}
