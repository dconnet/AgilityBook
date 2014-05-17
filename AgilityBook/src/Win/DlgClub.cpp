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
#include "ComboBoxes.h"
#include "ComboBoxVenue.h"
#include "Validators.h"

#include "ARB/ARBDogClub.h"
#include "ARBCommon/StringUtil.h"
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


BEGIN_EVENT_TABLE(CDlgClub, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgClub::OnOk)
END_EVENT_TABLE()


// If pClub is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgClub::CDlgClub(
		CAgilityBookDoc* pDoc,
		ARBDogClubList& inClubs,
		ARBDogClubPtr pClub,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pClub(pClub)
	, m_ctrlClubs(nullptr)
	, m_ctrlVenues(nullptr)
	, m_Club()
	, m_Venue()
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CLUB"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	if (m_pClub)
	{
		m_Club = StringUtil::stringWX(m_pClub->GetName());
		m_Venue = StringUtil::stringWX(m_pClub->GetVenue());
	}
	std::set<std::wstring> clubnames;
	for (ARBDogClubList::const_iterator iClub = inClubs.begin(); iClub != inClubs.end(); ++iClub)
		clubnames.insert((*iClub)->GetName());
	m_pDoc->Book().GetAllClubNames(clubnames, true, true);
	if (m_pClub && !m_pClub->GetName().empty())
		clubnames.insert(m_pClub->GetName());

	wxArrayString clubs;
	for (std::set<std::wstring>::const_iterator iter = clubnames.begin(); iter != clubnames.end(); ++iter)
	{
		clubs.Add(StringUtil::stringWX(*iter));
	}
	clubs.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CLUB_CLUBS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlClubs = new CAutoFillComboBox(this, wxID_ANY, m_Club,
		wxDefaultPosition, wxDefaultSize,
		clubs, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Club, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	m_ctrlClubs->SetHelpText(_("HIDC_CLUB_CLUBS"));
	m_ctrlClubs->SetToolTip(_("HIDC_CLUB_CLUBS"));
	m_ctrlClubs->AutoComplete(clubs);

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_CLUB_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(this,
		m_pDoc->Book().GetConfig().GetVenues(), m_Venue, false,
		wxGenericValidator(&m_Venue));
	m_ctrlVenues->SetHelpText(_("HIDC_CLUB_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_CLUB_VENUES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(m_ctrlClubs, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 0);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(m_ctrlVenues, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizerVenue, 0, wxEXPAND, 0);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgClub, m_ctrlClubs)
}


DEFINE_ON_INIT(CDlgClub)


std::wstring CDlgClub::Club() const
{
	return StringUtil::stringW(m_Club);
}


std::wstring CDlgClub::Venue() const
{
	return StringUtil::stringW(m_Venue);
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

	if (m_pClub)
	{
		m_pClub->SetName(StringUtil::stringW(m_Club));
		m_pClub->SetVenue(StringUtil::stringW(m_Venue));
	}
	EndDialog(wxID_OK);
}
