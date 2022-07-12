#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Assign columns for import/export.
 * @author David Connet
 *
 * Revision History
 * 2012-12-23 Changed default Runs view columns.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2010-02-17 Added SubName to runs view.
 * 2009-01-26 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-08-11 Added verified column to trial in tree.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-12-10 Created
 */

#include "AgilityBookOptions.h"
#include "NamedColumns.h"
#include <vector>
class wxListEvent;


namespace dconSoft
{
namespace ARBWin
{
class CListCtrl;
} // namespace ARBWin
class CAgilityBookDoc;
class ColumnData;


class CDlgAssignColumns : public wxDialog
{
public:
	static int GetFormatFromColumnID(long column);
	static std::wstring GetNameFromColumnID(long column);
	static bool GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long>& values,
		bool bDefaultValues = false);
	static bool GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long>& values,
		bool bDefaultValues = false);
	static bool SetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long> const& values);

public:
	CDlgAssignColumns(
		CAgilityBookOptions::ColumnOrder eOrder,
		wxWindow* pParent,
		CAgilityBookDoc* pDoc = nullptr,
		int initSelection = 0); // IO_TYPE list to initially select (if multiple)

private:
	CAgilityBookDoc* m_pDoc;
	CNamedColumns m_Configs;
	int m_initSelection;
	bool m_bIncludeBlank;
	wxString m_ConfigName;
	wxComboBox* m_ctrlConfig;
	ARBWin::CListCtrl* m_ctrlType;
	wxListBox* m_ctrlAvailable;
	wxListBox* m_ctrlColumns;
	wxButton* m_btnAdd;
	wxButton* m_btnRemove;
	wxButton* m_btnUp;
	wxButton* m_btnDown;

private:
	ColumnData* GetAvailableData(long idx) const;
	ColumnData* GetInUseData(long idx) const;
	void FillColumns();
	void UpdateColumnVector();
	void UpdateButtons();

protected:
	DECLARE_EVENT_TABLE()
	void OnSelchangeNames(wxCommandEvent& evt);
	void OnClickedOptNamesSave(wxCommandEvent& evt);
	void OnClickedOptNamesDelete(wxCommandEvent& evt);
	void OnItemchanged(wxListEvent& evt);
	void OnSelchangeAvailable(wxCommandEvent& evt);
	void OnSelchangeColumns(wxCommandEvent& evt);
	void OnAdd(wxCommandEvent& evt);
	void OnRemove(wxCommandEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnReset(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
