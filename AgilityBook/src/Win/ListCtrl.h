#pragma once

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
 * 2010-03-28 Moved SetColumnWidth override to CListCtrl.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-12-14 Created
 */

#include "Widgets.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include <map>
#include <vector>
#include <wx/imaglist.h>

ARB_TYPEDEF(CListData)


// wxListView adds some convenient functions to wxListCtrl
class CReportListCtrl : public CListCtrl
{
	DECLARE_CLASS(CReportListCtrl)
public:
	enum SortHeader { eNoHeader, eNoSortHeader, eSortHeader };
	CReportListCtrl(
			wxWindow *parent,
			bool bSingleSel,
			SortHeader sortHeader = eSortHeader,
			bool bHasBorder = false,
			bool bHasImageList = true);
	CReportListCtrl(
			wxWindow *parent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool bSingleSel = false,
			SortHeader sortHeader = eSortHeader,
			bool bHasBorder = false,
			bool bHasImageList = true);

	int ImageEmpty() const			{return m_imgEmpty;}
	int ImageSortUp() const			{return m_imgSortUp;}
	int ImageSortDown() const		{return m_imgSortDn;}
	int AddIcon(wxIcon const& icon)
	{
		return m_ImageList.Add(icon);
	}

	void SetColumnSort(long column, int iconDirection);

	long InsertItem(CListDataPtr inData);
	long InsertItem(long index, CListDataPtr inData);

	/**
	 * Returns the first selected item.
	 * @param bRestricted In multi-select lists, only allow a single selection.
	 */
	long GetSelection(bool bRestricted) const
	{
		std::vector<long> indices;
		GetSelection(indices);
		bool bSingle = (bRestricted || HasFlag(wxLC_SINGLE_SEL));
		if ((bSingle && 1 == indices.size())
		|| (!bSingle && 0 < indices.size()))
			return indices[0];
		else
			return -1;
	}

	/**
	 * Returns the number of selected items.
	 */
	size_t GetSelection(std::vector<long>& items) const
	{
		items.clear();
		long item = GetNextSelected(-1);
		while (0 <= item)
		{
			items.push_back(item);
			item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		}
		return items.size();
	}

	bool CanSelectAll() const;
	void SelectAll();

	void SetSelection(
			long index,
			bool bEnsureVisible = false,
			bool bSetFocus = true);
	void SetSelection(
			std::vector<long>& indices,
			bool bEnsureVisible = false,
			bool bSetFocus = true);

	/// Return listdata using data value
	CListDataPtr GetDataByData(wxUIntPtr data) const;
	/// Return listdata by list id
	CListDataPtr GetData(long item) const;
	bool SetData(
			long item,
			CListDataPtr inData);

	std::wstring GetPrintDataAsHtmlTable() const;

	/**
	 * Returns the data required to print/copy a line.
	 */
	void GetPrintLine(
			long item,
			std::vector<std::wstring>& line) const;

	void RefreshItem(long item);

	void SwapEntries(
			long oldIndex,
			long newIndex)
	{
		long data1 = static_cast<long>(GetItemData(oldIndex));
		long data2 = static_cast<long>(GetItemData(newIndex));
		SetItemData(oldIndex, data2);
		SetItemData(newIndex, data1);
		SetSelection(newIndex);
		RefreshItem(oldIndex);
		RefreshItem(newIndex);
	}

protected:
	bool Create(
			wxWindow *parent,
			const wxPoint& pos,
			const wxSize& size,
			bool bSingleSel,
			SortHeader sortHeader,
			bool bHasBorder,
			bool bHasImageList);
	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgSortUp;
	int m_imgSortDn;
	// Instead of putting the data in the control, we put keys in. Currently,
	// wxWidgets sorts using 'long's for data - this will cause a problem on
	// 64bit systems if we try to put an object in - a 64bit ptr won't fit
	// in a long. This has a nice side benefit that we don't have to do
	// memory management of objects within list items!
	int m_NextId;
	typedef std::map<wxUIntPtr, CListDataPtr> DataMap;
	DataMap m_OwnerData;

private:
	void OnDeleteItem(wxListEvent& evt);
	// Hide wxListCtrl insertions - note, this will not prevent insertion since
	// these are NOT virtual! Do not implement.
	long InsertItem(const wxListItem& info);
	long InsertItem(long index, const wxString& label);
	long InsertItem(long index, int imageIndex);
	long InsertItem(long index, const wxString& label, int imageIndex);
	// 2.9.4 changed this to virtual
	virtual void SetImageList(wxImageList* imageList, int which) {}
};


// Right now, this is specifically used in DlgCalendarQueryDetail. We could
// make it more generic (sort headers, etc), but that can wait until needed.
class CCheckListCtrl : public CListCtrl
{
	DECLARE_CLASS(CCheckListCtrl)
public:
	CCheckListCtrl(
			wxWindow *parent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool bHasChecks = true);

	int ImageEmpty() const			{return m_imgEmpty;}
	int ImageNoCheck() const		{return m_imgNoCheck;}
	int ImageChecked() const		{return m_imgChecked;}
	int AddIcon(wxIcon const& icon)
	{
		return m_ImageList.Add(icon);
	}

	bool HasCheck(long item) const;
	bool IsChecked(long item) const;
	bool SetCheck(long item, bool checked = true);
	bool ClearCheck(long item);

	long InsertItem(long index, const wxString& label);
	long InsertItem(long index, const wxString& label, bool checked);
	long InsertItem(long index, const wxString& label, int imageIndex);

protected:
	bool m_HasChecks;
	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgNoCheck;
	int m_imgChecked;

private:
	// Hide wxListCtrl apis - note, this will not prevent usage since
	// these are NOT virtual! Do not implement.
	long InsertItem(wxListItem& info);
	long InsertItem(long index, int imageIndex);
	virtual void SetImageList(wxImageList* imageList, int which) {}

	void OnClick(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
};
