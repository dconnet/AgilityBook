#pragma once

/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @li 2008-12-14 DRC Created
 */

#include "ARBString.h"
#include "ARBTypes.h"
#include <map>
#include <wx/listctrl.h>
ARB_TYPEDEF(CListData)


// wxListView adds some convenient functions to wxListCtrl
class CReportListCtrl : public wxListView
{
	DECLARE_CLASS(CReportListCtrl)
public:
	enum SortHeader { eNoHeader, eNoSortHeader, eSortHeader };
	CReportListCtrl(
			wxWindow *parent,
			bool bSingleSel,
			SortHeader sortHeader = eSortHeader,
			bool bHasBorder = false);
	CReportListCtrl(
			wxWindow *parent,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool bSingleSel = false,
			SortHeader sortHeader = eSortHeader,
			bool bHasBorder = false);

	int ImageEmpty() const			{return m_imgEmpty;}
	int ImageSortUp() const			{return m_imgSortUp;}
	int ImageSortDown() const		{return m_imgSortDn;}
	int AddIcon(wxIcon const& icon)
	{
		return m_ImageList.Add(icon);
	}

	void SetColumnSort(long column, int iconDirection);

    long InsertItem(CListDataPtr inData);

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
			bool bEnsureVisible = false);
	void SetSelection(
			std::vector<long>& indices,
			bool bEnsureVisible = false);

	/// Return listdata using data value
	CListDataPtr GetDataByData(long data) const;
	/// Return listdata by list id
	CListDataPtr GetData(long item) const;
	bool SetData(
			long item,
			CListDataPtr inData);

	/**
	 * Returns the data required to print/copy a line.
	 */
	void GetPrintLine(
			long item,
			std::vector<wxString>& line) const;

	void RefreshItem(long item);

protected:
	bool Create(
			wxWindow *parent,
			const wxPoint& pos,
			const wxSize& size,
			bool bSingleSel,
			SortHeader sortHeader,
			bool bHasBorder);
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
	typedef std::map<int, CListDataPtr> DataMap;
	DataMap m_OwnerData;

private:
	void OnDeleteItem(wxListEvent& evt);
	// Hide wxListCtrl insertions - note, this will not prevent insertion since
	// these are NOT virtual! Do not implement.
    long InsertItem(const wxListItem& info);
    long InsertItem(long index, const wxString& label);
    long InsertItem(long index, int imageIndex);
    long InsertItem(long index, const wxString& label, int imageIndex);
};
