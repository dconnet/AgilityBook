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
 * @brief implementation of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-09-01 DRC Fix setting of 'After' (start) date filter.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "DlgOptionsFilter.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBConfigVenue.h"
#include "CheckTreeCtrl.h"
#include "Validators.h"
#include <wx/datectrl.h>


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
	, m_bNotEntered(true)
	, m_bPlanning(true)
	, m_bEntered(true)
	, m_ctrlFilters(NULL)
	, m_ctrlDatesAll(NULL)
	, m_ctrlDatesSome(NULL)
	, m_ctrlDateStartCheck(NULL)
	, m_ctrlDateStart(NULL)
	, m_ctrlDateEndCheck(NULL)
	, m_ctrlDateEnd(NULL)
	, m_ctrlLogAll(NULL)
	, m_ctrlLogSome(NULL)
	, m_ctrlNames(NULL)
	, m_ctrlQsAll(NULL)
	, m_ctrlQsQs(NULL)
	, m_ctrlQsNonQs(NULL)
	, m_ctrlVenueAll(NULL)
	, m_ctrlVenueSome(NULL)
	, m_ctrlVenue(NULL)
{
	// Controls (these are done first to control tab order)

	m_ctrlFilters = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_FilterName, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlFilters->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnSelchangeFilterNames), NULL, this);
	m_ctrlFilters->SetHelpText(_("HIDC_OPT_FILTER_NAMES"));
	m_ctrlFilters->SetToolTip(_("HIDC_OPT_FILTER_NAMES"));
	std::vector<tstring> filterNames;
	m_FilterOptions.GetAllFilterNames(filterNames);
	for (std::vector<tstring>::iterator iterName = filterNames.begin();
		iterName != filterNames.end();
		++iterName)
	{
		int idx = m_ctrlFilters->Append((*iterName).c_str());
		if ((*iterName) == m_FilterOptions.GetCurrentFilter())
		{
			m_FilterName = m_FilterOptions.GetCurrentFilter().c_str();
			m_ctrlFilters->SetSelection(idx);
		}
	}

	wxButton* btnSave = new wxButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_NAMES_SAVE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnSave->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsFilter::OnClickedOptFilterNamesSave), NULL, this);
	btnSave->SetHelpText(_("HIDC_OPT_FILTER_NAMES_SAVE"));
	btnSave->SetToolTip(_("HIDC_OPT_FILTER_NAMES_SAVE"));

	wxButton* btnDelete = new wxButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_NAMES_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsFilter::OnClickedOptFilterNamesDelete), NULL, this);
	btnDelete->SetHelpText(_("HIDC_OPT_FILTER_NAMES_DELETE"));
	btnDelete->SetToolTip(_("HIDC_OPT_FILTER_NAMES_DELETE"));

	m_ctrlDatesAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlDatesAll->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterDates), NULL, this);
	m_ctrlDatesAll->SetHelpText(_("HIDC_OPT_FILTER_DATE_ALL"));
	m_ctrlDatesAll->SetToolTip(_("HIDC_OPT_FILTER_DATE_ALL"));

	m_ctrlDatesSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_DATE_RANGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDatesSome->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterDates), NULL, this);
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

	m_ctrlLogAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_LOG_NAME_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlLogAll->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterLog), NULL, this);
	m_ctrlLogAll->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME_ALL"));
	m_ctrlLogAll->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME_ALL"));

	m_ctrlLogSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_LOG_NAME_SELECTED"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLogSome->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterLog), NULL, this);
	m_ctrlLogSome->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME_SELECTED"));
	m_ctrlLogSome->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME_SELECTED"));

	m_ctrlNames = new wxCheckListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(-1, 80),
		0, NULL, wxLB_SORT);
	m_ctrlNames->Connect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterLogNames), NULL, this);
	m_ctrlNames->SetHelpText(_("HIDC_OPT_FILTER_LOG_NAME"));
	m_ctrlNames->SetToolTip(_("HIDC_OPT_FILTER_LOG_NAME"));

	m_ctrlQsAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlQsAll->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterQs), NULL, this);
	m_ctrlQsAll->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_ALL"));
	m_ctrlQsAll->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_ALL"));

	m_ctrlQsQs = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_Q"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlQsQs->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterQs), NULL, this);
	m_ctrlQsQs->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_Q"));
	m_ctrlQsQs->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_Q"));

	m_ctrlQsNonQs = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_RUNS_NON_Q"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlQsNonQs->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterQs), NULL, this);
	m_ctrlQsNonQs->SetHelpText(_("HIDC_OPT_FILTER_RUN_RUNS_NON_Q"));
	m_ctrlQsNonQs->SetToolTip(_("HIDC_OPT_FILTER_RUN_RUNS_NON_Q"));

	m_ctrlVenueAll = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_VENUES_ALL"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_ctrlVenueAll->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterVenues), NULL, this);
	m_ctrlVenueAll->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES_ALL"));
	m_ctrlVenueAll->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES_ALL"));

	m_ctrlVenueSome = new wxRadioButton(this, wxID_ANY,
		_("IDC_OPT_FILTER_RUN_VENUES_SELECTED"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlVenueSome->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CDlgOptionsFilter::OnFilterVenues), NULL, this);
	m_ctrlVenueSome->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES_SELECTED"));
	m_ctrlVenueSome->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES_SELECTED"));

	m_ctrlVenue = new CCheckTreeCtrl(this);
	m_ctrlVenueSome->Connect(wxEVT_COMMAND_TREE_CHECK_CHANGED, wxTreeEventHandler(CDlgOptionsFilter::OnFilterVenueCheck), NULL, this);
	m_ctrlVenue->SetHelpText(_("HIDC_OPT_FILTER_RUN_VENUES"));
	m_ctrlVenue->SetToolTip(_("HIDC_OPT_FILTER_RUN_VENUES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerFilter = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerCol1 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerFilters = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_NAMES")), wxVERTICAL);
	sizerFilters->Add(m_ctrlFilters, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(btnSave, 0, wxALL, 5);
	sizerBtns->Add(btnDelete, 0, wxALL, 5);

	sizerFilters->Add(sizerBtns, 1, wxEXPAND, 5);

	sizerCol1->Add(sizerFilters, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerDates = new wxStaticBoxSizer(new wxStaticBox( this, wxID_ANY, _("IDC_OPT_FILTER_DATE")), wxVERTICAL);
	sizerDates->Add(m_ctrlDatesAll, 0, wxALL, 5);
	sizerDates->Add(m_ctrlDatesSome, 0, wxALL, 5);

	wxFlexGridSizer* sizerDateRange = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDateRange->SetFlexibleDirection(wxBOTH);
	sizerDateRange->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDateRange->Add(m_ctrlDateStartCheck, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDateRange->Add(m_ctrlDateStart, 0, wxALL, 3);
	sizerDateRange->Add(m_ctrlDateEndCheck, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDateRange->Add(m_ctrlDateEnd, 0, wxALL, 3);

	sizerDates->Add(sizerDateRange, 1, wxEXPAND, 5);

	sizerCol1->Add(sizerDates, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerCal1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_CAL") ), wxVERTICAL);
	sizerCal1->Add(ctrlCalNot, 0, wxALL, 5);
	sizerCal1->Add(ctrlCalPlan, 0, wxALL, 5);
	sizerCal1->Add(ctrlCalEntered, 0, wxALL, 5);

	sizerCol1->Add(sizerCal1, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerLog = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_LOG_NAME")), wxVERTICAL);
	sizerLog->Add(m_ctrlLogAll, 0, wxALL, 5);
	sizerLog->Add(m_ctrlLogSome, 0, wxALL, 5);
	sizerLog->Add(m_ctrlNames, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5);

	sizerCol1->Add(sizerLog, 0, wxALL|wxEXPAND, 5);

	sizerFilter->Add(sizerCol1, 1, 0, 5);

	wxBoxSizer* sizerCol2 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* m_ctrlQs = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_RUN_RUNS")), wxVERTICAL);
	m_ctrlQs->Add(m_ctrlQsAll, 0, wxALL, 5);
	m_ctrlQs->Add(m_ctrlQsQs, 0, wxALL, 5);
	m_ctrlQs->Add(m_ctrlQsNonQs, 0, wxALL, 5);

	sizerCol2->Add(m_ctrlQs, 0, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerVenue = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_FILTER_RUN_VENUES")), wxVERTICAL);
	sizerVenue->Add(m_ctrlVenueAll, 0, wxALL, 5);
	sizerVenue->Add(m_ctrlVenueSome, 0, wxALL, 5);
	sizerVenue->Add(m_ctrlVenue, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5);

	sizerCol2->Add(sizerVenue, 1, wxALL|wxEXPAND, 5);

	sizerFilter->Add(sizerCol2, 1, wxEXPAND, 5);

	FillControls();

	SetSizer(sizerFilter);
	Layout();
	sizerFilter->Fit(this);
}


void CDlgOptionsFilter::Save()
{
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
	std::set<tstring> allLogNames;
	m_pDoc->Book().GetTraining().GetAllNames(allLogNames);
	std::set<tstring> names;
	m_FilterOptions.GetTrainingFilterNames(names);
	bool bFix = false;
	for (std::set<tstring>::iterator iter = names.begin();
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
	for (std::set<tstring>::iterator iterLog = allLogNames.begin();
		iterLog != allLogNames.end();
		++iterLog)
	{
		int item = m_ctrlNames->Append((*iterLog).c_str());
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
		wxTreeItemId root = m_ctrlVenue->AddRoot(wxT("root"));
		for (ARBConfigVenueList::const_iterator iVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
			iVenue != m_pDoc->Book().GetConfig().GetVenues().end();
			++iVenue)
		{
			ARBConfigVenuePtr pVenue = (*iVenue);
			wxTreeItemId hVenue = m_ctrlVenue->AppendItem(root, pVenue->GetName().c_str());
			m_ctrlVenue->ShowCheckbox(hVenue, true);
			if (m_FilterOptions.FilterExists(pVenue->GetName(), wxT(""), wxT("")))
				m_ctrlVenue->SetChecked(hVenue, true);
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++iterDiv)
			{
				ARBConfigDivisionPtr pDiv = *iterDiv;
				wxTreeItemId hDiv = m_ctrlVenue->AppendItem(hVenue, pDiv->GetName().c_str());
				m_ctrlVenue->ShowCheckbox(hDiv, true);
				if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), wxT("")))
				{
					m_ctrlVenue->SetChecked(hDiv, true);
					m_ctrlVenue->EnsureVisible(hDiv);
				}
				for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
					iterLevel != pDiv->GetLevels().end();
					++iterLevel)
				{
					ARBConfigLevelPtr pLevel = *iterLevel;
					wxTreeItemId hLevel = m_ctrlVenue->AppendItem(hDiv, pLevel->GetName().c_str());
					if (0 < pLevel->GetSubLevels().size())
					{
						m_ctrlVenue->ShowCheckbox(hLevel, false);
						for (ARBConfigSubLevelList::const_iterator iterSubLevel = pLevel->GetSubLevels().begin();
							iterSubLevel != pLevel->GetSubLevels().end();
							++iterSubLevel)
						{
							ARBConfigSubLevelPtr pSubLevel = *iterSubLevel;
							wxTreeItemId hSubLevel = m_ctrlVenue->AppendItem(hLevel, pSubLevel->GetName().c_str());
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
		wxString path,
		std::vector<CVenueFilter>& outVenues)
{
	if (hItem.IsOk())
	{
		// Don't worry about enabled. Things are only "disabled" if the parent
		// checkbox is not set. If that's not set, we won't be calling down
		// into this.
		if (m_ctrlVenue->IsCheckVisible(hItem))
		{
			if (!m_ctrlVenue->GetChecked(hItem))
				return;
			if (!path.IsEmpty())
				path += wxT("/");
			path += m_ctrlVenue->GetItemText(hItem);
		}
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_ctrlVenue->GetFirstChild(hItem, cookie);
	if (!hChildItem.IsOk())
	{
		// We're at a leaf...
		std::vector<wxString> rawFilter;
		int pos;
		while (0 <= (pos = path.Find('/')))
		{
			rawFilter.push_back(path.Left(pos));
			path = path.Mid(pos+1);
		}
		rawFilter.push_back(path);
		if (0 < rawFilter.size())
		{
			CVenueFilter filter;
			switch (rawFilter.size())
			{
			default:
			case 3:
				filter.level = rawFilter[2].c_str();
				// fallthru
			case 2:
				filter.division = rawFilter[1].c_str();
				// fallthru
			case 1:
				filter.venue = rawFilter[0].c_str();
			}
			outVenues.push_back(filter);
		}
	}
	else
	{
		while (hChildItem.IsOk())
		{
			FillFilter(hChildItem, path, outVenues);
			hChildItem = m_ctrlVenue->GetNextChild(hChildItem, cookie);
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
		wxString str = m_ctrlFilters->GetString(idx);
		m_FilterOptions.SetCurrentFilter(str.c_str());
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
	if (!m_FilterName.empty())
	{
		m_FilterOptions.AddFilter(m_FilterName.c_str());
		m_ctrlFilters->Append(m_FilterName);
	}
	else
		wxMessageBox(_("IDS_NAME_FILTER"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
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
			tstring name = m_FilterName.c_str();
			m_FilterOptions.DeleteFilter(name);
			m_ctrlFilters->Delete(idx);
			m_FilterName.clear();
		}
	}
}


void CDlgOptionsFilter::OnFilterDates(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	bool bAllDates = m_ctrlDatesAll->GetValue();
	if (bAllDates
	|| (!bAllDates && !m_bDateStart && !m_bDateEnd))
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


void CDlgOptionsFilter::OnFilterCal(wxCommandEvent& evt)
{
	TransferDataFromWindow();
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


void CDlgOptionsFilter::OnFilterLog(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_FilterOptions.SetTrainingViewAllNames(m_ctrlLogAll->GetValue());
	UpdateControls();
}


void CDlgOptionsFilter::OnFilterLogNames(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	std::set<tstring> names;
	for (unsigned int item = 0; item < m_ctrlNames->GetCount(); ++item)
	{
		if (m_ctrlNames->IsChecked(item))
		{
			wxString str = m_ctrlNames->GetString(item);
			names.insert(str.c_str());
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


void CDlgOptionsFilter::OnFilterVenueCheck(wxTreeEvent& evt)
{
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	std::vector<CVenueFilter> venues;
	FillFilter(m_ctrlVenue->GetRootItem(), wxT(""), venues);
	m_FilterOptions.SetFilterVenue(venues);
}
