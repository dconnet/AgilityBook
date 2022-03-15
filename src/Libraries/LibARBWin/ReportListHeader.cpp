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
 * 2020-12-11 Moved out of ListCtrl.cpp
 */

#include "stdafx.h"
#include "LibARBWin/ReportListHeader.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DPI.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/RegItemsBase.h"
#include "LibARBWin/ReportListCtrl.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

// TODO: wx-generic is missing the G/SetColumnsOrder api
// (this define section is the same as wx/listctrl.h)
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
#define HAS_COLUMNSORDER 1
#elif defined(__WXQT__) && !defined(__WXUNIVERSAL__)
#define HAS_COLUMNSORDER 1
#else
#define HAS_COLUMNSORDER 0
#endif


int wxCALLBACK ReportListCompareItems(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	if (!item1)
		return -1;
	else if (!item2)
		return 1;

	long iCol = std::abs(pSortInfo->nCol) - 1;
	int rc = item1->OnCompare(item2, iCol);

	if (0 > pSortInfo->nCol)
		rc *= -1;
	return rc;
}


CReportListHeader::CReportListHeader()
	: m_parent(nullptr)
	, m_ctrlList(nullptr)
	, m_columnInfo()
	, m_idFirst(0)
	, m_baseConfig()
	, m_defaultWidths()
	, m_colWidths()
	, m_columnOrder()
	, m_columnVisible()
	, m_sortingEnabled(true)
	, m_iSortCol(0)
	, m_bIsSorted(false)
	, m_saveFlags(SaveFlags::Default)
{
}


CReportListHeader::~CReportListHeader()
{
}


void CReportListHeader::Initialize(wxWindow* parent, CReportListCtrl* ctrlList)
{
	assert(parent);
	assert(ctrlList);
	assert(!m_parent);
	assert(!m_ctrlList);

	m_parent = parent;
	m_ctrlList = ctrlList;
}


void CReportListHeader::Save()
{
	assert(m_ctrlList);
#if HAS_COLUMNSORDER
	m_columnOrder = m_ctrlList->GetColumnsOrder();
#endif
	OnSave();
}


void CReportListHeader::CreateColumns(
	std::vector<ColumnInfo> const& inColumns,
	long defaultSort,
	std::wstring const& baseConfig,
	std::vector<int> const* pColWidths,
	unsigned int idFirst)
{
	assert(m_parent);
	assert(m_ctrlList);
	assert(inColumns.size() > 0); // Gotta have some columns!

	// Clear existing

	if (m_ctrlList->GetColumnCount() != 0)
	{
		OnSave();
		m_ctrlList->DeleteAllItems();
		for (int col = m_ctrlList->GetColumnCount() - 1; 0 <= col; --col)
			m_ctrlList->DeleteColumn(col);
	}
	bool wasBound = (m_columnInfo.size() > 0);
	if (0 < m_idFirst)
	{
		m_parent->Unbind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateColumn,
			this,
			m_idFirst,
			static_cast<int>(m_idFirst + m_columnInfo.size() - 1));
		m_parent->Unbind(
			wxEVT_MENU,
			&CReportListHeader::OnColumn,
			this,
			m_idFirst,
			static_cast<int>(m_idFirst + m_columnInfo.size() - 1));
		m_parent->Unbind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateRestore,
			this,
			static_cast<int>(m_idFirst + m_columnInfo.size()),
			wxID_ANY);
		m_parent->Unbind(
			wxEVT_MENU,
			&CReportListHeader::OnRestore,
			this,
			static_cast<int>(m_idFirst + m_columnInfo.size()),
			wxID_ANY);
		m_ctrlList->Unbind(wxEVT_LIST_COL_BEGIN_DRAG, &CReportListHeader::OnBeginColDrag, this);
		m_ctrlList->Unbind(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK, &CReportListHeader::OnColumnRClick, this);
	}

	// Initialize

#if defined(_DEBUG) || defined(__WXDEBUG__)
	for (long i = 0; i < static_cast<long>(inColumns.size()); ++i)
		assert(i == inColumns[i].index);
#endif
	m_columnInfo = inColumns;
	m_idFirst = idFirst;
	m_baseConfig = baseConfig;
	m_defaultWidths.clear();
	m_colWidths.clear();
	if (pColWidths)
	{
		assert(pColWidths->size() == m_columnInfo.size());
		m_defaultWidths = *pColWidths;
		if (DPI::GetScale(m_parent) != 100)
		{
			for (size_t i = 0; i < m_defaultWidths.size(); ++i)
			{
				m_defaultWidths[i] = DPI::Scale(m_defaultWidths[i]);
			}
		}
		m_colWidths = m_defaultWidths;
	}

	m_columnOrder.clear();
	for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
		m_columnOrder.push_back(i);
	m_iSortCol = (defaultSort < 0) ? 0 : defaultSort + 1;
	m_bIsSorted = (m_iSortCol != 0);

	// Allow derived class to override column visibility and registry loading
	GetDefaultColumns(m_columnVisible);
	OnLoad();

	// Bind

	if (0 < m_idFirst)
	{
		m_parent->Bind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateColumn,
			this,
			m_idFirst,
			static_cast<int>(m_idFirst + m_columnInfo.size() - 1));
		m_parent->Bind(
			wxEVT_MENU,
			&CReportListHeader::OnColumn,
			this,
			m_idFirst,
			static_cast<int>(m_idFirst + m_columnInfo.size() - 1));
		m_parent->Bind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateRestore,
			this,
			static_cast<int>(m_idFirst + m_columnInfo.size()),
			wxID_ANY);
		m_parent->Bind(
			wxEVT_MENU,
			&CReportListHeader::OnRestore,
			this,
			static_cast<int>(m_idFirst + m_columnInfo.size()),
			wxID_ANY);
		m_ctrlList->Bind(wxEVT_LIST_COL_BEGIN_DRAG, &CReportListHeader::OnBeginColDrag, this);
		m_ctrlList->Bind(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK, &CReportListHeader::OnColumnRClick, this);
	}

	if (!wasBound)
	{
		m_ctrlList->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &CReportListHeader::OnColumnClick, this);
	}

	// Create

	for (long iCol = 0; iCol < static_cast<long>(m_columnInfo.size()); ++iCol)
	{
		int width = -1;
		if (iCol < static_cast<long>(m_colWidths.size()))
			width = m_colWidths[iCol];

		m_ctrlList
			->InsertColumn(iCol, wxGetTranslation(m_columnInfo[iCol].name.c_str()), m_columnInfo[iCol].fmt, width);
		if (!m_columnVisible[iCol])
			m_ctrlList->SetColumnWidth(iCol, 0);
	}
#if HAS_COLUMNSORDER
	m_ctrlList->SetColumnsOrder(m_columnOrder);
#endif
}


void CReportListHeader::UpdateColumns()
{
	if (!m_ctrlList)
		return;
	for (long iCol = 0; iCol < static_cast<long>(m_columnInfo.size()); ++iCol)
	{
		wxListItem item;
		if (m_ctrlList->GetColumn(iCol, item))
		{
			item.SetText(wxGetTranslation(m_columnInfo[iCol].name.c_str()));
			m_ctrlList->SetColumn(iCol, item);
		}
	}
}


void CReportListHeader::SizeColumns()
{
	assert(m_ctrlList);
#if HAS_COLUMNSORDER
	m_columnOrder = m_ctrlList->GetColumnsOrder();
#endif
	if (m_colWidths.empty())
	{
		for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
		{
			if (m_columnVisible[m_columnOrder[i]])
				m_ctrlList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
			else
				m_ctrlList->SetColumnWidth(i, 0);
		}
	}
}


void CReportListHeader::Sort()
{
	assert(m_ctrlList);
	if (m_bIsSorted)
	{
		SortInfo sortInfo(GetSortColumn());
		m_ctrlList->SortItems(ReportListCompareItems, &sortInfo);
		m_ctrlList->SetColumnSort(std::abs(GetSortColumn()) - 1, GetSortColumn());
	}
}


void CReportListHeader::Sort(int col)
{
	assert(m_ctrlList);
	SetSorted(true);

	m_ctrlList->SetColumnSort(std::abs(GetSortColumn()) - 1, 0);
	int nBackwards = 1;
	if (GetSortColumn() == col + 1)
		nBackwards = -1;

	m_iSortCol = (col + 1) * nBackwards;

	if (0 != m_iSortCol)
	{
		SetSorted(true);
		Sort();
	}

	long sel = m_ctrlList->GetSelection(true);
	if (wxNOT_FOUND != sel)
		m_ctrlList->EnsureVisible(sel);
}


bool CReportListHeader::SetSorted(bool bSorted)
{
	assert(m_ctrlList);
	bool bReload = false;
	if (m_bIsSorted != bSorted)
	{
		if (m_bIsSorted)
		{
			bReload = m_bIsSorted;
			m_ctrlList->SetColumnSort(std::abs(GetSortColumn()) - 1, 0);
		}
		m_bIsSorted = bSorted;
	}
	return bReload;
}


size_t CReportListHeader::GetColumns(std::vector<std::pair<long, wxString>>& columns) const
{
	columns.clear();
#if HAS_COLUMNSORDER
	wxArrayInt columnOrder = m_ctrlList->GetColumnsOrder();
#else
	wxArrayInt columnOrder = m_columnOrder;
#endif
	for (size_t i = 0; i < columnOrder.size(); ++i)
	{
		if (m_columnVisible[columnOrder[i]])
		{
			wxListItem item;
			item.SetMask(wxLIST_MASK_TEXT);
			m_ctrlList->GetColumn(columnOrder[i], item);
			columns.push_back(std::make_pair(columnOrder[i], item.GetText()));
		}
	}
	return columns.size();
}


void CReportListHeader::GetDefaultColumns(std::vector<bool>& columns)
{
	columns.clear();
	columns.insert(columns.end(), m_columnInfo.size(), true);
}


void CReportListHeader::OnLoad()
{
	if (!m_baseConfig.empty())
	{
		wxString str = wxConfig::Get()->Read(CFG_SORTING_ORDER2(m_baseConfig), L"");
		for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
		{
			m_columnOrder[i] = StringUtil::ToCLong(StringUtil::stringW(str));
			int n = str.Find(',');
			if (n > 0)
				str = str.Mid(n + 1);
			else
				str.Empty();
		}

		str = wxConfig::Get()->Read(CFG_SORTING_VISIBLE(m_baseConfig), L"");
		for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
		{
			m_columnVisible[i] = (StringUtil::ToCLong(StringUtil::stringW(str)) != 0);
			int n = str.Find(',');
			if (n > 0)
				str = str.Mid(n + 1);
			else
				str.Empty();
		}
		// Sanity check: If all are hidden, turn all on. (Otherwise control is just blank)
		if (static_cast<int>(m_columnVisible.size())
			== std::count(m_columnVisible.begin(), m_columnVisible.end(), false))
		{
			std::fill(m_columnVisible.begin(), m_columnVisible.end(), true);
		}

		str = wxConfig::Get()->Read(CFG_COLUMN_WIDTHS(m_baseConfig), L"");
		m_colWidths.clear();
		for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
		{
			m_colWidths.push_back(StringUtil::ToCLong(StringUtil::stringW(str)));
			int n = str.Find(',');
			if (n > 0)
				str = str.Mid(n + 1);
			else
				str.Empty();
		}

		m_iSortCol = wxConfig::Get()->Read(CFG_SORT_COLUMN(m_baseConfig), m_iSortCol);
		m_bIsSorted = (m_iSortCol != 0);
	}
}


void CReportListHeader::OnSave()
{
	if (!m_baseConfig.empty())
	{
		wxString str;
		if (m_saveFlags & SaveFlags::ColumnOrder)
		{
			for (size_t i = 0; i < m_columnOrder.size(); ++i)
			{
				if (0 < i)
					str << L",";
				str << m_columnOrder[i];
			}
			wxConfig::Get()->Write(CFG_SORTING_ORDER2(m_baseConfig), str);
		}
		else if (m_saveFlags & SaveFlags::ClearUnused)
		{
			if (wxConfig::Get()->HasEntry(CFG_SORTING_ORDER2(m_baseConfig)))
			{
				wxConfig::Get()->DeleteEntry(CFG_SORTING_ORDER2(m_baseConfig));
			}
		}

		if (m_saveFlags & SaveFlags::ColumnVisible)
		{
			str.clear();
			for (size_t i = 0; i < m_columnVisible.size(); ++i)
			{
				if (0 < i)
					str << L",";
				str << (m_columnVisible[i] ? L"1" : L"0");
			}
			wxConfig::Get()->Write(CFG_SORTING_VISIBLE(m_baseConfig), str);
		}
		else if (m_saveFlags & SaveFlags::ClearUnused)
		{
			if (wxConfig::Get()->HasEntry(CFG_SORTING_VISIBLE(m_baseConfig)))
			{
				wxConfig::Get()->DeleteEntry(CFG_SORTING_VISIBLE(m_baseConfig));
			}
		}

		if (m_saveFlags & SaveFlags::ColumnWidth)
		{
			str.clear();
			for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
			{
				if (0 < i)
					str << L",";
				if (m_columnVisible[i])
					str << m_ctrlList->GetColumnWidth(i);
				else if (i < static_cast<int>(m_colWidths.size()))
					str << m_colWidths[i];
				else
					str << "0";
			}
			wxConfig::Get()->Write(CFG_COLUMN_WIDTHS(m_baseConfig), str);
		}
		else if (m_saveFlags & SaveFlags::ClearUnused)
		{
			if (wxConfig::Get()->HasEntry(CFG_COLUMN_WIDTHS(m_baseConfig)))
			{
				wxConfig::Get()->DeleteEntry(CFG_COLUMN_WIDTHS(m_baseConfig));
			}
		}

		if (m_saveFlags & SaveFlags::CurrentSort)
			wxConfig::Get()->Write(CFG_SORT_COLUMN(m_baseConfig), m_iSortCol);
		else if (m_saveFlags & SaveFlags::ClearUnused)
		{
			if (wxConfig::Get()->HasEntry(CFG_SORT_COLUMN(m_baseConfig)))
			{
				wxConfig::Get()->DeleteEntry(CFG_SORT_COLUMN(m_baseConfig));
			}
		}
	}
}


void CReportListHeader::OnBeginColDrag(wxListEvent& evt)
{
	if (!m_columnVisible[evt.GetColumn()])
		evt.Veto();
}


void CReportListHeader::OnColumnRClick(wxListEvent& evt)
{
	if (0 == m_idFirst)
		return;

	assert(m_ctrlList);
	wxMenu menu;
#if HAS_COLUMNSORDER
	m_columnOrder = m_ctrlList->GetColumnsOrder();
#endif
	for (auto col : m_columnOrder)
	{
		assert(col >= 0 && col < static_cast<int>(m_columnInfo.size()));
		menu.AppendCheckItem(m_idFirst + col, StringUtil::GetTranslation(m_columnInfo[col].name.c_str()));
	}
	menu.AppendSeparator();
	menu.Append(static_cast<int>(m_idFirst + m_columnInfo.size()), _("Restore"));

	m_parent->PopupMenu(&menu, evt.GetPoint());
}


void CReportListHeader::OnColumnClick(wxListEvent& evt)
{
	if (m_sortingEnabled)
		Sort(evt.GetColumn());
}


void CReportListHeader::OnUpdateColumn(wxUpdateUIEvent& evt)
{
	evt.Check(m_columnVisible[evt.GetId() - m_idFirst]);
}


void CReportListHeader::OnColumn(wxCommandEvent& evt)
{
	long col = evt.GetId() - m_idFirst;
	if (evt.IsChecked())
	{
		m_columnVisible[col] = true;
		m_ctrlList->SetColumnWidth(col, wxLIST_AUTOSIZE_USEHEADER);
	}
	else
	{
		m_columnVisible[col] = false;
		m_ctrlList->SetColumnWidth(col, 0);
	}
}


void CReportListHeader::OnUpdateRestore(wxUpdateUIEvent& evt)
{
	std::vector<bool> columns;
	GetDefaultColumns(columns);
	bool isSame = columns == m_columnVisible;
	if (isSame)
	{
#if HAS_COLUMNSORDER
		m_columnOrder = m_ctrlList->GetColumnsOrder();
#endif
		wxArrayInt order;
		for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
			order.push_back(i);
		isSame = order == m_columnOrder;
	}
	evt.Enable(!isSame);
}


void CReportListHeader::OnRestore(wxCommandEvent& evt)
{
	GetDefaultColumns(m_columnVisible);
	m_columnOrder.clear();
	for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
		m_columnOrder.push_back(i);
#if HAS_COLUMNSORDER
	m_ctrlList->SetColumnsOrder(m_columnOrder);
#endif
	for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
	{
		int width = 0;
		if (m_columnVisible[m_columnOrder[i]])
		{
			if (m_defaultWidths.empty())
				width = wxLIST_AUTOSIZE_USEHEADER;
			else
				width = m_defaultWidths[i];
		}
		m_ctrlList->SetColumnWidth(i, width);
	}
	m_ctrlList->Refresh();
}
