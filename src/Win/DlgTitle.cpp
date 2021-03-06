/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * 2016-08-09 Change tab order so Venue combo is right before Title combo.
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-03-24 Show title in caption to aid with repeating titles.
 * 2013-01-13 Added new recurring title suffix style.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-08-17 Add support to get newly created title.
 * 2010-01-10 Forcing hidden title didn't update internal flags.
 *            Unearned titles didn't disable Hide Checkbox.
 * 2009-12-19 Make side effects of an unearned title more obvious.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-09 Ported to wxWidgets.
 * 2007-06-25 Allow "1" as the start for recurring titles.
 * 2006-05-08 Changing the date unselected a title.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-01-26 The wrong name was saved into the ARBDogTitle object.
 */

#include "stdafx.h"
#include "DlgTitle.h"

#include "AgilityBook.h"
#include "ComboBoxVenue.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTitle.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CTitleTitleData : public wxClientData
{
public:
	CTitleTitleData(ARBConfigTitlePtr const& inTitle)
		: m_Title(inTitle)
	{
	}
	ARBConfigTitlePtr m_Title;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgTitle, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgTitle::OnOk)
wxEND_EVENT_TABLE()


// If inTitle is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgTitle::CDlgTitle(ARBConfig const& config, ARBDogTitleList& titles, ARBDogTitlePtr const& inTitle, wxWindow* pParent)
	: wxDialog()
	, m_Titles(titles)
	, m_pTitle(inTitle)
	, m_newTitle()
	, m_ctrlDate(nullptr)
	, m_ctrlReceived(nullptr)
	, m_ctrlVenues(nullptr)
	, m_ctrlHide(nullptr)
	, m_ctrlTitles(nullptr)
	, m_ctrlDesc(nullptr)
	, m_Venue()
	, m_bEarned(true)
	, m_bHidden(false)
	, m_bReceived(false)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_TITLE"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxDateTime date(wxDateTime::Now());
	if (m_pTitle)
	{
		if (m_pTitle->GetDate().IsValid())
			m_pTitle->GetDate().GetDate(date);
		else
			m_bEarned = false;
		if (m_pTitle->IsHidden())
			m_bHidden = true;
		if (m_pTitle->GetReceived())
			m_bReceived = true;
		m_Venue = StringUtil::stringWX(m_pTitle->GetVenue());
	}

	// Controls (these are done first to control tab order)

	wxCheckBox* checkEarned = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_TITLE_EARNED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bEarned));
	checkEarned->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgTitle::OnClickedEarned, this);
	checkEarned->SetHelpText(_("HIDC_TITLE_EARNED"));
	checkEarned->SetToolTip(_("HIDC_TITLE_EARNED"));

	m_ctrlDate = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY);
	m_ctrlDate->Bind(wxEVT_DATE_CHANGED, &CDlgTitle::OnTitleDateChanged, this);
	m_ctrlDate->SetHelpText(_("HIDC_TITLE_DATE"));
	m_ctrlDate->SetToolTip(_("HIDC_TITLE_DATE"));
	if (!m_bEarned)
		m_ctrlDate->Enable(false);

	m_ctrlHide = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_TITLE_HIDDEN"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bHidden));
	m_ctrlHide->SetHelpText(_("HIDC_TITLE_HIDDEN"));
	m_ctrlHide->SetToolTip(_("HIDC_TITLE_HIDDEN"));
	if (!m_bEarned)
		m_ctrlHide->Enable(false);

	m_ctrlReceived = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_TITLE_RECEIVED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bReceived));
	m_ctrlReceived->SetHelpText(_("HIDC_TITLE_RECEIVED"));
	m_ctrlReceived->SetToolTip(_("HIDC_TITLE_RECEIVED"));
	if (!m_bEarned)
		m_ctrlReceived->Enable(false);

	wxStaticText* textVenue
		= new wxStaticText(this, wxID_ANY, _("IDC_TITLE_VENUES"), wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(this, config.GetVenues(), m_Venue, false, wxGenericValidator(&m_Venue));
	m_ctrlVenues->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgTitle::OnSelchangeVenues, this);
	m_ctrlVenues->SetHelpText(_("HIDC_TITLE_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_TITLE_VENUES"));

	wxStaticText* textTitle
		= new wxStaticText(this, wxID_ANY, _("IDC_TITLE_TITLES"), wxDefaultPosition, wxDefaultSize, 0);
	textTitle->Wrap(-1);

	m_ctrlTitles = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlTitles->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgTitle::OnSelchangeTitles, this);
	m_ctrlTitles->SetHelpText(_("HIDC_TITLE_TITLES"));
	m_ctrlTitles->SetToolTip(_("HIDC_TITLE_TITLES"));

	m_ctrlDesc
		= new CRichEditCtrl2(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize(260, 65)), true);
	m_ctrlDesc->SetHelpText(_("HIDC_TITLE_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_TITLE_DESC"));

	FillTitles(true);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerTop2Rows = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDateVenue = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(checkEarned, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate->Add(m_ctrlDate, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerDateVenue->Add(sizerDate, 0, wxEXPAND, 0);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerVenue->Add(m_ctrlVenues, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerDateVenue->Add(sizerVenue, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerTop2Rows->Add(sizerDateVenue, 0, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerChecks = new wxBoxSizer(wxVERTICAL);
	sizerChecks->Add(m_ctrlHide, 0, 0, 0);
	sizerChecks->Add(m_ctrlReceived, 0, wxTOP, wxDLG_UNIT_X(this, 5));

	sizerTop2Rows->Add(sizerChecks, 0, wxEXPAND, 0);

	bSizer->Add(sizerTop2Rows, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTitle = new wxBoxSizer(wxHORIZONTAL);
	sizerTitle->Add(textTitle, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerTitle->Add(m_ctrlTitles, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerTitle, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	bSizer->Add(m_ctrlDesc, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlDate->SetFocus();
}


ARBConfigTitlePtr CDlgTitle::GetTitleData(int index) const
{
	wxClientData* pData = m_ctrlTitles->GetClientObject(index);
	if (pData)
		return dynamic_cast<CTitleTitleData*>(pData)->m_Title;
	return ARBConfigTitlePtr();
}


ARBDate CDlgTitle::GetDate()
{
	ARBDate date;
	if (m_bEarned)
	{
		wxDateTime d = m_ctrlDate->GetValue();
		date = ARBDate(d.GetYear(), d.GetMonth() + 1, d.GetDay());
	}
	return date;
}


void CDlgTitle::FillTitles(bool bIniting)
{
	ARBConfigTitlePtr pSelTitle;
	int index = m_ctrlTitles->GetSelection();
	if (wxNOT_FOUND != index)
		pSelTitle = GetTitleData(index);
	m_ctrlTitles->Clear();
	index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
	{
		ARBDate date = GetDate();
		ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(index);
		assert(pVenue);
		for (ARBConfigTitleList::const_iterator iterTitle = pVenue->GetTitles().begin();
			 iterTitle != pVenue->GetTitles().end();
			 ++iterTitle)
		{
			ARBConfigTitlePtr pTitle = (*iterTitle);
			// Suppress any titles we already have.
			if (pTitle->IsRecurring() || 0 == m_Titles.NumTitlesInUse(pVenue->GetName(), pTitle->GetName())
				|| (m_pTitle && m_pTitle->GetRawName() == pTitle->GetName()))
			{
				if (pTitle->IsValidOn(date))
				{
					int idx = m_ctrlTitles->Append(StringUtil::stringWX(pTitle->GetCompleteName()));
					m_ctrlTitles->SetClientObject(idx, new CTitleTitleData(pTitle));
					if ((bIniting && m_pTitle && m_pTitle->GetRawName() == pTitle->GetName())
						|| (!bIniting && pSelTitle && pSelTitle->GetName() == pTitle->GetName()))
					{
						m_ctrlTitles->SetSelection(idx);
						FillTitleInfo();
					}
				}
			}
		}
	}
}


void CDlgTitle::FillTitleInfo()
{
	wxString caption(_("IDD_TITLE"));
	wxString desc;

	int index = m_ctrlTitles->GetSelection();
	if (wxNOT_FOUND != index)
	{
		ARBConfigTitlePtr pTitle = GetTitleData(index);
		desc = StringUtil::stringWX(pTitle->GetDescription());

		caption << L": ";
		if (m_pTitle)
			caption << m_pTitle->GetGenericName().c_str();
		else
			caption << pTitle->GetTitleName(GetInstance(pTitle)).c_str();
	}

	SetTitle(caption);
	m_ctrlDesc->SetValue(desc);
}


short CDlgTitle::GetInstance(ARBConfigTitlePtr const& inTitle, std::vector<short>* outMissing) const
{
	short instance = 0;
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
	{
		ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(index);
		if (pVenue && inTitle->IsRecurring())
		{
			if (m_pTitle && m_pTitle->GetRawName() == inTitle->GetName())
				instance = m_pTitle->GetInstance();
			else
			{
				instance = m_Titles.FindMaxInstance(pVenue->GetName(), inTitle->GetName(), outMissing) + 1;
			}
		}
	}
	return instance;
}


void CDlgTitle::OnTitleDateChanged(wxDateEvent& evt)
{
	TransferDataFromWindow();
	FillTitles();
}


void CDlgTitle::OnClickedEarned(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	static bool bLastHidden = false;
	static bool bLastReceived = false;
	m_ctrlDate->Enable(m_bEarned);
	m_ctrlHide->Enable(m_bEarned);
	m_ctrlReceived->Enable(m_bEarned);
	if (m_bEarned)
	{
		m_ctrlHide->SetValue(bLastHidden);
		m_ctrlReceived->SetValue(bLastReceived);
	}
	else
	{
		bLastHidden = m_bHidden;
		m_bHidden = true;
		m_ctrlHide->SetValue(m_bHidden);
		bLastReceived = m_bReceived;
		m_bReceived = false;
		m_ctrlReceived->SetValue(m_bReceived);
	}
	FillTitles();
}


void CDlgTitle::OnSelchangeVenues(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	FillTitles();
}


void CDlgTitle::OnSelchangeTitles(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	FillTitleInfo();
}


void CDlgTitle::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;

	ARBConfigVenuePtr pVenue = m_ctrlVenues->GetVenue(index);
	assert(pVenue);

	index = m_ctrlTitles->GetSelection();
	if (wxNOT_FOUND == index)
		return;

	ARBConfigTitlePtr pTitle = GetTitleData(index);
	assert(pTitle);

	ARBDate date = GetDate();
	if (!m_bEarned)
	{
		m_bHidden = true;
		m_bReceived = false;
	}

	std::vector<short> instances;
	short instance = GetInstance(pTitle, &instances);
	if (0 < instances.size())
	{
		// Note: This will only trigger on a new title.
#pragma PRAGMA_TODO(Ask user about missing titles)
		// Possible option to not ask again - this should be per-dog, hence
		// saved into file - do not have support for that yet.
	}

	ARBDogTitlePtr title(ARBDogTitle::New());
	title->SetDate(date);
	title->SetHidden(m_bHidden);
	title->SetVenue(pVenue->GetName());
	title->SetName(pTitle->GetName(), instance, pTitle);
	title->SetReceived(m_bReceived);
	if (m_pTitle)
	{
		bool bSort = false;
		if (m_pTitle->GetDate() != title->GetDate())
			bSort = true;
		*m_pTitle = *title;
		if (bSort)
			m_Titles.sort();
	}
	else
		m_Titles.AddTitle(title);
	m_newTitle = title;
	EndDialog(wxID_OK);
}
