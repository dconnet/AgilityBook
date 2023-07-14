/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigLifetimeName class
 * @author David Connet
 *
 * Revision History
 * 2016-01-16 Made 'Name' field wider (cut off on hiDPI)
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-01-13 Added new recurring title suffix style.
 * 2012-05-07 Fixed some comboboxes that should have been readonly.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2008-09-06 Added roman numeral support
 * 2007-06-25 Allow "1" as the start for recurring titles.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-01-05 Created.
 */

#include "stdafx.h"
#include "DlgConfigLifetimeName.h"

#include "AgilityBook.h"

#include "ARB/ARBConfigLifetimeName.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;


CDlgConfigLifetimeName::CDlgConfigLifetimeName(ARBConfigVenuePtr const& inVenue, wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_bCheckOldName(false)
	, m_Name()
	, m_NewName()
	, m_trace("CDlgConfigLifetimeName")
{
	InitDlg(pParent);
}


CDlgConfigLifetimeName::CDlgConfigLifetimeName(
	ARB::ARBConfigVenuePtr const& inVenue,
	std::wstring const& inName,
	wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_bCheckOldName(true)
	, m_Name(StringUtil::stringW(inName))
	, m_NewName(inName)
	, m_trace("CDlgConfigLifetimeName")
{
	InitDlg(pParent);
}


void CDlgConfigLifetimeName::InitDlg(wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_CONFIG_LIFETIMENAME"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	CTextCtrl* ctrlName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(50, -1)),
		0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	ctrlName->SetHelpText(_("HIDC_CONFIG_LIFETIME_NAME"));
	ctrlName->SetToolTip(_("HIDC_CONFIG_LIFETIME_NAME"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer->Add(ctrlName, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigLifetimeName::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgConfigLifetimeName, ctrlName)
}


void CDlgConfigLifetimeName::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	std::wstring oldName = m_NewName;
	m_NewName = StringUtil::stringW(m_Name);

	if (!m_bCheckOldName || m_NewName != oldName)
	{
		if (m_Venue->GetLifetimeNames().FindLifetimeName(m_NewName))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return;
		}
	}

	EndDialog(wxID_OK);
}

} // namespace dconSoft
