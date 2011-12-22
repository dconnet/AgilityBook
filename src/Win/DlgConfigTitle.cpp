/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-09-06 DRC Added roman numeral support
 * @li 2007-06-25 DRC Allow "1" as the start for recurring titles.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "stdafx.h"
#include "DlgConfigTitle.h"

#include "AgilityBook.h"
#include "ARBConfigTitle.h"
#include "Validators.h"
#include "Widgets.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CMultipleValidator : public CGenericValidator
{
public:
	CMultipleValidator(
			wxCheckBox* ctrlAllowMany,
			short* val)
		: CGenericValidator(val)
		, m_ctrlAllowMany(ctrlAllowMany)
	{
	}
	virtual bool Validate(wxWindow* parent);
private:
	wxCheckBox* m_ctrlAllowMany;
};


bool CMultipleValidator::Validate(wxWindow* parent)
{
	if (!CGenericValidator::Validate(parent))
		return false;
	CTextCtrl* pControl = (CTextCtrl*)m_validatorWindow;
	short val = static_cast<short>(wxAtol(pControl->GetValue()));
	if (m_ctrlAllowMany->GetValue() && 1 > val)
	{
		wxBell();
		pControl->SetValue(wxT("2"));
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgConfigTitle, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigTitle::OnOk)
END_EVENT_TABLE()


CDlgConfigTitle::CDlgConfigTitle(
		ARBConfigTitlePtr inTitle,
		wxWindow* pParent)
	: wxDialog()
	, m_Title(inTitle)
	, m_Name(inTitle->GetName())
	, m_Prefix(inTitle->GetPrefix())
	, m_LongName(inTitle->GetLongName())
	, m_Desc(inTitle->GetDescription())
	, m_AllowMany(0 < inTitle->GetMultiple())
	, m_Multiple(inTitle->GetMultiple())
	, m_DateFrom(inTitle->GetValidFrom().IsValid())
	, m_DateTo(inTitle->GetValidTo().IsValid())
	, m_ctrlMultiple(NULL)
	, m_ctrlDateFrom(NULL)
	, m_ctrlDateTo(NULL)
	, m_ctrlStyle(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_TITLE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	CTextCtrl* ctrlName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Name, _("IDS_ENTER_NAME")));
	ctrlName->SetHelpText(_("HIDC_CONFIG_TITLE_NAME"));
	ctrlName->SetToolTip(_("HIDC_CONFIG_TITLE_NAME"));

	wxCheckBox* ctrlPrefix = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_PREFIX"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_Prefix));
	ctrlPrefix->SetHelpText(_("HIDC_CONFIG_TITLE_PREFIX"));
	ctrlPrefix->SetToolTip(_("HIDC_CONFIG_TITLE_PREFIX"));

	wxStaticText* textLong = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_LONG_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLong->Wrap(-1);

	CTextCtrl* ctrlLongName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_LongName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlLongName->SetHelpText(_("HIDC_CONFIG_TITLE_LONG_NAME"));
	ctrlLongName->SetToolTip(_("HIDC_CONFIG_TITLE_LONG_NAME"));

	wxStaticText* textDesc = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CTextCtrl* ctrlDesc = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 110), wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Desc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_CONFIG_TITLE_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_CONFIG_TITLE_DESC"));

	wxCheckBox* ctrlAllowMany = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_ALLOW_MULTIPLE"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_AllowMany));
	BIND_OR_CONNECT_CTRL(ctrlAllowMany, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgConfigTitle::OnAllowMultiple);
	ctrlAllowMany->SetHelpText(_("HIDC_CONFIG_TITLE_ALLOW_MULTIPLE"));
	ctrlAllowMany->SetToolTip(_("HIDC_CONFIG_TITLE_ALLOW_MULTIPLE"));

	wxStaticText* textStart = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_MULTIPLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	m_ctrlMultiple = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CMultipleValidator(ctrlAllowMany, &m_Multiple));
	m_ctrlMultiple->Enable(m_AllowMany);

	wxCheckBox* ctrlValidFrom = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_VALID_FROM"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_DateFrom));
	BIND_OR_CONNECT_CTRL(ctrlValidFrom, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgConfigTitle::OnCheck);
	ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_FROM"));
	ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_FROM"));

	wxDateTime date(wxDateTime::Now());
	if (m_DateFrom)
		date = m_Title->GetValidFrom().GetDate();
	m_ctrlDateFrom = new wxDatePickerCtrl(this, wxID_ANY, date,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	m_ctrlDateFrom->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_FROM_DATE"));
	m_ctrlDateFrom->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_FROM_DATE"));

	wxCheckBox* ctrlValidTo = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_VALID_TO"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_DateTo));
	BIND_OR_CONNECT_CTRL(ctrlValidTo, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgConfigTitle::OnCheck);
	ctrlValidTo->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_TO"));
	ctrlValidTo->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_TO"));

	date = wxDateTime::Now();
	if (m_DateTo)
		date = m_Title->GetValidTo().GetDate();
	m_ctrlDateTo = new wxDatePickerCtrl(this, wxID_ANY, date,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));

	wxStaticText* textDisplay = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_STYLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDisplay->Wrap(-1);

	m_ctrlStyle = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, NULL, 0); 
	m_ctrlStyle->SetHelpText(_("HIDC_CONFIG_TITLE_STYLE"));
	m_ctrlStyle->SetToolTip(_("HIDC_CONFIG_TITLE_STYLE"));
	static struct
	{
		wxChar const* idRes;
		ARBTitleStyle style;
	} styles[] = {
		{wxT("IDS_CONFIG_TITLE_NUMBER"), eTitleNumber},
		{wxT("IDS_CONFIG_TITLE_ROMAN"), eTitleRoman},
	};
	static int nStyles = sizeof(styles) / sizeof(styles[0]);
	for (int n = 0; n < nStyles; ++n)
	{
		wxString str = wxGetTranslation(styles[n].idRes);
		int idx = m_ctrlStyle->Append(str);
		m_ctrlStyle->SetClientData(idx, reinterpret_cast<void*>(styles[n].style));
		if (m_Title->GetMultipleStyle() == styles[n].style)
			m_ctrlStyle->SetSelection(n);
	}
	if (0 > m_ctrlStyle->GetSelection())
		m_ctrlStyle->SetSelection(0);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 0, wxALL, 5);
	sizerName->Add(ctrlPrefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 5);
	bSizer->Add(textLong, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlLongName, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(textDesc, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerDetails = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerMultiple = new wxBoxSizer(wxVERTICAL);
	sizerMultiple->Add(ctrlAllowMany, 0, wxALL, 5);

	wxBoxSizer* sizerStart = new wxBoxSizer(wxHORIZONTAL);
	sizerStart->Add(15, 0, 0, 0, 5);
	sizerStart->Add(textStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStart->Add(m_ctrlMultiple, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

	sizerMultiple->Add(sizerStart, 0, wxTOP, 2);

	sizerDetails->Add(sizerMultiple, 0, wxEXPAND, 5);

	wxFlexGridSizer* sizerDates;
	sizerDates = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates->Add(ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
	sizerDates->Add(m_ctrlDateFrom, 0, wxALL, 3);
	sizerDates->Add(ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
	sizerDates->Add(m_ctrlDateTo, 0, wxALL, 3);

	sizerDetails->Add(sizerDates, 0, wxEXPAND, 5);

	bSizer->Add(sizerDetails, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerDisplay = new wxBoxSizer(wxHORIZONTAL);
	sizerDisplay->Add(15, 0, 0, 0, 5);
	sizerDisplay->Add(textDisplay, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
	sizerDisplay->Add(m_ctrlStyle, 0, wxLEFT|wxRIGHT, 5);

	bSizer->Add(sizerDisplay, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateButtons();
}


void CDlgConfigTitle::UpdateButtons()
{
	m_ctrlDateFrom->Enable(m_DateFrom);
	m_ctrlDateTo->Enable(m_DateTo);
	m_ctrlStyle->Enable(m_ctrlMultiple->IsEnabled());
}


void CDlgConfigTitle::OnAllowMultiple(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	if (m_AllowMany && 1 > m_Multiple)
	{
		m_Multiple = 2;
		TransferDataToWindow();
	}
	m_ctrlMultiple->Enable(m_AllowMany);
	UpdateButtons();
}


void CDlgConfigTitle::OnCheck(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateButtons();
}


void CDlgConfigTitle::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (!m_AllowMany && 0 != m_Multiple)
	{
		m_Multiple = 0;
	}

	m_Title->SetName(m_Name);
	m_Title->SetLongName(m_LongName);
	m_Title->SetDescription(m_Desc);
	m_Title->SetPrefix(m_Prefix);
	m_Title->SetMultiple(m_Multiple);
	if (0 <= m_ctrlStyle->GetSelection())
		m_Title->SetMultipleStyle(
			static_cast<ARBTitleStyle>(
				reinterpret_cast<int>(
					m_ctrlStyle->GetClientData(m_ctrlStyle->GetSelection()))));
	ARBDate date;
	if (m_DateFrom)
	{
		wxDateTime d = m_ctrlDateFrom->GetValue();
		date = ARBDate(d.GetYear(), d.GetMonth() + 1, d.GetDay());
	}
	else
		date.clear();
	m_Title->SetValidFrom(date);
	if (m_DateTo)
	{
		wxDateTime d = m_ctrlDateTo->GetValue();
		date = ARBDate(d.GetYear(), d.GetMonth() + 1, d.GetDay());
	}
	else
		date.clear();
	m_Title->SetValidTo(date);

	EndDialog(wxID_OK);
}
