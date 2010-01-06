/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2009-12-19 DRC Make side effects of an unearned title more obvious.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2007-06-25 DRC Allow "1" as the start for recurring titles.
 * @li 2006-05-08 DRC Changing the date unselected a title.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-26 DRC The wrong name was saved into the ARBDogTitle object.
 */

#include "stdafx.h"
#include "DlgTitle.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "ARBDogTitle.h"
#include "ComboBoxes.h"
#include "RichEditCtrl2.h"
#include "Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

/////////////////////////////////////////////////////////////////////////////

class CTitleTitleData : public wxClientData
{
public:
	CTitleTitleData(ARBConfigTitlePtr title) : m_Title(title) {}
	ARBConfigTitlePtr m_Title;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgTitle, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgTitle::OnOk)
END_EVENT_TABLE()


// If pTitle is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgTitle::CDlgTitle(
		ARBConfig const& config,
		ARBDogTitleList& titles,
		ARBDogTitlePtr pTitle,
		wxWindow* pParent)
	: wxDialog()
	, m_Titles(titles)
	, m_pTitle(pTitle)
	, m_ctrlDate(NULL)
	, m_ctrlReceived(NULL)
	, m_ctrlVenues(NULL)
	, m_ctrlHide(NULL)
	, m_ctrlTitles(NULL)
	, m_ctrlDesc(NULL)
	, m_Venue()
	, m_bEarned(true)
	, m_bHidden(false)
	, m_bReceived(false)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_TITLE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxDateTime date(wxDateTime::Now());
	if (m_pTitle)
	{
		if (m_pTitle->GetDate().IsValid())
			date.Set(m_pTitle->GetDate().GetDate());
		else
			m_bEarned = false;
		if (m_pTitle->IsHidden())
			m_bHidden = true;
		if (m_pTitle->GetReceived())
			m_bReceived = true;
		m_Venue = m_pTitle->GetVenue();
	}

	// Controls (these are done first to control tab order)

	wxCheckBox* checkEarned = new wxCheckBox(this, wxID_ANY,
		_("IDC_TITLE_EARNED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bEarned));
	checkEarned->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgTitle::OnClickedEarned), NULL, this);
	checkEarned->SetHelpText(_("HIDC_TITLE_EARNED"));
	checkEarned->SetToolTip(_("HIDC_TITLE_EARNED"));

	m_ctrlDate = new wxDatePickerCtrl(this, wxID_ANY, date,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	m_ctrlDate->Connect(wxEVT_DATE_CHANGED, wxDateEventHandler(CDlgTitle::OnTitleDateChanged), NULL, this);
	m_ctrlDate->SetHelpText(_("HIDC_TITLE_DATE"));
	m_ctrlDate->SetToolTip(_("HIDC_TITLE_DATE"));
	if (!m_bEarned)
		m_ctrlDate->Enable(false);

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_TITLE_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new CVenueComboBox(this,
		config.GetVenues(), m_Venue, false,
		wxGenericValidator(&m_Venue));
	m_ctrlVenues->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgTitle::OnSelchangeVenues), NULL, this);
	m_ctrlVenues->SetHelpText(_("HIDC_TITLE_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_TITLE_VENUES"));

	m_ctrlHide = new wxCheckBox(this, wxID_ANY,
		_("IDC_TITLE_HIDDEN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHidden));
	m_ctrlHide->SetHelpText(_("HIDC_TITLE_HIDDEN"));
	m_ctrlHide->SetToolTip(_("HIDC_TITLE_HIDDEN"));

	m_ctrlReceived = new wxCheckBox(this, wxID_ANY,
		_("IDC_TITLE_RECEIVED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bReceived));
	m_ctrlReceived->SetHelpText(_("HIDC_TITLE_RECEIVED"));
	m_ctrlReceived->SetToolTip(_("HIDC_TITLE_RECEIVED"));
	if (!m_bEarned)
		m_ctrlReceived->Enable(false);

	wxStaticText* textTitle = new wxStaticText(this, wxID_ANY,
		_("IDC_TITLE_TITLES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTitle->Wrap(-1);

	m_ctrlTitles = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlTitles->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgTitle::OnSelchangeTitles), NULL, this);
	m_ctrlTitles->SetHelpText(_("HIDC_TITLE_TITLES"));
	m_ctrlTitles->SetToolTip(_("HIDC_TITLE_TITLES"));

	m_ctrlDesc = new CRichEditCtrl2(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(450, 120), true);
	m_ctrlDesc->SetHelpText(_("HIDC_TITLE_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_TITLE_DESC"));

	FillTitles(true);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerTop2Rows = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDateVenue = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(checkEarned, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(m_ctrlDate, 0, wxALL, 5);

	sizerDateVenue->Add(sizerDate, 0, wxEXPAND, 5);

	wxBoxSizer* sizerVenue = new wxBoxSizer(wxHORIZONTAL);
	sizerVenue->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerVenue->Add(m_ctrlVenues, 0, wxALL, 5);

	sizerDateVenue->Add(sizerVenue, 0, wxEXPAND, 5);

	sizerTop2Rows->Add(sizerDateVenue, 0, wxEXPAND, 5);

	wxBoxSizer* sizerChecks = new wxBoxSizer(wxVERTICAL);
	sizerChecks->Add(m_ctrlHide, 0, wxALL, 5);
	sizerChecks->Add(m_ctrlReceived, 0, wxALL, 5);

	sizerTop2Rows->Add(sizerChecks, 0, wxEXPAND, 5);

	bSizer->Add(sizerTop2Rows, 0, wxEXPAND, 5);

	wxBoxSizer* sizerTitle = new wxBoxSizer(wxHORIZONTAL);
	sizerTitle->Add(textTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerTitle->Add(m_ctrlTitles, 1, wxALL, 5);

	bSizer->Add(sizerTitle, 0, wxEXPAND, 5);

	bSizer->Add(m_ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

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
			if (0 < pTitle->GetMultiple()
			|| 0 == m_Titles.NumTitlesInUse(pVenue->GetName(), pTitle->GetName())
			|| (m_pTitle && m_pTitle->GetRawName() == pTitle->GetName()))
			{
				if (pTitle->IsValidOn(date))
				{
					int idx = m_ctrlTitles->Append(pTitle->GetCompleteName());
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
	int index = m_ctrlTitles->GetSelection();
	wxString str;
	if (wxNOT_FOUND != index)
	{
		ARBConfigTitlePtr pTitle = GetTitleData(index);
		str = pTitle->GetDescription();
	}
	m_ctrlDesc->SetValue(str);
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
		bLastReceived = m_bReceived;
		m_ctrlHide->SetValue(true);
		m_ctrlReceived->SetValue(false);
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

	short instance = 0;
	if (0 < pTitle->GetMultiple())
	{
		if (m_pTitle && m_pTitle->GetRawName() == pTitle->GetName())
			instance = m_pTitle->GetInstance();
		else
		{
			instance = pTitle->GetMultiple();
			short next = m_Titles.FindMaxInstance(pVenue->GetName(), pTitle->GetName()) + 1;
			if (next > instance || 1 == next)
				instance = next;
		}
	}

	ARBDogTitlePtr title(ARBDogTitle::New());
	title->SetDate(date);
	title->SetHidden(m_bHidden);
	title->SetVenue(pVenue->GetName());
	title->SetName(pTitle->GetName(), instance, pTitle->GetMultiple() == 1, pTitle->GetMultipleStyle());
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
	EndDialog(wxID_OK);
}
