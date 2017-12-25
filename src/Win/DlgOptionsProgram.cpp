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
#include "Widgets.h"

#include "ARBCommon/StringUtil.h"
#include <wx/url.h>
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgOptionsProgram::CDlgOptionsProgram(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_bResetHtmlView(false)
	, m_bAutoCheck(CAgilityBookOptions::GetAutoUpdateCheck())
	, m_Backups(CAgilityBookOptions::GetNumBackupFiles())
	, m_bAutoShow(CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
	, m_bShowHtml(CAgilityBookOptions::ShowHtmlPoints())
	, m_UseProxy(CAgilityBookOptions::GetUseProxy())
	, m_Proxy(StringUtil::stringWX(CAgilityBookOptions::GetProxyServer()))
	, m_ctrlProxy(nullptr)
{
	// Controls (these are done first to control tab order)

	wxCheckBox* ctrlUpdates = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTO_CHECK"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bAutoCheck));
	ctrlUpdates->SetHelpText(_("HIDC_OPT_PGM_AUTO_CHECK"));
	ctrlUpdates->SetToolTip(_("HIDC_OPT_PGM_AUTO_CHECK"));

	wxStaticText* textUpdates = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTO_CHECK_TEXT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textUpdates->Wrap(wxDLG_UNIT_X(this, 230));

	wxStaticText* textBackups = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBackups->Wrap(-1);

	CTextCtrl* ctrlBackups = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		wxGenericValidator(&m_Backups));
	ctrlBackups->SetHelpText(_("HIDC_OPT_PGM_EDIT"));
	ctrlBackups->SetToolTip(_("HIDC_OPT_PGM_EDIT"));

	wxStaticText* textBackupHelp = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PGM_EDIT_TEXT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBackupHelp->Wrap(wxDLG_UNIT_X(this, 155));

	wxCheckBox* ctrlShowDog = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_AUTOSHOW"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bAutoShow));
	ctrlShowDog->SetHelpText(_("HIDC_OPT_PGM_AUTOSHOW"));
	ctrlShowDog->SetToolTip(_("HIDC_OPT_PGM_AUTOSHOW"));

	wxCheckBox* ctrlHtml = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_SHOWHTML"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bShowHtml));
	ctrlHtml->SetHelpText(_("HIDC_OPT_PGM_SHOWHTML"));
	ctrlHtml->SetToolTip(_("HIDC_OPT_PGM_SHOWHTML"));

	wxCheckBox* ctrlUseProxy = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_PGM_USEPROXY"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_UseProxy));
	BIND_OR_CONNECT_CTRL(ctrlUseProxy, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgOptionsProgram::OnUseProxy);
	ctrlUseProxy->SetHelpText(_("HIDC_OPT_PGM_USEPROXY"));
	ctrlUseProxy->SetToolTip(_("HIDC_OPT_PGM_USEPROXY"));

	m_ctrlProxy = new CTextCtrl(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_Proxy));
	m_ctrlProxy->SetHelpText(_("HIDC_OPT_PGM_PROXY"));
	m_ctrlProxy->SetToolTip(_("HIDC_OPT_PGM_PROXY"));
	m_ctrlProxy->Enable(m_UseProxy);

	// Sizers

	wxBoxSizer* sizerPgm = new wxBoxSizer(wxVERTICAL);
	sizerPgm->Add(ctrlUpdates, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerPgm->Add(textUpdates, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBackups = new wxBoxSizer(wxHORIZONTAL);
	sizerBackups->Add(textBackups, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerBackups->Add(ctrlBackups, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerBackups->Add(textBackupHelp, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerPgm->Add(sizerBackups, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerPgm->Add(ctrlShowDog, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerPgm->Add(ctrlHtml, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerPgm->Add(ctrlUseProxy, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerProxy = new wxBoxSizer(wxHORIZONTAL);
	sizerProxy->Add(wxDLG_UNIT_X(this, 10), 0, 0, 0, 0);
	sizerProxy->Add(m_ctrlProxy, 1, wxEXPAND, 0);
	sizerPgm->Add(sizerProxy, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(sizerPgm);
	Layout();
	sizerPgm->Fit(this);
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
	if (CAgilityBookOptions::ShowHtmlPoints() != m_bShowHtml)
	{
		m_bResetHtmlView = true;
		CAgilityBookOptions::SetShowHtmlPoints(m_bShowHtml);
	}
	std::wstring oldProxy(CAgilityBookOptions::GetProxy());
	if (m_Proxy.empty())
		m_UseProxy = false;
	CAgilityBookOptions::SetUseProxy(m_UseProxy);
	CAgilityBookOptions::SetProxyServer(StringUtil::stringW(m_Proxy));
	std::wstring newProxy = CAgilityBookOptions::GetProxy();
	if (newProxy != oldProxy)
		wxURL::SetDefaultProxy(StringUtil::stringWX(newProxy));
}
