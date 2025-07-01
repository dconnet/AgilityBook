/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRegNum class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-06-29 Added Note to regnum.
 */

#include "stdafx.h"
#include "DlgRegNum.h"

#include "AgilityBook.h"
#include "ComboBoxVenue.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogRegNum.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;


// If inRegNum is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgRegNum::CDlgRegNum(
	ARBConfig const& config,
	ARBDogRegNumList& regnums,
	ARBDogRegNumPtr const& inRegNum,
	wxWindow* pParent)
	: wxDialog()
	, m_RegNums(regnums)
	, m_pRegNum(inRegNum)
	, m_ctrlVenues(nullptr)
	, m_Venue()
	, m_RegNum()
	, m_Height()
	, m_bReceived(false)
	, m_Note()
	, m_trace("CDlgRegNum")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_REG_NUM"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_pRegNum)
	{
		m_Venue = m_pRegNum->GetVenue();
		m_RegNum = m_pRegNum->GetNumber();
		m_Height = m_pRegNum->GetHeight();
		m_bReceived = m_pRegNum->GetReceived();
		m_Note = m_pRegNum->GetNote();
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textVenue
		= new wxStaticText(this, wxID_ANY, _("IDC_REGNUM_VENUES"), wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(this, config.GetVenues(), m_Venue, false, wxGenericValidator(&m_Venue));
	m_ctrlVenues->SetHelpText(_("HIDC_REGNUM_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_REGNUM_VENUES"));

	wxStaticText* textRegNum
		= new wxStaticText(this, wxID_ANY, _("IDC_REGNUM_REG_NUM"), wxDefaultPosition, wxDefaultSize, 0);
	textRegNum->Wrap(-1);

	CTextCtrl* ctrlRegNum = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(60, -1)),
		0,
		CTrimValidator(&m_RegNum));
	ctrlRegNum->SetHelpText(_("HIDC_REGNUM_REG_NUM"));
	ctrlRegNum->SetToolTip(_("HIDC_REGNUM_REG_NUM"));

	wxStaticText* textHeight
		= new wxStaticText(this, wxID_ANY, _("IDC_REGNUM_HEIGHT"), wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);

	CTextCtrl* ctrlHeight = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(30, -1)),
		0,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHeight->SetHelpText(_("HIDC_REGNUM_HEIGHT"));
	ctrlHeight->SetToolTip(_("HIDC_REGNUM_HEIGHT"));

	wxCheckBox* checkReceived = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_REGNUM_RECEIVED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bReceived));
	checkReceived->SetHelpText(_("HIDC_REGNUM_RECEIVED"));
	checkReceived->SetToolTip(_("HIDC_REGNUM_RECEIVED"));

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(200, 50)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Note, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlDesc->SetHelpText(_("HIDC_REGNUM_NOTE"));
	ctrlDesc->SetToolTip(_("HIDC_REGNUM_NOTE"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerVenue->Add(m_ctrlVenues);

	bSizer->Add(sizerVenue, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerRegNum = new wxBoxSizer(wxHORIZONTAL);
	sizerRegNum->Add(textRegNum, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerRegNum->Add(ctrlRegNum);

	bSizer->Add(sizerRegNum, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerHeight = new wxBoxSizer(wxHORIZONTAL);
	sizerHeight->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerHeight->Add(ctrlHeight);

	bSizer->Add(sizerHeight, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(checkReceived, 0, wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(ctrlDesc, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRegNum::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(m_ctrlVenues)
}


void CDlgRegNum::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (wxNOT_FOUND == m_ctrlVenues->GetSelection())
	{
		wxMessageBox(_("IDS_SELECT_VENUE"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, this);
		m_ctrlVenues->SetFocus();
		return;
	}

	if (m_pRegNum)
	{
		m_pRegNum->SetNumber(m_RegNum);
		m_pRegNum->SetVenue(m_Venue);
		m_pRegNum->SetHeight(m_Height);
		m_pRegNum->SetReceived(m_bReceived);
		m_pRegNum->SetNote(m_Note);
	}
	else
	{
		ARBDogRegNumPtr pRegNum;
		if (m_RegNums.AddRegNum(m_Venue, m_RegNum, &pRegNum))
		{
			pRegNum->SetHeight(m_Height);
			pRegNum->SetReceived(m_bReceived);
			pRegNum->SetNote(m_Note);
		}
	}
	EndDialog(wxID_OK);
}

} // namespace dconSoft
