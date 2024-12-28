/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgName class
 * @author David Connet
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2009-02-11 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgName.h"

#include "AgilityBook.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgName::CDlgName(wxString const& name, wxWindow* pParent)
	: m_Name(name)
	, m_trace("CDlgName")
{
	Create(_("IDD_NAME"), pParent);
}


CDlgName::CDlgName(wxString const& name, wxString const& caption, wxWindow* pParent)
	: m_Name(name)
	, m_trace("CDlgName")
{
	Create(caption, pParent);
}


bool CDlgName::Create(wxString const& caption, wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	if (!wxDialog::Create(
			pParent,
			wxID_ANY,
			caption,
			wxDefaultPosition,
			wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	// Controls (these are done first to control tab order)

	CTextCtrl* textCtrl = new CTextCtrl(
		this,
		wxID_ANY,
		m_Name,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 170), -1),
		0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	textCtrl->SetHelpText(_("HIDC_NAME"));
	textCtrl->SetToolTip(_("HIDC_NAME"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgName, textCtrl)

	return true;
}


wxString CDlgName::Name() const
{
	return m_Name;
}

} // namespace dconSoft
