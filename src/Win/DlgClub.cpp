/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgClub class
 * @author David Connet
 *
 * Revision History
 * 2016-11-29 Fix sizer assertions.
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Fix initial focus.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2008-02-01 Only modify existing club, don't add a new one (caller does).
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgClub.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "ComboBoxVenue.h"

#include "ARB/ARBDogClub.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CDlgClubData : public wxClientData
{
public:
	CDlgClubData(ARBDogClubPtr const& inClub)
		: m_Club(inClub)
	{
	}
	ARBDogClubPtr m_Club;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgClub, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgClub::OnOk)
wxEND_EVENT_TABLE()


// If inClub is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgClub::CDlgClub(
	CAgilityBookDoc* pDoc,
	ARBDogClubList& inClubs,
	ARBDogClubPtr const& inClub,
	bool bShowCoSanction,
	wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pClub(inClub)
	, m_ctrlClubs(nullptr)
	, m_ctrlVenues(nullptr)
	, m_ctrlPrimary(nullptr)
	, m_Club()
	, m_Venue()
	, m_bCoSanction(inClub && !!inClub->GetPrimaryClub())
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_CLUB"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_pClub)
	{
		m_Club = StringUtil::stringWX(m_pClub->GetName());
		m_Venue = StringUtil::stringWX(m_pClub->GetVenue());
	}
	bool bEditingCosanction = false;
	bool bEnableCosanction = true;
	ARBDogClubPtr pPrimary;
	std::set<std::wstring> clubnames;
	for (ARBDogClubList::const_iterator iClub = inClubs.begin(); iClub != inClubs.end(); ++iClub)
	{
		if ((*iClub)->GetPrimaryClub())
		{
			pPrimary = (*iClub)->GetPrimaryClub();
			// Do not enable cosanctioning on primary club.
			if (m_pClub && *m_pClub == *pPrimary)
				bEnableCosanction = false;
			// Is this us?
			if (m_pClub && *m_pClub == *(*iClub))
				bEditingCosanction = true;
		}
		clubnames.insert((*iClub)->GetName());
	}
	m_pDoc->Book().GetAllClubNames(clubnames, true, true);
	if (m_pClub && !m_pClub->GetName().empty())
		clubnames.insert(m_pClub->GetName());
	if (bEnableCosanction)
	{
		if (inClubs.size() < 2 || (pPrimary && !bEditingCosanction))
			bEnableCosanction = false;
	}
	if (bEnableCosanction && !bShowCoSanction)
		bEnableCosanction = false;

	wxArrayString clubs;
	for (std::set<std::wstring>::const_iterator iter = clubnames.begin(); iter != clubnames.end(); ++iter)
	{
		clubs.Add(StringUtil::stringWX(*iter));
	}
	clubs.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY, _("IDC_CLUB_CLUBS"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlClubs = new wxComboBox(
		this,
		wxID_ANY,
		m_Club,
		wxDefaultPosition,
		wxDefaultSize,
		clubs,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Club, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	m_ctrlClubs->SetHelpText(_("HIDC_CLUB_CLUBS"));
	m_ctrlClubs->SetToolTip(_("HIDC_CLUB_CLUBS"));
	m_ctrlClubs->AutoComplete(clubs);

	wxStaticText* textVenue
		= new wxStaticText(this, wxID_ANY, _("IDC_CLUB_VENUES"), wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(
		this,
		m_pDoc->Book().GetConfig().GetVenues(),
		m_Venue,
		false,
		wxGenericValidator(&m_Venue));
	m_ctrlVenues->SetHelpText(_("HIDC_CLUB_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_CLUB_VENUES"));

	wxCheckBox* ctrl = nullptr;
	if (bEnableCosanction)
	{
		ctrl = new wxCheckBox(
			this,
			wxID_ANY,
			_("IDS_CLUB_COSANCTION"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bCoSanction));
		ctrl->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgClub::OnClickedPrimary, this);
		ctrl->SetHelpText(_("HIDC_CLUB_COSANCTION"));
		ctrl->SetToolTip(_("HIDC_CLUB_COSANCTION"));

		m_ctrlPrimary = new wxComboBox(
			this,
			wxID_ANY,
			wxString(),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			nullptr,
			wxCB_DROPDOWN | wxCB_READONLY);
		m_ctrlPrimary->SetHelpText(_("HIDC_CLUB_PRIMARY"));
		m_ctrlPrimary->SetToolTip(_("HIDC_CLUB_PRIMARY"));
		m_ctrlPrimary->Show(m_bCoSanction);
		for (auto item : inClubs)
		{
			if (item->GetPrimaryClub())
				continue;
			if (m_pClub && *m_pClub == *item)
				continue;
			int index = m_ctrlPrimary->Append(fmt::format(L"[{}] {}", item->GetVenue(), item->GetName()));
			m_ctrlPrimary->SetClientObject(index, new CDlgClubData(item));
			if (m_pClub->GetPrimaryClub() && m_pClub->GetPrimaryClub()->GetVenue() == item->GetVenue())
				m_ctrlPrimary->SetSelection(index);
		}
		if (wxNOT_FOUND == m_ctrlVenues->GetSelection())
			m_ctrlPrimary->SetSelection(0);
	}

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(m_ctrlClubs, 1, wxEXPAND, 0);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerVenue->Add(m_ctrlVenues, 0, wxEXPAND, 0);

	bSizer->Add(sizerVenue, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	if (ctrl)
		bSizer->Add(ctrl, 0, wxALL, wxDLG_UNIT_X(this, 5));
	if (m_ctrlPrimary)
		bSizer->Add(m_ctrlPrimary, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgClub, m_ctrlClubs)
}


DEFINE_ON_INIT(CDlgClub)


ARBDogClubPtr CDlgClub::AddClub(ARBDogClubList& clubs) const
{
	ARBDogClubPtr club;
	if (clubs.AddClub(StringUtil::stringW(m_Club), StringUtil::stringW(m_Venue), &club))
		SetPrimaryClub(club);
	else
		club.reset();
	return club;
}


CDlgClubData* CDlgClub::GetClubData(int index) const
{
	return dynamic_cast<CDlgClubData*>(m_ctrlPrimary->GetClientObject(index));
}


void CDlgClub::SetPrimaryClub(ARBDogClubPtr pClub) const
{
	ARBDogClubPtr pPrimary;
	if (m_bCoSanction && m_ctrlPrimary->IsShown())
	{
		int index = m_ctrlPrimary->GetSelection();
		if (wxNOT_FOUND != index)
			pPrimary = GetClubData(index)->m_Club;
	}
	pClub->SetPrimaryClub(pPrimary);
}


void CDlgClub::OnClickedPrimary(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlPrimary->Show(m_bCoSanction);

	SetSizeHints(wxDefaultSize, wxDefaultSize);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
}


void CDlgClub::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (wxNOT_FOUND == m_ctrlVenues->GetSelection())
	{
		wxMessageBox(_("IDS_SELECT_VENUE"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, this);
		m_ctrlVenues->SetFocus();
		return;
	}

	if (m_bCoSanction && m_ctrlPrimary->IsShown())
	{
		if (wxNOT_FOUND == m_ctrlPrimary->GetSelection())
		{
			wxMessageBox(_("IDS_SELECT_COSANCTION"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, this);
			m_ctrlPrimary->SetFocus();
			return;
		}
	}

	if (m_pClub)
	{
		m_pClub->SetName(StringUtil::stringW(m_Club));
		m_pClub->SetVenue(StringUtil::stringW(m_Venue));
		SetPrimaryClub(m_pClub);
	}
	EndDialog(wxID_OK);
}
