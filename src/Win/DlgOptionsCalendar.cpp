/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-01-21 Fixed calendar font selection.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-14 Fixed group box creation order.
 * 2009-02-11 Ported to wxWidgets.
 * 2009-01-11 Fixed a bug that added an 8th day to week.
 * 2006-11-22 Turned this into Calendar only, created new Print page.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2003-08-09 Moved fonts to new page.
 * 2003-07-31 Allow screen fonts for printer font selection. Also, the
 *            wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "DlgOptionsCalendar.h"

#include "Widgets.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/StringUtil.h"
#include <wx/colordlg.h>
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
	, m_ctrlOpeningNear(nullptr)
	, m_ctrlOpeningNearColor(nullptr)
	, m_ctrlOpeningNearSet(nullptr)
	, m_ctrlClosingNear(nullptr)
	, m_ctrlClosingNearColor(nullptr)
	, m_ctrlClosingNearSet(nullptr)
	, m_ctrlDayOfWeek(nullptr)
	, m_ctrlCalEntries(nullptr)
	, m_ctrlColor(nullptr)
	, m_ctrlCalView(nullptr)
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

	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorPast, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorNotEntered, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorPlanning, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorOpening, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorClosing, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorPending, wxColour()));
	m_CalColors.push_back(tColorInfo(CAgilityBookOptions::eCalColorEntered, wxColour()));
	std::vector<tColorInfo>::iterator iColor;
	for (iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		(*iColor).second = CAgilityBookOptions::CalendarColor((*iColor).first);
	}

	// Controls (these are done first to control tab order)

	wxStaticBox* boxCalList = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_LIST"));

	wxCheckBox* ctrlWarnOpen = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_WARN_OPENNEAR"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bOpeningNear));
	BIND_OR_CONNECT_CTRL(ctrlWarnOpen, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnCalNear);
	ctrlWarnOpen->SetHelpText(_("HIDC_OPT_CAL_WARN_OPENNEAR"));
	ctrlWarnOpen->SetToolTip(_("HIDC_OPT_CAL_WARN_OPENNEAR"));

	m_ctrlOpeningNear = new CTextCtrl(this, wxID_ANY, wxEmptyString,
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
	BIND_OR_CONNECT_CTRL(m_ctrlOpeningNearSet, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnCalColorOpeningNear);
	m_ctrlOpeningNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));
	m_ctrlOpeningNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));

	wxCheckBox* ctrlWarnClose = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_CAL_WARN_CLOSENEAR"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bClosingNear));
	BIND_OR_CONNECT_CTRL(ctrlWarnClose, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnCalNear);
	ctrlWarnClose->SetHelpText(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));
	ctrlWarnClose->SetToolTip(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));

	m_ctrlClosingNear = new CTextCtrl(this, wxID_ANY, wxEmptyString,
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
	BIND_OR_CONNECT_CTRL(m_ctrlClosingNearSet, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnCalColorClosingNear);
	m_ctrlClosingNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));
	m_ctrlClosingNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));

	wxStaticBox* boxCalView = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_VIEW"));

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

	CTextCtrl* ctrlPast = new CTextCtrl(this, wxID_ANY, wxEmptyString,
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
		0, nullptr, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlCalEntries, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler, CDlgOptionsCalendar::OnSelchangeCalEntries);
	m_ctrlCalEntries->SetHelpText(_("HIDC_OPT_CAL_ENTRIES"));
	m_ctrlCalEntries->SetToolTip(_("HIDC_OPT_CAL_ENTRIES"));
	for (iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		m_ctrlCalEntries->Append(StringUtil::stringWX(GetCalText((*iColor).first, false)));
	}
	m_ctrlCalEntries->SetSelection(0);

	m_ctrlColor = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(20, 20), wxSUNKEN_BORDER);
	m_ctrlColor->Wrap(-1);
	SetCalColor();

	wxButton* btnColor = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_COLOR_SET"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnColor, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnCalColors);
	btnColor->SetHelpText(_("HIDC_OPT_CAL_COLOR_SET"));
	btnColor->SetToolTip(_("HIDC_OPT_CAL_COLOR_SET"));

	m_ctrlCalView = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);
	m_ctrlCalView->SetFont(m_fontCalView);
	SetRichText();

	wxButton* ctrlFont = new wxButton(this, wxID_ANY,
		_("IDC_OPT_CAL_FONT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(ctrlFont, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsCalendar::OnFontCalView);
	ctrlFont->SetHelpText(_("HIDC_OPT_CAL_FONT"));
	ctrlFont->SetToolTip(_("HIDC_OPT_CAL_FONT"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerCal = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerCalList = new wxStaticBoxSizer(boxCalList, wxVERTICAL);

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

	wxStaticBoxSizer* sizerCalView = new wxStaticBoxSizer(boxCalView, wxHORIZONTAL);

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

	sizerCalView->Add(sizerCalEntries, 0, wxEXPAND, 0);

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


static std::wstring ForDisplay(std::wstring const& text)
{
	return StringUtil::stringW(wxString::Format(_("IDS_CALENDAR_DISPLAY_COLOR"), text.c_str()));
}


std::wstring CDlgOptionsCalendar::GetCalText(
		CAgilityBookOptions::CalendarColorItem type,
		bool bForDisplay) const
{
	std::wstring text;
	switch (type)
	{
	case CAgilityBookOptions::eCalColorPast:
		text = Localization()->CalendarPast();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorNotEntered:
		text = Localization()->CalendarNotEntered();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorPlanning:
		text += Localization()->CalendarPlanning();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorOpening:
		if (!bForDisplay)
			text = L"  ";
		text += _("IDS_COL_OPENING");
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorClosing:
		if (!bForDisplay)
			text = L"  ";
		text += _("IDS_COL_CLOSING");
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorPending:
		text = Localization()->CalendarPending();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case CAgilityBookOptions::eCalColorEntered:
		text = Localization()->CalendarEntered();
		if (bForDisplay)
			text = ForDisplay(text);
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
	endLines.push_back(0);

	std::wstring data;
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin();
		iColor != m_CalColors.end();
		++iColor)
	{
		data += GetCalText((*iColor).first, true);
		data += L"\n";
		endLines.push_back(static_cast<long>(data.length()));
	}

	m_ctrlCalView->ChangeValue(StringUtil::stringWX(data));

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
	data.EnableEffects(false);
	wxFontDialog dlg(this, data);
	if (wxID_OK == dlg.ShowModal())
	{
		m_fontCalViewInfo.CreateFont(dlg, m_fontCalView);
		m_ctrlCalView->SetFont(m_fontCalView);
		SetCalColor();
		SetRichText();
	}
}
