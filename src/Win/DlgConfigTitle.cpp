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
#include "DlgConfigTitle.h"

#include "AgilityBook.h"

#include "ARB/ARBConfigTitle.h"
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

class CMultipleValidator : public CGenericValidator
{
public:
	CMultipleValidator(wxCheckBox* ctrlAllowMany, short* val)
		: CGenericValidator(val)
		, m_ctrlAllowMany(ctrlAllowMany)
	{
	}
	bool Validate(wxWindow* parent) override;

private:
	wxCheckBox* m_ctrlAllowMany;
};


bool CMultipleValidator::Validate(wxWindow* parent)
{
	if (!CGenericValidator::Validate(parent))
		return false;
	CTextCtrl* pControl = dynamic_cast<CTextCtrl*>(m_validatorWindow);
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

CDlgConfigTitle::CDlgConfigTitle(ARBConfigTitlePtr const& inTitle, wxWindow* pParent)
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
	, m_trace("CDlgConfigTitle")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_CONFIG_TITLE"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textName
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	CTextCtrl* ctrlName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(50, -1)),
		0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	ctrlName->SetHelpText(_("HIDC_CONFIG_TITLE_NAME"));
	ctrlName->SetToolTip(_("HIDC_CONFIG_TITLE_NAME"));

	wxCheckBox* ctrlPrefix = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLE_PREFIX"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_Prefix));
	ctrlPrefix->SetHelpText(_("HIDC_CONFIG_TITLE_PREFIX"));
	ctrlPrefix->SetToolTip(_("HIDC_CONFIG_TITLE_PREFIX"));

	wxStaticText* textLong
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_LONG_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textLong->Wrap(-1);

	CTextCtrl* ctrlLongName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_LongName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlLongName->SetHelpText(_("HIDC_CONFIG_TITLE_LONG_NAME"));
	ctrlLongName->SetToolTip(_("HIDC_CONFIG_TITLE_LONG_NAME"));

	wxStaticText* textDesc
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_DESC"), wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 60)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Desc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_CONFIG_TITLE_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_CONFIG_TITLE_DESC"));

	wxCheckBox* ctrlAllowMany = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLE_ALLOW_MULTIPLE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_AllowMany));
	ctrlAllowMany->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigTitle::OnAllowMultiple, this);
	ctrlAllowMany->SetHelpText(_("HIDC_CONFIG_TITLE_ALLOW_MULTIPLE"));
	ctrlAllowMany->SetToolTip(_("HIDC_CONFIG_TITLE_ALLOW_MULTIPLE"));

	m_ctrlShowFirst = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLE_SHOW_FIRST"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bShowFirst));
	m_ctrlShowFirst->SetHelpText(_("HIDC_CONFIG_TITLE_SHOW_FIRST"));
	m_ctrlShowFirst->SetToolTip(_("HIDC_CONFIG_TITLE_SHOW_FIRST"));

	wxStaticText* textStart
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_STARTAT"), wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	m_ctrlMultipleStartAt = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CMultipleValidator(ctrlAllowMany, &m_MultipleStartAt));
	m_ctrlMultipleStartAt->SetHelpText(_("HIDC_CONFIG_TITLE_MULTIPLE_STARTAT"));
	m_ctrlMultipleStartAt->SetToolTip(_("HIDC_CONFIG_TITLE_MULTIPLE_STARTAT"));

	wxStaticText* textInc
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_INCREMENT"), wxDefaultPosition, wxDefaultSize, 0);
	textInc->Wrap(-1);

	m_ctrlMultipleInc = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_MultipleInc));
	m_ctrlMultipleInc->SetHelpText(_("HIDC_CONFIG_TITLE_MULTIPLE_INC"));
	m_ctrlMultipleInc->SetToolTip(_("HIDC_CONFIG_TITLE_MULTIPLE_INC"));

	wxStaticText* textDisplay
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_STYLE"), wxDefaultPosition, wxDefaultSize, 0);
	textDisplay->Wrap(-1);

	m_ctrlStyle = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlStyle->SetHelpText(_("HIDC_CONFIG_TITLE_STYLE"));
	m_ctrlStyle->SetToolTip(_("HIDC_CONFIG_TITLE_STYLE"));
	constexpr struct
	{
		wchar_t const* idRes;
		ARBTitleStyle style;
	} styles[] = {
		{arbT("IDS_CONFIG_TITLE_NONE"), ARBTitleStyle::None},
		{arbT("IDS_CONFIG_TITLE_NUMBER"), ARBTitleStyle::Number},
		{arbT("IDS_CONFIG_TITLE_ROMAN"), ARBTitleStyle::Roman},
	};
	int idxCur = 0; // Default to first item.
	wxArrayString items;
	std::vector<void*> data;
	for (auto const& style : styles)
	{
		items.Add(wxGetTranslation(style.idRes));
		data.push_back(reinterpret_cast<void*>(style.style));
		if (m_Title->GetMultipleStyle() == style.style)
			idxCur = static_cast<int>(items.size()) - 1;
	}
	if (!items.empty())
	{
		m_ctrlStyle->Append(items, data.data());
		m_ctrlStyle->SetSelection(idxCur);
	}

	wxStaticText* textSep
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLE_SEP"), wxDefaultPosition, wxDefaultSize, 0);
	textSep->Wrap(-1);

	m_ctrlSep = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlSep->SetHelpText(_("HIDC_CONFIG_TITLE_SEP"));
	m_ctrlSep->SetToolTip(_("HIDC_CONFIG_TITLE_SEP"));
	constexpr struct
	{
		wchar_t const* idRes;
		ARBTitleSeparator sep;
	} seps[] = {
		{arbT("IDS_CONFIG_TITLE_SEP_NONE"), ARBTitleSeparator::None},
		{arbT("IDS_CONFIG_TITLE_SEP_SPACE"), ARBTitleSeparator::Space},
		{arbT("IDS_CONFIG_TITLE_SEP_HYPHEN"), ARBTitleSeparator::Hyphen},
	};
	idxCur = 0; // Default to first item.
	items.clear();
	data.clear();
	for (auto const& sep : seps)
	{
		items.Add(wxGetTranslation(sep.idRes));
		data.push_back(reinterpret_cast<void*>(sep.sep));
		if (m_Title->GetMultipleStyleSeparator() == sep.sep)
			idxCur = static_cast<int>(items.size()) - 1;
	}
	if (!items.empty())
	{
		m_ctrlSep->Append(items, data.data());
		m_ctrlSep->SetSelection(idxCur);
	}

	wxCheckBox* ctrlValidFrom = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLE_VALID_FROM"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT,
		wxGenericValidator(&m_DateFrom));
	ctrlValidFrom->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigTitle::OnCheck, this);
	ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_FROM"));
	ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_FROM"));

	wxDateTime date(wxDateTime::Now());
	if (m_DateFrom)
		m_Title->GetValidFrom().GetDate(date);
	m_ctrlDateFrom = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY);
	m_ctrlDateFrom->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_FROM_DATE"));
	m_ctrlDateFrom->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_FROM_DATE"));

	wxCheckBox* ctrlValidTo = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLE_VALID_TO"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT,
		wxGenericValidator(&m_DateTo));
	ctrlValidTo->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigTitle::OnCheck, this);
	ctrlValidTo->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_TO"));
	ctrlValidTo->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_TO"));

	date = wxDateTime::Now();
	if (m_DateTo)
		m_Title->GetValidTo().GetDate(date);
	m_ctrlDateTo = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY);
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_TITLE_VALID_TO_DATE"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerName->Add(ctrlName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerName->Add(ctrlPrefix, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(textLong, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlLongName, 0, wxEXPAND | wxLEFT | wxRIGHT, padding.Controls());
	bSizer->Add(textDesc, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlDesc, 1, wxEXPAND | wxLEFT | wxRIGHT, padding.Controls());

	wxBoxSizer* sizerMultiple = new wxBoxSizer(wxVERTICAL);
	sizerMultiple->Add(ctrlAllowMany, 0, wxBOTTOM, padding.TightControls());

	wxBoxSizer* sizerShowFirst = new wxBoxSizer(wxHORIZONTAL);
	sizerShowFirst->AddSpacer(padding.CheckboxOffset());

	wxBoxSizer* sizerShowMulti = new wxBoxSizer(wxVERTICAL);
	sizerShowMulti->Add(m_ctrlShowFirst, 0, wxBOTTOM, padding.TightControls());

	wxFlexGridSizer* sizerMultiInfo
		= new wxFlexGridSizer(4, 2, padding.TightControls(), padding.Inner()); // rows/cols/vgap/hgap
	sizerMultiInfo->SetFlexibleDirection(wxBOTH);
	sizerMultiInfo->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerMultiInfo->Add(textStart, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerMultiInfo->Add(m_ctrlMultipleStartAt, 0, wxALIGN_CENTER_VERTICAL);
	sizerMultiInfo->Add(textInc, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerMultiInfo->Add(m_ctrlMultipleInc, 0, wxALIGN_CENTER_VERTICAL);
	sizerMultiInfo->Add(textDisplay, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerMultiInfo->Add(m_ctrlStyle, 0, wxALIGN_CENTER_VERTICAL);
	sizerMultiInfo->Add(textSep, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerMultiInfo->Add(m_ctrlSep, 0, wxALIGN_CENTER_VERTICAL);
	sizerShowMulti->Add(sizerMultiInfo);
	sizerShowFirst->Add(sizerShowMulti);
	sizerMultiple->Add(sizerShowFirst);

	wxBoxSizer* sizerDetails = new wxBoxSizer(wxHORIZONTAL);
	sizerDetails->Add(sizerMultiple);
	sizerDetails->AddStretchSpacer();

	wxFlexGridSizer* sizerDates = new wxFlexGridSizer(2, 2, padding.Controls(), padding.Inner());
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates->Add(ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateFrom, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates->Add(ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateTo, 0, wxALIGN_CENTER_VERTICAL);
	sizerDetails->Add(sizerDates);

	bSizer->Add(sizerDetails, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigTitle::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateButtons();

	IMPLEMENT_ON_INIT(CDlgConfigTitle, ctrlName)
}


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
					reinterpret_cast<size_t>(m_ctrlStyle->GetClientData(m_ctrlStyle->GetSelection()))),
				static_cast<ARBTitleSeparator>(
					reinterpret_cast<size_t>(m_ctrlSep->GetClientData(m_ctrlSep->GetSelection()))));
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

} // namespace dconSoft
