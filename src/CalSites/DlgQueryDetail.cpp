/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-12-24 Created
 */

#include "stdafx.h"
#include "DlgQueryDetail.h"

#include "CalSites.h"

#include "../Win/ComboBoxVenue.h"
#include "ARB/ARBConfig.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;


CDlgQueryDetail::CDlgQueryDetail(
	bool bReadOnly,
	const wchar_t* inCode,
	const wchar_t* inName,
	wxWindow* pParent,
	ARBConfigCalSiteList const* sites)
	: wxDialog()
	, m_ReadOnly(bReadOnly)
	, m_sites(sites)
	, m_strCode(inCode)
	, m_ctrlCode(nullptr)
	, m_strName(inName)
	, m_ctrlVenues(nullptr)
	, m_Code(inCode)
	, m_Name(inName)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(
		pParent,
		wxID_ANY,
		_("IDD_QUERY_DETAIL"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textCode
		= new wxStaticText(this, wxID_ANY, _("IDC_QUERYDETAIL_CODE"), wxDefaultPosition, wxDefaultSize, 0);
	textCode->Wrap(-1);

	m_ctrlCode = new CTextCtrl(this, wxID_ANY, m_strCode, wxDefaultPosition, wxDefaultSize, 0);
	if (bReadOnly)
		m_ctrlCode->SetEditable(false);
	else
		m_ctrlCode->SetValidator(CTrimValidator(&m_strCode, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlCode->SetHelpText(_("HIDC_QUERYDETAIL_CODE"));
	m_ctrlCode->SetToolTip(_("HIDC_QUERYDETAIL_CODE"));

	wxStaticText* textLocation = new wxStaticText(
		this,
		wxID_ANY,
		m_sites ? _("IDS_COL_VENUE") : _("IDS_COL_LOCATION"),
		wxDefaultPosition,
		wxDefaultSize,
		0);
	textLocation->Wrap(-1);

	wxWindow* ctrlLocationOrVenue = nullptr;
	if (!bReadOnly && m_sites)
	{
		if (m_Name.empty())
			m_strName = m_strCode;

#pragma PRAGMA_TODO(Supported venues)
		ARBConfigVenueList venues;
		venues.AddVenue(L"AKC");
		venues.AddVenue(L"USDAA");

		m_ctrlVenues = new CVenueComboBox(
			this,
			venues,
			wxEmptyString,
			true,
			CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH),
			true);
		m_ctrlVenues->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgQueryDetail::OnSelchangeVenues, this);
		m_ctrlVenues->SetHelpText(_("HIDC_QUERYDETAIL_COMBO"));
		m_ctrlVenues->SetToolTip(_("HIDC_QUERYDETAIL_COMBO"));
		ctrlLocationOrVenue = m_ctrlVenues;
	}
	else
	{
		CTextCtrl* ctrlLocation
			= new CTextCtrl(this, wxID_ANY, m_strName, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 150), -1), 0);
		if (bReadOnly)
			ctrlLocation->SetEditable(false);
		else
			ctrlLocation->SetValidator(CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH));
		ctrlLocation->SetHelpText(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocation->SetToolTip(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocationOrVenue = ctrlLocation;
	}

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerCode = new wxBoxSizer(wxHORIZONTAL);
	sizerCode->Add(textCode, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerCode->Add(m_ctrlCode, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerCode, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerVenue->Add(ctrlLocationOrVenue, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerVenue, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgQueryDetail::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgQueryDetail, m_ctrlCode)
}


wxString CDlgQueryDetail::GetDetailCode() const
{
	return m_Code;
}


wxString CDlgQueryDetail::GetDetailName() const
{
	return m_Name;
}


void CDlgQueryDetail::OnSelchangeVenues(wxCommandEvent& evt)
{
	int idx = m_ctrlVenues->GetSelection();
	if (0 <= idx)
	{
		ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(idx);
		if (pVenue)
		{
			wxString wxName(pVenue->GetName());
			if (m_strCode != wxName)
			{
				TransferDataFromWindow();
				// Yes, this will kill the existing code. In general, most
				// sites will probably use the same acronyms I am. So I'm
				// just not going to worry about this.
				m_strCode = wxName;
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

		if (m_sites && m_strCode.empty())
			m_strCode = m_strName;
		if (m_strCode.empty())
		{
			wxBell();
			m_ctrlCode->SetFocus();
			wxMessageBox(_("Please enter a value"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION);
			return;
		}

		m_Code = m_strCode;
		m_Name = m_strName;
		if (m_sites)
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

} // namespace dconSoft
