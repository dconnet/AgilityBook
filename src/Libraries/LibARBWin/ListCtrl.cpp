/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * 2020-12-07 Add a generic way to handle sorting/moving columns.
 * 2020-12-05 In SetColumnSort, always set the icon.
 * 2020-01-27 Add option for row coloring.
 * 2019-05-17 Enable alternate row coloring.
 * 2018-12-16 Convert to fmt.
 * 2018-10-11 Moved to Win LibARBWin
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-03-28 Moved SetColumnWidth override to CListCtrl.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-02-20 Added subitem editing to lists.
 * 2007-02-26 Fix a problem redrawing list columns.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-30 Allow restricted selection in multi-select lists.
 * 2004-10-04 Added div-by-0 tests.
 * 2004-09-16 Override SetColumnWidth to auto-fix tooltip rectangles.
 * 2004-09-09 Added tooltips to header control.
 * 2004-09-03 Using the sort icon forced alignment to left.
 * 2004-08-26 Added GetPrintLine to CListCtrl2.
 * 2003-11-22 When copying, only create a header if more than 1 line
 *            is selected (or if only one line exists).
 * 2003-11-21 Added multi-select and copy/selectall support.
 */

#include "stdafx.h"
#include "LibARBWin/ListCtrl.h"

#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DPI.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/ListData.h"
#include "fmt/format.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


bool CReportListCtrl::m_enableRowColors = true;

// Note to self: If this is immediately after includes, doxygen throws a hissy fit.
wxIMPLEMENT_CLASS(CReportListCtrl, CListCtrl)


void CReportListCtrl::EnableRowColors(bool bEnable)
{
	m_enableRowColors = bEnable;
}


CReportListCtrl::CReportListCtrl(
	wxWindow* parent,
	bool bSingleSel,
	SortHeader sortHeader,
	bool bHasBorder,
	bool bHasImageList)
	: CListCtrl()
	, m_ImageList()
	, m_imgEmpty(-1)
	, m_imgSortUp(-1)
	, m_imgSortDn(-1)
{
	Create(
		parent,
		wxDefaultPosition,
		wxDefaultSize,
		bSingleSel,
		sortHeader,
		bHasBorder,
		bHasImageList,
		m_enableRowColors);
}


CReportListCtrl::CReportListCtrl(
	wxWindow* parent,
	const wxPoint& pos,
	const wxSize& size,
	bool bSingleSel,
	SortHeader sortHeader,
	bool bHasBorder,
	bool bHasImageList)
	: CListCtrl()
	, m_ImageList()
	, m_imgEmpty(-1)
	, m_imgSortUp(-1)
	, m_imgSortDn(-1)
{
	Create(parent, pos, size, bSingleSel, sortHeader, bHasBorder, bHasImageList, m_enableRowColors);
}


bool CReportListCtrl::Create(
	wxWindow* parent,
	const wxPoint& pos,
	const wxSize& size,
	bool bSingleSel,
	SortHeader sortHeader,
	bool bHasBorder,
	bool bHasImageList,
	bool bEnableRowColors)
{
	long style = wxLC_REPORT | wxLC_VIRTUAL | (bSingleSel ? wxLC_SINGLE_SEL : 0)
				 | ((sortHeader == SortHeader::Sort)
						? 0
						: (sortHeader == SortHeader::NoSort) ? wxLC_NO_SORT_HEADER : wxLC_NO_HEADER)
				 | (bHasBorder ? wxBORDER : wxNO_BORDER);

	if (!CListCtrl::Create(parent, wxID_ANY, pos, size, style))
	{
		return false;
	}
	EnableAlternateRowColours(bEnableRowColors);

	Bind(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, &CReportListCtrl::OnDeleteAllItems, this);
	Bind(wxEVT_COMMAND_LIST_DELETE_ITEM, &CReportListCtrl::OnDeleteItem, this);

	// Make the blank one the 1st icon so if an icon isn't set in a list
	// it will use this by default
	if (bHasImageList || sortHeader == SortHeader::Sort)
	{
		m_ImageList.Create(DPI::Scale(this, 16), DPI::Scale(this, 16));
		m_imgEmpty = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrBlank));
		m_imgSortUp = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrHeaderUp));
		m_imgSortDn = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrHeaderDown));
		CListCtrl::SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	}
	return true;
}


wxItemAttr* CReportListCtrl::OnGetItemAttr(long item) const
{
	return CListCtrl::OnGetItemAttr(item);
}


wxItemAttr* CReportListCtrl::OnGetItemColumnAttr(long item, long column) const
{
#if defined(__WXMSW__)
	// Only defined in the MSW and QT ports
	return CListCtrl::OnGetItemColumnAttr(item, column);
#else
	return nullptr;
#endif
}


int CReportListCtrl::OnGetItemColumnImage(long item, long column) const
{
	int index = -1;
	if (0 <= item && item < static_cast<long>(m_items.size()))
	{
		wxListItem info;
		m_items[item]->OnNeedListItem(column, info);
		if (info.GetMask() & wxLIST_MASK_IMAGE)
			index = info.GetImage();
	}
	return index;
}


int CReportListCtrl::OnGetItemImage(long item) const
{
	return OnGetItemColumnImage(item, 0);
}


wxString CReportListCtrl::OnGetItemText(long item, long column) const
{
	wxString text;
	if (0 <= item && item < static_cast<long>(m_items.size()))
	{
		wxListItem info;
		m_items[item]->OnNeedListItem(column, info);
		if (info.GetMask() & wxLIST_MASK_TEXT)
			text = info.GetText();
	}
	return text;
}


void CReportListCtrl::SetColumnSort(long column, int iconDirection)
{
	bool bSet = false;
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT | wxLIST_MASK_IMAGE);
	GetColumn(column, item);
	if (iconDirection > 0)
	{
		item.SetImage(m_imgSortUp);
		bSet = true;
	}
	else if (iconDirection < 0)
	{
		item.SetImage(m_imgSortDn);
		bSet = true;
	}
	else if (iconDirection == 0)
	{
		item.SetImage(-1);
		bSet = true;
	}
	if (bSet)
		SetColumn(column, item);
}


bool CReportListCtrl::SortItems(CListCtrlCompare fnSortCallBack, SortInfo const* pSortInfo)
{
	// Since the underlaying control doesn't know that we changed item order,
	// we need to specifically get the selection and reset it after the sort.

	// Get old selection, clear it.
	long itemFocus = GetFocusedItem();
	std::vector<long> items;
	GetSelection(items);
	ClearSelection();

	// Translate selection indices to data pointers.
	std::vector<CListDataPtr> realItems;
	for (auto item : items)
		realItems.push_back(GetData(item));
	CListDataPtr pFocusItem = itemFocus >= 0 ? GetData(itemFocus) : nullptr;

	std::stable_sort(
		m_items.begin(),
		m_items.end(),
		[fnSortCallBack, pSortInfo](CListDataPtr const& one, CListDataPtr const& two) {
			return fnSortCallBack(one, two, pSortInfo) < 0;
		});

	// Now, restore selection/focus.
	items.clear();
	for (long index = 0; index < GetItemCount(); ++index)
	{
		CListDataPtr data = GetData(index);
		if (pFocusItem && pFocusItem == data)
			Focus(index);
		for (auto item : realItems)
		{
			if (item == data)
			{
				items.push_back(index);
				break;
			}
		}
	}
	SetSelection(items);

	Refresh();
	return true;
}


long CReportListCtrl::InsertItem(CListDataPtr const& inData)
{
	return InsertItem(GetItemCount(), inData);
}


long CReportListCtrl::InsertItem(long index, CListDataPtr const& inData)
{
	long item = -1;

	if (inData)
	{
		if (static_cast<long>(m_items.size()) <= index)
		{
			item = static_cast<long>(m_items.size());
			m_items.push_back(inData);
		}
		else
		{
			if (index < 0)
				index = 0;
			item = index;
			m_items.insert(m_items.begin() + index, inData);
		}
		SetItemCount(static_cast<long>(m_items.size()));
		Refresh();
	}

	return item;
}


bool CReportListCtrl::CanSelectAll() const
{
	return !HasFlag(wxLC_SINGLE_SEL);
}


void CReportListCtrl::SelectAll()
{
	if (!HasFlag(wxLC_SINGLE_SEL))
	{
		long count = GetItemCount();
		for (long item = 0; item < count; ++item)
		{
			Select(item, true);
		}
	}
}


void CReportListCtrl::SetSelection(long index, bool bEnsureVisible, bool bSetFocus)
{
	std::vector<long> indices;
	indices.push_back(index);
	SetSelection(indices);
	if (bEnsureVisible)
		EnsureVisible(index);
	if (bSetFocus)
		Focus(index);
}


void CReportListCtrl::SetSelection(std::vector<long>& indices)
{
	// Clear everything.
	long item = GetNextSelected(-1);
	while (0 <= item)
	{
		Select(item, false);
		item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}
	// Set it.
	for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		long index = *iter;
		if (index >= 0 && index < GetItemCount())
		{
			Select(index);
		}
	}
}


void CReportListCtrl::ClearSelection()
{
	std::vector<long> indices;
	SetSelection(indices);
}


CListDataPtr CReportListCtrl::GetData(long item) const
{
	CListDataPtr ptr;
	if (0 <= item && item < GetItemCount())
	{
		ptr = m_items[item];
	}
	return ptr;
}


static void PushData(fmt::wmemory_buffer& data, CReportListCtrl const* ctrl, int item, bool bBold)
{
	fmt::format_to(data, L"<tr>");
	std::vector<std::wstring> line;
	ctrl->GetPrintLine(item, line);
	for (std::vector<std::wstring>::const_iterator i = line.begin(); i != line.end(); ++i)
	{
		if (bBold)
			fmt::format_to(data, L"<td><strong>{}</strong></td>\n", *i);
		else
			fmt::format_to(data, L"<td>{}</td>\n", *i);
	}
	fmt::format_to(data, L"</tr>\n");
}


std::wstring CReportListCtrl::GetPrintDataAsHtmlTable(bool bFirstLineIsHeader) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<table border=\"0\">");
	if (!bFirstLineIsHeader)
		PushData(data, this, -1, true);
	for (long item = 0; item < GetItemCount(); ++item)
	{
		PushData(data, this, item, bFirstLineIsHeader && item == 0);
	}
	fmt::format_to(data, L"</table>\n");
	return fmt::to_string(data);
}


// This allows a derived class to print a subset of columns if it wants.
void CReportListCtrl::GetPrintLine(long item, std::vector<std::wstring>& line) const
{
	line.clear();
	CListDataPtr data = GetData(item);
	if (0 > item || data)
	{
		long nCols = GetColumnCount();
		for (long iCol = 0; iCol < nCols; ++iCol)
		{
			if (0 > item)
				line.push_back(GetListColumnText(this, item, iCol));
			else
				line.push_back(data->OnNeedText(iCol));
		}
	}
}


void CReportListCtrl::RefreshItem(long item)
{
	CListDataPtr data = GetData(item);
	if (data)
	{
		int nColumns = GetColumnCount();
		for (int iCol = 0; iCol < nColumns; ++iCol)
		{
			wxListItem info;
			info.SetId(item);
			info.SetColumn(iCol);
			data->OnNeedListItem(iCol, info);
			SetItem(info);
		}
		CListCtrl::RefreshItem(item);
	}
}


void CReportListCtrl::OnDeleteAllItems(wxListEvent& evt)
{
	m_items.clear();
	SetItemCount(0);
	evt.Skip();
}


void CReportListCtrl::OnDeleteItem(wxListEvent& evt)
{
	long index = evt.GetIndex();
	if (0 <= index && index < static_cast<long>(m_items.size()))
	{
		auto iter = m_items.begin() + index;
		m_items.erase(iter);
	}
	evt.Skip();
}

/////////////////////////////////////////////////////////////////////////////

struct ColSortInfo : public SortInfo
{
	CReportListHeader* pColInfo;

	ColSortInfo(CReportListHeader* inColInfo)
		: SortInfo(inColInfo->GetSortColumn())
		, pColInfo(inColInfo)
	{
	}
};

int wxCALLBACK CompareItems(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	ColSortInfo const* pInfo = dynamic_cast<ColSortInfo const*>(pSortInfo);
	assert(pInfo);

	if (!item1)
		return -1;
	else if (!item2)
		return 1;

	long iCol = std::abs(pInfo->nCol) - 1;
	int rc = item1->OnCompare(item2, iCol);

	if (0 > pInfo->nCol)
		rc *= -1;
	return rc;
}


CReportListHeader::CReportListHeader(int idFirst, std::vector<ColumnInfo> const& columns)
	: m_idFirst(idFirst)
	, m_columns()
	, m_ctrlList(nullptr)
	, m_columnInfo(columns)
	, m_iSortCol(0)
	, m_bIsSorted(false)
{
#ifdef _DEBUG
	for (auto i = 0; i < m_columnInfo.size(); ++i)
		assert(i == m_columnInfo[i].index);
#endif
}


CReportListHeader::~CReportListHeader()
{
}


void CReportListHeader::Initialize(wxWindow* parent, CReportListCtrl* ctrlList)
{
	assert(parent);
	assert(ctrlList);
	m_ctrlList = ctrlList;

	GetDefaultColumns(m_columns);

	parent->Bind(
		wxEVT_UPDATE_UI,
		&CReportListHeader::OnUpdateColumn,
		this,
		m_idFirst,
		m_idFirst + m_columnInfo.size() - 1);
	parent->Bind(wxEVT_MENU, &CReportListHeader::OnColumn, this, m_idFirst, m_idFirst + m_columnInfo.size() - 1);
	parent->Bind(wxEVT_UPDATE_UI, &CReportListHeader::OnUpdateRestore, this, m_idFirst + m_columnInfo.size(), wxID_ANY);
	parent->Bind(wxEVT_MENU, &CReportListHeader::OnRestore, this, m_idFirst + m_columnInfo.size(), wxID_ANY);

	m_order.clear();
	for (long i = 0; i < m_columnInfo.size(); ++i)
		m_order.push_back(i);
	m_bIsSorted = false;
	m_iSortCol = 0;

	OnLoadColumnOrder();
	OnLoadSorted();
	OnLoadSortedColumn();

	parent->Bind(wxEVT_CLOSE_WINDOW, &CReportListHeader::OnCloseParent, this);
	m_ctrlList->Bind(wxEVT_LIST_COL_BEGIN_DRAG, &CReportListHeader::OnBeginColDrag, this);
}


void CReportListHeader::CreateMenu(wxMenu& menu)
{
	assert(m_ctrlList);
	m_order = m_ctrlList->GetColumnsOrder();
	for (auto col : m_order)
	{
		assert(col >= 0 && col < m_columnInfo.size());
		menu.AppendCheckItem(m_idFirst + col, StringUtil::GetTranslation(m_columnInfo[col].name));
	}
	menu.AppendSeparator();
	menu.Append(m_idFirst + m_columnInfo.size(), _("Restore"));
}


void CReportListHeader::Update()
{
	assert(m_ctrlList);
	m_order = m_ctrlList->GetColumnsOrder();

	OnSaveColumnOrder();
	OnSaveSorted();
	OnSaveSortedColumn();
}


void CReportListHeader::CreateColumns()
{
	assert(m_ctrlList);

	if (m_ctrlList->GetColumnCount() != 0)
		m_order = m_ctrlList->GetColumnsOrder();

	m_ctrlList->DeleteAllItems();
	for (int col = m_ctrlList->GetColumnCount() - 1; 0 <= col; --col)
		m_ctrlList->DeleteColumn(col);

	for (long iCol = 0; iCol < m_columnInfo.size(); ++iCol)
	{
		m_ctrlList->InsertColumn(iCol, wxGetTranslation(m_columnInfo[iCol].name), m_columnInfo[iCol].fmt);
		if (!m_columns[iCol])
			m_ctrlList->SetColumnWidth(iCol, 0);
	}
	m_ctrlList->SetColumnsOrder(m_order);
}


void CReportListHeader::SizeColumns()
{
	assert(m_ctrlList);
	m_order = m_ctrlList->GetColumnsOrder();
	for (int i = 0; i < m_ctrlList->GetColumnCount(); ++i)
	{
		if (m_columns[m_order[i]])
			m_ctrlList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
		else
			m_ctrlList->SetColumnWidth(i, 0);
	}
}


void CReportListHeader::Sort()
{
	assert(m_ctrlList);
	if (m_bIsSorted)
	{
		ColSortInfo sortInfo(this);
		m_ctrlList->SortItems(CompareItems, &sortInfo);
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
	SetSortColumn((col + 1) * nBackwards);

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


void CReportListHeader::OnLoadColumnOrder()
{
}


void CReportListHeader::OnSaveColumnOrder()
{
}


void CReportListHeader::OnLoadSorted()
{
}


void CReportListHeader::OnSaveSorted()
{
}


void CReportListHeader::OnLoadSortedColumn()
{
}


void CReportListHeader::OnSaveSortedColumn()
{
}


void CReportListHeader::SetSortColumn(int iCol)
{
	assert(m_ctrlList);
	m_iSortCol = iCol;

	if (0 == iCol)
		return;

	SetSorted(true);
	long neg = 1;
	long col = iCol;
	if (0 > iCol)
	{
		neg = -1;
		col = iCol * -1;
	}

	Sort();
}


void CReportListHeader::OnCloseParent(wxCloseEvent& evt)
{
	Update();
	evt.Skip();
}


void CReportListHeader::OnBeginColDrag(wxListEvent& evt)
{
	if (!m_columns[evt.GetColumn()])
		evt.Veto();
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
		m_order = m_ctrlList->GetColumnsOrder();
		wxArrayInt order;
		for (long i = 0; i < m_columnInfo.size(); ++i)
			order.push_back(i);
		isSame = order == m_order;
	}
	evt.Enable(!isSame);
}


void CReportListHeader::OnRestore(wxCommandEvent& evt)
{
	GetDefaultColumns(m_columns);
	wxArrayInt order;
	for (long i = 0; i < m_columnInfo.size(); ++i)
		order.push_back(i);
	m_ctrlList->SetColumnsOrder(order);
	SizeColumns();
	m_ctrlList->Refresh();
}

/////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CCheckListCtrl, CListCtrl)


CCheckListCtrl::CCheckListCtrl(wxWindow* parent, const wxPoint& pos, const wxSize& size, bool bHasChecks)
	: m_HasChecks(bHasChecks)
	, m_ImageList()
	, m_imgEmpty(-1)
	, m_imgNoCheck(-1)
	, m_imgChecked(-1)
{
	int flags = wxLC_NO_SORT_HEADER | wxLC_REPORT | wxLC_SORT_ASCENDING;
	if (!bHasChecks)
		flags |= wxLC_SINGLE_SEL;
	Create(parent, wxID_ANY, pos, size, flags);
	if (bHasChecks)
	{
		Bind(wxEVT_LEFT_DOWN, &CCheckListCtrl::OnClick, this);
		Bind(wxEVT_KEY_DOWN, &CCheckListCtrl::OnKeyDown, this);
		m_ImageList.Create(DPI::Scale(this, 16), DPI::Scale(this, 16));
		m_imgEmpty = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrBlank));
		m_imgNoCheck = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrUnChecked));
		m_imgChecked = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrChecked));
		CListCtrl::SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	}
}


bool CCheckListCtrl::HasCheck(long item) const
{
	if (m_HasChecks)
	{
		wxListItem info;
		info.SetId(item);
		info.SetColumn(0);
		info.SetMask(wxLIST_MASK_IMAGE);
		if (GetItem(info))
			return info.m_image != m_imgEmpty;
	}
	return false;
}


bool CCheckListCtrl::IsChecked(long item) const
{
	if (m_HasChecks)
	{
		wxListItem info;
		info.SetId(item);
		info.SetColumn(0);
		info.SetMask(wxLIST_MASK_IMAGE);
		if (GetItem(info))
			return info.m_image == m_imgChecked;
	}
	return false;
}


bool CCheckListCtrl::SetCheck(long item, bool checked)
{
	if (m_HasChecks)
	{
		wxListItem info;
		info.SetId(item);
		info.SetColumn(0);
		info.SetMask(wxLIST_MASK_IMAGE);
		info.m_image = checked ? m_imgChecked : m_imgNoCheck;
		return SetItem(info);
	}
	return false;
}


bool CCheckListCtrl::ClearCheck(long item)
{
	if (m_HasChecks)
	{
		wxListItem info;
		info.SetId(item);
		info.SetColumn(0);
		info.SetMask(wxLIST_MASK_IMAGE);
		info.m_image = m_imgEmpty;
		return SetItem(info);
	}
	return false;
}


long CCheckListCtrl::InsertItem(long index, const wxString& label)
{
	if (m_HasChecks)
		return InsertItem(index, label, m_imgNoCheck);
	else
		return CListCtrl::InsertItem(index, label);
}


long CCheckListCtrl::InsertItem(long index, const wxString& label, bool checked)
{
	return InsertItem(index, label, checked ? m_imgChecked : m_imgNoCheck);
}


long CCheckListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
	if (m_HasChecks)
		return CListCtrl::InsertItem(index, label, imageIndex);
	else
		return CListCtrl::InsertItem(index, label);
}


void CCheckListCtrl::OnClick(wxMouseEvent& evt)
{
	if (m_HasChecks)
	{
		int flags = 0;
		long item = HitTest(evt.GetPosition(), flags);
		if (wxNOT_FOUND != item && (flags & wxLIST_HITTEST_ONITEMICON))
		{
			if (HasCheck(item))
				SetCheck(item, !IsChecked(item));
		}
	}
	evt.Skip();
}


void CCheckListCtrl::OnKeyDown(wxKeyEvent& evt)
{
	if (m_HasChecks)
	{
		switch (evt.GetKeyCode())
		{
		case WXK_SPACE:
		case WXK_NUMPAD_SPACE:
			if (0 < GetSelectedItemCount())
			{
				long item = -1;
				while (-1 != (item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)))
				{
					if (HasCheck(item))
						SetCheck(item, !IsChecked(item));
				}
			}
			break;
		}
	}
	evt.Skip();
}
