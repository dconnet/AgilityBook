/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Configuration updater
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-16 DRC Fix initial focus.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-26 DRC Display errors on non-fatal load.
 * @li 2003-12-07 DRC Changed Load/Save api to support new info section.
 */

#include "stdafx.h"
#include "DlgConfigUpdate.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "ARBAgilityRecordBook.h"
#include "ConfigHandler.h"
#include "Element.h"
#include "Widgets.h"
#include <wx/filedlg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgConfigUpdate, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigUpdate::OnOk)
END_EVENT_TABLE()


CDlgConfigUpdate::CDlgConfigUpdate(wxWindow* pParent)
	: wxDialog()
	, m_radioDefault(NULL)
	, m_radioExisting(NULL)
	, m_btnPick(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_UPDATE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	m_radioDefault = new wxRadioButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_DEFAULT"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_radioDefault, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgConfigUpdate::OnUpdateDefault);
	m_radioDefault->SetHelpText(_("HIDC_CONFIG_UPDATE_DEFAULT"));
	m_radioDefault->SetToolTip(_("HIDC_CONFIG_UPDATE_DEFAULT"));

	m_radioExisting = new wxRadioButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_EXISTING"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_radioExisting, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgConfigUpdate::OnUpdateExisting);
	m_radioExisting->SetHelpText(_("HIDC_CONFIG_UPDATE_EXISTING"));
	m_radioExisting->SetToolTip(_("HIDC_CONFIG_UPDATE_EXISTING"));

	m_radioDefault->SetValue(true);

	m_btnPick = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_btnPick, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigUpdate::OnName);
	m_btnPick->SetHelpText(_("HIDC_CONFIG_UPDATE_NAME"));
	m_btnPick->SetToolTip(_("HIDC_CONFIG_UPDATE_NAME"));

	m_FileName = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0);
	m_FileName->SetHelpText(_("HIDC_CONFIG_UPDATE_FILENAME"));
	m_FileName->SetToolTip(_("HIDC_CONFIG_UPDATE_FILENAME"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_radioDefault, 0, wxALL, 5);

	wxBoxSizer* bSizerRadio = new wxBoxSizer(wxHORIZONTAL);
	bSizerRadio->Add(m_radioExisting, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bSizerRadio->Add(m_btnPick, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(bSizerRadio, 0, 0, 5);

	wxBoxSizer* bSizerWithSpace = new wxBoxSizer(wxHORIZONTAL);
	bSizerWithSpace->Add(15, 0, 0, wxEXPAND, 5);
	bSizerWithSpace->Add(m_FileName, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(bSizerWithSpace, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_radioDefault->SetFocus();

	EnableControls();
}


bool CDlgConfigUpdate::LoadConfig(wxChar const* pFile)
{
	wxBusyCursor wait;
	if (!pFile)
	{
		CConfigHandler handler;
		m_Book.GetConfig().Default(&handler);
	}
	else
	{
		wxString errMsg;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		if (!tree->LoadXMLFile(pFile, errMsg))
		{
			wxString msg = _("AFX_IDP_FAILED_TO_OPEN_DOC");
			if (0 < errMsg.length())
			{
				msg += wxT("\n\n");
				msg += errMsg;
			}
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		CErrorCallback err;
		if (!m_Book.Load(tree, false, false, true, false, false, err))
		{
			if (0 < err.m_ErrMsg.length())
				wxMessageBox(err.m_ErrMsg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			return false;
		}
		else if (0 < err.m_ErrMsg.length())
			wxMessageBox(err.m_ErrMsg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return true;
}


void CDlgConfigUpdate::EnableControls()
{
	if (m_radioDefault->GetValue())
	{
		m_btnPick->Enable(false);
		m_FileName->Enable(false);
	}
	else
	{
		m_btnPick->Enable(true);
		m_FileName->Enable(true);
	}
}


void CDlgConfigUpdate::OnUpdateDefault(wxCommandEvent& evt)
{
	EnableControls();
}


void CDlgConfigUpdate::OnUpdateExisting(wxCommandEvent& evt)
{
	EnableControls();
}


void CDlgConfigUpdate::OnName(wxCommandEvent& evt)
{
	wxFileDialog file(this,
			wxT(""), // caption
			wxT(""), // default dir
			m_FileName->GetValue(),
			_("IDS_FILEEXT_FILTER_ARB"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (wxID_OK == file.ShowModal())
	{
		m_FileName->SetValue(file.GetPath());
	}
}


void CDlgConfigUpdate::OnOk(wxCommandEvent& evt)
{
	if (Validate() && TransferDataFromWindow())
	{
		wxChar const* pFile = NULL;
		wxString source(m_FileName->GetValue());
		if (m_radioExisting->GetValue())
			pFile = source;
		if (!LoadConfig(pFile))
			return;
		EndDialog(wxID_OK);
	}
}
