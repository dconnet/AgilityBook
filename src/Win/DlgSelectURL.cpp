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
 * 2013-01-12 Clean up spacing.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-03-30 Created.
 */

#include "stdafx.h"
#include "DlgSelectURL.h"

#include "AgilityBook.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARBCommon/StringUtil.h"

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
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_SELECT_URL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_textCtrl = new CTextCtrl(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxSize(300, -1), 0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	m_textCtrl->SetHelpText(_("HIDC_SELECTURL_NAME"));
	m_textCtrl->SetToolTip(_("HIDC_SELECTURL_NAME"));

	wxButton* btnSelect = new wxButton(this, wxID_ANY,
		_("IDC_SELECTURL_FILENAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnSelect, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgSelectURL::OnFilename);
	btnSelect->SetHelpText(_("HIDC_SELECTURL_FILENAME"));
	btnSelect->SetToolTip(_("HIDC_SELECTURL_FILENAME"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bTextSizer = new wxBoxSizer(wxHORIZONTAL);
	bTextSizer->Add(m_textCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bTextSizer->Add(btnSelect, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	bSizer->Add(bTextSizer, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 1, wxALL|wxEXPAND, 5);

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
