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


CReportListHeader::CReportListHeader(unsigned int idFirst, std::wstring const& baseConfig)
	: m_baseConfig(baseConfig)
	, m_idFirst(idFirst)
	, m_columnInfo()
	, m_parent(nullptr)
	, m_ctrlList(nullptr)
	, m_order()
	, m_columns()
	, m_colWidths()
	, m_iSortCol(0)
	, m_bIsSorted(false)
{
}


CReportListHeader::~CReportListHeader()
{
}


void CReportListHeader::Initialize(
	wxWindow* parent,
	CReportListCtrl* ctrlList,
	std::vector<ColumnInfo> const& columns,
	long defaultSort,
	std::vector<int> const* pColWidths)
{
	assert(parent);
	assert(ctrlList);

	m_parent = parent;
	m_ctrlList = ctrlList;
	m_columnInfo = columns;

#ifdef _DEBUG
	for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
		assert(i == m_columnInfo[i].index);
#endif
	if (pColWidths)
	{
		assert(pColWidths->size() == m_columnInfo.size());
		m_colWidths = *pColWidths;
		if (DPI::GetScale(parent) != 100)
		{
			for (size_t i = 0; i < m_colWidths.size(); ++i)
			{
				m_colWidths[i] = DPI::Scale(m_colWidths[i]);
			}
		}
	}
	else
		m_colWidths.clear();

	m_order.clear();
	for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
		m_order.push_back(i);
	m_bIsSorted = false;
	m_iSortCol = defaultSort + 1;

	GetDefaultColumns(m_columns);
	OnLoad();

	if (0 < m_idFirst)
	{
		parent->Bind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateColumn,
			this,
			m_idFirst,
			m_idFirst + m_columnInfo.size() - 1);
		parent->Bind(wxEVT_MENU, &CReportListHeader::OnColumn, this, m_idFirst, m_idFirst + m_columnInfo.size() - 1);
		parent->Bind(
			wxEVT_UPDATE_UI,
			&CReportListHeader::OnUpdateRestore,
			this,
			m_idFirst + m_columnInfo.size(),
			wxID_ANY);
		parent->Bind(wxEVT_MENU, &CReportListHeader::OnRestore, this, m_idFirst + m_columnInfo.size(), wxID_ANY);
	}

	parent->Bind(wxEVT_CLOSE_WINDOW, &CReportListHeader::OnCloseParent, this);
	m_ctrlList->Bind(wxEVT_LIST_COL_BEGIN_DRAG, &CReportListHeader::OnBeginColDrag, this);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK, &CReportListHeader::OnColumnRClick, this);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &CReportListHeader::OnColumnClick, this);
}


bool CReportListHeader::CreateMenu(wxMenu& menu)
{
	if (0 == m_idFirst)
		return false;

	assert(m_ctrlList);
#if HAS_COLUMNSORDER
	m_order = m_ctrlList->GetColumnsOrder();
#endif
	for (auto col : m_order)
	{
		assert(col >= 0 && col < static_cast<int>(m_columnInfo.size()));
		menu.AppendCheckItem(m_idFirst + col, StringUtil::GetTranslation(m_columnInfo[col].name));
	}
	menu.AppendSeparator();
	menu.Append(m_idFirst + m_columnInfo.size(), _("Restore"));
	return true;
}


void CReportListHeader::CreateColumns()
{
	assert(m_ctrlList);

	if (m_ctrlList->GetColumnCount() != 0)
	{
#if HAS_COLUMNSORDER
		m_order = m_ctrlList->GetColumnsOrder();
#endif
		if (!m_colWidths.empty())
		{
			for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
			{
				int width = m_ctrlList->GetColumnWidth(i);
				if (m_columns[i])
					m_colWidths[i] = width;
			}
		}
	}

	m_ctrlList->DeleteAllItems();
	for (int col = m_ctrlList->GetColumnCount() - 1; 0 <= col; --col)
		m_ctrlList->DeleteColumn(col);

	for (long iCol = 0; iCol < static_cast<long>(m_columnInfo.size()); ++iCol)
	{
		int width = -1;
		if (iCol < static_cast<long>(m_colWidths.size()))
			width = m_colWidths[iCol];

		m_ctrlList->InsertColumn(iCol, wxGetTranslation(m_columnInfo[iCol].name), m_columnInfo[iCol].fmt, width);
		if (!m_columns[iCol])
			m_ctrlList->SetColumnWidth(iCol, 0);
	}
#if HAS_COLUMNSORDER
	m_ctrlList->SetColumnsOrder(m_order);
#endif
}


void CReportListHeader::SizeColumns()
{
	assert(m_ctrlList);
#if HAS_COLUMNSORDER
	m_order = m_ctrlList->GetColumnsOrder();
#endif
	if (m_colWidths.empty())
	{
		for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
		{
			if (m_columns[m_order[i]])
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


void CReportListHeader::GetDefaultColumns(std::vector<bool>& columns)
{
	columns.clear();
	columns.insert(columns.end(), m_columnInfo.size(), true);
}


void CReportListHeader::OnLoad()
{
	if (!m_baseConfig.empty())
	{
		wxString str = wxConfig::Get()->Read(CFG_SORTING_ORDER(m_baseConfig), L"");
		for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
		{
			m_order[i] = StringUtil::ToCLong(StringUtil::stringW(str));
			int n = str.Find(',');
			if (n > 0)
				str = str.Mid(n + 1);
			else
				str.Empty();
		}

		str = wxConfig::Get()->Read(CFG_SORTING_SORT(m_baseConfig), L"");
		for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
		{
			m_columns[i] = (StringUtil::ToCLong(StringUtil::stringW(str)) != 0);
			int n = str.Find(',');
			if (n > 0)
				str = str.Mid(n + 1);
			else
				str.Empty();
		}

		if (!m_colWidths.empty())
		{
			str = wxConfig::Get()->Read(CFG_COLUMN_WIDTHS(m_baseConfig), L"");
			for (size_t i = 0; i < m_columnInfo.size() && !str.IsEmpty(); ++i)
			{
				m_colWidths[i] = StringUtil::ToCLong(StringUtil::stringW(str));
				int n = str.Find(',');
				if (n > 0)
					str = str.Mid(n + 1);
				else
					str.Empty();
			}
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
		for (size_t i = 0; i < m_order.size(); ++i)
		{
			if (0 < i)
				str << L",";
			str << m_order[i];
		}
		wxConfig::Get()->Write(CFG_SORTING_ORDER(m_baseConfig), str);

		str.clear();
		for (size_t i = 0; i < m_columns.size(); ++i)
		{
			if (0 < i)
				str << L",";
			str << (m_columns[i] ? L"1" : L"0");
		}
		wxConfig::Get()->Write(CFG_SORTING_SORT(m_baseConfig), str);

		str.clear();
		for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
		{
			if (0 < i)
				str << L",";
			if (m_columns[i])
				str << m_ctrlList->GetColumnWidth(i);
			else
				str << m_colWidths[i];
		}
		wxConfig::Get()->Write(CFG_COLUMN_WIDTHS(m_baseConfig), str);

		wxConfig::Get()->Write(CFG_SORT_COLUMN(m_baseConfig), m_iSortCol);
	}
}


void CReportListHeader::OnCloseParent(wxCloseEvent& evt)
{
	assert(m_ctrlList);
#if HAS_COLUMNSORDER
	m_order = m_ctrlList->GetColumnsOrder();
#endif

	OnSave();

	evt.Skip();
}


void CReportListHeader::OnBeginColDrag(wxListEvent& evt)
{
	if (!m_columns[evt.GetColumn()])
		evt.Veto();
}


void CReportListHeader::OnColumnRClick(wxListEvent& evt)
{
	wxMenu menu;
	if (CreateMenu(menu))
		m_parent->PopupMenu(&menu, evt.GetPoint());
}


void CReportListHeader::OnColumnClick(wxListEvent& evt)
{
	Sort(evt.GetColumn());
}


void CReportListHeader::OnUpdateColumn(wxUpdateUIEvent& evt)
{
	evt.Check(m_columns[evt.GetId() - m_idFirst]);
}


void CReportListHeader::OnColumn(wxCommandEvent& evt)
{
	long col = evt.GetId() - m_idFirst;
	if (evt.IsChecked())
	{
		m_columns[col] = true;
		m_ctrlList->SetColumnWidth(col, wxLIST_AUTOSIZE_USEHEADER);
	}
	else
	{
		m_columns[col] = false;
		m_ctrlList->SetColumnWidth(col, 0);
	}
}


void CReportListHeader::OnUpdateRestore(wxUpdateUIEvent& evt)
{
	std::vector<bool> columns;
	GetDefaultColumns(columns);
	bool isSame = columns == m_columns;
	if (isSame)
	{
#if HAS_COLUMNSORDER
		m_order = m_ctrlList->GetColumnsOrder();
#endif
		wxArrayInt order;
		for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
			order.push_back(i);
		isSame = order == m_order;
	}
	evt.Enable(!isSame);
}


void CReportListHeader::OnRestore(wxCommandEvent& evt)
{
	GetDefaultColumns(m_columns);
	wxArrayInt order;
	for (long i = 0; i < static_cast<long>(m_columnInfo.size()); ++i)
		order.push_back(i);
#if HAS_COLUMNSORDER
	m_ctrlList->SetColumnsOrder(order);
#endif
	SizeColumns();
	m_ctrlList->Refresh();
}
