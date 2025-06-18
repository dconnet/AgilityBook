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
 * 2015-01-01 Changed pixels to dialog units.
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

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Widgets.h"
#include <wx/colordlg.h>
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
wxString ForDisplay(wxString const& text)
{
	return wxString::Format(_("IDS_CALENDAR_DISPLAY_COLOR"), text);
}
} // namespace


CDlgOptionsCalendar::CDlgOptionsCalendar(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_fontCalViewInfo()
	, m_fontCalView()
	, m_OpeningNear(CAgilityBookOptions::CalendarColor(ARBCalColorItem::OpeningNear))
	, m_ClosingNear(CAgilityBookOptions::CalendarColor(ARBCalColorItem::ClosingNear))
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

	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Past, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::NotEntered, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Planning, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Opening, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Closing, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Pending, wxColour()));
	m_CalColors.push_back(tColorInfo(ARBCalColorItem::Entered, wxColour()));
	std::vector<tColorInfo>::iterator iColor;
	for (iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		(*iColor).second = CAgilityBookOptions::CalendarColor((*iColor).first);
	}

	// Controls (these are done first to control tab order)

	wxStaticBox* boxCalList = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_LIST"));

	wxCheckBox* ctrlWarnOpen = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_WARN_OPENNEAR"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bOpeningNear));
	ctrlWarnOpen->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgOptionsCalendar::OnCalNear, this);
	ctrlWarnOpen->SetHelpText(_("HIDC_OPT_CAL_WARN_OPENNEAR"));
	ctrlWarnOpen->SetToolTip(_("HIDC_OPT_CAL_WARN_OPENNEAR"));

	m_ctrlOpeningNear = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		wxGenericValidator(&m_nOpeningNear));
	m_ctrlOpeningNear->SetHelpText(_("HIDC_OPT_CAL_OPENNEAR"));
	m_ctrlOpeningNear->SetToolTip(_("HIDC_OPT_CAL_OPENNEAR"));

	wxStaticText* textOpen
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_OPENNEAR"), wxDefaultPosition, wxDefaultSize, 0);
	textOpen->Wrap(-1);

	m_ctrlOpeningNearColor = new wxStaticText(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(10, 10)),
		wxSUNKEN_BORDER);
	m_ctrlOpeningNearColor->Wrap(-1);
	m_ctrlOpeningNearColor->SetBackgroundColour(m_OpeningNear);

	m_ctrlOpeningNearSet
		= new wxButton(this, wxID_ANY, _("IDC_OPT_CAL_COLOR_OPENNEAR_SET"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlOpeningNearSet->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOptionsCalendar::OnCalColorOpeningNear, this);
	m_ctrlOpeningNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));
	m_ctrlOpeningNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_OPENNEAR_SET"));

	wxCheckBox* ctrlWarnClose = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_WARN_CLOSENEAR"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bClosingNear));
	ctrlWarnClose->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgOptionsCalendar::OnCalNear, this);
	ctrlWarnClose->SetHelpText(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));
	ctrlWarnClose->SetToolTip(_("HIDC_OPT_CAL_WARN_CLOSENEAR"));

	m_ctrlClosingNear = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		wxGenericValidator(&m_nClosingNear));
	m_ctrlClosingNear->SetHelpText(_("HIDC_OPT_CAL_CLOSENEAR"));
	m_ctrlClosingNear->SetToolTip(_("HIDC_OPT_CAL_CLOSENEAR"));

	wxStaticText* textClose
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_CLOSENEAR"), wxDefaultPosition, wxDefaultSize, 0);
	textClose->Wrap(-1);

	m_ctrlClosingNearColor = new wxStaticText(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(10, 10)),
		wxSUNKEN_BORDER);
	m_ctrlClosingNearColor->Wrap(-1);
	m_ctrlClosingNearColor->SetBackgroundColour(m_ClosingNear);

	m_ctrlClosingNearSet
		= new wxButton(this, wxID_ANY, _("IDC_OPT_CAL_COLOR_CLOSENEAR_SET"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlClosingNearSet->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOptionsCalendar::OnCalColorClosingNear, this);
	m_ctrlClosingNearSet->SetHelpText(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));
	m_ctrlClosingNearSet->SetToolTip(_("HIDC_OPT_CAL_COLOR_CLOSENEAR_SET"));

	wxStaticBox* boxCalView = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_CAL_VIEW"));

	wxStaticText* textDOW
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_DAY_OF_WEEK"), wxDefaultPosition, wxDefaultSize, 0);
	textDOW->Wrap(-1);

	assert(static_cast<size_t>(ARBDayOfWeek::Sunday) == 0);
	wxString choicesDOW[] = {
		_("Sunday"),
		_("Monday"),
		_("Tuesday"),
		_("Wednesday"),
		_("Thursday"),
		_("Friday"),
		_("Saturday"),
	};
	int numChoicesDOW = sizeof(choicesDOW) / sizeof(choicesDOW[0]);
	m_ctrlDayOfWeek = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, numChoicesDOW, choicesDOW, 0);
	m_ctrlDayOfWeek->SetSelection(static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek()));
	m_ctrlDayOfWeek->SetHelpText(_("HIDC_OPT_CAL_DAY_OF_WEEK"));
	m_ctrlDayOfWeek->SetToolTip(_("HIDC_OPT_CAL_DAY_OF_WEEK"));

	wxCheckBox* ctrlAutoDel = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_AUTO_DELETE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bAutoDelete));
	ctrlAutoDel->SetHelpText(_("HIDC_OPT_CAL_AUTO_DELETE"));
	ctrlAutoDel->SetToolTip(_("HIDC_OPT_CAL_AUTO_DELETE"));

	wxCheckBox* ctrlHide = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_HIDE_OLD"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bHideOld));
	ctrlHide->SetHelpText(_("HIDC_OPT_CAL_HIDE_OLD"));
	ctrlHide->SetToolTip(_("HIDC_OPT_CAL_HIDE_OLD"));

	wxStaticText* textPast1
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_OLD_ENTRY_DAYS"), wxDefaultPosition, wxDefaultSize, 0);
	textPast1->Wrap(-1);

	CTextCtrl* ctrlPast = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		wxGenericValidator(&m_Days));
	ctrlPast->SetHelpText(_("HIDC_OPT_CAL_OLD_ENTRY_DAYS"));
	ctrlPast->SetToolTip(_("HIDC_OPT_CAL_OLD_ENTRY_DAYS"));

	wxStaticText* textPast2
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_OLD_ENTRY_DAYS2"), wxDefaultPosition, wxDefaultSize, 0);
	textPast2->Wrap(-1);

	wxCheckBox* ctrlHideEntered = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_HIDE_OVERLAP"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bHideOverlapping));
	ctrlHideEntered->SetHelpText(_("HIDC_OPT_CAL_HIDE_OVERLAP"));
	ctrlHideEntered->SetToolTip(_("HIDC_OPT_CAL_HIDE_OVERLAP"));

	wxCheckBox* ctrlViewOpen = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_OPENING"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bOpening));
	ctrlViewOpen->SetHelpText(_("HIDC_OPT_CAL_OPENING"));
	ctrlViewOpen->SetToolTip(_("HIDC_OPT_CAL_OPENING"));

	wxCheckBox* ctrlViewClose = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_OPT_CAL_CLOSING"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_bClosing));
	ctrlViewClose->SetHelpText(_("HIDC_OPT_CAL_CLOSING"));
	ctrlViewClose->SetToolTip(_("HIDC_OPT_CAL_CLOSING"));

	wxStaticText* textCalEntry
		= new wxStaticText(this, wxID_ANY, _("IDC_OPT_CAL_ENTRIES"), wxDefaultPosition, wxDefaultSize, 0);
	textCalEntry->Wrap(-1);

	m_ctrlCalEntries = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, 0);
	m_ctrlCalEntries->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &CDlgOptionsCalendar::OnSelchangeCalEntries, this);
	m_ctrlCalEntries->SetHelpText(_("HIDC_OPT_CAL_ENTRIES"));
	m_ctrlCalEntries->SetToolTip(_("HIDC_OPT_CAL_ENTRIES"));
	for (iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		m_ctrlCalEntries->Append(GetCalText((*iColor).first, false));
	}
	m_ctrlCalEntries->SetSelection(0);

	m_ctrlColor = new wxStaticText(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(10, 10)),
		wxSUNKEN_BORDER);
	m_ctrlColor->Wrap(-1);
	SetCalColor();

	wxButton* btnColor = new wxButton(this, wxID_ANY, _("IDC_OPT_CAL_COLOR_SET"), wxDefaultPosition, wxDefaultSize, 0);
	btnColor->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOptionsCalendar::OnCalColors, this);
	btnColor->SetHelpText(_("HIDC_OPT_CAL_COLOR_SET"));
	btnColor->SetToolTip(_("HIDC_OPT_CAL_COLOR_SET"));

	m_ctrlCalView = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	m_ctrlCalView->SetFont(m_fontCalView);
	m_ctrlCalView->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	SetRichText();

	wxButton* ctrlFont = new wxButton(this, wxID_ANY, _("IDC_OPT_CAL_FONT"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlFont->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOptionsCalendar::OnFontCalView, this);
	ctrlFont->SetHelpText(_("HIDC_OPT_CAL_FONT"));
	ctrlFont->SetToolTip(_("HIDC_OPT_CAL_FONT"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerCalList = new wxStaticBoxSizer(boxCalList, wxVERTICAL);

	wxFlexGridSizer* sizerCalListItems = new wxFlexGridSizer(2, 2, padding.Controls(), padding.Controls());
	sizerCalListItems->SetFlexibleDirection(wxBOTH);
	sizerCalListItems->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerOpen = new wxBoxSizer(wxVERTICAL);
	sizerOpen->Add(ctrlWarnOpen);

	wxBoxSizer* sizerOpenDays = new wxBoxSizer(wxHORIZONTAL);
	sizerOpenDays->AddSpacer(padding.CheckboxOffset());
	sizerOpenDays->Add(m_ctrlOpeningNear, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerOpenDays->Add(textOpen, 0, wxALIGN_CENTER_VERTICAL);

	sizerOpen->Add(sizerOpenDays, 0, wxTOP, padding.Tight());

	sizerCalListItems->Add(sizerOpen);

	wxBoxSizer* sizerOpenColor = new wxBoxSizer(wxHORIZONTAL);
	sizerOpenColor->Add(m_ctrlOpeningNearColor, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerOpenColor->Add(m_ctrlOpeningNearSet, 0, wxALIGN_CENTER_VERTICAL);

	sizerCalListItems->Add(sizerOpenColor);

	wxBoxSizer* sizerClose = new wxBoxSizer(wxVERTICAL);
	sizerClose->Add(ctrlWarnClose);

	wxBoxSizer* sizerCloseDays = new wxBoxSizer(wxHORIZONTAL);
	sizerCloseDays->AddSpacer(padding.CheckboxOffset());
	sizerCloseDays->Add(m_ctrlClosingNear, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerCloseDays->Add(textClose, 0, wxALIGN_CENTER_VERTICAL);

	sizerClose->Add(sizerCloseDays, 0, wxTOP, padding.Tight());

	sizerCalListItems->Add(sizerClose);

	wxBoxSizer* sizerCloseColor = new wxBoxSizer(wxHORIZONTAL);
	sizerCloseColor->Add(m_ctrlClosingNearColor, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerCloseColor->Add(m_ctrlClosingNearSet, 0, wxALIGN_CENTER_VERTICAL);

	sizerCalListItems->Add(sizerCloseColor);

	sizerCalList->Add(sizerCalListItems, 0, wxALL, padding.Inner());

	bSizer->Add(sizerCalList, 0, wxALL, padding.Controls());

	wxStaticBoxSizer* sizerCalView = new wxStaticBoxSizer(boxCalView, wxHORIZONTAL);
	wxBoxSizer* sizerCalView2 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDOW = new wxBoxSizer(wxHORIZONTAL);
	sizerDOW->Add(textDOW, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerDOW->Add(m_ctrlDayOfWeek, 0, wxALIGN_CENTER_VERTICAL);

	sizerCalView2->Add(sizerDOW, 0, wxBOTTOM, padding.Controls());
	sizerCalView2->Add(ctrlAutoDel, 0, wxBOTTOM, padding.Controls());
	sizerCalView2->Add(ctrlHide, 0, wxBOTTOM, padding.TightControls());

	wxBoxSizer* sizePast = new wxBoxSizer(wxHORIZONTAL);
	sizePast->AddSpacer(padding.CheckboxOffset());
	sizePast->Add(textPast1, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizePast->Add(ctrlPast, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizePast->Add(textPast2, 0, wxALIGN_CENTER_VERTICAL);

	sizerCalView2->Add(sizePast, 0, wxBOTTOM, padding.Controls());
	sizerCalView2->Add(ctrlHideEntered, 0, wxBOTTOM, padding.Controls());
	sizerCalView2->Add(ctrlViewOpen, 0, wxBOTTOM, padding.Controls());
	sizerCalView2->Add(ctrlViewClose);

	sizerCalView->Add(sizerCalView2, 0, wxALL, padding.Inner());

	wxBoxSizer* sizerCalEntries = new wxBoxSizer(wxVERTICAL);
	sizerCalEntries->Add(textCalEntry, 0, wxBOTTOM, padding.Controls());

	wxBoxSizer* sizerEntryColor = new wxBoxSizer(wxHORIZONTAL);
	sizerEntryColor->Add(m_ctrlCalEntries, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerEntryColor->Add(m_ctrlColor, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerEntryColor->Add(btnColor, 0, wxALIGN_CENTER_VERTICAL);

	sizerCalEntries->Add(sizerEntryColor, 0, wxBOTTOM, padding.TightControls());
	sizerCalEntries->Add(m_ctrlCalView, 1, wxEXPAND | wxBOTTOM, padding.Controls());
	sizerCalEntries->Add(ctrlFont, 0, wxALIGN_CENTER_HORIZONTAL);

	sizerCalView->Add(sizerCalEntries, 0, wxALL, padding.Inner());

	bSizer->Add(sizerCalView, 0, wxALL, padding.Controls());

	SetSizer(bSizer);
	Layout();
	bSizer->Fit(this);

	UpdateControls();
}


void CDlgOptionsCalendar::Save()
{
	if (!m_bOpeningNear)
		m_nOpeningNear = -1;

	CAgilityBookOptions::SetCalendarColor(ARBCalColorItem::OpeningNear, m_OpeningNear);
	CAgilityBookOptions::SetCalendarColor(ARBCalColorItem::ClosingNear, m_ClosingNear);
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		CAgilityBookOptions::SetCalendarColor((*iColor).first, (*iColor).second);
	}

	CAgilityBookOptions::SetCalendarOpeningNear(m_nOpeningNear);
	if (!m_bClosingNear)
		m_nClosingNear = -1;
	CAgilityBookOptions::SetCalendarClosingNear(m_nClosingNear);

	CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDayOfWeek>(m_ctrlDayOfWeek->GetSelection()));
	CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_bAutoDelete);
	CAgilityBookOptions::SetViewAllCalendarEntries(!m_bHideOld);
	CAgilityBookOptions::SetDaysTillEntryIsPast(m_Days);
	CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_bHideOverlapping);
	CAgilityBookOptions::SetViewAllCalendarOpening(m_bOpening);
	CAgilityBookOptions::SetViewAllCalendarClosing(m_bClosing);
	CAgilityBookOptions::SetCalendarFontInfo(m_fontCalViewInfo);
}


wxString CDlgOptionsCalendar::GetCalText(ARBCalColorItem type, bool bForDisplay) const
{
	wxString text;
	switch (type)
	{
	case ARBCalColorItem::OpeningNear:
	case ARBCalColorItem::ClosingNear:
		// These are not needed here.
		// This is only used for populating the Calendar Entries section.
		break;
	case ARBCalColorItem::Past:
		text = Localization()->CalendarPast();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::NotEntered:
		text = Localization()->CalendarNotEntered();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::Planning:
		text += Localization()->CalendarPlanning();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::Opening:
		if (!bForDisplay)
			text = L"  ";
		text += _("IDS_COL_OPENING");
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::Closing:
		if (!bForDisplay)
			text = L"  ";
		text += _("IDS_COL_CLOSING");
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::Pending:
		text = Localization()->CalendarPending();
		if (bForDisplay)
			text = ForDisplay(text);
		break;
	case ARBCalColorItem::Entered:
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

	wxString data;
	for (std::vector<tColorInfo>::iterator iColor = m_CalColors.begin(); iColor != m_CalColors.end(); ++iColor)
	{
		data += GetCalText((*iColor).first, true);
		data += L"\n";
		endLines.push_back(static_cast<long>(data.length()));
	}

	m_ctrlCalView->ChangeValue(data);

	for (size_t i = 1; i < endLines.size(); ++i)
	{
		wxTextAttr style;
		style.SetTextColour(m_CalColors[i - 1].second);
		m_ctrlCalView->SetStyle(endLines[i - 1], endLines[i], style);
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

} // namespace dconSoft
