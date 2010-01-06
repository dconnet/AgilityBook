/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgName class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgName.h"

#include "AgilityBook.h"
#include "Validators.h"
#include "Widgets.h"


bool CDlgName::Create(
		wxString const& caption,
		wxWindow* pParent)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	if (!wxDialog::Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	// Controls (these are done first to control tab order)

	CTextCtrl* textCtrl = new CTextCtrl(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxSize(300, -1), 0,
		CTrimValidator(&m_Name, _("IDS_ENTER_NAME")));
	textCtrl->SetHelpText(_("HIDC_NAME"));
	textCtrl->SetToolTip(_("HIDC_NAME"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 0, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	textCtrl->SetFocus();

	return true;
}
