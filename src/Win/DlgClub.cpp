/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgClub class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Only modify existing club, don't add a new one (caller does).
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgClub.h"

#include "AgilityBookDoc.h"
#include "ARBDogClub.h"
#include "ComboBoxes.h"
#include "Validators.h"
#include <set>


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
	, m_ctrlClubs(NULL)
	, m_ctrlVenues(NULL)
	, m_Club()
	, m_Venue()
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_CLUB"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	if (m_pClub)
	{
		m_Club = m_pClub->GetName().c_str();
		m_Venue = m_pClub->GetVenue().c_str();
	}
	std::set<tstring> clubnames;
	for (ARBDogClubList::const_iterator iClub = inClubs.begin(); iClub != inClubs.end(); ++iClub)
		clubnames.insert((*iClub)->GetName());
	m_pDoc->Book().GetAllClubNames(clubnames, true, true);
	if (m_pClub && !m_pClub->GetName().empty())
		clubnames.insert(m_pClub->GetName());

	wxArrayString clubs;
	for (std::set<tstring>::const_iterator iter = clubnames.begin(); iter != clubnames.end(); ++iter)
	{
		clubs.Add((*iter).c_str());
	}
	clubs.Sort();

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CLUB_CLUBS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlClubs = new wxComboBox(this, wxID_ANY, m_Club,
		wxDefaultPosition, wxDefaultSize,
		clubs, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Club));
	m_ctrlClubs->SetHelpText(_("HIDC_CLUB_CLUBS"));
	m_ctrlClubs->SetToolTip(_("HIDC_CLUB_CLUBS"));

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

	bSizer->Add(sizerName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(m_ctrlVenues, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizerVenue, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	m_ctrlClubs->SetFocus();
}


void CDlgClub::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (m_pClub)
	{
		m_pClub->SetName(m_Club.c_str());
		m_pClub->SetVenue(m_Venue.c_str());
	}
	EndDialog(wxID_OK);
}
