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
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgConfigTitlePoints::CDlgConfigTitlePoints(
		ARBConfigVenuePtr inVenue,
		double inValue, // Faults or Place
		double inPoints,
		ETitlePointType inType,
		ARBPointsType inTypeNormal,
		wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_Type(inType)
	, m_TypeNormal(inTypeNormal)
	, m_textValue(nullptr)
	, m_ctrlValue(nullptr)
	, m_textPoints(nullptr)
	, m_ctrlPoints(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlTypeNormal(nullptr)
	, m_textLifetimeName(nullptr)
	, m_ctrlLifetimeName(nullptr)
	, m_Faults(inValue)
	, m_Place(static_cast<short>(inValue))
	, m_Points(inPoints)
	, m_LifetimeName()
{
	Init(pParent);
}


CDlgConfigTitlePoints::CDlgConfigTitlePoints(
		ARBConfigVenuePtr inVenue,
		wxString const& inLifetimeName,
		wxWindow* pParent)
	: wxDialog()
	, m_Venue(inVenue)
	, m_Type(eTitleLifetime)
	, m_TypeNormal(ePointsTypeNormal)
	, m_textValue(nullptr)
	, m_ctrlValue(nullptr)
	, m_textPoints(nullptr)
	, m_ctrlPoints(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlTypeNormal(nullptr)
	, m_textLifetimeName(nullptr)
	, m_ctrlLifetimeName(nullptr)
	, m_Faults(0)
	, m_Place(0)
	, m_Points(0)
	, m_LifetimeName(inLifetimeName)
{
	Init(pParent);
}


void CDlgConfigTitlePoints::Init(wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_TITLE_POINTS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxString caption = (eTitlePlacement == m_Type) ? _("IDC_CONFIG_TITLEPTS_VALUE_TEXT2") : _("IDC_CONFIG_TITLEPTS_VALUE_TEXT1");
	m_textValue = new wxStaticText(this, wxID_ANY,
		caption,
		wxDefaultPosition, wxDefaultSize, 0);
	m_textValue->Wrap(-1);

	m_ctrlValue = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0);
	if (eTitlePlacement == m_Type)
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
	else
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
	m_ctrlValue->SetHelpText(_("HIDC_CONFIG_TITLEPTS_VALUE"));
	m_ctrlValue->SetToolTip(_("HIDC_CONFIG_TITLEPTS_VALUE"));
	if (eTitleNormal == m_Type && ePointsTypeNormal != m_TypeNormal)
	{
		m_textValue->Hide();
		m_ctrlValue->Hide();
	}

	m_textPoints = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_TITLEPTS_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textPoints->Wrap(-1);

	m_ctrlPoints = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Points));
	m_ctrlPoints->SetHelpText(_("HIDC_CONFIG_TITLEPTS_POINTS"));
	m_ctrlPoints->SetToolTip(_("HIDC_CONFIG_TITLEPTS_POINTS"));
	if (eTitleNormal == m_Type && ePointsTypeNormal != m_TypeNormal)
	{
		m_textPoints->Hide();
		m_ctrlPoints->Hide();
	}

	wxArrayString types;
	types.Add(_("IDS_TITLEPOINT_NORMAL"));
#pragma PRAGMA_TODO(lifetime points)
#if 0
	if (m_Venue->HasLifetimeName())
		types.Add(StringUtil::stringWX(m_Venue->GetLifetimeName()));
	else
#endif
	types.Add(_("IDS_TITLEPOINT_LIFETIME"));
	types.Add(_("IDS_TITLEPOINT_PLACEMENT"));
	assert(types.size() == eTitleMax);
	m_ctrlType = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, types, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlType, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler, CDlgConfigTitlePoints::OnSelchangeTitlePoints);
	m_ctrlType->SetHelpText(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetToolTip(_("HIDC_CONFIG_TITLEPTS_TITLE_POINTS"));
	m_ctrlType->SetSelection(m_Type);

	wxArrayString typesNorm;
	typesNorm.Add(_("IDS_TITLEPOINT_NORMAL_NORMAL"));
	typesNorm.Add(_("IDS_TITLEPOINT_NORMAL_T2B"));
	typesNorm.Add(_("IDS_TITLEPOINT_NORMAL_UKI"));
	assert(typesNorm.size() == ePointsTypeMax);
	m_ctrlTypeNormal = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, typesNorm, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlTypeNormal, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler, CDlgConfigTitlePoints::OnSelchangeTitlePoints);
	m_ctrlTypeNormal->SetHelpText(_("HIDC_CONFIG_TITLEPTS_CALC"));
	m_ctrlTypeNormal->SetToolTip(_("HIDC_CONFIG_TITLEPTS_CALC"));
	m_ctrlTypeNormal->SetSelection(m_TypeNormal);
	if (eTitleNormal != m_Type)
		m_ctrlTypeNormal->Hide();

#pragma PRAGMA_TODO(lifetime name)
	m_textLifetimeName = new wxStaticText(this, wxID_ANY,
		_("Lifetime Name (todo)"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textLifetimeName->Wrap(-1);

	m_ctrlLifetimeName = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_LifetimeName, TRIMVALIDATOR_TRIM_BOTH));
#pragma PRAGMA_TODO(lifetime name)
	// Should validator have 3rd parameter for error prompt?
	m_ctrlLifetimeName->SetHelpText(_("HIDC_CONFIG_TITLEPTS_LIFETIMENAME"));
	m_ctrlLifetimeName->SetToolTip(_("HIDC_CONFIG_TITLEPTS_LIFETIMENAME"));
	if (eTitleLifetime != m_Type)
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
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, sz);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgConfigTitlePoints, m_ctrlValue)
}


DEFINE_ON_INIT(CDlgConfigTitlePoints)


void CDlgConfigTitlePoints::OnSelchangeTitlePoints(wxCommandEvent& evt)
{
	int oldType = m_Type;
	m_Type = m_ctrlType->GetSelection();
	m_TypeNormal = m_ctrlTypeNormal->GetSelection();
	TransferDataFromWindow();
	bool bRefit = false;

	if (eTitleNormal != m_Type || ePointsTypeNormal == m_TypeNormal)
	{
		bRefit = true;
		m_textValue->Show();
		m_ctrlValue->Show();
		m_textPoints->Show();
		m_ctrlPoints->Show();
		if (eTitleLifetime == m_Type)
		{
			m_textLifetimeName->Show();
			m_ctrlLifetimeName->Show();
		}
		else
		{
			m_textLifetimeName->Hide();
			m_ctrlLifetimeName->Hide();
		}
	}
	else if (eTitleNormal == m_Type && ePointsTypeNormal != m_TypeNormal)
	{
		bRefit = true;
		m_textValue->Hide();
		m_ctrlValue->Hide();
		m_textPoints->Hide();
		m_ctrlPoints->Hide();
		m_textLifetimeName->Hide();
		m_ctrlLifetimeName->Hide();
	}

	if (eTitleNormal == m_Type)
	{
		m_ctrlTypeNormal->Show();
		bRefit = true;
	}
	else if (eTitleNormal == oldType)
	{
		m_ctrlTypeNormal->Hide();
		bRefit = true;
	}
	if (eTitlePlacement == m_Type)
	{
		m_Place = static_cast<short>(m_Faults); // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT2"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Place));
		bRefit = true;
	}
	else if (eTitlePlacement == oldType)
	{
		m_Faults = m_Place; // Copy old value
		m_textValue->SetLabel(_("IDC_CONFIG_TITLEPTS_VALUE_TEXT1"));
		m_ctrlValue->SetValidator(CGenericValidator(&m_Faults));
		bRefit = true;
	}
	if (bRefit)
	{
		TransferDataToWindow();
		GetSizer()->Fit(this);
	}
}
