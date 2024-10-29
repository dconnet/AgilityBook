/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-08-31 Added option to disable splash screen.
 * 2004-07-23 Auto-check the config version on document open.
 * 2004-04-08 Created
 */

#include "stdafx.h"
#include "DlgOptionsProgram.h"

#include "AgilityBookOptions.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Widgets.h"
#include <wx/url.h>
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgOptionsProgram::CDlgOptionsProgram(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_bAutoCheck(CAgilityBookOptions::GetAutoUpdateCheck())
	, m_Backups(CAgilityBookOptions::GetNumBackupFiles())
	, m_bAutoShow(CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
	, m_bShowCosanction(CAgilityBookOptions::ShowCoSanctioning())
	, m_UseProxy(CAgilityBookOptions::GetUseProxy())
	, m_Proxy(StringUtil::stringWX(CAgilityBookOptions::GetProxyServer()))
	, m_ctrlProxy(nullptr)
	, m_enableDarkMode(CAgilityBookOptions::EnableDarkMode())
{
	// Controls (these are done first to control tab order)

	wxCheckBox* ctrlUpdates = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_PGM_AUTO_CHECK"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bAutoCheck));
	ctrlUpdates->SetHelpText(_("HIDC_OPT_PGM_AUTO_CHECK"));
	ctrlUpdates->SetToolTip(_("HIDC_OPT_PGM_AUTO_CHECK"));

	wxStaticText* textUpdates
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_PGM_AUTO_CHECK_TEXT"), wxDefaultPosition, wxDefaultSize, 0);
	textUpdates->Wrap(wxDLG_UNIT_X(this, 230));

	wxStaticText* textBackups
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_PGM_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	textBackups->Wrap(-1);

	CTextCtrl* ctrlBackups = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		wxGenericValidator(&m_Backups));
	ctrlBackups->SetHelpText(_("HIDC_OPT_PGM_EDIT"));
	ctrlBackups->SetToolTip(_("HIDC_OPT_PGM_EDIT"));

	wxStaticText* textBackupHelp
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_PGM_EDIT_TEXT"), wxDefaultPosition, wxDefaultSize, 0);
	textBackupHelp->Wrap(wxDLG_UNIT_X(this, 155));

	wxCheckBox* ctrlShowDog = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_PGM_AUTOSHOW"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bAutoShow));
	ctrlShowDog->SetHelpText(_("HIDC_OPT_PGM_AUTOSHOW"));
	ctrlShowDog->SetToolTip(_("HIDC_OPT_PGM_AUTOSHOW"));

	wxCheckBox* ctrlShowCoSanction = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_PGM_COSANCTION"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bShowCosanction));
	ctrlShowCoSanction->SetHelpText(_("HIDC_OPT_PGM_COSANCTION"));
	ctrlShowCoSanction->SetToolTip(_("HIDC_OPT_PGM_COSANCTION"));

	wxCheckBox* ctrlUseProxy = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_PGM_USEPROXY"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_UseProxy));
	ctrlUseProxy->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgOptionsProgram::OnUseProxy, this);
	ctrlUseProxy->SetHelpText(_("HIDC_OPT_PGM_USEPROXY"));
	ctrlUseProxy->SetToolTip(_("HIDC_OPT_PGM_USEPROXY"));

	m_ctrlProxy = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_Proxy));
	m_ctrlProxy->SetHelpText(_("HIDC_OPT_PGM_PROXY"));
	m_ctrlProxy->SetToolTip(_("HIDC_OPT_PGM_PROXY"));
	m_ctrlProxy->Enable(m_UseProxy);

#if wxCHECK_VERSION(3, 3, 0)
	wxCheckBox* ctrlDarkMode = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_PGM_DARKMODE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_enableDarkMode));
	ctrlDarkMode->SetHelpText(_("HIDC_OPT_PGM_ENABLEDARKMODE"));
	ctrlDarkMode->SetToolTip(_("HIDC_OPT_PGM_ENABLEDARKMODE"));
#endif

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(ctrlUpdates, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(textUpdates, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerBackups = new wxBoxSizer(wxHORIZONTAL);
	sizerBackups->Add(textBackups, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerBackups->Add(ctrlBackups, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerBackups->Add(textBackupHelp, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerBackups, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlShowDog, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlShowCoSanction, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlUseProxy, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerProxy = new wxBoxSizer(wxHORIZONTAL);
	sizerProxy->AddSpacer(padding.CheckboxOffset());
	sizerProxy->Add(m_ctrlProxy, 1, wxEXPAND);
	bSizer->Add(sizerProxy, 0, wxEXPAND | wxALL, padding.Controls());

#if wxCHECK_VERSION(3, 3, 0)
	bSizer->Add(ctrlDarkMode, 0, wxLEFT | wxRIGHT | wxBOTTOM, padding.Controls());
#endif

	SetSizer(bSizer);
	Layout();
	bSizer->Fit(this);
}


void CDlgOptionsProgram::OnUseProxy(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlProxy->Enable(m_UseProxy);
}


void CDlgOptionsProgram::Save()
{
	CAgilityBookOptions::SetAutoUpdateCheck(m_bAutoCheck);
	CAgilityBookOptions::SetNumBackupFiles(m_Backups);
	CAgilityBookOptions::AutoShowPropertiesOnNewTitle(m_bAutoShow);
	CAgilityBookOptions::SetShowCoSanctioning(m_bShowCosanction);
	std::wstring oldProxy(CAgilityBookOptions::GetProxy());
	if (m_Proxy.empty())
		m_UseProxy = false;
	CAgilityBookOptions::SetUseProxy(m_UseProxy);
	CAgilityBookOptions::SetProxyServer(StringUtil::stringW(m_Proxy));
	std::wstring newProxy = CAgilityBookOptions::GetProxy();
	if (newProxy != oldProxy)
		wxURL::SetDefaultProxy(StringUtil::stringWX(newProxy));
#if wxCHECK_VERSION(3, 3, 0)
	CAgilityBookOptions::SetEnableDarkMode(m_enableDarkMode);
#endif
}

} // namespace dconSoft
