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
 * 2014-12-31 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-07-16 Created
 */

#include "stdafx.h"
#include "DlgPointsViewSort.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "Globals.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CPointViewData : public wxClientData
{
public:
	CPointViewData(CAgilityBookOptions::PointsViewSort item)
		: m_Item(item)
	{
	}
	CAgilityBookOptions::PointsViewSort m_Item;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgPointsViewSort, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgPointsViewSort::OnOk)
END_EVENT_TABLE()


CDlgPointsViewSort::CDlgPointsViewSort(wxWindow* pParent)
	: wxDialog()
	, m_ctrlPrimary(nullptr)
	, m_ctrlSecondary(nullptr)
	, m_ctrlTertiary(nullptr)
	, m_ctrlOk(nullptr)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_POINTS_VIEW_SORT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	CAgilityBookOptions::GetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);

	// Controls (these are done first to control tab order)

	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C1"),
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(-1);

	m_ctrlPrimary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr,
		wxCB_DROPDOWN|wxCB_READONLY);
	BIND_OR_CONNECT_CTRL(m_ctrlPrimary, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgPointsViewSort::OnSelchangePrimary);
	m_ctrlPrimary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C1"));
	m_ctrlPrimary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C1"));

	wxStaticText* text2 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C2"),
		wxDefaultPosition, wxDefaultSize, 0);
	text2->Wrap(-1);

	m_ctrlSecondary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr,
		wxCB_DROPDOWN|wxCB_READONLY);
	BIND_OR_CONNECT_CTRL(m_ctrlSecondary, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgPointsViewSort::OnSelchangeSecondary);
	m_ctrlSecondary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C2"));
	m_ctrlSecondary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C2"));

	wxStaticText* text3 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C3"),
		wxDefaultPosition, wxDefaultSize, 0);
	text3->Wrap(-1);

	m_ctrlTertiary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr,
		wxCB_DROPDOWN|wxCB_READONLY);
	BIND_OR_CONNECT_CTRL(m_ctrlTertiary, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgPointsViewSort::OnSelchangeTertiary);
	m_ctrlTertiary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C3"));
	m_ctrlTertiary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C3"));

	FillPrimary();

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(text1, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizer1->Add(m_ctrlPrimary, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);

	bSizer->Add(sizer1, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(text2, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizer2->Add(m_ctrlSecondary, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);

	bSizer->Add(sizer2, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(text3, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizer3->Add(m_ctrlTertiary, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);

	bSizer->Add(sizer3, 1, wxEXPAND | wxLEFT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));
	m_ctrlOk = wxDynamicCast(FindWindowInSizer(bSizer, wxID_OK), wxButton);
	assert(!!m_ctrlOk);

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
		if (wxNOT_FOUND != m_ctrlPrimary->GetSelection()
		&& wxNOT_FOUND != m_ctrlSecondary->GetSelection()
		&& wxNOT_FOUND != m_ctrlTertiary->GetSelection())
		{
			bEnable = true;
		}
		m_ctrlOk->Enable(bEnable);
	}
}


static int AddItem(
		wxComboBox* ctrl,
		CAgilityBookOptions::PointsViewSort item,
		CAgilityBookOptions::PointsViewSort inSelect)
{
	int idx = -1;
	wxString str;
	switch (item)
	{
	default:
		break;
	case CAgilityBookOptions::ePointsViewSortDivision:
		str = _("IDS_COL_DIVISION");
		idx = ctrl->Append(str);
		ctrl->SetClientObject(idx, new CPointViewData(item));
		break;
	case CAgilityBookOptions::ePointsViewSortLevel:
		str = _("IDS_COL_LEVEL");
		idx = ctrl->Append(str);
		ctrl->SetClientObject(idx, new CPointViewData(item));
		break;
	case CAgilityBookOptions::ePointsViewSortEvent:
		str = _("IDS_COL_EVENT");
		idx = ctrl->Append(str);
		ctrl->SetClientObject(idx, new CPointViewData(item));
		break;
	}
	if (item == inSelect && 0 <= idx)
		ctrl->SetSelection(idx);
	return idx;
}


void CDlgPointsViewSort::FillPrimary()
{
	m_ctrlPrimary->Clear();
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortDivision, m_Primary);
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortLevel, m_Primary);
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortEvent, m_Primary);
	FillSecondary();
}


void CDlgPointsViewSort::FillSecondary()
{
	m_ctrlSecondary->Clear();
	if (m_Primary != CAgilityBookOptions::ePointsViewSortDivision)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortDivision, m_Secondary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortLevel)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortLevel, m_Secondary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortEvent)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortEvent, m_Secondary);
	FillTertiary();
}


void CDlgPointsViewSort::FillTertiary()
{
	m_ctrlTertiary->Clear();
	if (m_Primary != CAgilityBookOptions::ePointsViewSortDivision
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortDivision)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortDivision, m_Tertiary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortLevel
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortLevel)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortLevel, m_Tertiary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortEvent
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortEvent)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortEvent, m_Tertiary);
	UpdateControls();
}


void CDlgPointsViewSort::OnSelchangePrimary(wxCommandEvent& evt)
{
	int idx = m_ctrlPrimary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Primary = CAgilityBookOptions::ePointsViewSortUnknown;
	else
		m_Primary = dynamic_cast<CPointViewData*>(m_ctrlPrimary->GetClientObject(idx))->m_Item;
	FillSecondary();
}


void CDlgPointsViewSort::OnSelchangeSecondary(wxCommandEvent& evt)
{
	int idx = m_ctrlSecondary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Secondary = CAgilityBookOptions::ePointsViewSortUnknown;
	else
		m_Secondary = dynamic_cast<CPointViewData*>(m_ctrlSecondary->GetClientObject(idx))->m_Item;
	FillTertiary();
}


void CDlgPointsViewSort::OnSelchangeTertiary(wxCommandEvent& evt)
{
	int idx = m_ctrlTertiary->GetSelection();
	if (wxNOT_FOUND == idx)
		m_Tertiary = CAgilityBookOptions::ePointsViewSortUnknown;
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
