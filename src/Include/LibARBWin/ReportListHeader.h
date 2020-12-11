#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Enable easy management of listctrl column sorting/ordering
 * @author David Connet
 *
 * Revision History
 * 2020-12-11 Moved out of ListCtrl.h
 */

#include "LibwxARBWin.h"

#include <vector>
class CReportListCtrl;


class ARBWIN_API CReportListHeader
{
	DECLARE_NO_COPY_IMPLEMENTED(CReportListHeader)
public:
	struct ColumnInfo
	{
		long index; // For debugging to ensure integrity
		int fmt;
		wchar_t const* name;
	};

	// If idFirst is 0, do not show the context menu
	CReportListHeader(unsigned int idFirst, std::wstring const& baseConfig = std::wstring());
	virtual ~CReportListHeader();

	// defaultSort: Initial column (normal 0-based)
	// pColWidths: initial column widths
	void Initialize(
		wxWindow* parent,
		CReportListCtrl* ctrlList,
		std::vector<ColumnInfo> const& columns,
		long defaultSort = 0,
		std::vector<int> const* pColWidths = nullptr);
	bool CreateMenu(wxMenu& menu);
	void CreateColumns();
	void SizeColumns();
	void Sort();        // Only sorts if we're sorting
	void Sort(int col); // Actual column index from listctrl, turns on sorting

	bool IsSorted() const
	{
		return m_bIsSorted;
	}
	bool SetSorted(bool bSorted);

	int GetSortColumn() const
	{
		return m_iSortCol;
	}

protected:
	// Override this to change default columns visibility (default: all visisble)
	virtual void GetDefaultColumns(std::vector<bool>& columns);
	// Load/Save settings
	virtual void OnLoad();
	virtual void OnSave();

	void OnCloseParent(wxCloseEvent& evt);
	void OnBeginColDrag(wxListEvent& evt);
	void OnColumnRClick(wxListEvent& evt);
	void OnColumnClick(wxListEvent& evt);
	void OnUpdateColumn(wxUpdateUIEvent& evt);
	void OnColumn(wxCommandEvent& evt);
	void OnUpdateRestore(wxUpdateUIEvent& evt);
	void OnRestore(wxCommandEvent& evt);

	const std::wstring m_baseConfig;
	const unsigned int m_idFirst;
	std::vector<ColumnInfo> m_columnInfo; // Do not modify after Initialize is called
	wxWindow* m_parent;
	CReportListCtrl* m_ctrlList;
	wxArrayInt m_order;
	std::vector<bool> m_columns;
	std::vector<int> m_colWidths; // May be empty, if so, we don't remember widths
	long m_iSortCol;
	bool m_bIsSorted;
};
