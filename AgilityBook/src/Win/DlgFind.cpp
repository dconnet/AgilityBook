/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgFind class
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-16 DRC Fix initial focus.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2003-12-27 DRC Created
 */

#include "stdafx.h"
#include "DlgFind.h"

#include "AgilityBook.h"
#include "StringUtil.h"
#include "Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgFind::CDlgFind(
		IFindCallback& callback,
		wxWindow* pParent)
	: wxDialog()
	, m_Callback(callback)
	, m_textCtrl(NULL)
	, m_radioBoxSearch(NULL)
	, m_radioBoxDir(NULL)
	, m_btnFind(NULL)
{
	wxString caption = StringUtil::stringWX(m_Callback.GetCaption());
	if (caption.empty())
		caption = _("IDD_FIND");
	wxString text = StringUtil::stringWX(m_Callback.Text());

	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* staticText = new wxStaticText(this, wxID_ANY,
		_("IDC_FIND_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	staticText->Wrap(-1);

	m_textCtrl = new CTextCtrl(this, wxID_ANY, text,
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_textCtrl, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler, CDlgFind::OnChangeName);
	m_textCtrl->SetHelpText(_("HIDC_FIND_NAME"));

	m_checkBox = new wxCheckBox(this, wxID_ANY,
		_("IDC_FIND_CASE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_checkBox->SetValue(m_Callback.MatchCase());
	m_checkBox->SetHelpText(_("HIDC_FIND_CASE"));

	wxString radioSearchFields[] =
	{
		_("IDC_FIND_ALL"),
		_("IDC_FIND_VISIBLE")
	};
	int radioSearchCount = sizeof(radioSearchFields) / sizeof(radioSearchFields[0]);
	m_radioBoxSearch = new wxRadioBox(this, wxID_ANY,
		_("IDC_FIND_FIELDS"),
		wxDefaultPosition, wxDefaultSize,
		radioSearchCount, radioSearchFields,
		1, wxRA_SPECIFY_COLS);
	m_radioBoxSearch->SetItemHelpText(0, _("HIDC_FIND_ALL"));
	m_radioBoxSearch->SetItemToolTip(0, _("HIDC_FIND_ALL"));
	m_radioBoxSearch->SetItemHelpText(1, _("HIDC_FIND_VISIBLE"));
	m_radioBoxSearch->SetItemToolTip(1, _("HIDC_FIND_VISIBLE"));
	m_radioBoxSearch->SetSelection(m_Callback.SearchAll() ? 0 : 1);
	if (!m_Callback.EnableSearch())
		m_radioBoxSearch->Enable(false);

	wxString radioDirectionFields[] =
	{
		_("IDC_FIND_UP"),
		_("IDC_FIND_DOWN")
	};
	int radioDirectionCount = sizeof(radioDirectionFields) / sizeof(radioDirectionFields[0]);
	m_radioBoxDir = new wxRadioBox(this, wxID_ANY,
		_("IDC_FIND_DIRECTION"),
		wxDefaultPosition, wxDefaultSize,
		radioDirectionCount, radioDirectionFields,
		1, wxRA_SPECIFY_COLS);
	m_radioBoxDir->SetItemHelpText(0, _("HIDC_FIND_UP"));
	m_radioBoxDir->SetItemToolTip(0, _("HIDC_FIND_UP"));
	m_radioBoxDir->SetItemHelpText(1, _("HIDC_FIND_DOWN"));
	m_radioBoxDir->SetItemToolTip(1, _("HIDC_FIND_DOWN"));
	m_radioBoxDir->SetSelection(m_Callback.SearchDown() ? 1 : 0);
	if (!m_Callback.EnableDirection())
		m_radioBoxDir->Enable(false);

	m_btnFind = new wxButton(this, wxID_ANY,
		_("IDC_FIND_NEXT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnFind->SetDefault();
	m_btnFind->Enable(0 < text.length());
	BIND_OR_CONNECT_CTRL(m_btnFind, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgFind::OnFind);

	wxButton* btnClose = new wxButton(this, wxID_CANCEL,
		_("IDC_FIND_CLOSE"),
		wxDefaultPosition, wxDefaultSize, 0);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	bSizer2->Add(staticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bSizer2->Add(m_textCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(bSizer2, 0, wxEXPAND, 0);
	bSizer->Add(m_checkBox, 0, wxALL, 5);

	wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
	bSizer3->Add(m_radioBoxSearch, 0, wxALL, 5);
	bSizer3->Add(m_radioBoxDir, 0, wxALL, 5);

	bSizer->Add(bSizer3, 0, 0, 5);

	wxBoxSizer* bSizerBtns = new wxBoxSizer(wxHORIZONTAL);
	bSizerBtns->Add(0, 0, 1, wxEXPAND, 0);
	bSizerBtns->Add(m_btnFind, 0, wxALL, 5);
	bSizerBtns->Add(btnClose, 0, wxALL, 5);

	bSizer->Add(bSizerBtns, 0, wxEXPAND, 0);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgFind, m_textCtrl)
}


DEFINE_ON_INIT(CDlgFind)


void CDlgFind::OnChangeName(wxCommandEvent& evt)
{
	m_btnFind->Enable(0 < m_textCtrl->GetValue().length());
}


void CDlgFind::OnFind(wxCommandEvent& evt)
{
	m_Callback.Text(StringUtil::stringW(m_textCtrl->GetValue()));
	m_Callback.MatchCase(m_checkBox->GetValue());
	m_Callback.SearchAll(m_radioBoxSearch->GetSelection() == 0);
	m_Callback.SearchDown(m_radioBoxDir->GetSelection() == 1);
	m_Callback.Search(this);
}
