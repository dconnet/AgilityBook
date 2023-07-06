/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated from DlgDog.cpp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-05-30 DOB/Deceased were not initialized in ctor.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDogPanelProp.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgDogPanelProp::CDlgDogPanelProp(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: CDlgDogPanelBase(pDoc, inDog, parent)
	, m_CallName(StringUtil::stringWX(inDog->GetCallName()))
	, m_Breed(StringUtil::stringWX(inDog->GetBreed()))
	, m_RegName(StringUtil::stringWX(inDog->GetRegisteredName()))
	, m_DOB(inDog->GetDOB())
	, m_IsDeceased(inDog->GetDeceased().IsValid())
	, m_Deceased(inDog->GetDeceased())
	, m_Notes(StringUtil::stringWX(inDog->GetNote()))
	, m_ctrlName(nullptr)
	, m_ctrlAge(nullptr)
	, m_ctrlDDay(nullptr)
{
	wxStaticText* textCallName
		= new wxStaticText(this, wxID_ANY, _("IDC_DOG_CALLNAME"), wxDefaultPosition, wxDefaultSize, 0);
	textCallName->Wrap(-1);

	m_ctrlName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_CallName, TRIMVALIDATOR_DEFAULT, _("IDS_BLANK_CALLNAME")));
	m_ctrlName->SetHelpText(_("HIDC_DOG_CALLNAME"));
	m_ctrlName->SetToolTip(_("HIDC_DOG_CALLNAME"));

	wxStaticText* textBreed = new wxStaticText(this, wxID_ANY, _("IDC_DOG_BREED"), wxDefaultPosition, wxDefaultSize, 0);
	textBreed->Wrap(-1);

	CTextCtrl* ctrlBreed = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_Breed, TRIMVALIDATOR_TRIM_BOTH));
	ctrlBreed->SetHelpText(_("HIDC_DOG_BREED"));
	ctrlBreed->SetToolTip(_("HIDC_DOG_BREED"));

	wxStaticText* textRegName
		= new wxStaticText(this, wxID_ANY, _("IDC_DOG_REG_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textRegName->Wrap(-1);

	CTextCtrl* ctrlRegName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_RegName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlRegName->SetHelpText(_("HIDC_DOG_REG_NAME"));
	ctrlRegName->SetToolTip(_("HIDC_DOG_REG_NAME"));

	wxStaticText* textAge
		= new wxStaticText(this, wxID_ANY, _("IDC_DOG_AGE_TEXT"), wxDefaultPosition, wxDefaultSize, 0);
	textAge->Wrap(-1);

	m_ctrlAge = new wxStaticText(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		wxALIGN_CENTRE | wxST_NO_AUTORESIZE | wxSUNKEN_BORDER);
	m_ctrlAge->Wrap(-1);

	wxStaticText* textBDay = new wxStaticText(this, wxID_ANY, _("IDC_DOG_DOB"), wxDefaultPosition, wxDefaultSize, 0);
	textBDay->Wrap(-1);

	wxDatePickerCtrl* ctrlBDay = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_DOB));
	ctrlBDay->Bind(wxEVT_DATE_CHANGED, &CDlgDogPanelProp::OnDateChanged, this);
	ctrlBDay->SetHelpText(_("HIDC_DOG_DOB"));
	ctrlBDay->SetToolTip(_("HIDC_DOG_DOB"));

	wxCheckBox* ctrlDeceased = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_DOG_IS_DECEASED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_IsDeceased));
	ctrlDeceased->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgDogPanelProp::OnDeceased, this);
	ctrlDeceased->SetHelpText(_("HIDC_DOG_IS_DECEASED"));
	ctrlDeceased->SetToolTip(_("HIDC_DOG_IS_DECEASED"));

	m_ctrlDDay = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_Deceased));
	m_ctrlDDay->Bind(wxEVT_DATE_CHANGED, &CDlgDogPanelProp::OnDateChanged, this);
	m_ctrlDDay->SetHelpText(_("HIDC_DOG_DECEASED"));
	m_ctrlDDay->SetToolTip(_("HIDC_DOG_DECEASED"));

	wxStaticText* textNotes = new wxStaticText(this, wxID_ANY, _("IDC_DOG_NOTES"), wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CSpellCheckCtrl* ctrlNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 80)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_DOG_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_DOG_NOTES"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* sizerProp = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textCallName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerName->Add(m_ctrlName, 2, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerName->Add(textBreed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerName->Add(ctrlBreed, 3, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerRegName = new wxBoxSizer(wxHORIZONTAL);
	sizerRegName->Add(textRegName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerRegName->Add(ctrlRegName, 1, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerRegName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerAge = new wxBoxSizer(wxHORIZONTAL);
	sizerAge->Add(textAge, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerAge->Add(m_ctrlAge, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerAge->Add(textBDay, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerAge->Add(ctrlBDay, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerAge->Add(ctrlDeceased, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerAge->Add(m_ctrlDDay, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerAge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	sizerProp->Add(textNotes, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	sizerProp->Add(ctrlNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.Controls());

	SetSizer(sizerProp);
	Layout();
	sizerProp->Fit(this);

	UpdateDeceased();
}


wxWindow* CDlgDogPanelProp::GetInitialControl()
{
	return m_ctrlName;
}


bool CDlgDogPanelProp::Save(unsigned int& hint)
{
	if (!m_IsDeceased)
		m_Deceased.clear();

	bool bModified = false;

	if (m_pDog->GetCallName() != StringUtil::stringW(m_CallName))
	{
		std::wstring newCallName = StringUtil::stringW(m_CallName);
		CAgilityBookOptions::CleanLastItems(m_pDog->GetCallName(), newCallName);
		hint |= UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW;
		m_pDog->SetCallName(newCallName);
	}
	if (m_pDog->GetDOB() != m_DOB)
	{
		// All we really need to do here is to refresh the tree item. But
		// that may not be the current one right now, so use the big hammer.
		hint |= UPDATE_TREE_VIEW;
		m_pDog->SetDOB(m_DOB);
	}
	if (m_pDog->GetDeceased() != m_Deceased)
	{
		// big hammer
		hint |= UPDATE_TREE_VIEW;
		m_pDog->SetDeceased(m_Deceased);
	}
	if (m_pDog->GetBreed() != StringUtil::stringW(m_Breed))
	{
		bModified = true;
		m_pDog->SetBreed(StringUtil::stringW(m_Breed));
	}
	if (m_pDog->GetRegisteredName() != StringUtil::stringW(m_RegName))
	{
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->SetRegisteredName(StringUtil::stringW(m_RegName));
	}
	if (m_pDog->GetNote() != StringUtil::stringW(m_Notes))
	{
		bModified = true;
		m_pDog->SetNote(StringUtil::stringW(m_Notes));
	}

	return bModified;
}


void CDlgDogPanelProp::UpdateAge()
{
	long ageDays = 0;
	if (m_DOB.IsValid())
	{
		ARBDate current(ARBDate::Today());
		if (m_IsDeceased)
			current = m_Deceased;
		ageDays = current - m_DOB;
	}
	m_ctrlAge->SetLabel(StringUtil::stringWX(ARBDouble::ToString(ageDays / 365.0, 1)));
}


void CDlgDogPanelProp::UpdateDeceased()
{
	m_ctrlDDay->Show(m_IsDeceased);
	UpdateAge();
}


void CDlgDogPanelProp::OnDateChanged(wxDateEvent& evt)
{
	TransferDataFromWindow();
	UpdateAge();
}


void CDlgDogPanelProp::OnDeceased(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateDeceased();
}

} // namespace dconSoft
