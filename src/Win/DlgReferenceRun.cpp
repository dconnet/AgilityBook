/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-05-22 Fix initialization of Q.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix killfocus handling.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgReferenceRun.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ComboBoxQ.h"
#include "ValidatorsQ.h"

#include "ARB/ARBDogReferenceRun.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <set>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxBEGIN_EVENT_TABLE(CDlgReferenceRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgReferenceRun::OnOk)
wxEND_EVENT_TABLE()


CDlgReferenceRun::CDlgReferenceRun(
	CAgilityBookDoc* pDoc,
	ARBDogRunPtr const& inRun,
	std::set<std::wstring> const& inHeights,
	std::set<std::wstring> const& inNames,
	std::set<std::wstring> const& inBreeds,
	ARBDogReferenceRunPtr const& inRef,
	wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Run(inRun)
	, m_Ref(inRef)
	, m_Place(inRef->GetPlace())
	, m_Q(inRef->GetQ())
	, m_Time(inRef->GetTime())
	, m_ctrlTime(nullptr)
	, m_ctrlYPS(nullptr)
	, m_Points(StringUtil::stringWX(inRef->GetScore()))
	, m_Height(StringUtil::stringWX(inRef->GetHeight()))
	, m_Name(StringUtil::stringWX(inRef->GetName()))
	, m_Breed(StringUtil::stringWX(inRef->GetBreed()))
	, m_Notes(StringUtil::stringWX(inRef->GetNote()))
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_REF_RUN"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (m_Points.empty())
		m_Points = L"0";
	if (m_Height.empty())
		m_Height = StringUtil::stringWX(CAgilityBookOptions::GetLastEnteredRefHeight());

	std::wstring strYPS;
	double yps;
	if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), m_Time, yps))
	{
		strYPS = ARBDouble::ToString(yps, 3);
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textPlace
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_PLACE"), wxDefaultPosition, wxDefaultSize, 0);
	textPlace->Wrap(-1);

	CTextCtrl* ctrlPlace = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 25), -1),
		0,
		CGenericValidator(&m_Place));
	ctrlPlace->SetHelpText(_("HIDC_REFRUN_PLACE"));
	ctrlPlace->SetToolTip(_("HIDC_REFRUN_PLACE"));

	wxStaticText* textQ = new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_Q"), wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	CQualifyingComboBox* ctrlQ = new CQualifyingComboBox(this, m_Ref, CQualifyingValidator(&m_Q));
	ctrlQ->SetHelpText(_("HIDC_REFRUN_Q"));
	ctrlQ->SetToolTip(_("HIDC_REFRUN_Q"));

	wxStaticText* textTime
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_TIME"), wxDefaultPosition, wxDefaultSize, 0);
	textTime->Wrap(-1);

	m_ctrlTime = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 35), -1),
		0,
		CGenericValidator(&m_Time));
	m_ctrlTime->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgReferenceRun::OnEnChangeRefRunTime, this);
	m_ctrlTime->SetHelpText(_("HIDC_REFRUN_TIME"));
	m_ctrlTime->SetToolTip(_("HIDC_REFRUN_TIME"));

	wxStaticText* textYPS = new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_YPS"), wxDefaultPosition, wxDefaultSize, 0);
	textYPS->Wrap(-1);

	m_ctrlYPS = new wxStaticText(
		this,
		wxID_ANY,
		strYPS.c_str(),
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 25), -1),
		wxALIGN_CENTRE | wxSTATIC_BORDER);
	m_ctrlYPS->Wrap(-1);

	wxStaticText* textScore
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_POINTS"), wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	CTextCtrl* ctrlScore = new CTextCtrl(
		this,
		wxID_ANY,
		wxString(),
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 25), -1),
		0,
		CTrimValidator(&m_Points, TRIMVALIDATOR_TRIM_BOTH));
	ctrlScore->SetHelpText(_("HIDC_REFRUN_POINTS"));
	ctrlScore->SetToolTip(_("HIDC_REFRUN_POINTS"));

	wxStaticText* textHt
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_HEIGHT"), wxDefaultPosition, wxDefaultSize, 0);
	textHt->Wrap(-1);

	wxArrayString choices;
	std::set<std::wstring>::const_iterator iter;
	for (iter = inHeights.begin(); iter != inHeights.end(); ++iter)
	{
		choices.Add(StringUtil::stringWX(*iter));
	}
	choices.Sort();
	wxComboBox* ctrlHt = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		choices,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHt->SetHelpText(_("HIDC_REFRUN_HEIGHT"));
	ctrlHt->SetToolTip(_("HIDC_REFRUN_HEIGHT"));
	ctrlHt->AutoComplete(choices);

	wxStaticText* textName
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	choices.Clear();
	for (iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		choices.Add(StringUtil::stringWX(*iter));
	}
	choices.Sort();
	wxComboBox* ctrlName = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		choices,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	ctrlName->SetHelpText(_("HIDC_REFRUN_NAME"));
	ctrlName->SetToolTip(_("HIDC_REFRUN_NAME"));
	ctrlName->AutoComplete(choices);

	wxStaticText* textBreed
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_BREED"), wxDefaultPosition, wxDefaultSize, 0);
	textBreed->Wrap(-1);

	choices.Clear();
	for (iter = inBreeds.begin(); iter != inBreeds.end(); ++iter)
	{
		choices.Add(StringUtil::stringWX(*iter));
	}
	choices.Sort();
	wxComboBox* ctrlBreed = new wxComboBox(
		this,
		wxID_ANY,
		wxString(),
		wxDefaultPosition,
		wxDefaultSize,
		choices,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Breed, TRIMVALIDATOR_TRIM_BOTH));
	ctrlBreed->SetHelpText(_("HIDC_REFRUN_BREED"));
	ctrlBreed->SetToolTip(_("HIDC_REFRUN_BREED"));
	ctrlBreed->AutoComplete(choices);

	wxStaticText* textNotes
		= new wxStaticText(this, wxID_ANY, _("IDC_REFRUN_NOTES"), wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CSpellCheckCtrl* ctrlNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxString(),
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_X(this, 35)),
		0,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_REFRUN_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_REFRUN_NOTES"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerStats = new wxBoxSizer(wxHORIZONTAL);
	sizerStats->Add(textPlace, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(ctrlPlace, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(textQ, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(ctrlQ, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(textTime, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(m_ctrlTime, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(textYPS, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(m_ctrlYPS, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(textScore, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(ctrlScore, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(textHt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerStats->Add(ctrlHt, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerStats, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(ctrlName, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBreed = new wxBoxSizer(wxHORIZONTAL);
	sizerBreed->Add(textBreed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerBreed->Add(ctrlBreed, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerBreed, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerNotes->Add(textNotes, 0, wxALIGN_TOP | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerNotes->Add(ctrlNotes, 1, wxEXPAND, 0);

	bSizer->Add(sizerNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgReferenceRun, ctrlPlace)
}


DEFINE_ON_INIT(CDlgReferenceRun)


void CDlgReferenceRun::OnEnChangeRefRunTime(wxCommandEvent& evt)
{
	wxString strTime = m_ctrlTime->GetValue();
	double time = 0.0;
	std::wstring strYPS;
	if (StringUtil::ToDouble(StringUtil::stringW(strTime), time))
	{
		double yps;
		if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), time, yps))
		{
			strYPS = ARBDouble::ToString(yps, 3);
		}
	}
	m_ctrlYPS->SetLabel(StringUtil::stringWX(strYPS));
	evt.Skip();
}


void CDlgReferenceRun::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	CAgilityBookOptions::SetLastEnteredRefHeight(m_Height);

	m_Ref->SetQ(m_Q);
	m_Ref->SetPlace(m_Place);
	m_Ref->SetScore(StringUtil::stringW(m_Points));
	m_Ref->SetTime(m_Time); // Letting the prec default to 2 is fine.
	m_Ref->SetName(StringUtil::stringW(m_Name));
	m_Ref->SetHeight(StringUtil::stringW(m_Height));
	m_Ref->SetBreed(StringUtil::stringW(m_Breed));
	m_Ref->SetNote(StringUtil::stringW(m_Notes));

	EndDialog(wxID_OK);
}
