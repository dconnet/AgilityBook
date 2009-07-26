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
 * @brief Configure plugins for direct download of calendar data.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-12-24 DRC Created
 */

#include "stdafx.h"
#include "DlgPluginDetails.h"

#include "ARBConfig.h"
#include "ARBConfigCalSite.h"
#include "DlgCalendarQueryDetail.h"
#include "Validators.h"


BEGIN_EVENT_TABLE(CDlgPluginDetails, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgPluginDetails::OnOk)
END_EVENT_TABLE()


CDlgPluginDetails::CDlgPluginDetails(
		ARBConfig& inConfig,
		ARBConfigCalSitePtr calSite,
		wxWindow* pParent)
	: wxDialog()
	, m_Config(inConfig)
	, m_OrigCalSite(calSite)
	, m_CalSite()
	, m_strName()
	, m_strDesc()
	, m_strSearch()
	, m_ctrlCodes(NULL)
	, m_strHelp()
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_CALENDAR_PLUGIN_DETAIL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	if (calSite)
	{
		m_CalSite = calSite->Clone();
		m_strName = calSite->GetName().c_str();
		m_strDesc = calSite->GetDescription().c_str();
		m_strSearch = calSite->GetSearchURL().c_str();
		m_strHelp = calSite->GetHelpURL().c_str();
	}
	else
		m_CalSite = ARBConfigCalSite::New();

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strName));
	m_ctrlName->SetHelpText(_("HIDC_PLUGINDETAIL_NAME"));
	m_ctrlName->SetToolTip(_("HIDC_PLUGINDETAIL_NAME"));

	wxStaticText* textDesc = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	wxTextCtrl* ctrlDesc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_strDesc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_PLUGINDETAIL_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_PLUGINDETAIL_DESC"));

	wxStaticText* textSearchURL = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_SEARCH"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSearchURL->Wrap(-1);

	wxTextCtrl* ctrlSearchURL = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strSearch, TRIMVALIDATOR_TRIM_BOTH));
	ctrlSearchURL->SetHelpText(_("HIDC_PLUGINDETAIL_SEARCH"));
	ctrlSearchURL->SetToolTip(_("HIDC_PLUGINDETAIL_SEARCH"));

	wxStaticText* textHelpURL = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_HELP"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHelpURL->Wrap(-1);

	wxTextCtrl* ctrlHelpURL = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strHelp, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHelpURL->SetHelpText(_("HIDC_PLUGINDETAIL_HELP"));
	ctrlHelpURL->SetToolTip(_("HIDC_PLUGINDETAIL_HELP"));

	wxButton* btnCodes = new wxButton(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_CODES"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnCodes->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgPluginDetails::OnPluginDetailCodes), NULL, this);
	btnCodes->SetHelpText(_("HIDC_PLUGINDETAIL_CODES"));
	btnCodes->SetToolTip(_("HIDC_PLUGINDETAIL_CODES"));

	m_ctrlCodes = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
	m_ctrlCodes->Wrap(-1);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER|wxALL, 5);
	sizerName->Add(m_ctrlName, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 5);
	bSizer->Add(textDesc, 0, wxALL, 5);
	bSizer->Add(ctrlDesc, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(textSearchURL, 0, wxALL, 5);
	bSizer->Add(ctrlSearchURL, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(textHelpURL, 0, wxALL, 5);
	bSizer->Add(ctrlHelpURL, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(btnCodes, 0, wxALL, 5);
	bSizer->Add(m_ctrlCodes, 0, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	SetCodeText();
}


void CDlgPluginDetails::SetCodeText()
{
	wxString str = wxString::Format(_("IDC_PLUGINDETAIL_CODES_TEXT"),
		static_cast<int>(m_CalSite->LocationCodes().size()),
		static_cast<int>(m_CalSite->VenueCodes().size()));
	m_ctrlCodes->SetLabel(str);
}


void CDlgPluginDetails::OnPluginDetailCodes(wxCommandEvent& evt)
{
	CDlgCalendarQueryDetail dlg(m_Config, m_CalSite->LocationCodes(), m_CalSite->VenueCodes(), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_CalSite->RemoveAllLocationCodes();
		std::map<tstring, tstring>::const_iterator i;
		for (i = dlg.GetLocationCodes().begin(); i != dlg.GetLocationCodes().end(); ++i)
		{
			m_CalSite->AddLocationCode(i->first, i->second);
		}
		m_CalSite->RemoveAllVenueCodes();
		for (i = dlg.GetVenueCodes().begin(); i != dlg.GetVenueCodes().end(); ++i)
		{
			m_CalSite->AddVenueCode(i->first, i->second);
		}
		SetCodeText();
	}
}


void CDlgPluginDetails::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	m_CalSite->SetName(m_strName.c_str());
	m_CalSite->SetDescription(m_strDesc.c_str());
	m_CalSite->SetSearchURL(m_strSearch.c_str());
	m_CalSite->SetHelpURL(m_strHelp.c_str());

	if ((!m_OrigCalSite || m_OrigCalSite->GetName() != m_CalSite->GetName())
	&& m_Config.GetCalSites().FindSite(m_CalSite->GetName()))
	{
		wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
		m_ctrlName->SetFocus();
		return;
	}

	if (m_OrigCalSite)
		*m_OrigCalSite = *m_CalSite;
	else
		m_Config.GetCalSites().AddSite(m_CalSite);
	EndDialog(wxID_OK);
}
