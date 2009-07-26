/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-12-24 DRC Created
 */

#include "stdafx.h"
#include "DlgQueryDetail.h"

#include "ARBConfig.h"
#include "ComboBoxes.h"
#include "Validators.h"


BEGIN_EVENT_TABLE(CDlgQueryDetail, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgQueryDetail::OnOk)
END_EVENT_TABLE()


CDlgQueryDetail::CDlgQueryDetail(
		bool bReadOnly,
		const wxChar* inCode,
		const wxChar* inName,
		wxWindow* pParent,
		ARBConfig const* inConfig)
	: wxDialog()
	, m_ReadOnly(bReadOnly)
	, m_Config(inConfig)
	, m_strCode(inCode)
	, m_ctrlCode(NULL)
	, m_strName(inName)
	, m_ctrlVenues(NULL)
	, m_Code(inCode)
	, m_Name(inName)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	wxDialog::Create(pParent, wxID_ANY, _("IDD_QUERY_DETAIL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textCode = new wxStaticText(this, wxID_ANY,
		_("IDC_QUERYDETAIL_CODE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCode->Wrap(-1);

	m_ctrlCode = new wxTextCtrl(this, wxID_ANY, m_strCode,
		wxDefaultPosition, wxDefaultSize, 0);
	if (bReadOnly)
		m_ctrlCode->SetEditable(false);
	else
		m_ctrlCode->SetValidator(CTrimValidator(&m_strCode, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlCode->SetHelpText(_("HIDC_QUERYDETAIL_CODE"));
	m_ctrlCode->SetToolTip(_("HIDC_QUERYDETAIL_CODE"));

	wxStaticText* textLocation = new wxStaticText(this, wxID_ANY,
		m_Config ? _("IDS_COL_VENUE") : _("IDS_COL_LOCATION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLocation->Wrap(-1);

	wxWindow* ctrlLocationOrVenue = NULL;
	if (!bReadOnly && m_Config)
	{
		if (m_Name.empty())
			m_strName = m_strCode;
		m_ctrlVenues = new CVenueComboBox(this,
			m_Config->GetVenues(), wxEmptyString, true,
			CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH), true);
		m_ctrlVenues->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgQueryDetail::OnSelchangeVenues), NULL, this);
		m_ctrlVenues->SetHelpText(_("HIDC_QUERYDETAIL_COMBO"));
		m_ctrlVenues->SetToolTip(_("HIDC_QUERYDETAIL_COMBO"));
		ctrlLocationOrVenue = m_ctrlVenues;
	}
	else
	{
		wxTextCtrl* ctrlLocation = new wxTextCtrl(this, wxID_ANY, m_strName,
			wxDefaultPosition, wxSize(200, -1), 0);
		if (bReadOnly)
			ctrlLocation->SetEditable(false);
		else
			ctrlLocation->SetValidator(CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH));
		ctrlLocation->SetHelpText(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocation->SetToolTip(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocationOrVenue = ctrlLocation;
	}

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerCode = new wxBoxSizer(wxHORIZONTAL);
	sizerCode->Add(textCode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCode->Add(m_ctrlCode, 0, wxALL, 5);

	bSizer->Add(sizerCode, 1, wxEXPAND, 5);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(ctrlLocationOrVenue, 1, wxALL, 5);

	bSizer->Add(sizerVenue, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();
}


void CDlgQueryDetail::OnSelchangeVenues(wxCommandEvent& evt)
{
	int idx = m_ctrlVenues->GetSelection();
	if (0 <= idx)
	{
		ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(idx);
		if (pVenue)
		{
			if (m_strCode != pVenue->GetName().c_str())
			{
				TransferDataFromWindow();
				// Yes, this will kill the existing code. In general, most
				// sites will probably use the same acronyms I am. So I'm
				// just not going to worry about this.
				m_strCode = pVenue->GetName().c_str();
				TransferDataToWindow();
			}
		}
	}
}


void CDlgQueryDetail::OnOk(wxCommandEvent& evt)
{
	if (!m_ReadOnly)
	{
		if (!Validate() || !TransferDataFromWindow())
		return;

		if (m_Config && m_strCode.empty())
			m_strCode = m_strName;
		if (m_strCode.empty())
		{
			wxBell();
			m_ctrlCode->SetFocus();
			wxMessageBox(_("IDS_NEED_VALUE"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION);
			return;
		}

		m_Code = m_strCode.c_str();
		m_Name = m_strName.c_str();
		if (m_Config)
		{
			// If there's no name, use the code.
			if (m_Name.empty())
				m_Name = m_Code;
			// If no code, use the name.
			if (m_Code.empty())
				m_Code = m_Name;
			// Do not verify against our venues. Originally, we set the name
			// to the code if the code was one of our venues - but it's
			// possible that the code someone uses may map to one of our
			// codes, but actually be a different venue.
		}
	}

	EndDialog(wxID_OK);
}
