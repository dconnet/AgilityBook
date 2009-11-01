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
 * @brief implementation of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-12 DRC Fix killfocus handling.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgReferenceRun.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBDogReferenceRun.h"
#include "ARBTypes.h"
#include "ComboBoxes.h"
#include "Validators.h"
#include "Widgets.h"
#include <set>


BEGIN_EVENT_TABLE(CDlgReferenceRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgReferenceRun::OnOk)
END_EVENT_TABLE()


CDlgReferenceRun::CDlgReferenceRun(
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr inRun,
		std::set<wxString> const& inHeights,
		std::set<wxString> const& inNames,
		std::set<wxString> const& inBreeds,
		ARBDogReferenceRunPtr ref,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Run(inRun)
	, m_Ref(ref)
	, m_Place(m_Ref->GetPlace())
	, m_Q(ARB_Q::eQ_NA)
	, m_Time(m_Ref->GetTime())
	, m_ctrlYPS(NULL)
	, m_Points(m_Ref->GetScore())
	, m_Height(m_Ref->GetHeight())
	, m_Name(m_Ref->GetName())
	, m_Breed(m_Ref->GetBreed())
	, m_Notes(m_Ref->GetNote())
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_REF_RUN"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	if (m_Points.empty())
		m_Points = wxT("0");
	if (m_Height.empty())
		m_Height = CAgilityBookOptions::GetLastEnteredRefHeight();

	wxString strYPS;
	double yps;
	if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), m_Time, yps))
	{
		strYPS = ARBDouble::ToString(yps, 3);
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textPlace = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_PLACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPlace->Wrap(-1);

	CTextCtrl* ctrlPlace = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(40, -1), 0,
		CGenericValidator(&m_Place));
	ctrlPlace->SetHelpText(_("HIDC_REFRUN_PLACE"));
	ctrlPlace->SetToolTip(_("HIDC_REFRUN_PLACE"));

	wxStaticText* textQ = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	CQualifyingComboBox* ctrlQ = new CQualifyingComboBox(this, m_Ref,
		CQualifyingValidator(&m_Q));
	ctrlQ->SetHelpText(_("HIDC_REFRUN_Q"));
	ctrlQ->SetToolTip(_("HIDC_REFRUN_Q"));

	wxStaticText* textTime = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_TIME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTime->Wrap(-1);

	CTextCtrl* ctrlTime = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(60, -1), 0,
		CGenericValidator(&m_Time));
	ctrlTime->SetHelpText(_("HIDC_REFRUN_TIME"));
	ctrlTime->SetToolTip(_("HIDC_REFRUN_TIME"));

	wxStaticText* textYPS = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_YPS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textYPS->Wrap(-1);

	m_ctrlYPS = new wxStaticText(this, wxID_ANY,
		strYPS, wxDefaultPosition, wxSize(40, -1),
		wxALIGN_CENTRE|wxSTATIC_BORDER);
	m_ctrlYPS->Wrap(-1);

	wxStaticText* textScore = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	CTextCtrl* ctrlScore = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(40, -1), 0,
		CTrimValidator(&m_Points, TRIMVALIDATOR_TRIM_BOTH));
	ctrlScore->SetHelpText(_("HIDC_REFRUN_POINTS"));
	ctrlScore->SetToolTip(_("HIDC_REFRUN_POINTS"));

	wxStaticText* textHt = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHt->Wrap(-1);

	wxArrayString choices;
	std::set<wxString>::const_iterator iter;
	for (iter = inHeights.begin(); iter != inHeights.end(); ++iter)
	{
		choices.Add((*iter));
	}
	choices.Sort();
	wxComboBox* ctrlHt = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1),
		choices, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHt->SetHelpText(_("HIDC_REFRUN_HEIGHT"));
	ctrlHt->SetToolTip(_("HIDC_REFRUN_HEIGHT"));

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	choices.Clear();
	for (iter = inNames.begin(); iter != inNames.end(); ++iter)
	{
		choices.Add((*iter));
	}
	choices.Sort();
	wxComboBox* ctrlName = new wxComboBox(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		choices, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Name, TRIMVALIDATOR_TRIM_BOTH));
	ctrlName->SetHelpText(_("HIDC_REFRUN_NAME"));
	ctrlName->SetToolTip(_("HIDC_REFRUN_NAME"));

	wxStaticText* textBreed = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_BREED"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBreed->Wrap(-1);

	choices.Clear();
	for (iter = inBreeds.begin(); iter != inBreeds.end(); ++iter)
	{
		choices.Add((*iter));
	}
	choices.Sort();
	wxComboBox* ctrlBreed = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		choices, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Breed, TRIMVALIDATOR_TRIM_BOTH));
	ctrlBreed->SetHelpText(_("HIDC_REFRUN_BREED"));
	ctrlBreed->SetToolTip(_("HIDC_REFRUN_BREED"));

	wxStaticText* textNotes = new wxStaticText(this, wxID_ANY,
		_("IDC_REFRUN_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CTextCtrl* ctrlNotes = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 70), 0,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_REFRUN_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_REFRUN_NOTES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerStats = new wxBoxSizer(wxHORIZONTAL);
	sizerStats->Add(textPlace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(ctrlPlace, 0, wxALL, 5);
	sizerStats->Add(textQ, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(ctrlQ, 0, wxALL, 5);
	sizerStats->Add(textTime, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(ctrlTime, 0, wxALL, 5);
	sizerStats->Add(textYPS, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(m_ctrlYPS, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(textScore, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(ctrlScore, 0, wxALL, 5);
	sizerStats->Add(textHt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStats->Add(ctrlHt, 0, wxALL, 5);

	bSizer->Add(sizerStats, 0, wxEXPAND, 5);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 1, wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerBreed = new wxBoxSizer(wxHORIZONTAL);
	sizerBreed->Add(textBreed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerBreed->Add(ctrlBreed, 1, wxALL, 5);

	bSizer->Add(sizerBreed, 0, wxEXPAND, 5);

	wxBoxSizer* sizerNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerNotes->Add(textNotes, 0, wxALIGN_TOP|wxALL, 5);
	sizerNotes->Add(ctrlNotes, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerNotes, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	// Connect killfocus handlers last
	ctrlTime->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CDlgReferenceRun::OnKillfocusRefRunTime), NULL, this);
}


void CDlgReferenceRun::OnKillfocusRefRunTime(wxFocusEvent& evt)
{
	TransferDataFromWindow();
	wxString strYPS;
	double yps;
	if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), m_Time, yps))
	{
		strYPS = ARBDouble::ToString(yps, 3);
	}
	else
	{
		strYPS.Empty();
	}
	m_ctrlYPS->SetLabel(strYPS);
	evt.Skip();
}


void CDlgReferenceRun::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	CAgilityBookOptions::SetLastEnteredRefHeight(m_Height);

	m_Ref->SetQ(m_Q);
	m_Ref->SetPlace(m_Place);
	m_Ref->SetScore(m_Points);
	m_Ref->SetTime(m_Time); // Letting the prec default to 2 is fine.
	m_Ref->SetName(m_Name);
	m_Ref->SetHeight(m_Height);
	m_Ref->SetBreed(m_Breed);
	m_Ref->SetNote(m_Notes);

	EndDialog(wxID_OK);
}
