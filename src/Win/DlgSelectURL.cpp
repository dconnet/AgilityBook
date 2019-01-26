/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgSelectURL class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-01-12 Clean up spacing.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-03-30 Created.
 */

#include "stdafx.h"
#include "DlgSelectURL.h"

#include "AgilityBook.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgSelectURL::CDlgSelectURL(
		std::wstring const& name,
		wxWindow* pParent)
	: wxDialog()
	, m_Name(StringUtil::stringWX(name))
	, m_textCtrl(nullptr)
{
	InitDlg(pParent, false);
}


CDlgSelectURL::CDlgSelectURL(
		std::wstring const& name,
		bool bAllowEmpty,
		wxWindow* pParent)
	: wxDialog()
	, m_Name(StringUtil::stringWX(name))
	, m_textCtrl(nullptr)
{
	InitDlg(pParent, bAllowEmpty);
}


void CDlgSelectURL::InitDlg(wxWindow* pParent, bool bAllowEmpty)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_SELECT_URL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_textCtrl = new CTextCtrl(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 170), -1), 0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH | (bAllowEmpty ? 0 : TRIMVALIDATOR_NONEMPTY), _("IDS_ENTER_NAME")));
	m_textCtrl->SetHelpText(_("HIDC_SELECTURL_NAME"));
	m_textCtrl->SetToolTip(_("HIDC_SELECTURL_NAME"));

	wxButton* btnSelect = new wxButton(this, wxID_ANY,
		_("IDC_SELECTURL_FILENAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnSelect, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgSelectURL::OnFilename);
	btnSelect->SetHelpText(_("HIDC_SELECTURL_FILENAME"));
	btnSelect->SetToolTip(_("HIDC_SELECTURL_FILENAME"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bTextSizer = new wxBoxSizer(wxHORIZONTAL);
	bTextSizer->Add(m_textCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	bTextSizer->Add(btnSelect, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(bTextSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgSelectURL, m_textCtrl)
}


DEFINE_ON_INIT(CDlgSelectURL)


std::wstring CDlgSelectURL::Name() const
{
	return StringUtil::stringW(m_Name);
}


void CDlgSelectURL::OnFilename(wxCommandEvent& evt)
{
	wxFileDialog file(this,
			L"", // caption
			L"", // default dir
			m_textCtrl->GetValue(),
			_("IDS_FILEEXT_FILTER_ALL"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (wxID_OK == file.ShowModal())
	{
		m_textCtrl->SetValue(file.GetPath());
	}
}
