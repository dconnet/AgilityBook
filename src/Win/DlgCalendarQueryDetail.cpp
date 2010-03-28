/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-10-05 DRC Created
 */

#include "stdafx.h"
#include "DlgCalendarQueryDetail.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "DlgQueryDetail.h"
#include "Globals.h"
#include "ListCtrl.h"
#include <algorithm>
#include <wx/listctrl.h>


BEGIN_EVENT_TABLE(CDlgCalendarQueryDetail, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgCalendarQueryDetail::OnOk)
END_EVENT_TABLE()


CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
		ARBConfig const& inConfig,
		std::map<wxString, wxString> const& inLocCodes,
		std::map<wxString, wxString> const& inVenueCodes,
		wxWindow* pParent)
	: wxDialog()
	, m_EditCodes(true)
	, m_Config(inConfig)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations()
	, m_Venues()
	, m_ctrlLocations(NULL)
	, m_ctrlVenues(NULL)
	, m_ctrlNewLoc(NULL)
	, m_ctrlEditLoc(NULL)
	, m_ctrlDeleteLoc(NULL)
	, m_ctrlNewVenue(NULL)
	, m_ctrlEditVenue(NULL)
	, m_ctrlDeleteVenue(NULL)
{
	Create(pParent);
}


CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
		ARBConfig const& inConfig,
		std::map<wxString, wxString> const& inLocCodes,
		std::vector<wxString> const& inSelectedLocCodes,
		std::map<wxString, wxString> const& inVenueCodes,
		std::vector<wxString> const& inSelectedVenueCodes,
		wxWindow* pParent)
	: wxDialog()
	, m_EditCodes(false)
	, m_Config(inConfig)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations(inSelectedLocCodes)
	, m_Venues(inSelectedVenueCodes)
	, m_ctrlLocations(NULL)
	, m_ctrlVenues(NULL)
	, m_ctrlNewLoc(NULL)
	, m_ctrlEditLoc(NULL)
	, m_ctrlDeleteLoc(NULL)
	, m_ctrlNewVenue(NULL)
	, m_ctrlEditVenue(NULL)
	, m_ctrlDeleteVenue(NULL)
{
	Create(pParent);
}


void CDlgCalendarQueryDetail::Create(wxWindow* pParent)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(pParent, wxID_ANY, _("IDD_CALENDAR_QUERY_DETAIL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textLocCodes = new wxStaticText(this, wxID_ANY,
		_("IDC_QUERY_LOCATIONS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLocCodes->Wrap(-1);

	m_ctrlLocations = new CCheckListCtrl(this, wxDefaultPosition, wxSize(300, 120), !m_EditCodes);
	m_ctrlLocations->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgCalendarQueryDetail::OnLocationsItemSelected), NULL, this);
	m_ctrlLocations->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(CDlgCalendarQueryDetail::OnLocationsItemActivated), NULL, this);
	m_ctrlLocations->SetHelpText(_("HIDC_QUERY_LOCATIONS"));
	m_ctrlLocations->SetToolTip(_("HIDC_QUERY_LOCATIONS"));
	m_ctrlLocations->InsertColumn(0, _("IDS_COL_CODE"));
	m_ctrlLocations->InsertColumn(1, _("IDS_COL_LOCATION"));
	std::map<wxString, wxString>::const_iterator i;
	for (i = m_LocCodes.begin(); i != m_LocCodes.end(); ++i)
	{
		// If no checkboxes, 3rd arg is ignored
		long idx = m_ctrlLocations->InsertItem(
			m_ctrlLocations->GetItemCount(),
			i->first,
			(m_Locations.end() != std::find(m_Locations.begin(), m_Locations.end(), i->first)));
		SetListColumnText(m_ctrlLocations, idx, 1, i->second);
	}
	m_ctrlLocations->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlLocations->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);

	if (m_EditCodes)
	{
		m_ctrlNewLoc = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_LOCCODE_NEW"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlNewLoc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnNewLocationCode), NULL, this);
		m_ctrlNewLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_NEW"));
		m_ctrlNewLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_NEW"));

		m_ctrlEditLoc = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_LOCCODE_EDIT"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlEditLoc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnEditLocationCode), NULL, this);
		m_ctrlEditLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_EDIT"));
		m_ctrlEditLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_EDIT"));

		m_ctrlDeleteLoc = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_LOCCODE_DELETE"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlDeleteLoc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnDeleteLocationCode), NULL, this);
		m_ctrlDeleteLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_DELETE"));
		m_ctrlDeleteLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_DELETE"));
	}

	wxStaticText* textVenueCodes = new wxStaticText(this, wxID_ANY,
		_("IDC_QUERY_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenueCodes->Wrap(-1);

	m_ctrlVenues = new CCheckListCtrl(this, wxDefaultPosition, wxSize(300, 120), !m_EditCodes);
	m_ctrlVenues->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgCalendarQueryDetail::OnVenuesItemSelected), NULL, this);
	m_ctrlVenues->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(CDlgCalendarQueryDetail::OnVenuesItemActivated), NULL, this);
	m_ctrlVenues->SetHelpText(_("HIDC_QUERY_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_QUERY_VENUES"));
	m_ctrlVenues->InsertColumn(0, _("IDS_COL_CODE"));
	m_ctrlVenues->InsertColumn(1, _("IDS_COL_VENUE"));
	for (i = m_VenueCodes.begin(); i != m_VenueCodes.end(); ++i)
	{
		int idx = m_ctrlVenues->InsertItem(
			m_ctrlVenues->GetItemCount(),
			i->first,
			(m_Venues.end() != std::find(m_Venues.begin(), m_Venues.end(), i->first)));
		SetListColumnText(m_ctrlVenues, idx, 1, i->second);
	}
	m_ctrlVenues->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlVenues->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);

	if (m_EditCodes)
	{
		m_ctrlNewVenue = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_VENUE_NEW"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlNewVenue->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnNewVenueCode), NULL, this);
		m_ctrlNewVenue->SetHelpText(_("HIDC_QUERY_VENUE_NEW"));
		m_ctrlNewVenue->SetToolTip(_("HIDC_QUERY_VENUE_NEW"));

		m_ctrlEditVenue = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_VENUE_EDIT"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlEditVenue->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnEditVenueCode), NULL, this);
		m_ctrlEditVenue->SetHelpText(_("HIDC_QUERY_VENUE_EDIT"));
		m_ctrlEditVenue->SetToolTip(_("HIDC_QUERY_VENUE_EDIT"));

		m_ctrlDeleteVenue = new wxButton(this, wxID_ANY,
			_("IDC_QUERY_VENUE_DELETE"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlDeleteVenue->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgCalendarQueryDetail::OnDeleteVenueCode), NULL, this);
		m_ctrlDeleteVenue->SetHelpText(_("HIDC_QUERY_VENUE_DELETE"));
		m_ctrlDeleteVenue->SetToolTip(_("HIDC_QUERY_VENUE_DELETE"));
	}

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerLocation = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerLocationCodes = new wxBoxSizer(wxVERTICAL);
	sizerLocationCodes->Add(textLocCodes, 0, wxALL, 5);
	sizerLocationCodes->Add(m_ctrlLocations, 1, wxALL|wxEXPAND, 5);

	sizerLocation->Add(sizerLocationCodes, 1, wxEXPAND, 5);

	if (m_EditCodes)
	{
		wxBoxSizer* sizerLocBtns = new wxBoxSizer(wxVERTICAL);
		sizerLocBtns->Add(0, 0, 1, wxEXPAND, 5);
		sizerLocBtns->Add(m_ctrlNewLoc, 0, wxALL, 5);
		sizerLocBtns->Add(m_ctrlEditLoc, 0, wxALL, 5);
		sizerLocBtns->Add(m_ctrlDeleteLoc, 0, wxALL, 5);

		sizerLocation->Add(sizerLocBtns, 0, wxEXPAND, 5);
	}

	bSizer->Add(sizerLocation, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerVenueCodes = new wxBoxSizer(wxVERTICAL);
	sizerVenueCodes->Add(textVenueCodes, 0, wxALL, 5);
	sizerVenueCodes->Add(m_ctrlVenues, 1, wxALL|wxEXPAND, 5);

	sizerVenue->Add(sizerVenueCodes, 1, wxEXPAND, 5);

	if (m_EditCodes)
	{
		wxBoxSizer* sizerVenueBtns = new wxBoxSizer(wxVERTICAL);
		sizerVenueBtns->Add(0, 0, 1, wxEXPAND, 5);
		sizerVenueBtns->Add(m_ctrlNewVenue, 0, wxALL, 5);
		sizerVenueBtns->Add(m_ctrlEditVenue, 0, wxALL, 5);
		sizerVenueBtns->Add(m_ctrlDeleteVenue, 0, wxALL, 5);

		sizerVenue->Add(sizerVenueBtns, 0, wxEXPAND, 5);
	}

	bSizer->Add(sizerVenue, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateButtons();
}


void CDlgCalendarQueryDetail::UpdateButtons()
{
	if (m_EditCodes)
	{
		bool bSelected = (1 == m_ctrlLocations->GetSelectedItemCount());
		m_ctrlEditLoc->Enable(bSelected);
		m_ctrlDeleteLoc->Enable(bSelected);
		bSelected = (1 == m_ctrlVenues->GetSelectedItemCount());
		m_ctrlEditVenue->Enable(bSelected);
		m_ctrlDeleteVenue->Enable(bSelected);
	}
}


void CDlgCalendarQueryDetail::EditLocationCode()
{
	long idx = m_ctrlLocations->GetFirstSelected();
	if (0 <= idx)
	{
		CDlgQueryDetail dlg(!m_EditCodes, GetListColumnText(m_ctrlLocations, idx, 0), GetListColumnText(m_ctrlLocations, idx, 1), this);
		if (wxID_OK == dlg.ShowModal())
		{
			SetListColumnText(m_ctrlLocations, idx, 0, dlg.GetDetailCode());
			SetListColumnText(m_ctrlLocations, idx, 1, dlg.GetDetailName());
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::EditVenueCode()
{
	long idx = m_ctrlVenues->GetFirstSelected();
	if (0 <= idx)
	{
		CDlgQueryDetail dlg(!m_EditCodes, GetListColumnText(m_ctrlVenues, idx, 0), GetListColumnText(m_ctrlVenues, idx, 1), this, &m_Config);
		if (wxID_OK == dlg.ShowModal())
		{
			SetListColumnText(m_ctrlVenues, idx, 0, dlg.GetDetailCode());
			SetListColumnText(m_ctrlVenues, idx, 1, dlg.GetDetailName());
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::OnLocationsItemSelected(wxListEvent& evt)
{
	UpdateButtons();
}


void CDlgCalendarQueryDetail::OnLocationsItemActivated(wxListEvent& evt)
{
	EditLocationCode();
}


void CDlgCalendarQueryDetail::OnVenuesItemSelected(wxListEvent& evt)
{
	UpdateButtons();
}


void CDlgCalendarQueryDetail::OnVenuesItemActivated(wxListEvent& evt)
{
	EditVenueCode();
}


void CDlgCalendarQueryDetail::OnNewLocationCode(wxCommandEvent& evt)
{
	CDlgQueryDetail dlg(!m_EditCodes, wxT(""), wxT(""), this);
	if (wxID_OK == dlg.ShowModal())
	{
		long idx = m_ctrlLocations->InsertItem(m_ctrlLocations->GetItemCount(), dlg.GetDetailCode());
		SetListColumnText(m_ctrlLocations, idx, 1, dlg.GetDetailName());
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnEditLocationCode(wxCommandEvent& evt)
{
	EditLocationCode();
}


void CDlgCalendarQueryDetail::OnDeleteLocationCode(wxCommandEvent& evt)
{
	long idx = m_ctrlLocations->GetFirstSelected();
	if (0 <= idx)
	{
		m_ctrlLocations->DeleteItem(idx);
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnNewVenueCode(wxCommandEvent& evt)
{
	CDlgQueryDetail dlg(!m_EditCodes, wxT(""), wxT(""), this, &m_Config);
	if (wxID_OK == dlg.ShowModal())
	{
		wxString code = dlg.GetDetailCode();
		wxString name = dlg.GetDetailName();
		if (!code.empty())
		{
			long idx = m_ctrlVenues->InsertItem(m_ctrlVenues->GetItemCount(), code);
			SetListColumnText(m_ctrlVenues, idx, 1, name);
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::OnEditVenueCode(wxCommandEvent& evt)
{
	EditVenueCode();
}


void CDlgCalendarQueryDetail::OnDeleteVenueCode(wxCommandEvent& evt)
{
	long idx = m_ctrlVenues->GetFirstSelected();
	if (0 <= idx)
	{
		m_ctrlVenues->DeleteItem(idx);
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_EditCodes)
	{
		m_LocCodes.clear();
		m_VenueCodes.clear();

		int idx;
		int nItems = m_ctrlLocations->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			wxString str1 = GetListColumnText(m_ctrlLocations, idx, 0);
			wxString str2 = GetListColumnText(m_ctrlLocations, idx, 1);
			m_LocCodes[str1] = str2;
		}

		nItems = m_ctrlVenues->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			wxString str1 = GetListColumnText(m_ctrlVenues, idx, 0);
			wxString str2 = GetListColumnText(m_ctrlVenues, idx, 1);
			m_VenueCodes[str1] = str2;
		}
	}

	else
	{
		m_Locations.clear();
		m_Venues.clear();

		int idx;
		int nItems = m_ctrlLocations->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			if (m_ctrlLocations->IsChecked(idx))
			{
				wxString str1 = GetListColumnText(m_ctrlLocations, idx, 0);
				m_Locations.push_back(str1);
			}
		}

		nItems = m_ctrlVenues->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			if (m_ctrlVenues->IsChecked(idx))
			{
				wxString str1 = GetListColumnText(m_ctrlVenues, idx, 0);
				m_Venues.push_back(str1);
			}
		}
	}

	EndDialog(wxID_OK);
}
