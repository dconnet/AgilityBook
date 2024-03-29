/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Let user specify how to sort events within a venue.
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-07-16 Created
 */

#include "stdafx.h"
#include "DlgPointsViewSort.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"

#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgPadding.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class CPointViewData : public wxClientData
{
public:
	CPointViewData(ARBPointsViewSort item)
		: m_Item(item)
	{
	}
	ARBPointsViewSort m_Item;
};

/////////////////////////////////////////////////////////////////////////////

CDlgPointsViewSort::CDlgPointsViewSort(wxWindow* pParent)
	: wxDialog()
	, m_ctrlPrimary(nullptr)
	, m_ctrlSecondary(nullptr)
	, m_ctrlTertiary(nullptr)
	, m_ctrlOk(nullptr)
	, m_trace("CDlgPointsViewSort")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_POINTS_VIEW_SORT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	CAgilityBookOptions::GetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);

	// Controls (these are done first to control tab order)

	wxStaticText* text1
		= new wxStaticText(this, wxID_ANY, _("IDC_POINTS_VIEW_SORT_C1"), wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(-1);

	m_ctrlPrimary = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlPrimary->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgPointsViewSort::OnSelchangePrimary, this);
	m_ctrlPrimary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C1"));
	m_ctrlPrimary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C1"));

	wxStaticText* text2
		= new wxStaticText(this, wxID_ANY, _("IDC_POINTS_VIEW_SORT_C2"), wxDefaultPosition, wxDefaultSize, 0);
	text2->Wrap(-1);

	m_ctrlSecondary = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlSecondary->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgPointsViewSort::OnSelchangeSecondary, this);
	m_ctrlSecondary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C2"));
	m_ctrlSecondary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C2"));

	wxStaticText* text3
		= new wxStaticText(this, wxID_ANY, _("IDC_POINTS_VIEW_SORT_C3"), wxDefaultPosition, wxDefaultSize, 0);
	text3->Wrap(-1);

	m_ctrlTertiary = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlTertiary->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgPointsViewSort::OnSelchangeTertiary, this);
	m_ctrlTertiary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C3"));
	m_ctrlTertiary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C3"));

	FillPrimary();

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(text1, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer1->Add(m_ctrlPrimary, 1, wxEXPAND);

	bSizer->Add(sizer1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(text2, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer2->Add(m_ctrlSecondary, 1, wxEXPAND);

	bSizer->Add(sizer2, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(text3, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizer3->Add(m_ctrlTertiary, 1, wxEXPAND);

	bSizer->Add(sizer3, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	m_ctrlOk = wxDynamicCast(FindWindowInSizer(bSizer, wxID_OK), wxButton);
	assert(!!m_ctrlOk);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPointsViewSort::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, sz);
	CenterOnParent();

	m_ctrlOk->SetFocus();
	UpdateControls();
}


void CDlgPointsViewSort::UpdateControls()
{
	if (m_ctrlOk)
	{
		bool bEnable = false;
		if (wxNOT_FOUND != m_ctrlPrimary->GetSelection() && wxNOT_FOUND != m_ctrlSecondary->GetSelection()
			&& wxNOT_FOUND != m_ctrlTertiary->GetSelection())
		{
			bEnable = true;
		}
		m_ctrlOk->Enable(bEnable);
	}
}


namespace
{
int AddItem(wxComboBox* ctrl, ARBPointsViewSort item, ARBPointsViewSort inSelect)
{
	int idx = -1;
	wxString str;
	switch (item)
	{
	case ARBPointsViewSort::Unknown:
		break;
	case ARBPointsViewSort::Division:
		str = _("IDS_COL_DIVISION");
		idx = ctrl->Append(str, new CPointViewData(item));
		break;
	case ARBPointsViewSort::Level:
		str = _("IDS_COL_LEVEL");
		idx = ctrl->Append(str, new CPointViewData(item));
		break;
	case ARBPointsViewSort::Event:
		str = _("IDS_COL_EVENT");
		idx = ctrl->Append(str, new CPointViewData(item));
		break;
	}
	if (item == inSelect && 0 <= idx)
		ctrl->SetSelection(idx);
	return idx;
}
} // namespace


void CDlgPointsViewSort::FillPrimary()
{
	m_ctrlPrimary->Clear();
	AddItem(m_ctrlPrimary, ARBPointsViewSort::Division, m_Primary);
	AddItem(m_ctrlPrimary, ARBPointsViewSort::Level, m_Primary);
	AddItem(m_ctrlPrimary, ARBPointsViewSort::Event, m_Primary);
	FillSecondary();
}


void CDlgPointsViewSort::FillSecondary()
{
	m_ctrlSecondary->Clear();
	if (m_Primary != ARBPointsViewSort::Division)
		AddItem(m_ctrlSecondary, ARBPointsViewSort::Division, m_Secondary);
	if (m_Primary != ARBPointsViewSort::Level)
		AddItem(m_ctrlSecondary, ARBPointsViewSort::Level, m_Secondary);
	if (m_Primary != ARBPointsViewSort::Event)
		AddItem(m_ctrlSecondary, ARBPointsViewSort::Event, m_Secondary);
	FillTertiary();
}


void CDlgPointsViewSort::FillTertiary()
{
	m_ctrlTertiary->Clear();
	if (m_Primary != ARBPointsViewSort::Division && m_Secondary != ARBPointsViewSort::Division)
		AddItem(m_ctrlTertiary, ARBPointsViewSort::Division, m_Tertiary);
	if (m_Primary != ARBPointsViewSort::Level && m_Secondary != ARBPointsViewSort::Level)
		AddItem(m_ctrlTertiary, ARBPointsViewSort::Level, m_Tertiary);
	if (m_Primary != ARBPointsViewSort::Event && m_Secondary != ARBPointsViewSort::Event)
		AddItem(m_ctrlTertiary, ARBPointsViewSort::Event, m_Tertiary);
	UpdateControls();
}


void CDlgPointsViewSort::OnSelchangePrimary(wxCommandEvent& evt)
{
	int idx = m_ctrlPrimary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Primary = ARBPointsViewSort::Unknown;
	else
		m_Primary = dynamic_cast<CPointViewData*>(m_ctrlPrimary->GetClientObject(idx))->m_Item;
	FillSecondary();
}


void CDlgPointsViewSort::OnSelchangeSecondary(wxCommandEvent& evt)
{
	int idx = m_ctrlSecondary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Secondary = ARBPointsViewSort::Unknown;
	else
		m_Secondary = dynamic_cast<CPointViewData*>(m_ctrlSecondary->GetClientObject(idx))->m_Item;
	FillTertiary();
}


void CDlgPointsViewSort::OnSelchangeTertiary(wxCommandEvent& evt)
{
	int idx = m_ctrlTertiary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Tertiary = ARBPointsViewSort::Unknown;
	else
		m_Tertiary = dynamic_cast<CPointViewData*>(m_ctrlTertiary->GetClientObject(idx))->m_Item;
	UpdateControls();
}


void CDlgPointsViewSort::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	CAgilityBookOptions::SetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);
	EndDialog(wxID_OK);
}

} // namespace dconSoft
