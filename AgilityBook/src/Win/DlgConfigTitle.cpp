/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
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
#include "DlgConfigTitle.h"

#include "AgilityBook.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfigTitle.h"
#include "ARBCommon/StringUtil.h"
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
		pControl->SetValue(L"1");
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
	, m_Name(StringUtil::stringWX(inTitle->GetName()))
	, m_Prefix(inTitle->GetPrefix())
	, m_LongName(StringUtil::stringWX(inTitle->GetLongName()))
	, m_Desc(StringUtil::stringWX(inTitle->GetDescription()))

	, m_AllowMany(inTitle->IsRecurring())
	, m_MultipleStartAt(inTitle->GetMultipleStartAt())
	, m_MultipleInc(inTitle->GetMultipleIncrement())
	, m_bShowFirst(inTitle->ShowMultipleOnFirstInstance())

	, m_DateFrom(inTitle->GetValidFrom().IsValid())
	, m_DateTo(inTitle->GetValidTo().IsValid())
	, m_ctrlMultipleStartAt(nullptr)
	, m_ctrlMultipleInc(nullptr)
	, m_ctrlDateFrom(nullptr)
	, m_ctrlDateTo(nullptr)
	, m_ctrlStyle(nullptr)
	, m_ctrlSep(nullptr)
{
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
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
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

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
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

	m_ctrlShowFirst = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_SHOW_FIRST"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bShowFirst));
	m_ctrlShowFirst->SetHelpText(_("HIDC_CONFIG_TITLE_SHOW_FIRST"));
	m_ctrlShowFirst->SetToolTip(_("HIDC_CONFIG_TITLE_SHOW_FIRST"));

	wxStaticText* textStart = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_STARTAT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	m_ctrlMultipleStartAt = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CMultipleValidator(ctrlAllowMany, &m_MultipleStartAt));
	m_ctrlMultipleStartAt->SetHelpText(_("HIDC_CONFIG_TITLE_MULTIPLE_STARTAT"));
	m_ctrlMultipleStartAt->SetToolTip(_("HIDC_CONFIG_TITLE_MULTIPLE_STARTAT"));

	wxStaticText* textInc = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_INCREMENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textInc->Wrap(-1);

	m_ctrlMultipleInc = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_MultipleInc));
	m_ctrlMultipleInc->SetHelpText(_("HIDC_CONFIG_TITLE_MULTIPLE_INC"));
	m_ctrlMultipleInc->SetToolTip(_("HIDC_CONFIG_TITLE_MULTIPLE_INC"));

	wxStaticText* textDisplay = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_STYLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDisplay->Wrap(-1);

	m_ctrlStyle = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlStyle->SetHelpText(_("HIDC_CONFIG_TITLE_STYLE"));
	m_ctrlStyle->SetToolTip(_("HIDC_CONFIG_TITLE_STYLE"));
	static struct
	{
		wchar_t const* idRes;
		ARBTitleStyle style;
	} styles[] = {
		{arbT("IDS_CONFIG_TITLE_NONE"), eTitleStyleNone},
		{arbT("IDS_CONFIG_TITLE_NUMBER"), eTitleStyleNumber},
		{arbT("IDS_CONFIG_TITLE_ROMAN"), eTitleStyleRoman},
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

	wxStaticText* textSep = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_SEP"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSep->Wrap(-1);

	m_ctrlSep = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlSep->SetHelpText(_("HIDC_CONFIG_TITLE_SEP"));
	m_ctrlSep->SetToolTip(_("HIDC_CONFIG_TITLE_SEP"));
	static struct
	{
		wchar_t const* idRes;
		ARBTitleSeparator sep;
	} seps[] = {
		{arbT("IDS_CONFIG_TITLE_SEP_NONE"), eTitleSeparatorNone},
		{arbT("IDS_CONFIG_TITLE_SEP_SPACE"), eTitleSeparatorSpace},
		{arbT("IDS_CONFIG_TITLE_SEP_HYPHEN"), eTitleSeparatorHyphen},
	};
	static int nSeps = sizeof(seps) / sizeof(seps[0]);
	for (int n = 0; n < nSeps; ++n)
	{
		wxString str = wxGetTranslation(seps[n].idRes);
		int idx = m_ctrlSep->Append(str);
		m_ctrlSep->SetClientData(idx, reinterpret_cast<void*>(seps[n].sep));
		if (m_Title->GetMultipleStyleSeparator() == seps[n].sep)
			m_ctrlSep->SetSelection(n);
	}
	if (0 > m_ctrlSep->GetSelection())
		m_ctrlSep->SetSelection(0);

	wxCheckBox* ctrlValidFrom = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_TITLE_VALID_FROM"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_DateFrom));
	BIND_OR_CONNECT_CTRL(ctrlValidFrom, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgConfigTitle::OnCheck);
	ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_FROM"));
	ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_FROM"));

	wxDateTime date(wxDateTime::Now());
	if (m_DateFrom)
		m_Title->GetValidFrom().GetDate(date);
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
		m_Title->GetValidTo().GetDate(date);
	m_ctrlDateTo = new wxDatePickerCtrl(this, wxID_ANY, date,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 0, wxALL, 5);
	sizerName->Add(ctrlPrefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 0);
	bSizer->Add(textLong, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlLongName, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
	bSizer->Add(textDesc, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlDesc, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

	wxBoxSizer* sizerMultiple = new wxBoxSizer(wxVERTICAL);
	sizerMultiple->Add(ctrlAllowMany, 0, wxBOTTOM, 2);

	wxBoxSizer* sizerShowFirst = new wxBoxSizer(wxHORIZONTAL);
	sizerShowFirst->Add(15, 0, 0, 0, 0);
	sizerShowFirst->Add(m_ctrlShowFirst, 0, 0, 0);
	sizerMultiple->Add(sizerShowFirst, 0, wxTOP, 2);

	wxFlexGridSizer* sizerMultiInfo = new wxFlexGridSizer(4, 3, 0, 0);
	sizerMultiInfo->SetFlexibleDirection(wxBOTH);
	sizerMultiInfo->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerMultiInfo->Add(15, 0, 0, 0, 0);
	sizerMultiInfo->Add(textStart, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
	sizerMultiInfo->Add(m_ctrlMultipleStartAt, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
	sizerMultiInfo->Add(15, 0, 0, 0, 0);
	sizerMultiInfo->Add(textInc, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
	sizerMultiInfo->Add(m_ctrlMultipleInc, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
	sizerMultiInfo->Add(15, 0, 0, 0, 0);
	sizerMultiInfo->Add(textDisplay, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
	sizerMultiInfo->Add(m_ctrlStyle, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
	sizerMultiInfo->Add(15, 0, 0, 0, 0);
	sizerMultiInfo->Add(textSep, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
	sizerMultiInfo->Add(m_ctrlSep, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 2);
	sizerMultiple->Add(sizerMultiInfo, 0, wxTOP, 2);

	wxBoxSizer* sizerDetails = new wxBoxSizer(wxHORIZONTAL);
	sizerDetails->Add(sizerMultiple, 0, wxALL, 5);
	sizerDetails->Add(0, 0, 1, wxEXPAND, 0);

	wxFlexGridSizer* sizerDates = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates->Add(ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
	sizerDates->Add(m_ctrlDateFrom, 0, wxLEFT|wxBOTTOM, 3);
	sizerDates->Add(ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
	sizerDates->Add(m_ctrlDateTo, 0, wxLEFT|wxTOP, 3);

	sizerDetails->Add(sizerDates, 0, wxALL, 5);

	bSizer->Add(sizerDetails, 0, wxEXPAND, 0);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateButtons();

	IMPLEMENT_ON_INIT(CDlgConfigTitle, ctrlName)
}


DEFINE_ON_INIT(CDlgConfigTitle)


void CDlgConfigTitle::UpdateButtons()
{
	m_ctrlDateFrom->Enable(m_DateFrom);
	m_ctrlDateTo->Enable(m_DateTo);
	m_ctrlShowFirst->Enable(m_AllowMany);
	m_ctrlMultipleStartAt->Enable(m_AllowMany);
	m_ctrlMultipleInc->Enable(m_AllowMany);
	m_ctrlStyle->Enable(m_AllowMany);
	m_ctrlSep->Enable(m_AllowMany);
}


void CDlgConfigTitle::OnAllowMultiple(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	if (m_AllowMany && 1 > m_MultipleStartAt)
	{
		m_MultipleStartAt = 1;
		TransferDataToWindow();
	}
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
	if (!m_AllowMany && 0 != m_MultipleStartAt)
	{
		m_MultipleStartAt = 0;
	}

	m_Title->clear();
	m_Title->SetName(StringUtil::stringW(m_Name));
	m_Title->SetLongName(StringUtil::stringW(m_LongName));
	m_Title->SetDescription(StringUtil::stringW(m_Desc));
	m_Title->SetPrefix(m_Prefix);
	m_Title->SetMultipleStartAt(m_MultipleStartAt);
	if (0 < m_MultipleStartAt)
	{
		m_Title->SetMultipleIncrement(m_MultipleInc);
		m_Title->SetMultipleOnFirstInstance(m_bShowFirst);
		if (0 <= m_ctrlStyle->GetSelection())
		{
			m_Title->SetMultipleStyle(
				static_cast<ARBTitleStyle>(
					reinterpret_cast<int>(
						m_ctrlStyle->GetClientData(m_ctrlStyle->GetSelection()))),
				static_cast<ARBTitleSeparator>(
					reinterpret_cast<int>(
						m_ctrlSep->GetClientData(m_ctrlSep->GetSelection()))));
		}
	}
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
