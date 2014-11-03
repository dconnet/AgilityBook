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
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogRegNum.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
	, m_ctrlVenues(nullptr)
	, m_Venue()
	, m_RegNum()
	, m_Height()
	, m_bReceived(false)
	, m_Note()
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_REG_NUM"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_pRegNum)
	{
		m_Venue = StringUtil::stringWX(m_pRegNum->GetVenue());
		m_RegNum = StringUtil::stringWX(m_pRegNum->GetNumber());
		m_Height = StringUtil::stringWX(m_pRegNum->GetHeight());
		m_bReceived = m_pRegNum->GetReceived();
		m_Note = StringUtil::stringWX(m_pRegNum->GetNote());
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(this,
		config.GetVenues(), m_Venue, false,
		wxGenericValidator(&m_Venue));
	m_ctrlVenues->SetHelpText(_("HIDC_REGNUM_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_REGNUM_VENUES"));

	wxStaticText* textRegNum = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_REG_NUM"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRegNum->Wrap(-1);

	CTextCtrl* ctrlRegNum = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_RegNum));
	ctrlRegNum->SetHelpText(_("HIDC_REGNUM_REG_NUM"));
	ctrlRegNum->SetToolTip(_("HIDC_REGNUM_REG_NUM"));

	wxStaticText* textHeight = new wxStaticText(this, wxID_ANY,
		_("IDC_REGNUM_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);

	CTextCtrl* ctrlHeight = new CTextCtrl(this, wxID_ANY, wxEmptyString,
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

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(350, 100),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Note, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlDesc->SetHelpText(_("HIDC_REGNUM_NOTE"));
	ctrlDesc->SetToolTip(_("HIDC_REGNUM_NOTE"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(m_ctrlVenues, 0, wxALL, 5);

	bSizer->Add(sizerVenue, 0, wxEXPAND, 0);

	wxBoxSizer* sizerRegNum = new wxBoxSizer(wxHORIZONTAL);
	sizerRegNum->Add(textRegNum, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerRegNum->Add(ctrlRegNum, 0, wxALL, 5);

	bSizer->Add(sizerRegNum, 0, wxEXPAND, 0);

	wxBoxSizer* sizerHeight = new wxBoxSizer(wxHORIZONTAL);
	sizerHeight->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHeight->Add(ctrlHeight, 0, wxALL, 5);

	bSizer->Add(sizerHeight, 0, wxEXPAND, 0);
	bSizer->Add(checkReceived, 0, wxALL, 5);
	bSizer->Add(ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
	
	IMPLEMENT_ON_INIT(CDlgRegNum, m_ctrlVenues)
}


DEFINE_ON_INIT(CDlgRegNum)


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
		m_pRegNum->SetNumber(StringUtil::stringW(m_RegNum));
		m_pRegNum->SetVenue(StringUtil::stringW(m_Venue));
		m_pRegNum->SetHeight(StringUtil::stringW(m_Height));
		m_pRegNum->SetReceived(m_bReceived);
		m_pRegNum->SetNote(StringUtil::stringW(m_Note));
	}
	else
	{
		ARBDogRegNumPtr pRegNum;
		if (m_RegNums.AddRegNum(StringUtil::stringW(m_Venue), StringUtil::stringW(m_RegNum), &pRegNum))
		{
			pRegNum->SetHeight(StringUtil::stringW(m_Height));
			pRegNum->SetReceived(m_bReceived);
			pRegNum->SetNote(StringUtil::stringW(m_Note));
		}
	}
	EndDialog(wxID_OK);
}
