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
#include "ComboBoxes.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBDogRunPartner.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgPartner, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgPartner::OnOk)
END_EVENT_TABLE()


CDlgPartner::CDlgPartner(
		ARBDogRunPartnerPtr partner,
		std::set<std::wstring> const& inHandlers,
		std::set<std::wstring> const& inDogs,
		wxWindow* pParent)
	: wxDialog()
	, m_Handler(StringUtil::stringWX(partner->GetHandler()))
	, m_Dog(StringUtil::stringWX(partner->GetDog()))
	, m_RegNum(StringUtil::stringWX(partner->GetRegNum()))
	, m_Partner(partner)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_PARTNER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	wxArrayString handlers;
	std::set<std::wstring>::const_iterator iter;
	for (iter = inHandlers.begin(); iter != inHandlers.end(); ++iter)
	{
		handlers.Add(StringUtil::stringWX(*iter));
	}
	handlers.Sort();

	wxArrayString dogs;
	for (iter = inDogs.begin(); iter != inDogs.end(); ++iter)
	{
		dogs.Add(StringUtil::stringWX(*iter));
	}
	dogs.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textHandler = new wxStaticText(this, wxID_ANY,
		_("IDC_PARTNER_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	CAutoFillComboBox* ctrlHandler = new CAutoFillComboBox(this, wxID_ANY, m_Handler,
		wxDefaultPosition, wxDefaultSize,
		handlers, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Handler, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	ctrlHandler->SetHelpText(_("HIDC_PARTNER_NAME"));
	ctrlHandler->SetToolTip(_("HIDC_PARTNER_NAME"));
	ctrlHandler->AutoComplete(handlers);

	wxStaticText* textDog = new wxStaticText(this, wxID_ANY,
		_("IDC_PARTNER_CALLNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDog->Wrap(-1);

	CAutoFillComboBox* ctrlDog = new CAutoFillComboBox(this, wxID_ANY, m_Dog,
		wxDefaultPosition, wxDefaultSize,
		dogs, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Dog, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	ctrlDog->SetHelpText(_("HIDC_PARTNER_CALLNAME"));
	ctrlDog->SetToolTip(_("HIDC_PARTNER_CALLNAME"));
	ctrlDog->AutoComplete(dogs);

	wxStaticText* textNumber = new wxStaticText(this, wxID_ANY,
		_("IDC_PARTNER_REG_NUM"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNumber->Wrap(-1);

	CTextCtrl* ctrlNumber = new CTextCtrl(this, wxID_ANY, m_RegNum,
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_RegNum));
	ctrlNumber->SetHelpText(_("HIDC_PARTNER_REG_NUM"));
	ctrlNumber->SetToolTip(_("HIDC_PARTNER_REG_NUM"));

	// Sizers
#pragma PRAGMA_TODO(convert to dialog units)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(textHandler, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer1->Add(ctrlHandler, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer1, 0, wxEXPAND, 0);

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(textDog, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer2->Add(ctrlDog, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer2, 0, wxEXPAND, 0);

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(textNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer3->Add(ctrlNumber, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer3, 1, wxEXPAND, 0);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgPartner, ctrlHandler)
}


DEFINE_ON_INIT(CDlgPartner)


void CDlgPartner::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	m_Partner->SetHandler(StringUtil::stringW(m_Handler));
	m_Partner->SetDog(StringUtil::stringW(m_Dog));
	m_Partner->SetRegNum(StringUtil::stringW(m_RegNum));
	EndDialog(wxID_OK);
}
