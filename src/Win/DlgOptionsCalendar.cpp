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
 * @brief implementation of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2009-01-11 DRC Fixed a bug that added an 8th day to week.
 * @li 2006-11-22 DRC Turned this into Calendar only, created new Print page.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "DlgOptionsCalendar.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include <wx/colordlg.h>


CDlgOptionsCalendar::CDlgOptionsCalendar(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_fontCalViewInfo()
	, m_fontCalView()
	, m_OpeningNear(CAgilityBookOptions::CalendarOpeningNearColor())
	, m_ClosingNear(CAgilityBookOptions::CalendarClosingNearColor())
	, m_CalColors()
	, m_bOpeningNear(true)
	, m_nOpeningNear(CAgilityBookOptions::CalendarOpeningNear())
	, m_bClosingNear(true)
	, m_nClosingNear(CAgilityBookOptions::CalendarClosingNear())
	, m_bAutoDelete(CAgilityBookOptions::AutoDeleteCalendarEntries())
	, m_bHideOld(!CAgilityBookOptions::ViewAllCalendarEntries())
	, m_Days(CAgilityBookOptions::DaysTillEntryIsPast())
	, m_bHideOverlapping(CAgilityBookOptions::HideOverlappingCalendarEntries())
	, m_bOpening(CAgilityBookOptions::ViewAllCalendarOpening())
	, m_bClosing(CAgilityBookOptions::ViewAllCalendarClosing())
	, m_ctrlOpeningNear(NULL)
	, m_ctrlOpeningNearColor(NULL)
	, m_ctrlOpeningNearSet(NULL)
	, m_ctrlClosingNear(NULL)
	, m_ctrlClosingNearColor(NULL)
	, m_ctrlClosingNearSet(NULL)
	, m_ctrlDayOfWeek(NULL)
	, m_ctrlCalEntries(NULL)
	, m_ctrlColor(NULL)
	, m_ctrlCalView(NULL)
{
	CAgilityBookOptions::GetCalendarFontInfo(m_fontCalViewInfo);
	m_fontCalViewInfo.CreateFont(m_fontCalView);
	if (0 > m_nOpeningNear)
	{
		m_bOpeningNear = false;
		m_nOpeningNear = 0;
	}
	if (0 > m_nClosingNear)
	{
		m_bClosingNear = false;
		m_nClosingNear = 0;
	}

	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorNotEntered, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorPlanning, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorOpening, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorClosing, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorEntered, wxColour()));
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin();
		iColor != m_CalColors.end();
		++iColor)
	{
		(*iColor).second = CAgilityBookOptions::CalendarColor((*iColor).first);
	}

	// Controls (these are done first to control tab order)

	wxCheckBox* ctrlWarnOpen = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_WARN_OPENNEAR"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bOpeningNear));
	ctrlWarnOpen->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnCalNear), NULL, this);
	ctrlWarnOpen->SetHelpText(_("HIDC_OPT_CAL_WARN_OPENNEAR"));
	ctrlWarnOpen->SetToolTip(_("HIDC_OPT_CAL_WARN_OPENNEAR"));

	m_ctrlOpeningNear = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		wxGenericValidator(&m_nOpeningNear));
	m_ctrlOpeningNear->SetHelpText(_("HIDC_OPT_CAL_OPENNEAR"));
	m_ctrlOpeningNear->SetToolTip(_("HIDC_OPT_CAL_OPENNEAR"));

	wxStaticText* textOpen = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_OPENNEAR"),
		wxDefaultPosition, wxDefaultSize, 0);
	textOpen->Wrap(-1);

	m_ctrlOpeningNearColor = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(20, 20), wxSUNKEN_BORDER);
	m_ctrlOpeningNearColor->Wrap(-1);
	m_ctrlOpeningNearColor->SetBackgroundColour(m_OpeningNear);

	m_ctrlOpeningNearSet = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_COLOR_OPENNEAR_SET"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlOpeningNearSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnCalColorOpeningNear), NULL, this);
	m_ctrlOpeningNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));
	m_ctrlOpeningNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));

	wxCheckBox* ctrlWarnClose = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_WARN_CLOSENEAR"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bClosingNear));
	ctrlWarnClose->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnCalNear), NULL, this);
	ctrlWarnClose->SetHelpText(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));
	ctrlWarnClose->SetToolTip(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));

	m_ctrlClosingNear = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		wxGenericValidator(&m_nClosingNear));
	m_ctrlClosingNear->SetHelpText(_("HIDC_OPT_CAL_CLOSENEAR"));
	m_ctrlClosingNear->SetToolTip(_("HIDC_OPT_CAL_CLOSENEAR"));

	wxStaticText* textClose = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_CLOSENEAR"),
		wxDefaultPosition, wxDefaultSize, 0);
	textClose->Wrap(-1);

	m_ctrlClosingNearColor = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(20, 20), wxSUNKEN_BORDER);
	m_ctrlClosingNearColor->Wrap(-1);
	m_ctrlClosingNearColor->SetBackgroundColour(m_ClosingNear);

	m_ctrlClosingNearSet = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_COLOR_CLOSENEAR_SET"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlClosingNearSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnCalColorClosingNear), NULL, this);
	m_ctrlClosingNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));
	m_ctrlClosingNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));

	wxStaticText* textDOW = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_DAY_OF_WEEK"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDOW->Wrap(-1);

	assert(ARBDate::eSunday == 0);
	wxString choicesDOW[] =
	{
		_("Sunday"),
		_("Monday"),
		_("Tuesday"),
		_("Wednesday"),
		_("Thursday"),
		_("Friday"),
		_("Saturday"),
	};
	int numChoicesDOW = sizeof(choicesDOW) / sizeof(choicesDOW[0]);
	m_ctrlDayOfWeek = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		numChoicesDOW, choicesDOW, 0);
	m_ctrlDayOfWeek->SetSelection(static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek()));
	m_ctrlDayOfWeek->SetHelpText(_("HIDC_OPT_CAL_DAY_OF_WEEK"));
	m_ctrlDayOfWeek->SetToolTip(_("HIDC_OPT_CAL_DAY_OF_WEEK"));

	wxCheckBox* ctrlAutoDel = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_AUTO_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bAutoDelete));
	ctrlAutoDel->SetHelpText(_("HIDC_OPT_CAL_AUTO_DELETE"));
	ctrlAutoDel->SetToolTip(_("HIDC_OPT_CAL_AUTO_DELETE"));

	wxCheckBox* ctrlHide = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_HIDE_OLD"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHideOld));
	ctrlHide->SetHelpText(_("HIDC_OPT_CAL_HIDE_OLD"));
	ctrlHide->SetToolTip(_("HIDC_OPT_CAL_HIDE_OLD"));

	wxStaticText* textPast1 = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_OLD_ENTRY_DAYS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPast1->Wrap(-1);

	wxTextCtrl* ctrlPast = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(30, -1), 0,
		wxGenericValidator(&m_Days));
	ctrlPast->SetHelpText(_("HIDC_OPT_CAL_OLD_ENTRY_DAYS"));
	ctrlPast->SetToolTip(_("HIDC_OPT_CAL_OLD_ENTRY_DAYS"));

	wxStaticText* textPast2 = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_OLD_ENTRY_DAYS2"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPast2->Wrap(-1);

	wxCheckBox* ctrlHideEntered = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_HIDE_OVERLAP"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHideOverlapping));
	ctrlHideEntered->SetHelpText(_("HIDC_OPT_CAL_HIDE_OVERLAP"));
	ctrlHideEntered->SetToolTip(_("HIDC_OPT_CAL_HIDE_OVERLAP"));

	wxCheckBox* ctrlViewOpen = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_OPENING"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bOpening));
	ctrlViewOpen->SetHelpText(_("HIDC_OPT_CAL_OPENING"));
	ctrlViewOpen->SetToolTip(_("HIDC_OPT_CAL_OPENING"));

	wxCheckBox* ctrlViewClose = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_CLOSING"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bClosing));
	ctrlViewClose->SetHelpText(_("HIDC_OPT_CAL_CLOSING"));
	ctrlViewClose->SetToolTip(_("HIDC_OPT_CAL_CLOSING"));

	wxStaticText* textCalEntry = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_CAL_ENTRIES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCalEntry->Wrap(-1);

	m_ctrlCalEntries = new wxChoice(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	m_ctrlCalEntries->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CDlgOptionsCalendar::OnSelchangeCalEntries), NULL, this);
	m_ctrlCalEntries->SetHelpText(_("HIDC_OPT_CAL_ENTRIES"));
	m_ctrlCalEntries->SetToolTip(_("HIDC_OPT_CAL_ENTRIES"));
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin();
		iColor != m_CalColors.end();
		++iColor)
	{
		m_ctrlCalEntries->Append(GetCalText((*iColor).first, false));
	}
	m_ctrlCalEntries->SetSelection(0);

	m_ctrlColor = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(20, 20), wxSUNKEN_BORDER);
	m_ctrlColor->Wrap(-1);
	SetCalColor();

	wxButton* btnColor = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_COLOR_SET"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnColor->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnCalColors), NULL, this);
	btnColor->SetHelpText(_("HIDC_OPT_CAL_COLOR_SET"));
	btnColor->SetToolTip(_("HIDC_OPT_CAL_COLOR_SET"));

	m_ctrlCalView = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);
	m_ctrlCalView->SetFont(m_fontCalView);
	SetRichText();

	wxButton* ctrlFont = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_FONT"),
		wxDefaultPosition, wxDefaultSize, 0);
	ctrlFont->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsCalendar::OnFontCalView), NULL, this);
	ctrlFont->SetHelpText(_("HIDC_OPT_CAL_FONT"));
	ctrlFont->SetToolTip(_("HIDC_OPT_CAL_FONT"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerCal = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerCalList = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_LIST")), wxVERTICAL);

	wxFlexGridSizer* sizerCalListItems = new wxFlexGridSizer(2, 2, 0, 0);
	sizerCalListItems->SetFlexibleDirection(wxBOTH);
	sizerCalListItems->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerOpen = new wxBoxSizer(wxVERTICAL);
	sizerOpen->Add(ctrlWarnOpen, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* sizerOpenDays = new wxBoxSizer(wxHORIZONTAL);
	sizerOpenDays->Add(15, 0, 0, 0, 5);
	sizerOpenDays->Add(m_ctrlOpeningNear, 0, wxLEFT|wxRIGHT, 5);
	sizerOpenDays->Add(textOpen, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerOpen->Add(sizerOpenDays, 0, 0, 5);

	sizerCalListItems->Add(sizerOpen, 0, 0, 5);

	wxBoxSizer* sizerOpenColor = new wxBoxSizer(wxHORIZONTAL);
	sizerOpenColor->Add(m_ctrlOpeningNearColor, 0, wxALIGN_RIGHT|wxALL, 5);
	sizerOpenColor->Add(m_ctrlOpeningNearSet, 0, wxALIGN_RIGHT|wxALL, 5);

	sizerCalListItems->Add(sizerOpenColor, 0, 0, 5);

	wxBoxSizer* sizerClose = new wxBoxSizer(wxVERTICAL);
	sizerClose->Add(ctrlWarnClose, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* sizerCloseDays = new wxBoxSizer(wxHORIZONTAL);
	sizerCloseDays->Add(15, 0, 0, 0, 5);
	sizerCloseDays->Add(m_ctrlClosingNear, 0, wxLEFT|wxRIGHT, 5);
	sizerCloseDays->Add(textClose, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerClose->Add(sizerCloseDays, 0, 0, 5);

	sizerCalListItems->Add(sizerClose, 0, 0, 5);

	wxBoxSizer* sizerCloseColor = new wxBoxSizer(wxHORIZONTAL);
	sizerCloseColor->Add(m_ctrlClosingNearColor, 0, wxALL, 5);
	sizerCloseColor->Add(m_ctrlClosingNearSet, 0, wxALIGN_RIGHT|wxALL, 5);

	sizerCalListItems->Add(sizerCloseColor, 0, 0, 5);

	sizerCalList->Add(sizerCalListItems, 0, 0, 5);

	sizerCal->Add(sizerCalList, 0, 0, 5);

	wxStaticBoxSizer* sizerCalView = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_VIEW")), wxHORIZONTAL);

	wxBoxSizer* sizerCalView2 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDOW = new wxBoxSizer(wxHORIZONTAL);
	sizerDOW->Add(textDOW, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDOW->Add(m_ctrlDayOfWeek, 0, wxALL, 5);

	sizerCalView2->Add(sizerDOW, 0, 0, 5);
	sizerCalView2->Add(ctrlAutoDel, 0, wxALL, 5);
	sizerCalView2->Add(ctrlHide, 0, wxLEFT|wxRIGHT|wxTOP, 5);

	wxBoxSizer* sizePast = new wxBoxSizer(wxHORIZONTAL);
	sizePast->Add(15, 0, 0, 0, 5);
	sizePast->Add(textPast1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizePast->Add(ctrlPast, 0, wxALL, 5);
	sizePast->Add(textPast2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerCalView2->Add(sizePast, 0, 0, 5);
	sizerCalView2->Add(ctrlHideEntered, 0, wxALL, 5);
	sizerCalView2->Add(ctrlViewOpen, 0, wxALL, 5);
	sizerCalView2->Add(ctrlViewClose, 0, wxALL, 5);

	sizerCalView->Add(sizerCalView2, 0, 0, 5);

	wxBoxSizer* sizerCalEntries = new wxBoxSizer(wxVERTICAL);
	sizerCalEntries->Add(textCalEntry, 0, wxLEFT|wxRIGHT|wxTOP, 5);

	wxBoxSizer* sizerEntryColor = new wxBoxSizer(wxHORIZONTAL);
	sizerEntryColor->Add(m_ctrlCalEntries, 0, wxALL, 5);
	sizerEntryColor->Add(m_ctrlColor, 0, wxALL, 5);
	sizerEntryColor->Add(btnColor, 0, wxALL, 5);

	sizerCalEntries->Add(sizerEntryColor, 0, 0, 5);
	sizerCalEntries->Add(m_ctrlCalView, 1, wxALL|wxEXPAND, 5);
	sizerCalEntries->Add(ctrlFont, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	sizerCalView->Add(sizerCalEntries, 0, wxEXPAND, 5);

	sizerCal->Add(sizerCalView, 0, 0, 5);

	SetSizer(sizerCal);
	Layout();
	sizerCal->Fit(this);

	UpdateControls();
}


void CDlgOptionsCalendar::Save()
{
	if (!m_bOpeningNear)
		m_nOpeningNear = -1;

	CAgilityBookOptions::SetCalendarOpeningNearColor(m_OpeningNear);
	CAgilityBookOptions::SetCalendarClosingNearColor(m_ClosingNear);
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin();
		iColor != m_CalColors.end();
		++iColor)
	{
		CAgilityBookOptions::SetCalendarColor((*iColor).first, (*iColor).second);
	}

	CAgilityBookOptions::SetCalendarOpeningNear(m_nOpeningNear);
	if (!m_bClosingNear)
		m_nClosingNear = -1;
	CAgilityBookOptions::SetCalendarClosingNear(m_nClosingNear);

	CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDate::DayOfWeek>(m_ctrlDayOfWeek->GetSelection()));
	CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_bAutoDelete);
	CAgilityBookOptions::SetViewAllCalendarEntries(!m_bHideOld);
	CAgilityBookOptions::SetDaysTillEntryIsPast(m_Days);
	CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_bHideOverlapping);
	CAgilityBookOptions::SetViewAllCalendarOpening(m_bOpening);
	CAgilityBookOptions::SetViewAllCalendarClosing(m_bClosing);
	CAgilityBookOptions::SetCalendarFontInfo(m_fontCalViewInfo);
}


wxString CDlgOptionsCalendar::GetCalText(
		CAgilityBookOptions::CalendarColorItem type,
		bool bForDisplay) const
{
	wxString text;
	switch (type)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		text += Localization()->CalendarNotEntered().c_str();
		if (bForDisplay)
			text += wxT(" Text");
		break;
	case CAgilityBookOptions::eCalColorPlanning:
		text += Localization()->CalendarPlanning().c_str();
		if (bForDisplay)
			text += wxT(" Text");
		break;
	case CAgilityBookOptions::eCalColorOpening:
		if (!bForDisplay)
			text += wxT("  ");
		text += _("IDS_COL_OPENING");
		if (bForDisplay)
			text += wxT(" Text");
		break;
	case CAgilityBookOptions::eCalColorClosing:
		if (!bForDisplay)
			text += wxT("  ");
		text += _("IDS_COL_CLOSING");
		if (bForDisplay)
			text += wxT(" Text");
		break;
	case CAgilityBookOptions::eCalColorEntered:
		text += Localization()->CalendarEntered().c_str();
		if (bForDisplay)
			text += wxT(" Text");
		break;
	}
	return text;
}


void CDlgOptionsCalendar::UpdateControls()
{
	m_ctrlOpeningNear->Enable(m_bOpeningNear);
	m_ctrlOpeningNearSet->Enable(m_bOpeningNear);
	m_ctrlClosingNear->Enable(m_bClosingNear);
	m_ctrlClosingNearSet->Enable(m_bClosingNear);
}


void CDlgOptionsCalendar::SetCalColor()
{
	int idx = m_ctrlCalEntries->GetSelection();
	if (0 <= idx)
	{
		m_ctrlColor->SetBackgroundColour(m_CalColors[idx].second);
		m_ctrlColor->Refresh();
	}
}


void CDlgOptionsCalendar::SetRichText()
{
	m_ctrlCalView->Clear();

	std::vector<long> endLines;
	endLines.push_back(-1);

	wxString data;
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin();
		iColor != m_CalColors.end();
		++iColor)
	{
		data += GetCalText((*iColor).first, true);
		data += wxT('\n');
		endLines.push_back(data.length());
	}

	m_ctrlCalView->ChangeValue(data);

	for (size_t i = 1; i < endLines.size(); ++i)
	{
		wxTextAttr style;
		style.SetTextColour(m_CalColors[i-1].second);
		m_ctrlCalView->SetStyle(endLines[i-1], endLines[i], style);
	}
}


void CDlgOptionsCalendar::OnSelchangeCalEntries(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	SetCalColor();
	SetRichText();
}


void CDlgOptionsCalendar::OnCalNear(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateControls();
}


void CDlgOptionsCalendar::OnCalColorOpeningNear(wxCommandEvent& evt)
{
	wxColourData data;
	data.SetColour(m_OpeningNear);
	wxColourDialog dlg(this, &data);
	if (wxID_OK == dlg.ShowModal())
	{
		m_OpeningNear = dlg.GetColourData().GetColour();
		m_ctrlOpeningNearColor->SetBackgroundColour(m_OpeningNear);
		m_ctrlOpeningNearColor->Refresh();
	}
}


void CDlgOptionsCalendar::OnCalColorClosingNear(wxCommandEvent& evt)
{
	wxColourData data;
	data.SetColour(m_ClosingNear);
	wxColourDialog dlg(this, &data);
	if (wxID_OK == dlg.ShowModal())
	{
		m_ClosingNear = dlg.GetColourData().GetColour();
		m_ctrlClosingNearColor->SetBackgroundColour(m_ClosingNear);
		m_ctrlClosingNearColor->Refresh();
	}
}


void CDlgOptionsCalendar::OnCalColors(wxCommandEvent& evt)
{
	int idx = m_ctrlCalEntries->GetSelection();
	if (0 <= idx)
	{
		wxColourData data;
		data.SetColour(m_CalColors[idx].second);
		wxColourDialog dlg(this, &data);
		if (wxID_OK == dlg.ShowModal())
		{
			m_CalColors[idx].second = dlg.GetColourData().GetColour();
			SetCalColor();
			SetRichText();
		}
	}
}


void CDlgOptionsCalendar::OnFontCalView(wxCommandEvent& evt)
{
	wxFontData data;
	data.SetAllowSymbols(false);
	data.SetInitialFont(m_fontCalView);
	wxFontDialog dlg(this, data);
	if (wxID_OK == dlg.ShowModal())
	{
		m_fontCalViewInfo.CreateFont(dlg, m_fontCalView);
		m_ctrlCalView->SetFont(m_fontCalView);
	}
}
