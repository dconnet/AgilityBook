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
 * @brief implementation of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-10-06 DRC Removed ARB classes so it could be used to lifetime pts.
 */

#include "stdafx.h"
#include "DlgConfigTitlePoints.h"

#include "ARBConfigVenue.h"
#include "Validators.h"


CDlgConfigTitlePoints::CDlgConfigTitlePoints(
		ARBConfigVenuePtr inVenue,
		double inValue, // Faults or Place
		double inPoints,
		ETitlePointType inType,
		wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_Type(inType)
	, m_textValue(NULL)
	, m_ctrlValue(NULL)
	, m_ctrlType(NULL)
	, m_Faults(inValue)
	, m_Place(inValue)
	, m_Points(inPoints)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_CONFIG_TITLE_POINTS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxString caption = (eTitlePlacement == m_Type) ? _("IDC_CONFIG_TITLEPTS_VALUE_TEXT2") : _("IDC_CONFIG_TITLEPTS_VALUE_TEXT1");
	m_textValue = new wxStaticText(this, wxID_ANY,
		caption,
		wxDefaultPosition, wxDefaultSize, 0);
	m_textValue->Wrap(-1);

	m_ctrlValue = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0);
	if (eTitlePlacement == m_Type)
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
	else
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
	m_ctrlValue->SetHelpText(_("HIDC_CONFIG_TITLEPTS_VALUE"));
	m_ctrlValue->SetToolTip(_("HIDC_CONFIG_TITLEPTS_VALUE"));

	wxStaticText* textPoints = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLEPTS_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	wxTextCtrl* ctrlPoints = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Points));
	ctrlPoints->SetHelpText(_("HIDC_CONFIG_TITLEPTS_POINTS"));
	ctrlPoints->SetToolTip(_("HIDC_CONFIG_TITLEPTS_POINTS"));

	wxArrayString types;
	for (int i = eTitleNormal; i < eTitleMax; ++i)
	{
		switch (i)
		{
		default:
			assert(0);
			break;
		case eTitleNormal:
			types.Add(_("IDS_TITLEPOINT_NORMAL"));
			break;
		case eTitleLifetime:
			if (m_Venue->HasLifetimeName())
				types.Add(m_Venue->GetLifetimeName().c_str());
			else
				types.Add(_("IDS_TITLEPOINT_LIFETIME"));
			break;
		case eTitlePlacement:
			types.Add(_("IDS_TITLEPOINT_PLACEMENT"));
			break;
		}
	}
	m_ctrlType = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, types, 0);
	m_ctrlType->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CDlgConfigTitlePoints::OnSelchangeTitlePoints), NULL, this);
	m_ctrlType->SetHelpText(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetToolTip(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetSelection(m_Type);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerValue = new wxBoxSizer(wxHORIZONTAL);
	sizerValue->Add(m_textValue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerValue->Add(m_ctrlValue, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerValue, 0, wxEXPAND, 5);

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPoints->Add(ctrlPoints, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerPoints, 0, wxEXPAND, 5);
	bSizer->Add(m_ctrlType, 0, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, sz);
	CenterOnParent();

	m_ctrlValue->SetFocus();
}


void CDlgConfigTitlePoints::OnSelchangeTitlePoints(wxCommandEvent& evt)
{
	int oldType = m_Type;
	m_Type = m_ctrlType->GetSelection();
	TransferDataFromWindow();
	if (eTitlePlacement == m_Type)
	{
		m_Place = m_Faults; // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT2"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
		TransferDataToWindow();
		GetSizer()->Fit(this);
	}
	else if (oldType == eTitlePlacement)
	{
		m_Faults = m_Place; // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT1"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
		TransferDataToWindow();
		GetSizer()->Fit(this);
	}
}
