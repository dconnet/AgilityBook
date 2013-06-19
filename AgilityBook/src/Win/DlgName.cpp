/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgName class
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-16 DRC Fix initial focus.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgName.h"

#include "AgilityBook.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgName::CDlgName(
		std::wstring const& name,
		wxWindow* pParent)
	: m_Name(StringUtil::stringWX(name))
{
	Create(StringUtil::stringW(_("IDD_NAME")), pParent);
}


CDlgName::CDlgName(
		std::wstring const& name,
		std::wstring const& caption,
		wxWindow* pParent)
	: m_Name(StringUtil::stringWX(name))
{
	Create(caption, pParent);
}


bool CDlgName::Create(
		std::wstring const& caption,
		wxWindow* pParent)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	if (!wxDialog::Create(pParent, wxID_ANY, caption.c_str(), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	// Controls (these are done first to control tab order)

	CTextCtrl* textCtrl = new CTextCtrl(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxSize(300, -1), 0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	textCtrl->SetHelpText(_("HIDC_NAME"));
	textCtrl->SetToolTip(_("HIDC_NAME"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 0, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgName, textCtrl)

	return true;
}


DEFINE_ON_INIT(CDlgName)


std::wstring CDlgName::Name() const
{
	return StringUtil::stringW(m_Name);
}
