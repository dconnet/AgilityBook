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
 * 2022-03-15 Add save flag to do registry clean up.
 * 2022-03-09 Changed ColumnInfo::name to a string to allow dynamically adding.
 * 2021-10-15 Add ability to disable sort headers.
 * 2021-01-23 Add ability to change what is saved.
 * 2020-12-11 Moved out of ListCtrl.h
 */

#include "LibwxARBWin.h"

#include <string>
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
		std::wstring name;
	};

	CReportListHeader();
	virtual ~CReportListHeader();

	enum SaveFlags
	{
		ColumnOrder = 0x01,
		ColumnVisible = 0x02,
		ColumnWidth = 0x04,
		CurrentSort = 0x08,
		ClearUnused = 0x10, // Note: Not default for back-compatibility
		Default = (ColumnOrder | ColumnVisible | ColumnWidth | CurrentSort),
	};
	unsigned int SaveFlags() const
	{
		return m_saveFlags;
	}
	void SetSaveFlags(unsigned int saveFlags)
	{
		m_saveFlags = saveFlags;
	}

	/**
	 * Initialize usage (can only call once)
	 * @param parent Owner of the popup menu for binding menu events and listening to close event.
	 * @param ctrlList The report list we're handling
	 */
	void Initialize(wxWindow* parent, CReportListCtrl* ctrlList);

	/**
	 * Updates internal settings and calls OnSave to save them. There should be
	 * no need to override this. Just override OnSave.
	 * Note: This cannot watch the destroy message for either the list or the
	 * parent - this object is likely to have been destroyed already. The object
	 * that has this will have already invoked its dtor - the list is then
	 * destoryed in that objects wx base class.
	 */
	void Save();

	/**
	 * Create columns. This may be called multiple times with different columns to allow switching views.
	 * @param inColumns Format and name for each column.
	 * @param defaultSort Initial sort column (normal 0-based, <0: no sort)
	 * @param baseConfig Where to read saved values (will not load/save is empty)
	 * @param pColWidths Initial column widths (auto sized if null, must be same size as inColumns)
	 * @param idFirst First id for context menu. If 0, do not show the menu.
	 * Note: the widths in pColWidths are not DPI scaled (will use m_parent)
	 */
	void CreateColumns(
		std::vector<ColumnInfo> const& inColumns,
		long defaultSort = -1,
		std::wstring const& baseConfig = std::wstring(),
		std::vector<int> const* pColWidths = nullptr,
		unsigned int idFirst = 0);

	/**
	 * Update columns when language has changed.
	 */
	void UpdateColumns();

	/**
	 * Enabled/disable sorting when sort columns are enabled.
	 * (simply prevents Sort from happening on a column click)
	 */
	void EnableSorting(bool enableSort)
	{
		m_sortingEnabled = enableSort;
	}
	bool IsSortingEnabled() const
	{
		return m_sortingEnabled;
	}

	void SizeColumns();
	virtual void Sort(); // Only sorts if we're sorting (virtual to allow a different sort)
	void Sort(int col);  // Actual column index from listctrl, turns on sorting

	bool IsSorted() const
	{
		return m_bIsSorted;
	}
	bool SetSorted(bool bSorted);

	int GetSortColumn() const
	{
		return m_iSortCol;
	}

	// Get the visible columns index and name
	size_t GetColumns(std::vector<std::pair<long, wxString>>& columns) const;

protected:
	// Override this to change default columns visibility (default: all visisble)
	virtual void GetDefaultColumns(std::vector<bool>& columns);
	// Load/Save settings
	virtual void OnLoad();
	virtual void OnSave();

	void OnBeginColDrag(wxListEvent& evt);
	void OnColumnRClick(wxListEvent& evt);
	void OnColumnClick(wxListEvent& evt);
	void OnUpdateColumn(wxUpdateUIEvent& evt);
	void OnColumn(wxCommandEvent& evt);
	void OnUpdateRestore(wxUpdateUIEvent& evt);
	void OnRestore(wxCommandEvent& evt);

	wxWindow* m_parent;
	CReportListCtrl* m_ctrlList;
	std::vector<ColumnInfo> m_columnInfo; // Do not modify after Initialize is called
	unsigned int m_idFirst;
	std::wstring m_baseConfig;
	std::vector<int> m_defaultWidths; // May be empty, if so, we don't remember widths
	std::vector<int> m_colWidths;     // May be empty, if so, we don't remember widths
	wxArrayInt m_columnOrder;
	std::vector<bool> m_columnVisible;
	bool m_sortingEnabled;
	long m_iSortCol;
	bool m_bIsSorted;
	unsigned int m_saveFlags;
};
