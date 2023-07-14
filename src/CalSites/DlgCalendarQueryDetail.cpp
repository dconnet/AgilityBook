/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-10-05 Created
 */

#include "stdafx.h"
#include "DlgCalendarQueryDetail.h"

#include "CalSites.h"
#include "CheckListCtrl.h"
#include "DlgQueryDetail.h"

#include "ARB/ARBConfig.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgPadding.h"
#include <wx/listctrl.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;


CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
	ARBConfigCalSiteList const& sites,
	std::map<std::wstring, std::wstring> const& inLocCodes,
	std::map<std::wstring, std::wstring> const& inVenueCodes,
	wxWindow* pParent)
	: wxDialog()
	, m_EditCodes(true)
	, m_sites(sites)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations()
	, m_Venues()
	, m_ctrlLocations(nullptr)
	, m_ctrlVenues(nullptr)
	, m_ctrlNewLoc(nullptr)
	, m_ctrlEditLoc(nullptr)
	, m_ctrlDeleteLoc(nullptr)
	, m_ctrlNewVenue(nullptr)
	, m_ctrlEditVenue(nullptr)
	, m_ctrlDeleteVenue(nullptr)
{
	Create(pParent);
}


CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
	ARBConfigCalSiteList const& sites,
	std::map<std::wstring, std::wstring> const& inLocCodes,
	std::vector<std::wstring> const& inSelectedLocCodes,
	std::map<std::wstring, std::wstring> const& inVenueCodes,
	std::vector<std::wstring> const& inSelectedVenueCodes,
	wxWindow* pParent)
	: wxDialog()
	, m_EditCodes(false)
	, m_sites(sites)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations(inSelectedLocCodes)
	, m_Venues(inSelectedVenueCodes)
	, m_ctrlLocations(nullptr)
	, m_ctrlVenues(nullptr)
	, m_ctrlNewLoc(nullptr)
	, m_ctrlEditLoc(nullptr)
	, m_ctrlDeleteLoc(nullptr)
	, m_ctrlNewVenue(nullptr)
	, m_ctrlEditVenue(nullptr)
	, m_ctrlDeleteVenue(nullptr)
{
	Create(pParent);
}


void CDlgCalendarQueryDetail::Create(wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(
		pParent,
		wxID_ANY,
		_("IDD_CALENDAR_QUERY_DETAIL"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first                to control tab order)

	wxStaticText* textLocCodes
		= new wxStaticText(this, wxID_ANY, _("IDC_QUERY_LOCATIONS"), wxDefaultPosition, wxDefaultSize, 0);
	textLocCodes->Wrap(-1);

	m_ctrlLocations = new CCheckListCtrl(this, wxDefaultPosition, wxDLG_UNIT(this, wxSize(180, 75)), !m_EditCodes);
	m_ctrlLocations->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgCalendarQueryDetail::OnLocationsItemSelected, this);
	m_ctrlLocations->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgCalendarQueryDetail::OnLocationsItemActivated, this);
	m_ctrlLocations->SetHelpText(_("HIDC_QUERY_LOCATIONS"));
	m_ctrlLocations->SetToolTip(_("HIDC_QUERY_LOCATIONS"));
	m_ctrlLocations->InsertColumn(0, _("IDS_COL_CODE"));
	m_ctrlLocations->InsertColumn(1, _("IDS_COL_LOCATION"));
	std::map<std::wstring, std::wstring>::const_iterator i;
	for (i = m_LocCodes.begin(); i != m_LocCodes.end(); ++i)
	{
		// If no checkboxes, 3rd arg is ignored
		long idx = m_ctrlLocations->InsertItem(
			m_ctrlLocations->GetItemCount(),
			StringUtil::stringWX(i->first),
			(m_Locations.end() != std::find(m_Locations.begin(), m_Locations.end(), i->first)));
		SetListColumnText(m_ctrlLocations, idx, 1, i->second);
	}
	m_ctrlLocations->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlLocations->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	if (0 < m_LocCodes.size())
		m_ctrlLocations->Select(0);

	if (m_EditCodes)
	{
		m_ctrlNewLoc = new wxButton(this, wxID_ANY, _("IDC_QUERY_LOCCODE_NEW"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlNewLoc->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnNewLocationCode, this);
		m_ctrlNewLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_NEW"));
		m_ctrlNewLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_NEW"));

		m_ctrlEditLoc = new wxButton(this, wxID_ANY, _("IDC_QUERY_LOCCODE_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlEditLoc->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnEditLocationCode, this);
		m_ctrlEditLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_EDIT"));
		m_ctrlEditLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_EDIT"));

		m_ctrlDeleteLoc
			= new wxButton(this, wxID_ANY, _("IDC_QUERY_LOCCODE_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlDeleteLoc->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnDeleteLocationCode, this);
		m_ctrlDeleteLoc->SetHelpText(_("HIDC_QUERY_LOCCODE_DELETE"));
		m_ctrlDeleteLoc->SetToolTip(_("HIDC_QUERY_LOCCODE_DELETE"));
	}

	wxStaticText* textVenueCodes
		= new wxStaticText(this, wxID_ANY, _("IDC_QUERY_VENUES"), wxDefaultPosition, wxDefaultSize, 0);
	textVenueCodes->Wrap(-1);

	m_ctrlVenues = new CCheckListCtrl(this, wxDefaultPosition, wxDLG_UNIT(this, wxSize(180, 75)), !m_EditCodes);
	m_ctrlVenues->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgCalendarQueryDetail::OnVenuesItemSelected, this);
	m_ctrlVenues->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgCalendarQueryDetail::OnVenuesItemActivated, this);
	m_ctrlVenues->SetHelpText(_("HIDC_QUERY_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_QUERY_VENUES"));
	m_ctrlVenues->InsertColumn(0, _("IDS_COL_CODE"));
	m_ctrlVenues->InsertColumn(1, _("IDS_COL_VENUE"));
	for (i = m_VenueCodes.begin(); i != m_VenueCodes.end(); ++i)
	{
		int idx = m_ctrlVenues->InsertItem(
			m_ctrlVenues->GetItemCount(),
			StringUtil::stringWX(i->first),
			(m_Venues.end() != std::find(m_Venues.begin(), m_Venues.end(), i->first)));
		SetListColumnText(m_ctrlVenues, idx, 1, i->second);
	}
	m_ctrlVenues->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlVenues->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	if (0 < m_VenueCodes.size())
		m_ctrlVenues->Select(0);

	if (m_EditCodes)
	{
		m_ctrlNewVenue = new wxButton(this, wxID_ANY, _("IDC_QUERY_VENUE_NEW"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlNewVenue->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnNewVenueCode, this);
		m_ctrlNewVenue->SetHelpText(_("HIDC_QUERY_VENUE_NEW"));
		m_ctrlNewVenue->SetToolTip(_("HIDC_QUERY_VENUE_NEW"));

		m_ctrlEditVenue = new wxButton(this, wxID_ANY, _("IDC_QUERY_VENUE_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlEditVenue->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnEditVenueCode, this);
		m_ctrlEditVenue->SetHelpText(_("HIDC_QUERY_VENUE_EDIT"));
		m_ctrlEditVenue->SetToolTip(_("HIDC_QUERY_VENUE_EDIT"));

		m_ctrlDeleteVenue
			= new wxButton(this, wxID_ANY, _("IDC_QUERY_VENUE_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlDeleteVenue->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnDeleteVenueCode, this);
		m_ctrlDeleteVenue->SetHelpText(_("HIDC_QUERY_VENUE_DELETE"));
		m_ctrlDeleteVenue->SetToolTip(_("HIDC_QUERY_VENUE_DELETE"));
	}

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerLocation = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerLocationCodes = new wxBoxSizer(wxVERTICAL);
	sizerLocationCodes->Add(textLocCodes, 0, 0, 0);
	sizerLocationCodes->Add(m_ctrlLocations, 1, wxEXPAND | wxTOP, padding.Tight());

	sizerLocation->Add(sizerLocationCodes, 1, wxEXPAND, 0);

	if (m_EditCodes)
	{
		wxBoxSizer* sizerLocBtns = new wxBoxSizer(wxVERTICAL);
		sizerLocBtns->Add(0, 0, 1, wxEXPAND, 0);
		sizerLocBtns->Add(m_ctrlNewLoc, 0, 0, 0);
		sizerLocBtns->Add(m_ctrlEditLoc, 0, wxTOP, padding.TightControls());
		sizerLocBtns->Add(m_ctrlDeleteLoc, 0, wxTOP, padding.TightControls());

		sizerLocation->Add(sizerLocBtns, 0, wxEXPAND | wxLEFT, padding.Controls());
	}

	bSizer->Add(sizerLocation, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerVenueCodes = new wxBoxSizer(wxVERTICAL);
	sizerVenueCodes->Add(textVenueCodes, 0, 0, 0);
	sizerVenueCodes->Add(m_ctrlVenues, 1, wxEXPAND | wxTOP, padding.Tight());

	sizerVenue->Add(sizerVenueCodes, 1, wxEXPAND, 0);

	if (m_EditCodes)
	{
		wxBoxSizer* sizerVenueBtns = new wxBoxSizer(wxVERTICAL);
		sizerVenueBtns->Add(0, 0, 1, wxEXPAND, 0);
		sizerVenueBtns->Add(m_ctrlNewVenue, 0, 0, 0);
		sizerVenueBtns->Add(m_ctrlEditVenue, 0, wxTOP, padding.TightControls());
		sizerVenueBtns->Add(m_ctrlDeleteVenue, 0, wxTOP, padding.TightControls());

		sizerVenue->Add(sizerVenueBtns, 0, wxEXPAND | wxLEFT, padding.Controls());
	}

	bSizer->Add(sizerVenue, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgCalendarQueryDetail::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	if (m_ctrlNewLoc)
		m_ctrlNewLoc->SetFocus();
	else
		m_ctrlLocations->SetFocus();

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
		CDlgQueryDetail dlg(
			!m_EditCodes,
			GetListColumnText(m_ctrlLocations, idx, 0).c_str(),
			GetListColumnText(m_ctrlLocations, idx, 1).c_str(),
			this);
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
		CDlgQueryDetail dlg(
			!m_EditCodes,
			GetListColumnText(m_ctrlVenues, idx, 0).c_str(),
			GetListColumnText(m_ctrlVenues, idx, 1).c_str(),
			this,
			&m_sites);
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
	CDlgQueryDetail dlg(!m_EditCodes, L"", L"", this);
	if (wxID_OK == dlg.ShowModal())
	{
		long idx
			= m_ctrlLocations->InsertItem(m_ctrlLocations->GetItemCount(), StringUtil::stringWX(dlg.GetDetailCode()));
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
	CDlgQueryDetail dlg(!m_EditCodes, L"", L"", this, &m_sites);
	if (wxID_OK == dlg.ShowModal())
	{
		wxString code = StringUtil::stringWX(dlg.GetDetailCode());
		if (!code.empty())
		{
			long idx = m_ctrlVenues->InsertItem(m_ctrlVenues->GetItemCount(), code);
			SetListColumnText(m_ctrlVenues, idx, 1, dlg.GetDetailName());
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

		int idx = 0;
		int nItems = m_ctrlLocations->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			std::wstring str1 = GetListColumnText(m_ctrlLocations, idx, 0);
			std::wstring str2 = GetListColumnText(m_ctrlLocations, idx, 1);
			m_LocCodes[str1] = str2;
		}

		nItems = m_ctrlVenues->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			std::wstring str1 = GetListColumnText(m_ctrlVenues, idx, 0);
			std::wstring str2 = GetListColumnText(m_ctrlVenues, idx, 1);
			m_VenueCodes[str1] = str2;
		}
	}

	else
	{
		m_Locations.clear();
		m_Venues.clear();

		int idx = 0;
		int nItems = m_ctrlLocations->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			if (m_ctrlLocations->IsChecked(idx))
			{
				std::wstring str1 = GetListColumnText(m_ctrlLocations, idx, 0);
				m_Locations.push_back(str1);
			}
		}

		nItems = m_ctrlVenues->GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			if (m_ctrlVenues->IsChecked(idx))
			{
				std::wstring str1 = GetListColumnText(m_ctrlVenues, idx, 0);
				m_Venues.push_back(str1);
			}
		}
	}

	EndDialog(wxID_OK);
}

} // namespace dconSoft
