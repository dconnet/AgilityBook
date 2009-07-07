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
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-02-20 DRC Added subitem editing to lists.
 * @li 2007-02-26 DRC Fix a problem redrawing list columns.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-30 DRC Allow restricted selection in multi-select lists.
 * @li 2004-10-04 DRC Added div-by-0 tests.
 * @li 2004-09-16 DRC Override SetColumnWidth to auto-fix tooltip rectangles.
 * @li 2004-09-09 DRC Added tooltips to header control.
 * @li 2004-09-03 DRC Using the sort icon forced alignment to left.
 * @li 2004-08-26 DRC Added GetPrintLine to CListCtrl2.
 * @li 2003-11-22 DRC When copying, only create a header if more than 1 line
 *                    is selected (or if only one line exists).
 * @li 2003-11-21 DRC Added multi-select and copy/selectall support.
 */

#include "stdafx.h"
#include "ListCtrl.h"

#include "Globals.h"
#include "ListData.h"

#include "res/CalEmpty.xpm"
#include "res/checked.xpm"
#include "res/HdrDown.xpm"
#include "res/HdrUp.xpm"
#include "res/unchecked.xpm"


IMPLEMENT_CLASS(CReportListCtrl, wxListView)


CReportListCtrl::CReportListCtrl(
		wxWindow *parent,
		bool bSingleSel,
		SortHeader sortHeader,
		bool bHasBorder,
		bool bHasImageList)
	: wxListView()
	, m_ImageList(16,16)
	, m_imgEmpty(-1)
	, m_imgSortUp(-1)
	, m_imgSortDn(-1)
	, m_NextId(1)
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
	: wxListView()
	, m_ImageList(16,16)
	, m_imgEmpty(-1)
	, m_imgSortUp(-1)
	, m_imgSortDn(-1)
	, m_NextId(1)
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
	if (!wxListView::Create(parent, wxID_ANY, pos, size, wxLC_REPORT
		| (bSingleSel ? wxLC_SINGLE_SEL : 0)
		| ((sortHeader == eSortHeader) ? 0 : (sortHeader == eNoSortHeader) ? wxLC_NO_SORT_HEADER : wxLC_NO_HEADER)
		| (bHasBorder ? wxBORDER : wxNO_BORDER)))
	{
		return false;
	}
	Connect(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(CReportListCtrl::OnDeleteItem));
	// Make the blank one the 1st icon so if an icon isn't set in a list
	// it will use this by default
	if (bHasImageList || sortHeader == eSortHeader)
	{
		m_imgEmpty = m_ImageList.Add(wxIcon(CalEmpty_xpm));
		m_imgSortUp = m_ImageList.Add(wxIcon(HdrUp_xpm));
		m_imgSortDn = m_ImageList.Add(wxIcon(HdrDown_xpm));
		wxListView::SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	}
	return true;
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


long CReportListCtrl::InsertItem(CListDataPtr inData)
{
	if (!inData)
		return -1;
	wxListItem info;
	info.SetId(GetItemCount());
	info.SetMask(wxLIST_MASK_DATA);
	info.SetData(m_NextId);
	m_OwnerData[m_NextId] = inData;
	++m_NextId;
	inData->OnNeedListItem(0, info);

	long item = wxListView::InsertItem(info);

	if (0 <= item)
	{
		long nCols = GetColumnCount();
		for (long iCol = 1; iCol < nCols; ++iCol)
		{
			wxListItem info2;
			info2.SetId(item);
			info2.SetColumn(iCol);
			inData->OnNeedListItem(iCol, info2);
			SetItem(info2);
		}
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


CListDataPtr CReportListCtrl::GetDataByData(long data) const
{
	CListDataPtr ptr;
	if (0 < data)
	{
		DataMap::const_iterator iter = m_OwnerData.find(data);
		if (iter != m_OwnerData.end())
			ptr = iter->second;
	}
	return ptr;
}


CListDataPtr CReportListCtrl::GetData(long item) const
{
	CListDataPtr ptr;
	if (0 <= item && item < GetItemCount())
		ptr = GetDataByData(static_cast<long>(GetItemData(item)));
	return ptr;
}


bool CReportListCtrl::SetData(long item, CListDataPtr inData)
{
	bool bSet = false;
	if (0 <= item && item < GetItemCount())
	{
		bSet = true;
		long data = static_cast<long>(GetItemData(item));
		if (0 < data)
			m_OwnerData[data] = inData;
		else
		{
			SetItemData(item, m_NextId);
			m_OwnerData[m_NextId] = inData;
			++m_NextId;
		}
	}
	return bSet;
}


static void PushData(
		otstringstream& data,
		CReportListCtrl const* ctrl,
		int item,
		bool bBold)
{
	data << wxT("<tr>");
	std::vector<wxString> line;
	ctrl->GetPrintLine(item, line);
	for (std::vector<wxString>::const_iterator i = line.begin(); i != line.end(); ++i)
	{
		if (bBold)
			data << wxT("<td><strong>") << i->c_str() << wxT("</strong></td>\n");
		else
			data << wxT("<td>") << i->c_str() << wxT("</td>\n");
	}
	data << wxT("</tr>\n");
}


wxString CReportListCtrl::GetPrintDataAsHtmlTable() const
{
	otstringstream data;
	data << wxT("<table border=\"0\">");
	PushData(data, this, -1, true);
	for (long item = 0; item < GetItemCount(); ++item)
	{
		PushData(data, this, item, false);
	}
	data << wxT("</table>\n");
	return data.str().c_str();
}


// This allows a derived class to print a subset of columns if it wants.
void CReportListCtrl::GetPrintLine(
		long item,
		std::vector<wxString>& line) const
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
	}
}


void CReportListCtrl::OnDeleteItem(wxListEvent& evt)
{
	long data = evt.GetData();
	if (0 < data)
	{
		DataMap::iterator iter = m_OwnerData.find(data);
		if (iter != m_OwnerData.end())
			m_OwnerData.erase(iter);
	}
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CCheckListCtrl, wxListView)


CCheckListCtrl::CCheckListCtrl(
		wxWindow *parent,
		const wxPoint& pos,
		const wxSize& size,
		bool bHasChecks)
	: m_HasChecks(bHasChecks)
	, m_ImageList(16,16)
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
		Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CCheckListCtrl::OnClick));
		Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CCheckListCtrl::OnKeyDown));
		m_imgEmpty = m_ImageList.Add(wxIcon(CalEmpty_xpm));
		m_imgNoCheck = m_ImageList.Add(wxIcon(unchecked_xpm));
		m_imgChecked = m_ImageList.Add(wxIcon(checked_xpm));
		wxListView::SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
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
		return wxListView::InsertItem(index, label);
}


long CCheckListCtrl::InsertItem(long index, const wxString& label, bool checked)
{
	return InsertItem(index, label, checked ? m_imgChecked : m_imgNoCheck);
}


long CCheckListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
	if (m_HasChecks)
		return wxListView::InsertItem(index, label, imageIndex);
	else
		return wxListView::InsertItem(index, label);
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
