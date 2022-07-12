/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Configuration updater
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-26 Display errors on non-fatal load.
 * 2003-12-07 Changed Load/Save api to support new info section.
 */

#include "stdafx.h"
#include "DlgConfigUpdate.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "ConfigHandler.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Widgets.h"
#include <wx/filedlg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

wxBEGIN_EVENT_TABLE(CDlgConfigUpdate, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigUpdate::OnOk)
wxEND_EVENT_TABLE()


CDlgConfigUpdate::CDlgConfigUpdate(wxWindow* pParent)
	: wxDialog()
	, m_radioDefault(nullptr)
	, m_radioExisting(nullptr)
	, m_btnPick(nullptr)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_UPDATE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	m_radioDefault = new wxRadioButton(
		this,
		wxID_ANY,
		_("IDC_CONFIG_UPDATE_DEFAULT"),
		wxDefaultPosition,
		wxDefaultSize,
		wxRB_GROUP);
	m_radioDefault->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgConfigUpdate::OnUpdateDefault, this);
	m_radioDefault->SetHelpText(_("HIDC_CONFIG_UPDATE_DEFAULT"));
	m_radioDefault->SetToolTip(_("HIDC_CONFIG_UPDATE_DEFAULT"));

	m_radioExisting
		= new wxRadioButton(this, wxID_ANY, _("IDC_CONFIG_UPDATE_EXISTING"), wxDefaultPosition, wxDefaultSize, 0);
	m_radioExisting->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CDlgConfigUpdate::OnUpdateExisting, this);
	m_radioExisting->SetHelpText(_("HIDC_CONFIG_UPDATE_EXISTING"));
	m_radioExisting->SetToolTip(_("HIDC_CONFIG_UPDATE_EXISTING"));

	m_radioDefault->SetValue(true);

	m_btnPick = new wxButton(this, wxID_ANY, _("IDC_CONFIG_UPDATE_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnPick->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigUpdate::OnName, this);
	m_btnPick->SetHelpText(_("HIDC_CONFIG_UPDATE_NAME"));
	m_btnPick->SetToolTip(_("HIDC_CONFIG_UPDATE_NAME"));

	m_FileName = new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_FileName->SetHelpText(_("HIDC_CONFIG_UPDATE_FILENAME"));
	m_FileName->SetToolTip(_("HIDC_CONFIG_UPDATE_FILENAME"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_radioDefault, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerRadio = new wxBoxSizer(wxHORIZONTAL);
	bSizerRadio->Add(m_radioExisting, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	bSizerRadio->Add(m_btnPick, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(bSizerRadio, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerWithSpace = new wxBoxSizer(wxHORIZONTAL);
	bSizerWithSpace->Add(wxDLG_UNIT_X(this, 10), 0, 0, wxEXPAND, 0);
	bSizerWithSpace->Add(m_FileName, 1, wxEXPAND, 0);

	bSizer->Add(bSizerWithSpace, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_radioDefault->SetFocus();

	EnableControls();
}


bool CDlgConfigUpdate::LoadConfig(wchar_t const* pFile)
{
	if (!pFile)
	{
		wxBusyCursor wait;
		CConfigHandler handler;
		m_Book.GetConfig().Default(&handler);
	}
	else
	{
		fmt::wmemory_buffer errMsg;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		bool bOk = false;
		{
			wxBusyCursor wait;
			bOk = tree->LoadXML(pFile, errMsg);
		}
		if (!bOk)
		{
			wxString msg = _("AFX_IDP_FAILED_TO_OPEN_DOC");
			if (0 < errMsg.size())
			{
				msg << L"\n\n" << fmt::to_string(errMsg);
			}
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		CErrorCallback err;
		if (!m_Book.Load(tree, false, false, true, false, false, err))
		{
			if (0 < err.m_ErrMsg.size())
				wxMessageBox(
					StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)),
					wxMessageBoxCaptionStr,
					wxOK | wxCENTRE | wxICON_WARNING);
			return false;
		}
		else if (0 < err.m_ErrMsg.size())
			wxMessageBox(
				StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)),
				wxMessageBoxCaptionStr,
				wxOK | wxCENTRE | wxICON_INFORMATION);
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
	wxFileDialog file(
		this,
		L"", // caption
		L"", // default dir
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
		wchar_t const* pFile = nullptr;
		wxString source(m_FileName->GetValue());
		if (m_radioExisting->GetValue())
			pFile = source;
		if (!LoadConfig(pFile))
			return;
		EndDialog(wxID_OK);
	}
}

} // namespace dconSoft
