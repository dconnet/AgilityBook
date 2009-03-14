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
 * @brief implementation of the CDlgSelectURL class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-03-30 DRC Created.
 */

#include "stdafx.h"
#include "DlgSelectURL.h"

#include "Validators.h"


CDlgSelectURL::CDlgSelectURL(
		wxString const& name,
		wxWindow* pParent)
	: wxDialog()
	, m_Name(name)
	, m_textCtrl(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_SELECT_URL "), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_textCtrl = new wxTextCtrl(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxSize(300, -1), 0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	m_textCtrl->SetHelpText(_("HIDC_SELECTURL_NAME"));
	m_textCtrl->SetToolTip(_("HIDC_SELECTURL_NAME"));

	wxButton* btnSelect = new wxButton(this, wxID_ANY,
		_("IDC_SELECTURL_FILENAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnSelect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgSelectURL::OnFilename), NULL, this);
	btnSelect->SetHelpText(_("HIDC_SELECTURL_FILENAME"));
	btnSelect->SetToolTip(_("HIDC_SELECTURL_FILENAME"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bTextSizer = new wxBoxSizer(wxHORIZONTAL);
	bTextSizer->Add(m_textCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
	bTextSizer->Add(btnSelect, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	bSizer->Add(bTextSizer, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 1, wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_textCtrl->SetFocus();
}


void CDlgSelectURL::OnFilename(wxCommandEvent& evt)
{
	wxFileDialog file(this,
			wxT(""), // caption
			wxT(""), // default dir
			m_textCtrl->GetValue(),
			_("IDS_FILEEXT_FILTER_ALL"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (wxID_OK == file.ShowModal())
	{
		m_textCtrl->SetValue(file.GetPath());
	}
}
