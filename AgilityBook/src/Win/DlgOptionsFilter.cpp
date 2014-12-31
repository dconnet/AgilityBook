/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-25 Fixed dates (again). Damn bool->time_t autoconversion.
 * 2009-08-10 One group box was still off, dates are not stored
 *            correctly, and saving named filters is duplicating names.
 * 2009-07-14 Fixed group box creation order.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-09-01 Fix setting of 'After' (start) date filter.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2003-08-09 Moved fonts to new page.
 * 2003-07-31 Allow screen fonts for printer font selection. Also, the
 *            wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "DlgOptionsFilter.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "CheckTreeCtrl.h"
#include "ComboBoxes.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgOptionsFilter::CDlgOptionsFilter(
		wxWindow* parent,
		CAgilityBookDoc* pDoc)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_pDoc(pDoc)
	, m_FilterOptions()
	, m_FilterName()
	, m_bDateStart(false)
	, m_timeStart(ARBDate::Today())
	, m_bDateEnd(false)
	, m_timeEnd(ARBDate::Today())
	, m_bNotEntered(m_FilterOptions.FilterCalendarView().ViewNotEntered())
	, m_bPlanning(m_FilterOptions.FilterCalendarView().ViewPlanning())
	, m_bEntered(m_FilterOptions.FilterCalendarView().ViewEntered())
	, m_ctrlFilters(nullptr)
	, m_ctrlDatesAll(nullptr)
	, m_ctrlDatesSome(nullptr)
	, m_ctrlDateStartCheck(nullptr)
	, m_ctrlDateStart(nullptr)
	, m_ctrlDateEndCheck(nullptr)
	, m_ctrlDateEnd(nullptr)
	, m_ctrlLogAll(nullptr)
	, m_ctrlLogSome(nullptr)
	, m_ctrlNames(nullptr)
	, m_ctrlQsAll(nullptr)
	, m_ctrlQsQs(nullptr)
	, m_ctrlQsNonQs(nullptr)
	, m_ctrlVenueAll(nullptr)
	, m_ctrlVenueSome(nullptr)
	, m_ctrlVenue(nullptr)
{
	// Controls (these are done first to control tab order)

	wxStaticBox* boxFilters = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_NAMES"));

	m_ctrlFilters = new CAutoFillComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN,
		CTrimValidator(&m_FilterName, TRIMVALIDATOR_TRIM_BOTH));
	BIND_OR_CONNECT_CTRL(m_ctrlFilters, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnSelchangeFilterNames);
	m_ctrlFilters->SetHelpText(_("HIDC_OPT_FILTER_NAMES"));
	m_ctrlFilters->SetToolTip(_("HIDC_OPT_FILTER_NAMES"));
	std::vector<std::wstring> filterNames;
	m_FilterOptions.GetAllFilterNames(filterNames, true);
	wxArrayString choices;
	for (std::vector<std::wstring>::iterator iterName = filterNames.begin();
		iterName != filterNames.end();
		++iterName)
	{
		wxString wxName(StringUtil::stringWX(*iterName));
		int idx = m_ctrlFilters->Append(wxName);
		choices.Add(wxName);
		if ((*iterName) == m_FilterOptions.GetCurrentFilter())
		{
			m_FilterName = StringUtil::stringWX(m_FilterOptions.GetCurrentFilter());
			m_ctrlFilters->SetSelection(idx);
		}
	}
	m_ctrlFilters->AutoComplete(choices);

	wxButton* btnSave = new wxButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_NAMES_SAVE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnSave, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsFilter::OnClickedOptFilterNamesSave);
	btnSave->SetHelpText(_("HIDC_OPT_FILTER_NAMES_SAVE"));
	btnSave->SetToolTip(_("HIDC_OPT_FILTER_NAMES_SAVE"));

	wxButton* btnDelete = new wxButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_NAMES_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsFilter::OnClickedOptFilterNamesDelete);
	btnDelete->SetHelpText(_("HIDC_OPT_FILTER_NAMES_DELETE"));
	btnDelete->SetToolTip(_("HIDC_OPT_FILTER_NAMES_DELETE"));

	wxStaticBox* boxDates = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_DATE"));

	m_ctrlDatesAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlDatesAll, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterDates);
	m_ctrlDatesAll->SetHelpText(_("HIDC_OPT_FILTER_DATE_ALL"));
	m_ctrlDatesAll->SetToolTip(_("HIDC_OPT_FILTER_DATE_ALL"));

	m_ctrlDatesSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_RANGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDatesSome, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterDates);
	m_ctrlDatesSome->SetHelpText(_("HIDC_OPT_FILTER_DATE_RANGE"));
	m_ctrlDatesSome->SetToolTip(_("HIDC_OPT_FILTER_DATE_RANGE"));

	m_ctrlDateStartCheck = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_START_CHECK"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_bDateStart));
	m_ctrlDateStartCheck->SetHelpText(_("HIDC_OPT_FILTER_DATE_START_CHECK"));
	m_ctrlDateStartCheck->SetToolTip(_("HIDC_OPT_FILTER_DATE_START_CHECK"));

	m_ctrlDateStart = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_timeStart));
	m_ctrlDateStart->SetHelpText(_("HIDC_OPT_FILTER_DATE_START"));
	m_ctrlDateStart->SetToolTip(_("HIDC_OPT_FILTER_DATE_START"));

	m_ctrlDateEndCheck = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_END_CHECK"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_bDateEnd));
	m_ctrlDateEndCheck->SetHelpText(_("HIDC_OPT_FILTER_DATE_END_CHECK"));
	m_ctrlDateEndCheck->SetToolTip(_("HIDC_OPT_FILTER_DATE_END_CHECK"));

	m_ctrlDateEnd = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_timeEnd));
	m_ctrlDateEnd->SetHelpText(_("HIDC_OPT_FILTER_DATE_END"));
	m_ctrlDateEnd->SetToolTip(_("HIDC_OPT_FILTER_DATE_END"));

	wxStaticBox* boxCal1 = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_CAL"));

	wxCheckBox* ctrlCalNot = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_FILTER_CAL_NOT_ENTERED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bNotEntered));
	ctrlCalNot->SetHelpText(_("HIDC_OPT_FILTER_CAL_NOT_ENTERED"));
	ctrlCalNot->SetToolTip(_("HIDC_OPT_FILTER_CAL_NOT_ENTERED"));

	wxCheckBox* ctrlCalPlan = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_FILTER_CAL_PLANNING"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bPlanning));
	ctrlCalPlan->SetHelpText(_("HIDC_OPT_FILTER_CAL_PLANNING"));
	ctrlCalPlan->SetToolTip(_("HIDC_OPT_FILTER_CAL_PLANNING"));

	wxCheckBox* ctrlCalEntered = new wxCheckBox(this, wxID_ANY,
		_("IDC_OPT_FILTER_CAL_ENTERED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bEntered));
	ctrlCalEntered->SetHelpText(_("HIDC_OPT_FILTER_CAL_ENTERED"));
	ctrlCalEntered->SetToolTip(_("HIDC_OPT_FILTER_CAL_ENTERED"));

	wxStaticBox* boxLog = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_LOG_NAME"));

	m_ctrlLogAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_LOG_NAME_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlLogAll, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterLog);
	m_ctrlLogAll->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME_ALL"));
	m_ctrlLogAll->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME_ALL"));

	m_ctrlLogSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_LOG_NAME_SELECTED"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlLogSome, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterLog);
	m_ctrlLogSome->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME_SELECTED"));
	m_ctrlLogSome->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME_SELECTED"));

	m_ctrlNames = new wxCheckListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(-1, 80),
		0, nullptr, wxLB_SORT);
	BIND_OR_CONNECT_CTRL(m_ctrlNames, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterLogNames);
	m_ctrlNames->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME"));
	m_ctrlNames->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME"));

	wxStaticBox* boxQs = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_RUN_RUNS"));

	m_ctrlQsAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlQsAll, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterQs);
	m_ctrlQsAll->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_ALL"));
	m_ctrlQsAll->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_ALL"));

	m_ctrlQsQs = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlQsQs, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterQs);
	m_ctrlQsQs->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_Q"));
	m_ctrlQsQs->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_Q"));

	m_ctrlQsNonQs = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_NON_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlQsNonQs, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterQs);
	m_ctrlQsNonQs->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_NON_Q"));
	m_ctrlQsNonQs->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_NON_Q"));

	wxStaticBox* boxVenue = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_RUN_VENUES"));

	m_ctrlVenueAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_VENUES_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlVenueAll, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterVenues);
	m_ctrlVenueAll->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES_ALL"));
	m_ctrlVenueAll->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES_ALL"));

	m_ctrlVenueSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_VENUES_SELECTED"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlVenueSome, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CDlgOptionsFilter::OnFilterVenues);
	m_ctrlVenueSome->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES_SELECTED"));
	m_ctrlVenueSome->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES_SELECTED"));

	m_ctrlVenue = new CCheckTreeCtrl(this);
	BIND_OR_CONNECT_CTRL(m_ctrlVenue, wxEVT_COMMAND_TREE_CHECK_CHANGED, wxEventHandler, CDlgOptionsFilter::OnFilterVenueCheck);
	m_ctrlVenue->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES"));
	m_ctrlVenue->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES"));

	// Sizers
#pragma PRAGMA_TODO(convert to dialog units)

	wxBoxSizer* sizerFilter = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerCol1 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerFilters = new wxStaticBoxSizer(boxFilters, wxVERTICAL);
	sizerFilters->Add(m_ctrlFilters, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(btnSave, 0, wxALL, 5);
	sizerBtns->Add(btnDelete, 0, wxALL, 5);

	sizerFilters->Add(sizerBtns, 1, wxEXPAND, 0);

	sizerCol1->Add(sizerFilters, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerDates = new wxStaticBoxSizer(boxDates, wxVERTICAL);
	sizerDates->Add(m_ctrlDatesAll, 0, wxALL, 5);
	sizerDates->Add(m_ctrlDatesSome, 0, wxALL, 5);

	wxFlexGridSizer* sizerDateRange = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDateRange->SetFlexibleDirection(wxBOTH);
	sizerDateRange->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDateRange->Add(m_ctrlDateStartCheck, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDateRange->Add(m_ctrlDateStart, 0, wxALL, 3);
	sizerDateRange->Add(m_ctrlDateEndCheck, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDateRange->Add(m_ctrlDateEnd, 0, wxALL, 3);

	sizerDates->Add(sizerDateRange, 1, wxEXPAND, 0);

	sizerCol1->Add(sizerDates, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerCal1 = new wxStaticBoxSizer(boxCal1, wxVERTICAL);
	sizerCal1->Add(ctrlCalNot, 0, wxALL, 5);
	sizerCal1->Add(ctrlCalPlan, 0, wxALL, 5);
	sizerCal1->Add(ctrlCalEntered, 0, wxALL, 5);

	sizerCol1->Add(sizerCal1, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerLog = new wxStaticBoxSizer(boxLog, wxVERTICAL);
	sizerLog->Add(m_ctrlLogAll, 0, wxALL, 5);
	sizerLog->Add(m_ctrlLogSome, 0, wxALL, 5);
	sizerLog->Add(m_ctrlNames, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5);

	sizerCol1->Add(sizerLog, 0, wxALL|wxEXPAND, 5);

	sizerFilter->Add(sizerCol1, 1, 0, 5);

	wxBoxSizer* sizerCol2 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* m_ctrlQs = new wxStaticBoxSizer(boxQs, wxVERTICAL);
	m_ctrlQs->Add(m_ctrlQsAll, 0, wxALL, 5);
	m_ctrlQs->Add(m_ctrlQsQs, 0, wxALL, 5);
	m_ctrlQs->Add(m_ctrlQsNonQs, 0, wxALL, 5);

	sizerCol2->Add(m_ctrlQs, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerVenue = new wxStaticBoxSizer(boxVenue, wxVERTICAL);
	sizerVenue->Add(m_ctrlVenueAll, 0, wxALL, 5);
	sizerVenue->Add(m_ctrlVenueSome, 0, wxALL, 5);
	sizerVenue->Add(m_ctrlVenue, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5);

	sizerCol2->Add(sizerVenue, 1, wxALL|wxEXPAND, 5);

	sizerFilter->Add(sizerCol2, 1, wxEXPAND, 0);

	FillControls();

	SetSizer(sizerFilter);
	Layout();
	sizerFilter->Fit(this);
}


// Dates and calendar options aren't updated on-the-fly, so commit now.
void CDlgOptionsFilter::CommitChanges()
{
	if (!m_FilterOptions.GetViewAllDates())
	{
		if (m_bDateStart && !m_timeStart.IsValid())
			m_bDateStart = false;
		if (m_bDateEnd && !m_timeEnd.IsValid())
			m_bDateEnd = false;
		if (!m_bDateStart && !m_bDateEnd)
			m_FilterOptions.SetViewAllDates(true);
		m_FilterOptions.SetStartFilterDateSet(m_bDateStart);
		m_FilterOptions.SetStartFilterDate(m_timeStart);
		m_FilterOptions.SetEndFilterDateSet(m_bDateEnd);
		m_FilterOptions.SetEndFilterDate(m_timeEnd);
	}
	CCalendarViewFilter calView = m_FilterOptions.FilterCalendarView();
	calView.Clear();
	if (m_bNotEntered)
		calView.AddNotEntered();
	if (m_bPlanning)
		calView.AddPlanning();
	if (m_bEntered)
		calView.AddEntered();
	m_FilterOptions.SetFilterCalendarView(calView);
}


// This is called as part of CDlgOptions::OnOk
void CDlgOptionsFilter::Save()
{
	CommitChanges();
	// Commit to the registry
	m_FilterOptions.Save();
}


void CDlgOptionsFilter::FillControls()
{
	if (m_FilterOptions.GetViewAllDates())
		m_ctrlDatesAll->SetValue(true);
	else
		m_ctrlDatesSome->SetValue(true);
	if (m_FilterOptions.GetStartFilterDate().IsValid())
		m_timeStart = m_FilterOptions.GetStartFilterDate();
	else if (!m_FilterOptions.GetViewAllDates())
		m_timeStart.SetToday();
	m_bDateStart = m_FilterOptions.GetStartFilterDateSet();
	if (m_FilterOptions.GetEndFilterDate().IsValid())
		m_timeEnd = m_FilterOptions.GetEndFilterDate();
	else if (!m_FilterOptions.GetViewAllDates())
		m_timeEnd.SetToday();
	m_bDateEnd = m_FilterOptions.GetEndFilterDateSet();

	// This is reset everytime just to make checking items easier
	m_ctrlNames->Clear();
	std::set<std::wstring> allLogNames;
	m_pDoc->Book().GetTraining().GetAllNames(allLogNames);
	std::set<std::wstring> names;
	m_FilterOptions.GetTrainingFilterNames(names);
	bool bFix = false;
	for (std::set<std::wstring>::iterator iter = names.begin();
		iter != names.end();
		)
	{
		if (allLogNames.end() == allLogNames.find((*iter)))
		{
			bFix = true;
#ifdef ARB_SET_ERASE_RETURNS_ITERATOR
			iter = names.erase(iter);
#else
			names.erase(iter++);
#endif
		}
		else
			++iter;
	}
	if (bFix)
		m_FilterOptions.SetTrainingFilterNames(names);
	for (std::set<std::wstring>::iterator iterLog = allLogNames.begin();
		iterLog != allLogNames.end();
		++iterLog)
	{
		int item = m_ctrlNames->Append(StringUtil::stringWX(*iterLog));
		if (names.end() != names.find((*iterLog)))
			m_ctrlNames->Check(item, true);
	}

	if (0 == m_ctrlNames->GetCount())
	{
		m_ctrlLogAll->SetValue(true);
		m_ctrlLogSome->Enable(false);
	}
	else
	{
		if (m_FilterOptions.GetTrainingViewAllNames())
			m_ctrlLogAll->SetValue(true);
		else
			m_ctrlLogSome->SetValue(true);
	}

	m_bNotEntered = m_FilterOptions.FilterCalendarView().ViewNotEntered();
	m_bPlanning = m_FilterOptions.FilterCalendarView().ViewPlanning();
	m_bEntered = m_FilterOptions.FilterCalendarView().ViewEntered();

	// This is reset everytime just to make checking items easier
	m_ctrlVenue->DeleteAllItems();
	if (0 < m_pDoc->Book().GetConfig().GetVenues().size())
	{
		wxTreeItemId root = m_ctrlVenue->AddRoot(L"root");
		for (ARBConfigVenueList::const_iterator iVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
			iVenue != m_pDoc->Book().GetConfig().GetVenues().end();
			++iVenue)
		{
			ARBConfigVenuePtr pVenue = (*iVenue);
			wxTreeItemId hVenue = m_ctrlVenue->AppendItem(root, StringUtil::stringWX(pVenue->GetName()));
			m_ctrlVenue->ShowCheckbox(hVenue, true);
			if (m_FilterOptions.FilterExists(pVenue->GetName(), L"", L""))
				m_ctrlVenue->SetChecked(hVenue, true);
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++iterDiv)
			{
				ARBConfigDivisionPtr pDiv = *iterDiv;
				wxTreeItemId hDiv = m_ctrlVenue->AppendItem(hVenue, StringUtil::stringWX(pDiv->GetName()));
				m_ctrlVenue->ShowCheckbox(hDiv, true);
				if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), L""))
				{
					m_ctrlVenue->SetChecked(hDiv, true);
					m_ctrlVenue->EnsureVisible(hDiv);
				}
				for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
					iterLevel != pDiv->GetLevels().end();
					++iterLevel)
				{
					ARBConfigLevelPtr pLevel = *iterLevel;
					wxTreeItemId hLevel = m_ctrlVenue->AppendItem(hDiv, StringUtil::stringWX(pLevel->GetName()));
					if (0 < pLevel->GetSubLevels().size())
					{
						m_ctrlVenue->ShowCheckbox(hLevel, false);
						for (ARBConfigSubLevelList::const_iterator iterSubLevel = pLevel->GetSubLevels().begin();
							iterSubLevel != pLevel->GetSubLevels().end();
							++iterSubLevel)
						{
							ARBConfigSubLevelPtr pSubLevel = *iterSubLevel;
							wxTreeItemId hSubLevel = m_ctrlVenue->AppendItem(hLevel, StringUtil::stringWX(pSubLevel->GetName()));
							m_ctrlVenue->ShowCheckbox(hSubLevel, true);
							if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), pSubLevel->GetName()))
							{
								m_ctrlVenue->SetChecked(hSubLevel, true);
								m_ctrlVenue->EnsureVisible(hSubLevel);
							}
						}
					}
					else
					{
						m_ctrlVenue->ShowCheckbox(hLevel, true);
						if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), pLevel->GetName()))
						{
							m_ctrlVenue->SetChecked(hLevel, true);
							m_ctrlVenue->EnsureVisible(hLevel);
						}
					}
				}
			}
		}
	}

	if (!m_ctrlVenue->GetRootItem().IsOk())
	{
		m_ctrlVenueAll->SetValue(true);
		m_ctrlVenueSome->Enable(false);
	}
	else
	{
		if (m_FilterOptions.GetViewAllVenues())
			m_ctrlVenueAll->SetValue(true);
		else
			m_ctrlVenueSome->SetValue(true);
	}

	switch (m_FilterOptions.GetViewRuns())
	{
	default:
	case CFilterOptions::eViewRunsAll:
		m_ctrlQsAll->SetValue(true);
		break;
	case CFilterOptions::eViewRunsQs:
		m_ctrlQsQs->SetValue(true);
		break;
	case CFilterOptions::eViewRunsNQs:
		m_ctrlQsNonQs->SetValue(true);
		break;
	}

	UpdateControls();
}


void CDlgOptionsFilter::FillFilter(
		wxTreeItemId hItem,
		std::wstring path,
		std::vector<CVenueFilter>& outVenues)
{
	if (hItem.IsOk() && hItem != m_ctrlVenue->GetRootItem())
	{
		// Don't worry about enabled. Things are only "disabled" if the parent
		// checkbox is not set. If that's not set, we won't be calling down
		// into this.
		if (m_ctrlVenue->IsCheckVisible(hItem))
		{
			if (!m_ctrlVenue->GetChecked(hItem))
				return;
			if (!path.empty())
				path += L"/";
			path += m_ctrlVenue->GetItemText(hItem);
		}
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_ctrlVenue->GetFirstChild(hItem, cookie);
	if (!hChildItem.IsOk())
	{
		// We're at a leaf...
		std::vector<std::wstring> rawFilter;
		std::wstring::size_type pos;
		while (std::wstring::npos != (pos = path.find('/')))
		{
			rawFilter.push_back(path.substr(0, pos));
			path = path.substr(pos+1);
		}
		rawFilter.push_back(path);
		if (0 < rawFilter.size())
		{
			CVenueFilter filter;
			switch (rawFilter.size())
			{
			default:
			case 3:
				filter.level = rawFilter[2];
				// fallthru
			case 2:
				filter.division = rawFilter[1];
				// fallthru
			case 1:
				filter.venue = rawFilter[0];
			}
			outVenues.push_back(filter);
		}
	}
	else
	{
		while (hChildItem.IsOk())
		{
			FillFilter(hChildItem, path, outVenues);
			hChildItem = m_ctrlVenue->GetNextChild(hItem, cookie);
		}
	}
}


void CDlgOptionsFilter::UpdateControls()
{
	bool bEnableDates = !m_ctrlDatesAll->GetValue();
	m_ctrlDateStartCheck->Enable(bEnableDates);
	m_ctrlDateStart->Enable(bEnableDates);
	m_ctrlDateEndCheck->Enable(bEnableDates);
	m_ctrlDateEnd->Enable(bEnableDates);
	m_ctrlNames->Enable(!m_ctrlLogAll->GetValue());
	m_ctrlVenue->Enable(!m_ctrlVenueAll->GetValue());
}


void CDlgOptionsFilter::OnSelchangeFilterNames(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idx = m_ctrlFilters->GetSelection();
	if (0 <= idx)
	{
		m_FilterOptions.SetCurrentFilter(StringUtil::stringW(m_ctrlFilters->GetString(idx)));
		FillControls();
		TransferDataToWindow();
	}
	else
		wxBell();
}


void CDlgOptionsFilter::OnClickedOptFilterNamesSave(wxCommandEvent& evt)
{
	if (!TransferDataFromWindow())
		return;
	CommitChanges();
	if (!m_FilterName.empty())
	{
		if (m_FilterOptions.AddFilter(StringUtil::stringW(m_FilterName)))
			m_ctrlFilters->Append(m_FilterName);
		// After saving, reset in case anything was changed.
		FillControls();
		TransferDataToWindow();
	}
	else
		wxMessageBox(_("IDS_NAME_FILTER"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
}


void CDlgOptionsFilter::OnClickedOptFilterNamesDelete(wxCommandEvent& evt)
{
	if (!TransferDataFromWindow())
		return;
	if (!m_FilterName.empty())
	{
		int idx = m_ctrlFilters->FindString(m_FilterName, true);
		if (0 <= idx)
		{
			std::wstring name = StringUtil::stringW(m_FilterName);
			m_FilterOptions.DeleteFilter(name);
			m_ctrlFilters->Delete(idx);
			m_FilterName.clear();
			m_ctrlFilters->SetSelection(wxNOT_FOUND);
		}
	}
}


void CDlgOptionsFilter::OnFilterDates(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	if (m_ctrlDatesAll->GetValue())
	{
		m_FilterOptions.SetViewAllDates(true);
	}
	else
	{
		m_FilterOptions.SetViewAllDates(false);
		m_FilterOptions.SetStartFilterDate(m_timeStart);
		m_FilterOptions.SetStartFilterDateSet(m_bDateStart);
		m_FilterOptions.SetEndFilterDate(m_timeEnd);
		m_FilterOptions.SetEndFilterDateSet(m_bDateEnd);
	}
	UpdateControls();
}


void CDlgOptionsFilter::OnFilterLog(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_FilterOptions.SetTrainingViewAllNames(m_ctrlLogAll->GetValue());
	UpdateControls();
}


void CDlgOptionsFilter::OnFilterLogNames(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	std::set<std::wstring> names;
	for (unsigned int item = 0; item < m_ctrlNames->GetCount(); ++item)
	{
		if (m_ctrlNames->IsChecked(item))
		{
			names.insert(StringUtil::stringW(m_ctrlNames->GetString(item)));
		}
	}
	m_FilterOptions.SetTrainingFilterNames(names);
}


void CDlgOptionsFilter::OnFilterQs(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	if (m_ctrlQsAll->GetValue())
		m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsAll);
	else if (m_ctrlQsQs->GetValue())
		m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsQs);
	else if (m_ctrlQsNonQs->GetValue())
		m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsNQs);
	UpdateControls();
}


void CDlgOptionsFilter::OnFilterVenues(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_FilterOptions.SetViewAllVenues(m_ctrlVenueAll->GetValue());
	UpdateControls();
}


void CDlgOptionsFilter::OnFilterVenueCheck(wxEvent& evt)
{
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	std::vector<CVenueFilter> venues;
	FillFilter(m_ctrlVenue->GetRootItem(), L"", venues);
	m_FilterOptions.SetFilterVenue(venues);
}
