/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Configure plugins for direct download of calendar data.
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
#include "DlgPluginDetails.h"

#include "ARBConfigCalSite.h"
#include "CalSites.h"
#include "DlgCalendarQueryDetail.h"

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
using namespace ARBCommon;
using namespace ARBWin;


CDlgPluginDetails::CDlgPluginDetails(
	ARBConfigCalSiteList& sites,
	ARBConfigCalSitePtr const& inCalSite,
	wxWindow* pParent)
	: wxDialog()
	, m_sites(sites)
	, m_OrigCalSite(inCalSite)
	, m_CalSite()
	, m_strName()
	, m_strDesc()
	, m_strSearch()
	, m_ctrlCodes(nullptr)
	, m_strHelp()
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_CALENDAR_PLUGIN_DETAIL"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_OrigCalSite)
	{
		m_CalSite = m_OrigCalSite->Clone();
		m_strName = m_OrigCalSite->GetName();
		m_strDesc = m_OrigCalSite->GetDescription();
		m_strSearch = m_OrigCalSite->GetSearchURL();
		m_strHelp = m_OrigCalSite->GetHelpURL();
	}
	else
		m_CalSite = ARBConfigCalSite::New();

	// Controls (these are done first to control tab order)

	wxStaticText* textName
		= new wxStaticText(this, wxID_ANY, _("IDC_PLUGINDETAIL_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, CTrimValidator(&m_strName));
	m_ctrlName->SetHelpText(_("HIDC_PLUGINDETAIL_NAME"));
	m_ctrlName->SetToolTip(_("HIDC_PLUGINDETAIL_NAME"));

	wxStaticText* textDesc
		= new wxStaticText(this, wxID_ANY, _("IDC_PLUGINDETAIL_DESC"), wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_strDesc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_PLUGINDETAIL_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_PLUGINDETAIL_DESC"));

	wxStaticText* textSearchURL
		= new wxStaticText(this, wxID_ANY, _("IDC_PLUGINDETAIL_SEARCH"), wxDefaultPosition, wxDefaultSize, 0);
	textSearchURL->Wrap(-1);

	CTextCtrl* ctrlSearchURL = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_strSearch, TRIMVALIDATOR_TRIM_BOTH));
	ctrlSearchURL->SetHelpText(_("HIDC_PLUGINDETAIL_SEARCH"));
	ctrlSearchURL->SetToolTip(_("HIDC_PLUGINDETAIL_SEARCH"));

	wxStaticText* textHelpURL
		= new wxStaticText(this, wxID_ANY, _("IDC_PLUGINDETAIL_HELP"), wxDefaultPosition, wxDefaultSize, 0);
	textHelpURL->Wrap(-1);

	CTextCtrl* ctrlHelpURL = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_strHelp, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHelpURL->SetHelpText(_("HIDC_PLUGINDETAIL_HELP"));
	ctrlHelpURL->SetToolTip(_("HIDC_PLUGINDETAIL_HELP"));

	wxButton* btnCodes = new wxButton(this, wxID_ANY, _("IDC_PLUGINDETAIL_CODES"), wxDefaultPosition, wxDefaultSize, 0);
	btnCodes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPluginDetails::OnPluginDetailCodes, this);
	btnCodes->SetHelpText(_("HIDC_PLUGINDETAIL_CODES"));
	btnCodes->SetToolTip(_("HIDC_PLUGINDETAIL_CODES"));

	m_ctrlCodes = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
	m_ctrlCodes->Wrap(-1);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER | wxRIGHT, padding.Controls());
	sizerName->Add(m_ctrlName, 1, wxEXPAND, 0);
	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerDesc = new wxBoxSizer(wxVERTICAL);
	sizerDesc->Add(textDesc, 0, 0, 0);
	sizerDesc->Add(ctrlDesc, 0, wxEXPAND | wxTOP, padding.Tight());
	bSizer->Add(sizerDesc, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerSearch = new wxBoxSizer(wxVERTICAL);
	sizerSearch->Add(textSearchURL, 0, 0, 0);
	sizerSearch->Add(ctrlSearchURL, 0, wxEXPAND | wxTOP, padding.Tight());
	bSizer->Add(sizerSearch, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerHelp = new wxBoxSizer(wxVERTICAL);
	sizerHelp->Add(textHelpURL, 0, 0, 0);
	sizerHelp->Add(ctrlHelpURL, 0, wxEXPAND | wxTOP, padding.Tight());
	bSizer->Add(sizerHelp, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	bSizer->Add(btnCodes, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_ctrlCodes, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPluginDetails::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(m_ctrlName)

	SetCodeText();
}


void CDlgPluginDetails::SetCodeText()
{
	auto str = wxString::Format(
		_("IDC_PLUGINDETAIL_CODES_TEXT"),
		m_CalSite->LocationCodes().size(),
		m_CalSite->VenueCodes().size());
	m_ctrlCodes->SetLabel(str);
}


void CDlgPluginDetails::OnPluginDetailCodes(wxCommandEvent& evt)
{
	CDlgCalendarQueryDetail dlg(m_sites, m_CalSite->LocationCodes(), m_CalSite->VenueCodes(), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_CalSite->RemoveAllLocationCodes();
		std::map<wxString, wxString>::const_iterator i;
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

	m_CalSite->SetName(m_strName);
	m_CalSite->SetDescription(m_strDesc);
	m_CalSite->SetSearchURL(m_strSearch);
	m_CalSite->SetHelpURL(m_strHelp);

	if ((!m_OrigCalSite || m_OrigCalSite->GetName() != m_CalSite->GetName()) && m_sites.FindSite(m_CalSite->GetName()))
	{
		wxMessageBox(_("IDS_NAME_IN_USE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlName->SetFocus();
		return;
	}

	if (m_OrigCalSite)
		*m_OrigCalSite = *m_CalSite;
	else
		m_sites.AddSite(m_CalSite);
	EndDialog(wxID_OK);
}

} // namespace dconSoft
