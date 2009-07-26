/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Configuration updater
 * @author David Connet
 *
 * Revision History
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
#include <wx/filedlg.h>


BEGIN_EVENT_TABLE(CDlgConfigUpdate, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigUpdate::OnOk)
END_EVENT_TABLE()


CDlgConfigUpdate::CDlgConfigUpdate(wxWindow* parent)
	: wxDialog()
	, m_radioDefault(NULL)
	, m_radioExisting(NULL)
	, m_btnPick(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(parent, wxID_ANY, _("IDD_CONFIG_UPDATE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	m_radioDefault = new wxRadioButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_DEFAULT"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_radioDefault->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgConfigUpdate::OnUpdateDefault), NULL, this);
	m_radioDefault->SetHelpText(_("HIDC_CONFIG_UPDATE_DEFAULT"));
	m_radioDefault->SetToolTip(_("HIDC_CONFIG_UPDATE_DEFAULT"));

	m_radioExisting = new wxRadioButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_EXISTING"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_radioExisting->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgConfigUpdate::OnUpdateExisting), NULL, this);
	m_radioExisting->SetHelpText(_("HIDC_CONFIG_UPDATE_EXISTING"));
	m_radioExisting->SetToolTip(_("HIDC_CONFIG_UPDATE_EXISTING"));

	m_radioDefault->SetValue(true);

	m_btnPick = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnPick->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigUpdate::OnName), NULL, this);
	m_btnPick->SetHelpText(_("HIDC_CONFIG_UPDATE_NAME"));
	m_btnPick->SetToolTip(_("HIDC_CONFIG_UPDATE_NAME"));

	m_FileName = new wxTextCtrl(this, wxID_ANY,
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
		tstring errMsg;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		if (!tree->LoadXMLFile(pFile, errMsg))
		{
			wxString msg = _("AFX_IDP_FAILED_TO_OPEN_DOC");
			if (0 < errMsg.length())
			{
				msg += wxT("\n\n");
				msg += errMsg.c_str();
			}
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		CErrorCallback err;
		if (!m_Book.Load(tree, false, false, true, false, false, err))
		{
			if (0 < err.m_ErrMsg.length())
				wxMessageBox(err.m_ErrMsg.c_str(), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
			return false;
		}
		else if (0 < err.m_ErrMsg.length())
			wxMessageBox(err.m_ErrMsg.c_str(), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
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
