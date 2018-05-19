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
#include "ListCtrl.h"

#include "DPI.h"
#include "Globals.h"
#include "ImageHelper.h"
#include "ListData.h"
#include <sstream>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CReportListCtrl, CListCtrl)


CReportListCtrl::CReportListCtrl(
		wxWindow *parent,
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
	Create(parent, wxDefaultPosition, wxDefaultSize, bSingleSel, sortHeader, bHasBorder, bHasImageList);
}


CReportListCtrl::CReportListCtrl(
		wxWindow *parent,
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
	Create(parent, pos, size, bSingleSel, sortHeader, bHasBorder, bHasImageList);
}


bool CReportListCtrl::Create(
		wxWindow *parent,
		const wxPoint& pos,
		const wxSize& size,
		bool bSingleSel,
		SortHeader sortHeader,
		bool bHasBorder,
		bool bHasImageList)
{
	long style = wxLC_REPORT | wxLC_VIRTUAL
		| (bSingleSel ? wxLC_SINGLE_SEL : 0)
		| ((sortHeader == eSortHeader) ? 0 : (sortHeader == eNoSortHeader) ? wxLC_NO_SORT_HEADER : wxLC_NO_HEADER)
		| (bHasBorder ? wxBORDER : wxNO_BORDER);

	if (!CListCtrl::Create(parent, wxID_ANY, pos, size, style))
	{
		return false;
	}
	BIND_OR_CONNECT(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, wxListEventHandler, CReportListCtrl::OnDeleteAllItems);
	BIND_OR_CONNECT(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler, CReportListCtrl::OnDeleteItem);

	// Make the blank one the 1st icon so if an icon isn't set in a list
	// it will use this by default
	if (bHasImageList || sortHeader == eSortHeader)
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
	return nullptr;
}


wxItemAttr* CReportListCtrl::OnGetItemColumnAttr(long item, long column) const
{
	return nullptr;
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
	if (iconDirection > 0 && item.GetImage() != m_imgSortUp)
	{
		item.SetImage(m_imgSortUp);
		bSet = true;
	}
	else if (iconDirection < 0 && item.GetImage() != m_imgSortDn)
	{
		item.SetImage(m_imgSortDn);
		bSet = true;
	}
	else if (iconDirection == 0 && item.GetImage() >= 0)
	{
		item.SetImage(-1);
		bSet = true;
	}
	if (bSet)
		SetColumn(column, item);
}


bool CReportListCtrl::SortItems(CListCtrlCompare fnSortCallBack, SortInfo const* pSortInfo)
{
	std::stable_sort(m_items.begin(), m_items.end(), [fnSortCallBack, pSortInfo](CListDataPtr const& one, CListDataPtr const& two)
					 {
						 return fnSortCallBack(one, two, pSortInfo) < 0;
					 }
	);
	Refresh();
	return true;
}


long CReportListCtrl::InsertItem(CListDataPtr inData)
{
	return InsertItem(GetItemCount(), inData);
}


long CReportListCtrl::InsertItem(long index, CListDataPtr inData)
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


void CReportListCtrl::SetSelection(
		long index,
		bool bEnsureVisible,
		bool bSetFocus)
{
	std::vector<long> indices;
	indices.push_back(index);
	SetSelection(indices, bEnsureVisible, bSetFocus);
}


void CReportListCtrl::SetSelection(
		std::vector<long>& indices,
		bool bEnsureVisible,
		bool bSetFocus)
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
			if (bSetFocus)
				Focus(index);
		}
	}
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


static void PushData(
		std::wostringstream& data,
		CReportListCtrl const* ctrl,
		int item,
		bool bBold)
{
	data << L"<tr>";
	std::vector<std::wstring> line;
	ctrl->GetPrintLine(item, line);
	for (std::vector<std::wstring>::const_iterator i = line.begin(); i != line.end(); ++i)
	{
		if (bBold)
			data << L"<td><strong>" << *i << L"</strong></td>\n";
		else
			data << L"<td>" << *i << L"</td>\n";
	}
	data << L"</tr>\n";
}


std::wstring CReportListCtrl::GetPrintDataAsHtmlTable(bool bFirstLineIsHeader) const
{
	std::wostringstream data;
	data << L"<table border=\"0\">";
	if (!bFirstLineIsHeader)
		PushData(data, this, -1, true);
	for (long item = 0; item < GetItemCount(); ++item)
	{
		PushData(data, this, item, bFirstLineIsHeader && item == 0);
	}
	data << L"</table>\n";
	return data.str();
}


// This allows a derived class to print a subset of columns if it wants.
void CReportListCtrl::GetPrintLine(
		long item,
		std::vector<std::wstring>& line) const
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

IMPLEMENT_CLASS(CCheckListCtrl, CListCtrl)


CCheckListCtrl::CCheckListCtrl(
		wxWindow *parent,
		const wxPoint& pos,
		const wxSize& size,
		bool bHasChecks)
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
		BIND_OR_CONNECT(wxEVT_LEFT_DOWN, wxMouseEventHandler, CCheckListCtrl::OnClick);
		BIND_OR_CONNECT(wxEVT_KEY_DOWN, wxKeyEventHandler, CCheckListCtrl::OnKeyDown);
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
