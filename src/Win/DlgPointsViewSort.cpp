/*
 * Copyright © 2006-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Let user specify how to sort events within a venue.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-07-16 DRC Created
 */

#include "stdafx.h"
#include "DlgPointsViewSort.h"

#include "AgilityBookOptions.h"
#include "Globals.h"

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
	, m_ctrlPrimary(NULL)
	, m_ctrlSecondary(NULL)
	, m_ctrlTertiary(NULL)
	, m_ctrlOk(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_POINTS_VIEW_SORT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	CAgilityBookOptions::GetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);

	// Controls (these are done first to control tab order)

	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C1"),
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(-1);

	m_ctrlPrimary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlPrimary->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgPointsViewSort::OnSelchangePrimary), NULL, this);
	m_ctrlPrimary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C1"));
	m_ctrlPrimary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C1"));

	wxStaticText* text2 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C2"),
		wxDefaultPosition, wxDefaultSize, 0);
	text2->Wrap(-1);

	m_ctrlSecondary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlSecondary->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgPointsViewSort::OnSelchangeSecondary), NULL, this);
	m_ctrlSecondary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C2"));
	m_ctrlSecondary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C2"));

	wxStaticText* text3 = new wxStaticText(this, wxID_ANY,
		_("IDC_POINTS_VIEW_SORT_C3"),
		wxDefaultPosition, wxDefaultSize, 0);
	text3->Wrap(-1);

	m_ctrlTertiary = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlTertiary->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgPointsViewSort::OnSelchangeTertiary), NULL, this);
	m_ctrlTertiary->SetHelpText(_("HIDC_POINTS_VIEW_SORT_C3"));
	m_ctrlTertiary->SetToolTip(_("HIDC_POINTS_VIEW_SORT_C3"));

	FillPrimary();

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(text1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer1->Add(m_ctrlPrimary, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer1, 0, wxEXPAND, 5);

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(text2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer2->Add(m_ctrlSecondary, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer2, 0, wxEXPAND, 5);

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(text3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer3->Add(m_ctrlTertiary, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer3, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);
	m_ctrlOk = wxDynamicCast(FindWindowInSizer(bSizer, wxID_OK), wxButton);
	assert(m_ctrlOk != NULL);

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
