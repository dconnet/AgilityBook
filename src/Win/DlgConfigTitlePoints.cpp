/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 * 2016-06-14 Add support for named/speedpt lifetime points.
 * 2016-01-14 Fix initial control layout.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-10-06 Removed ARB classes so it could be used to lifetime pts.
 */

#include "stdafx.h"
#include "DlgConfigTitlePoints.h"

#include "AgilityBook.h"

#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxBEGIN_EVENT_TABLE(CDlgConfigTitlePoints, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigTitlePoints::OnOk)
wxEND_EVENT_TABLE()


CDlgConfigTitlePoints::CDlgConfigTitlePoints(
	ARBConfigVenuePtr const& inVenue,
	double inValue, // Faults or Place
	double inPoints,
	ARBTitlePointType inType,
	ARBPointsType inTypeNormal,
	wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_Type(inType)
	, m_TypeNormal(inTypeNormal)
	, m_textValue(nullptr)
	, m_ctrlValue(nullptr)
	, m_ctrlSpeedPts(nullptr)
	, m_textPoints(nullptr)
	, m_ctrlPoints(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlTypeNormal(nullptr)
	, m_textLifetimeName(nullptr)
	, m_ctrlLifetimeName(nullptr)
	, m_Faults(inValue)
	, m_Place(static_cast<short>(inValue))
	, m_bSpeed(false)
	, m_Points(inPoints)
	, m_LifetimeName()
{
	InitDlg(pParent);
}


CDlgConfigTitlePoints::CDlgConfigTitlePoints(
	ARBConfigVenuePtr const& inVenue,
	ARBConfigLifetimePointsPtr const& inLifetime,
	wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_Type(ARBTitlePointType::Lifetime)
	, m_TypeNormal(ARBPointsType::Normal)
	, m_textValue(nullptr)
	, m_ctrlValue(nullptr)
	, m_ctrlSpeedPts(nullptr)
	, m_textPoints(nullptr)
	, m_ctrlPoints(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlTypeNormal(nullptr)
	, m_textLifetimeName(nullptr)
	, m_ctrlLifetimeName(nullptr)
	, m_Faults(inLifetime->GetFaults())
	, m_Place(0)
	, m_bSpeed(inLifetime->UseSpeedPts())
	, m_Points(inLifetime->GetPoints())
	, m_LifetimeName(inLifetime->GetName())
{
	InitDlg(pParent);
}


void CDlgConfigTitlePoints::InitDlg(wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_TITLE_POINTS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxString caption = (ARBTitlePointType::Placement == m_Type) ? _("IDC_CONFIG_TITLEPTS_VALUE_TEXT2")
																: _("IDC_CONFIG_TITLEPTS_VALUE_TEXT1");
	m_textValue = new wxStaticText(this, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, 0);
	m_textValue->Wrap(-1);

	m_ctrlValue
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 20), -1), 0);
	if (ARBTitlePointType::Placement == m_Type)
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
	else
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
	m_ctrlValue->SetHelpText(_("HIDC_CONFIG_TITLEPTS_VALUE"));
	m_ctrlValue->SetToolTip(_("HIDC_CONFIG_TITLEPTS_VALUE"));

	m_ctrlSpeedPts = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_TITLEPTS_SPEEDPTS"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bSpeed));
	m_ctrlSpeedPts->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigTitlePoints::OnUseSpeedPoints, this);
	m_ctrlSpeedPts->SetHelpText(_("HIDC_CONFIG_TITLEPTS_SPEEDPTS"));
	m_ctrlSpeedPts->SetToolTip(_("HIDC_CONFIG_TITLEPTS_SPEEDPTS"));

	m_textPoints
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLEPTS_POINTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textPoints->Wrap(-1);

	m_ctrlPoints = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_Points));
	m_ctrlPoints->SetHelpText(_("HIDC_CONFIG_TITLEPTS_POINTS"));
	m_ctrlPoints->SetToolTip(_("HIDC_CONFIG_TITLEPTS_POINTS"));

	wxArrayString types;
	types.Add(_("IDS_TITLEPOINT_NORMAL"));
	types.Add(_("IDS_TITLEPOINT_LIFETIME"));
	types.Add(_("IDS_TITLEPOINT_PLACEMENT"));
	assert(types.size() == static_cast<size_t>(ARBTitlePointType::Max));
	m_ctrlType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, types, 0);
	m_ctrlType->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &CDlgConfigTitlePoints::OnSelchangeTitlePoints, this);
	m_ctrlType->SetHelpText(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetToolTip(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetSelection(static_cast<int>(m_Type));

	wxArrayString typesNorm;
	struct
	{
		wxString text;
		ARBPointsType type;
	} items[] = {
		{_("IDS_TITLEPOINT_NORMAL_NORMAL"), ARBPointsType::Normal},
		{_("IDS_TITLEPOINT_NORMAL_T2B"), ARBPointsType::T2B},
		{_("IDS_TITLEPOINT_NORMAL_UKI"), ARBPointsType::UKI},
	};
	m_ctrlTypeNormal = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	for (auto const& item : items)
	{
		auto type = m_ctrlTypeNormal->Append(item.text);
		m_ctrlTypeNormal->SetClientData(type, reinterpret_cast<void*>(static_cast<size_t>(item.type)));
		if (m_TypeNormal == item.type)
			m_ctrlTypeNormal->SetSelection(type);
	}
	m_ctrlTypeNormal->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &CDlgConfigTitlePoints::OnSelchangeTitlePoints, this);
	m_ctrlTypeNormal->SetHelpText(_("HIDC_CONFIG_TITLEPTS_CALC"));
	m_ctrlTypeNormal->SetToolTip(_("HIDC_CONFIG_TITLEPTS_CALC"));
	m_ctrlTypeNormal->SetSelection(static_cast<int>(m_TypeNormal));
	if (ARBTitlePointType::Normal != m_Type)
		m_ctrlTypeNormal->Hide();

	m_textLifetimeName
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_TITLEPTS_LIFETIMENAME"), wxDefaultPosition, wxDefaultSize, 0);
	m_textLifetimeName->Wrap(-1);

	m_ctrlLifetimeName = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT,
		CTrimValidator(&m_LifetimeName, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlLifetimeName->SetHelpText(_("HIDC_CONFIG_TITLEPTS_LIFETIMENAME"));
	m_ctrlLifetimeName->SetToolTip(_("HIDC_CONFIG_TITLEPTS_LIFETIMENAME"));

	for (ARBConfigLifetimeNameList::iterator iter = m_Venue->GetLifetimeNames().begin();
		 iter != m_Venue->GetLifetimeNames().end();
		 ++iter)
	{
		wxString str = StringUtil::stringWX((*iter)->GetName());
		bool bDefault = false;
		if (str.empty())
		{
			bDefault = true;
			str = _("IDS_TITLEPOINT_LIFETIME_NAME");
		}
		int index = m_ctrlLifetimeName->Append(str);
		m_ctrlLifetimeName->SetClientData(index, reinterpret_cast<void*>(static_cast<uintptr_t>(bDefault ? 1 : 0)));
		if (str == m_LifetimeName || (m_LifetimeName.empty() && bDefault))
		{
			m_ctrlLifetimeName->SetSelection(index);
		}
	}
	if (0 == m_ctrlLifetimeName->GetCount())
	{
		wxString str = _("IDS_TITLEPOINT_LIFETIME_NAME");
		int index = m_ctrlLifetimeName->Append(str);
		m_ctrlLifetimeName->SetClientData(index, reinterpret_cast<void*>(1));
		m_ctrlLifetimeName->SetSelection(index);
	}
	if (1 == m_ctrlLifetimeName->GetCount())
		m_ctrlLifetimeName->SetSelection(0);

	if (ARBTitlePointType::Lifetime != m_Type)
	{
		m_textLifetimeName->Hide();
		m_ctrlLifetimeName->Hide();
	}

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerValue = new wxBoxSizer(wxHORIZONTAL);
	sizerValue->Add(m_textValue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerValue->Add(m_ctrlValue, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerValue, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(m_ctrlSpeedPts, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_textPoints, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_ctrlPoints, 1, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer* sizerLTName = new wxBoxSizer(wxHORIZONTAL);
	sizerLTName->Add(m_textLifetimeName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerLTName->Add(m_ctrlLifetimeName, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerPoints, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlType, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlTypeNormal, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(sizerLTName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);

	UpdateControls(static_cast<ARBTitlePointType>(-1));

	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, sz);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgConfigTitlePoints, m_ctrlValue)
}


bool CDlgConfigTitlePoints::UpdateControls(ARBTitlePointType oldType)
{
	bool bRefit = false;
	if (ARBTitlePointType::Normal != m_Type || ARBPointsType::Normal == m_TypeNormal)
	{
		bRefit = true;
		m_textValue->Show();
		m_ctrlValue->Show();
		m_textPoints->Show();
		m_ctrlPoints->Show();
		m_ctrlPoints->Enable(!m_bSpeed);
		if (ARBTitlePointType::Lifetime == m_Type)
		{
			m_ctrlSpeedPts->Show();
			m_textLifetimeName->Show();
			m_ctrlLifetimeName->Show();
		}
		else
		{
			m_ctrlSpeedPts->Hide();
			m_textLifetimeName->Hide();
			m_ctrlLifetimeName->Hide();
		}
	}
	else if (ARBTitlePointType::Normal == m_Type && ARBPointsType::Normal != m_TypeNormal)
	{
		bRefit = true;
		m_textValue->Hide();
		m_ctrlValue->Hide();
		m_ctrlSpeedPts->Hide();
		m_textPoints->Hide();
		m_ctrlPoints->Hide();
		m_textLifetimeName->Hide();
		m_ctrlLifetimeName->Hide();
	}

	if (ARBTitlePointType::Normal == m_Type)
	{
		m_ctrlTypeNormal->Show();
		bRefit = true;
	}
	else if (ARBTitlePointType::Normal == oldType)
	{
		m_ctrlTypeNormal->Hide();
		bRefit = true;
	}
	if (ARBTitlePointType::Placement == m_Type)
	{
		m_Place = static_cast<short>(m_Faults); // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT2"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
		bRefit = true;
	}
	else if (ARBTitlePointType::Placement == oldType)
	{
		m_Faults = m_Place; // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT1"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
		bRefit = true;
	}
	return bRefit;
}


DEFINE_ON_INIT(CDlgConfigTitlePoints)


void CDlgConfigTitlePoints::OnUseSpeedPoints(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlPoints->Enable(!m_bSpeed);
}


void CDlgConfigTitlePoints::OnSelchangeTitlePoints(wxCommandEvent& evt)
{
	ARBTitlePointType oldType = m_Type;
	m_Type = static_cast<ARBTitlePointType>(m_ctrlType->GetSelection());
	auto sel = m_ctrlTypeNormal->GetSelection();
	if (0 <= sel)
	{
		m_TypeNormal = static_cast<ARBPointsType>(reinterpret_cast<size_t>(m_ctrlTypeNormal->GetClientData(sel)));
	}
	TransferDataFromWindow();

	if (UpdateControls(oldType))
	{
		TransferDataToWindow();
		Layout();
		GetSizer()->Fit(this);
	}
}


void CDlgConfigTitlePoints::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (ARBTitlePointType::Lifetime == m_Type)
	{
		int index = m_ctrlLifetimeName->GetSelection();
		void* isDefault = m_ctrlLifetimeName->GetClientData(index);
		if (isDefault)
			m_LifetimeName.clear();
	}

	EndDialog(wxID_OK);
}
