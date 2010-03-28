#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 */

#include "FilterOptions.h"
#include <wx/treectrl.h>
class CAgilityBookDoc;
class CCheckTreeCtrl;
class wxDatePickerCtrl;
class wxTreeEvent;


class CDlgOptionsFilter : public wxPanel
{
public:
	CDlgOptionsFilter(wxWindow* pParent, CAgilityBookDoc* pDoc);

	void Save();

private:
	void FillControls();
	void FillFilter(
			wxTreeItemId hItem,
			wxString path,
			std::vector<CVenueFilter>& outVenues);
	void UpdateControls();
	void CommitChanges();

	CAgilityBookDoc* m_pDoc;
	CFilterOptions m_FilterOptions;
	wxString m_FilterName;
	bool m_bDateStart;
	ARBDate m_timeStart;
	bool m_bDateEnd;
	ARBDate m_timeEnd;
	bool m_bNotEntered;
	bool m_bPlanning;
	bool m_bEntered;

	wxComboBox* m_ctrlFilters;
	wxRadioButton* m_ctrlDatesAll;
	wxRadioButton* m_ctrlDatesSome;
	wxCheckBox* m_ctrlDateStartCheck;
	wxDatePickerCtrl* m_ctrlDateStart;
	wxCheckBox* m_ctrlDateEndCheck;
	wxDatePickerCtrl* m_ctrlDateEnd;
	wxRadioButton* m_ctrlLogAll;
	wxRadioButton* m_ctrlLogSome;
	wxCheckListBox* m_ctrlNames;
	wxRadioButton* m_ctrlQsAll;
	wxRadioButton* m_ctrlQsQs;
	wxRadioButton* m_ctrlQsNonQs;
	wxRadioButton* m_ctrlVenueAll;
	wxRadioButton* m_ctrlVenueSome;
	CCheckTreeCtrl* m_ctrlVenue;

	void OnSelchangeFilterNames(wxCommandEvent& evt);
	void OnClickedOptFilterNamesSave(wxCommandEvent& evt);
	void OnClickedOptFilterNamesDelete(wxCommandEvent& evt);
	void OnFilterDates(wxCommandEvent& evt);
	void OnFilterLog(wxCommandEvent& evt);
	void OnFilterLogNames(wxCommandEvent& evt);
	void OnFilterQs(wxCommandEvent& evt);
	void OnFilterVenues(wxCommandEvent& evt);
	void OnFilterVenueCheck(wxTreeEvent& evt);
};
