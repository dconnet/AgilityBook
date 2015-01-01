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
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-12-24 Created
 */

#include "stdafx.h"
#include "DlgQueryDetail.h"

#include "AgilityBook.h"
#include "ComboBoxVenue.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfig.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgQueryDetail, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgQueryDetail::OnOk)
END_EVENT_TABLE()


CDlgQueryDetail::CDlgQueryDetail(
		bool bReadOnly,
		const wchar_t* inCode,
		const wchar_t* inName,
		wxWindow* pParent,
		ARBConfig const* inConfig)
	: wxDialog()
	, m_ReadOnly(bReadOnly)
	, m_Config(inConfig)
	, m_strCode(inCode)
	, m_ctrlCode(nullptr)
	, m_strName(inName)
	, m_ctrlVenues(nullptr)
	, m_Code(inCode)
	, m_Name(inName)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(pParent, wxID_ANY, _("IDD_QUERY_DETAIL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textCode = new wxStaticText(this, wxID_ANY,
		_("IDC_QUERYDETAIL_CODE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCode->Wrap(-1);

	m_ctrlCode = new CTextCtrl(this, wxID_ANY, m_strCode,
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

	wxWindow* ctrlLocationOrVenue = nullptr;
	if (!bReadOnly && m_Config)
	{
		if (m_Name.empty())
			m_strName = m_strCode;
		m_ctrlVenues = new CVenueComboBox(this,
			m_Config->GetVenues(), wxEmptyString, true,
			CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH), true);
		BIND_OR_CONNECT_CTRL(m_ctrlVenues, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgQueryDetail::OnSelchangeVenues);
		m_ctrlVenues->SetHelpText(_("HIDC_QUERYDETAIL_COMBO"));
		m_ctrlVenues->SetToolTip(_("HIDC_QUERYDETAIL_COMBO"));
		ctrlLocationOrVenue = m_ctrlVenues;
	}
	else
	{
		CTextCtrl* ctrlLocation = new CTextCtrl(this, wxID_ANY, m_strName,
			wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 150), -1), 0);
		if (bReadOnly)
			ctrlLocation->SetEditable(false);
		else
			ctrlLocation->SetValidator(CTrimValidator(&m_strName, TRIMVALIDATOR_TRIM_BOTH));
		ctrlLocation->SetHelpText(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocation->SetToolTip(_("HIDC_QUERYDETAIL_NAME"));
		ctrlLocationOrVenue = ctrlLocation;
	}

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerCode = new wxBoxSizer(wxHORIZONTAL);
	sizerCode->Add(textCode, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerCode->Add(m_ctrlCode, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	bSizer->Add(sizerCode, 1, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, wxDLG_UNIT_X(this, 5));
	sizerVenue->Add(ctrlLocationOrVenue, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	bSizer->Add(sizerVenue, 1, wxEXPAND, 0);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgQueryDetail, m_ctrlCode)
}


DEFINE_ON_INIT(CDlgQueryDetail)


std::wstring CDlgQueryDetail::GetDetailCode() const
{
	return StringUtil::stringW(m_Code);
}


std::wstring CDlgQueryDetail::GetDetailName() const
{
	return StringUtil::stringW(m_Name);
}


void CDlgQueryDetail::OnSelchangeVenues(wxCommandEvent& evt)
{
	int idx = m_ctrlVenues->GetSelection();
	if (0 <= idx)
	{
		ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(idx);
		if (pVenue)
		{
			wxString wxName(StringUtil::stringWX(pVenue->GetName()));
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

		if (m_Config && m_strCode.empty())
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
