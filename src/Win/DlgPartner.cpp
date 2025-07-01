/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgPartner class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-03-12 Dog/Handler may not be empty.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Fix initial focus.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgPartner.h"

#include "AgilityBook.h"

#include "ARB/ARBDogRunPartner.h"
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


CDlgPartner::CDlgPartner(
	ARBDogRunPartnerPtr const& inPartner,
	std::set<wxString> const& inHandlers,
	std::set<wxString> const& inDogs,
	wxWindow* pParent)
	: wxDialog()
	, m_Handler(inPartner->GetHandler())
	, m_Dog(inPartner->GetDog())
	, m_RegNum(inPartner->GetRegNum())
	, m_Partner(inPartner)
	, m_trace("CDlgPartner")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_PARTNER"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxArrayString handlers;
	std::set<wxString>::const_iterator iter;
	for (iter = inHandlers.begin(); iter != inHandlers.end(); ++iter)
	{
		handlers.Add(*iter);
	}
	handlers.Sort();

	wxArrayString dogs;
	for (iter = inDogs.begin(); iter != inDogs.end(); ++iter)
	{
		dogs.Add(*iter);
	}
	dogs.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textHandler
		= new wxStaticText(this, wxID_ANY, _("IDC_PARTNER_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	wxComboBox* ctrlHandler = new wxComboBox(
		this,
		wxID_ANY,
		m_Handler,
		wxDefaultPosition,
		wxDefaultSize,
		handlers,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Handler, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	ctrlHandler->SetHelpText(_("HIDC_PARTNER_NAME"));
	ctrlHandler->SetToolTip(_("HIDC_PARTNER_NAME"));
	ctrlHandler->AutoComplete(handlers);

	wxStaticText* textDog
		= new wxStaticText(this, wxID_ANY, _("IDC_PARTNER_CALLNAME"), wxDefaultPosition, wxDefaultSize, 0);
	textDog->Wrap(-1);

	wxComboBox* ctrlDog = new wxComboBox(
		this,
		wxID_ANY,
		m_Dog,
		wxDefaultPosition,
		wxDefaultSize,
		dogs,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Dog, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	ctrlDog->SetHelpText(_("HIDC_PARTNER_CALLNAME"));
	ctrlDog->SetToolTip(_("HIDC_PARTNER_CALLNAME"));
	ctrlDog->AutoComplete(dogs);

	wxStaticText* textNumber
		= new wxStaticText(this, wxID_ANY, _("IDC_PARTNER_REG_NUM"), wxDefaultPosition, wxDefaultSize, 0);
	textNumber->Wrap(-1);

	CTextCtrl* ctrlNumber
		= new CTextCtrl(this, wxID_ANY, m_RegNum, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_RegNum));
	ctrlNumber->SetHelpText(_("HIDC_PARTNER_REG_NUM"));
	ctrlNumber->SetToolTip(_("HIDC_PARTNER_REG_NUM"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(textHandler, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer1->Add(ctrlHandler, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	bSizer->Add(sizer1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(textDog, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer2->Add(ctrlDog, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	bSizer->Add(sizer2, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(textNumber, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer3->Add(ctrlNumber, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	bSizer->Add(sizer3, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPartner::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(ctrlHandler)
}


void CDlgPartner::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	m_Partner->SetHandler(m_Handler);
	m_Partner->SetDog(m_Dog);
	m_Partner->SetRegNum(m_RegNum);
	EndDialog(wxID_OK);
}

} // namespace dconSoft
