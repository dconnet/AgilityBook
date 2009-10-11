#pragma once

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
