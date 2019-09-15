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
 * 2019-09-15 Quiet a wx debug message (see comment below)
 * 2018-10-11 Moved to Win LibARBWin
 * 2010-03-28 Moved SetColumnWidth override to CListCtrl.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-12-14 Created
 */

#include "LibwxARBWin.h"
#include "Widgets.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include <map>
#include <vector>
#include <wx/imaglist.h>

#if !wxCHECK_VERSION(3, 1, 1)
typedef wxListItemAttr wxItemAttr;
#endif

ARB_TYPEDEF(CListData)


struct SortInfo
{
	int nCol;

	SortInfo()
		: nCol(0)
	{
	}
	SortInfo(int col)
		: nCol(col)
	{
	}
	virtual ~SortInfo()
	{
	}
	DECLARE_NO_COPY_IMPLEMENTED(SortInfo)
};
typedef int (wxCALLBACK *CListCtrlCompare)(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo);

// wxListView adds some convenient functions to wxListCtrl
class ARBWIN_API CReportListCtrl : public CListCtrl
{
	DECLARE_CLASS(CReportListCtrl)
public:
	enum class SortHeader
	{
		None,   ///< No header
		NoSort, ///< Header, but no sorting
		Sort    ///< Full sort header
	};
	CReportListCtrl(
			wxWindow *parent,
			bool bSingleSel,
			SortHeader sortHeader = SortHeader::Sort,
			bool bHasBorder = false,
			bool bHasImageList = true);
	CReportListCtrl(
			wxWindow *parent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool bSingleSel = false,
			SortHeader sortHeader = SortHeader::Sort,
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

	bool SortItems(CListCtrlCompare fnSortCallBack, SortInfo const* pSortInfo = nullptr);

	long InsertItem(CListDataPtr const& inData);
	long InsertItem(long index, CListDataPtr const& inData);

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
	void SetSelection(std::vector<long>& indices);
	void ClearSelection();

	/// Return listdata by list id
	CListDataPtr GetData(long item) const;

	std::wstring GetPrintDataAsHtmlTable(bool bFirstLineIsHeader = false) const;

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
		if (oldIndex == newIndex)
			return;
		assert(0 <= oldIndex && oldIndex < static_cast<long>(m_items.size()));
		assert(0 <= newIndex && newIndex < static_cast<long>(m_items.size()));
		if (!(0 <= oldIndex && oldIndex < static_cast<long>(m_items.size())))
			return;
		if (!(0 <= newIndex && newIndex < static_cast<long>(m_items.size())))
			return;
#ifdef _DEBUG
		wxLogNull log; // SetItemPtrData always returns false. But works. Shutup.
#endif
		wxUIntPtr data1 = GetItemData(oldIndex);
		wxUIntPtr data2 = GetItemData(newIndex);
		SetItemPtrData(oldIndex, data2);
		SetItemPtrData(newIndex, data1);
		std::swap(m_items[oldIndex], m_items[newIndex]);
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
	wxItemAttr* OnGetItemAttr(long item) const override;
#if defined(__WXMSW__)
	// Only defined in the MSW and QT ports
	wxItemAttr* OnGetItemColumnAttr(long item, long column) const override;
#else
	virtual wxItemAttr* OnGetItemColumnAttr(long item, long column) const;
#endif
	int OnGetItemColumnImage(long item, long column) const override;
	int OnGetItemImage(long item) const override;
	wxString OnGetItemText(long item, long column) const override;

	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgSortUp;
	int m_imgSortDn;
	std::vector<CListDataPtr> m_items;

private:
	void OnDeleteAllItems(wxListEvent& evt);
	void OnDeleteItem(wxListEvent& evt);
	// Hide wxListCtrl apis - note, this will not prevent usage since
	// these are NOT virtual! Do not implement.
	bool SortItems(wxListCtrlCompare fnSortCallBack, wxIntPtr data);
	long InsertItem(const wxListItem& info);
	long InsertItem(long index, const wxString& label);
	long InsertItem(long index, int imageIndex);
	long InsertItem(long index, const wxString& label, int imageIndex);
	// 2.9.4 changed this to virtual
	void SetImageList(wxImageList* imageList, int which) override {}
};


// Right now, this is specifically used in DlgCalendarQueryDetail. We could
// make it more generic (sort headers, etc), but that can wait until needed.
class ARBWIN_API CCheckListCtrl : public CListCtrl
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
	void SetImageList(wxImageList* imageList, int which) override {}

	void OnClick(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
};
