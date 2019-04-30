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
 * 2018-12-16 Convert to fmt.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-12-24 Created
 */

#include "stdafx.h"
#include "DlgPluginDetails.h"

#include "AgilityBook.h"
#include "DlgCalendarQueryDetail.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigCalSite.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgPluginDetails, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgPluginDetails::OnOk)
END_EVENT_TABLE()


CDlgPluginDetails::CDlgPluginDetails(
		ARBConfig& inConfig,
		ARBConfigCalSitePtr const& inCalSite,
		wxWindow* pParent)
	: wxDialog()
	, m_Config(inConfig)
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
	Create(pParent, wxID_ANY, _("IDD_CALENDAR_PLUGIN_DETAIL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	if (m_OrigCalSite)
	{
		m_CalSite = m_OrigCalSite->Clone();
		m_strName = StringUtil::stringWX(m_OrigCalSite->GetName());
		m_strDesc = StringUtil::stringWX(m_OrigCalSite->GetDescription());
		m_strSearch = StringUtil::stringWX(m_OrigCalSite->GetSearchURL());
		m_strHelp = StringUtil::stringWX(m_OrigCalSite->GetHelpURL());
	}
	else
		m_CalSite = ARBConfigCalSite::New();

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strName));
	m_ctrlName->SetHelpText(_("HIDC_PLUGINDETAIL_NAME"));
	m_ctrlName->SetToolTip(_("HIDC_PLUGINDETAIL_NAME"));

	wxStaticText* textDesc = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_strDesc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_PLUGINDETAIL_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_PLUGINDETAIL_DESC"));

	wxStaticText* textSearchURL = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_SEARCH"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSearchURL->Wrap(-1);

	CTextCtrl* ctrlSearchURL = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strSearch, TRIMVALIDATOR_TRIM_BOTH));
	ctrlSearchURL->SetHelpText(_("HIDC_PLUGINDETAIL_SEARCH"));
	ctrlSearchURL->SetToolTip(_("HIDC_PLUGINDETAIL_SEARCH"));

	wxStaticText* textHelpURL = new wxStaticText(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_HELP"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHelpURL->Wrap(-1);

	CTextCtrl* ctrlHelpURL = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_strHelp, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHelpURL->SetHelpText(_("HIDC_PLUGINDETAIL_HELP"));
	ctrlHelpURL->SetToolTip(_("HIDC_PLUGINDETAIL_HELP"));

	wxButton* btnCodes = new wxButton(this, wxID_ANY,
		_("IDC_PLUGINDETAIL_CODES"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnCodes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPluginDetails::OnPluginDetailCodes, this);
	btnCodes->SetHelpText(_("HIDC_PLUGINDETAIL_CODES"));
	btnCodes->SetToolTip(_("HIDC_PLUGINDETAIL_CODES"));

	m_ctrlCodes = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
	m_ctrlCodes->Wrap(-1);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(m_ctrlName, 1, wxEXPAND, 0);
	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerDesc = new wxBoxSizer(wxVERTICAL);
	sizerDesc->Add(textDesc, 0, 0, 0);
	sizerDesc->Add(ctrlDesc, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));
	bSizer->Add(sizerDesc, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerSearch = new wxBoxSizer(wxVERTICAL);
	sizerSearch->Add(textSearchURL, 0, 0, 0);
	sizerSearch->Add(ctrlSearchURL, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));
	bSizer->Add(sizerSearch, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerHelp = new wxBoxSizer(wxVERTICAL);
	sizerHelp->Add(textHelpURL, 0, 0, 0);
	sizerHelp->Add(ctrlHelpURL, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));
	bSizer->Add(sizerHelp, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	bSizer->Add(btnCodes, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlCodes, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgPluginDetails, m_ctrlName)

	SetCodeText();
}


DEFINE_ON_INIT(CDlgPluginDetails)


void CDlgPluginDetails::SetCodeText()
{
	std::wstring str = fmt::format(_("IDC_PLUGINDETAIL_CODES_TEXT").wx_str(),
		m_CalSite->LocationCodes().size(),
		m_CalSite->VenueCodes().size());
	m_ctrlCodes->SetLabel(str);
}


void CDlgPluginDetails::OnPluginDetailCodes(wxCommandEvent& evt)
{
	CDlgCalendarQueryDetail dlg(m_Config, m_CalSite->LocationCodes(), m_CalSite->VenueCodes(), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_CalSite->RemoveAllLocationCodes();
		std::map<std::wstring, std::wstring>::const_iterator i;
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

	m_CalSite->SetName(StringUtil::stringW(m_strName));
	m_CalSite->SetDescription(StringUtil::stringW(m_strDesc));
	m_CalSite->SetSearchURL(StringUtil::stringW(m_strSearch));
	m_CalSite->SetHelpURL(StringUtil::stringW(m_strHelp));

	if ((!m_OrigCalSite || m_OrigCalSite->GetName() != m_CalSite->GetName())
	&& m_Config.GetCalSites().FindSite(m_CalSite->GetName()))
	{
		wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlName->SetFocus();
		return;
	}

	if (m_OrigCalSite)
		*m_OrigCalSite = *m_CalSite;
	else
		m_Config.GetCalSites().AddSite(m_CalSite);
	EndDialog(wxID_OK);
}
