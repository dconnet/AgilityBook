/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgRegNum class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-06-29 DRC Added Note to regnum.
 */

#include "stdafx.h"
#include "DlgRegNum.h"

#include "ARBConfig.h"
#include "ARBDogRegNum.h"
#include "ComboBoxes.h"
#include "Validators.h"


BEGIN_EVENT_TABLE(CDlgRegNum, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgRegNum::OnOk)
END_EVENT_TABLE()


// If pRegNum is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgRegNum::CDlgRegNum(
		ARBConfig const& config,
		ARBDogRegNumList& regnums,
		ARBDogRegNumPtr pRegNum,
		wxWindow* pParent)
	: wxDialog()
	, m_RegNums(regnums)
	, m_pRegNum(pRegNum)
	, m_Venue()
	, m_RegNum()
	, m_Height()
	, m_bReceived(false)
	, m_Note()
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_REG_NUM"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_pRegNum)
	{
		m_Venue = m_pRegNum->GetVenue().c_str();
		m_RegNum = m_pRegNum->GetNumber().c_str();
		m_Height = m_pRegNum->GetHeight().c_str();
		m_bReceived = m_pRegNum->GetReceived();
		m_Note = m_pRegNum->GetNote().c_str();
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	CVenueComboBox* ctrlVenues = new CVenueComboBox(this,
		config.GetVenues(), m_Venue, false,
		CTrimValidator(&m_Venue));
	ctrlVenues->SetHelpText(_("HIDC_REGNUM_VENUES"));
	ctrlVenues->SetToolTip(_("HIDC_REGNUM_VENUES"));

	wxStaticText* textRegNum = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_REG_NUM"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRegNum->Wrap(-1);

	wxTextCtrl* ctrlRegNum = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_RegNum));
	ctrlRegNum->SetHelpText(_("HIDC_REGNUM_REG_NUM"));
	ctrlRegNum->SetToolTip(_("HIDC_REGNUM_REG_NUM"));

	wxStaticText* textHeight = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);

	wxTextCtrl* ctrlHeight = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHeight->SetHelpText(_("HIDC_REGNUM_HEIGHT"));
	ctrlHeight->SetToolTip(_("HIDC_REGNUM_HEIGHT"));

	wxCheckBox* checkReceived = new wxCheckBox(this, wxID_ANY,
		_("IDC_REGNUM_RECEIVED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bReceived));
	checkReceived->SetHelpText(_("HIDC_REGNUM_RECEIVED"));
	checkReceived->SetToolTip(_("HIDC_REGNUM_RECEIVED"));

	wxTextCtrl* ctrlDesc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(350, 100),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Note, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlDesc->SetHelpText(_("HIDC_REGNUM_NOTE"));
	ctrlDesc->SetToolTip(_("HIDC_REGNUM_NOTE"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(ctrlVenues, 0, wxALL, 5);

	bSizer->Add(sizerVenue, 0, wxEXPAND, 5);

	wxBoxSizer* sizerRegNum = new wxBoxSizer(wxHORIZONTAL);
	sizerRegNum->Add(textRegNum, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerRegNum->Add(ctrlRegNum, 0, wxALL, 5);

	bSizer->Add(sizerRegNum, 0, wxEXPAND, 5);

	wxBoxSizer* sizerHeight = new wxBoxSizer(wxHORIZONTAL);
	sizerHeight->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHeight->Add(ctrlHeight, 0, wxALL, 5);

	bSizer->Add(sizerHeight, 0, wxEXPAND, 5);
	bSizer->Add(checkReceived, 0, wxALL, 5);
	bSizer->Add(ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
	
	ctrlVenues->SetFocus();
}


void CDlgRegNum::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_pRegNum)
	{
		m_pRegNum->SetNumber(m_RegNum.c_str());
		m_pRegNum->SetVenue(m_Venue.c_str());
		m_pRegNum->SetHeight(m_Height.c_str());
		m_pRegNum->SetReceived(m_bReceived);
		m_pRegNum->SetNote(m_Note.c_str());
	}
	else
	{
		ARBDogRegNumPtr pRegNum;
		if (m_RegNums.AddRegNum(m_Venue.c_str(), m_RegNum.c_str(), &pRegNum))
		{
			pRegNum->SetHeight(m_Height.c_str());
			pRegNum->SetReceived(m_bReceived);
			pRegNum->SetNote(m_Note.c_str());
		}
	}
	EndDialog(wxID_OK);
}
