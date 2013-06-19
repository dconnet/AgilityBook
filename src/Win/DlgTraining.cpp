/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2012-05-07 DRC Added autocompletion to combo boxes.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-09-21 DRC Created
 */

#include "stdafx.h"
#include "DlgTraining.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "ComboBoxes.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBTraining.h"
#include "ARBCommon/StringUtil.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgTraining, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgTraining::OnOk)
END_EVENT_TABLE()


CDlgTraining::CDlgTraining(
		ARBTrainingPtr pTraining,
		CAgilityBookDoc* pDoc,
		wxWindow* pParent)
	: wxDialog()
	, m_pTraining(pTraining)
	, m_pDoc(pDoc)
	, m_datePicker(NULL)
	, m_Name(StringUtil::stringWX(pTraining->GetName()))
	, m_SubName(StringUtil::stringWX(pTraining->GetSubName()))
	, m_Notes(StringUtil::stringWX(pTraining->GetNote()))
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_TRAINING"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxDateTime date(wxDateTime::Now());
	if (pTraining->GetDate().IsValid())
		pTraining->GetDate().GetDate(date);

	wxArrayString names, subnames;

	std::set<std::wstring> items;
	m_pDoc->Book().GetTraining().GetAllNames(items);
	std::set<std::wstring>::iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		names.Add(StringUtil::stringWX(*iter));
	}
	names.Sort();
	m_pDoc->Book().GetTraining().GetAllSubNames(items);
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		subnames.Add(StringUtil::stringWX(*iter));
	}
	subnames.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textDate = new wxStaticText(this, wxID_ANY,
		_("IDC_TRAINING_DATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	m_datePicker = new wxDatePickerCtrl(this, wxID_ANY, date,
		wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	m_datePicker->SetHelpText(_("HIDC_TRAINING_DATE"));
	m_datePicker->SetToolTip(_("HIDC_TRAINING_DATE"));

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_TRAINING_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	CAutoFillComboBox* ctrlName = new CAutoFillComboBox(this, wxID_ANY, m_Name,
		wxDefaultPosition, wxDefaultSize,
		names, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	ctrlName->SetHelpText(_("HIDC_TRAINING_NAME"));
	ctrlName->SetToolTip(_("HIDC_TRAINING_NAME"));
	ctrlName->AutoComplete(names);

	wxStaticText* textSubname = new wxStaticText(this, wxID_ANY,
		_("IDC_TRAINING_SUBNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSubname->Wrap(-1);

	CAutoFillComboBox* ctrlSubname = new CAutoFillComboBox(this, wxID_ANY, m_SubName,
		wxDefaultPosition, wxDefaultSize,
		subnames, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_SubName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlSubname->SetHelpText(_("HIDC_TRAINING_SUBNAME"));
	ctrlSubname->SetToolTip(_("HIDC_TRAINING_SUBNAME"));
	ctrlSubname->AutoComplete(subnames);

	CTextCtrl* ctrlNote = new CTextCtrl(this, wxID_ANY, m_Notes,
		wxDefaultPosition, wxSize(370,210),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlNote->SetHelpText(_("HIDC_TRAINING_NOTES"));
	ctrlNote->SetToolTip(_("HIDC_TRAINING_NOTES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(m_datePicker, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerDate, 0, wxEXPAND, 0);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 0);

	wxBoxSizer* sizerSubname = new wxBoxSizer(wxHORIZONTAL);
	sizerSubname->Add(textSubname, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerSubname->Add(ctrlSubname, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerSubname, 0, wxEXPAND, 0);
	bSizer->Add(ctrlNote, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

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
	m_pTraining->SetName(StringUtil::stringW(m_Name));
	m_pTraining->SetSubName(StringUtil::stringW(m_SubName));
	m_pTraining->SetNote(StringUtil::stringW(m_Notes));

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}
