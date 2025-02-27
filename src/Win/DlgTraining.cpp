/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-09-21 Created
 */

#include "stdafx.h"
#include "DlgTraining.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBTraining.h"
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


CDlgTraining::CDlgTraining(ARBTrainingPtr const& inTraining, CAgilityBookDoc* pDoc, wxWindow* pParent)
	: wxDialog()
	, m_pTraining(inTraining)
	, m_pDoc(pDoc)
	, m_datePicker(nullptr)
	, m_Name(inTraining->GetName())
	, m_SubName(inTraining->GetSubName())
	, m_Notes(inTraining->GetNote())
	, m_trace("CDlgTraining")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_TRAINING"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxDateTime date(wxDateTime::Now());
	if (inTraining->GetDate().IsValid())
		inTraining->GetDate().GetDate(date);

	wxArrayString names, subnames;

	std::set<wxString> items;
	m_pDoc->Book().GetTraining().GetAllNames(items);
	std::set<wxString>::iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		names.Add(*iter);
	}
	names.Sort();
	m_pDoc->Book().GetTraining().GetAllSubNames(items);
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		subnames.Add(*iter);
	}
	subnames.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textDate
		= new wxStaticText(this, wxID_ANY, _("IDC_TRAINING_DATE"), wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	m_datePicker = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY);
	m_datePicker->SetHelpText(_("HIDC_TRAINING_DATE"));
	m_datePicker->SetToolTip(_("HIDC_TRAINING_DATE"));

	wxStaticText* textName
		= new wxStaticText(this, wxID_ANY, _("IDC_TRAINING_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	wxComboBox* ctrlName = new wxComboBox(
		this,
		wxID_ANY,
		m_Name,
		wxDefaultPosition,
		wxDefaultSize,
		names,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	ctrlName->SetHelpText(_("HIDC_TRAINING_NAME"));
	ctrlName->SetToolTip(_("HIDC_TRAINING_NAME"));
	ctrlName->AutoComplete(names);

	wxStaticText* textSubname
		= new wxStaticText(this, wxID_ANY, _("IDC_TRAINING_SUBNAME"), wxDefaultPosition, wxDefaultSize, 0);
	textSubname->Wrap(-1);

	wxComboBox* ctrlSubname = new wxComboBox(
		this,
		wxID_ANY,
		m_SubName,
		wxDefaultPosition,
		wxDefaultSize,
		subnames,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_SubName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlSubname->SetHelpText(_("HIDC_TRAINING_SUBNAME"));
	ctrlSubname->SetToolTip(_("HIDC_TRAINING_SUBNAME"));
	ctrlSubname->AutoComplete(subnames);

	CSpellCheckCtrl* ctrlNote = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		m_Notes,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(210, 110)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlNote->SetHelpText(_("HIDC_TRAINING_NOTES"));
	ctrlNote->SetToolTip(_("HIDC_TRAINING_NOTES"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textDate, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerDate->Add(m_datePicker, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerDate, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerName->Add(ctrlName, 1, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerSubname = new wxBoxSizer(wxHORIZONTAL);
	sizerSubname->Add(textSubname, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerSubname->Add(ctrlSubname, 1, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerSubname, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlNote, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTraining::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_datePicker->SetFocus();
}


void CDlgTraining::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	wxDateTime date = m_datePicker->GetValue();

	m_pTraining->SetDate(ARBDate(date.GetYear(), date.GetMonth() + 1, date.GetDay()));
	m_pTraining->SetName(m_Name);
	m_pTraining->SetSubName(m_SubName);
	m_pTraining->SetNote(m_Notes);

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}

} // namespace dconSoft
