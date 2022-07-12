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
 * 2020-12-11 Reorganized classes
 * 2020-12-07 Add a generic way to handle sorting/moving columns.
 * 2020-01-27 Add option for row coloring.
 * 2019-09-15 Quiet a wx debug message (see comment below)
 * 2018-10-11 Moved to Win LibARBWin
 * 2010-03-28 Moved SetColumnWidth override to CListCtrl.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-12-14 Created
 */

#include "LibARBWin/Widgets.h"

#include "ARBCommon/ARBTypes.h"


namespace dconSoft
{

// Right now, this is specifically used in DlgCalendarQueryDetail. We could
// make it more generic (sort headers, etc), but that can wait until needed.
class CCheckListCtrl : public ARBWin::CListCtrl
{
	DECLARE_CLASS(CCheckListCtrl)
public:
	CCheckListCtrl(
		wxWindow* parent,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		bool bHasChecks = true);

	int ImageEmpty() const
	{
		return m_imgEmpty;
	}
	int ImageNoCheck() const
	{
		return m_imgNoCheck;
	}
	int ImageChecked() const
	{
		return m_imgChecked;
	}
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
	void SetImageList(wxImageList* imageList, int which) override
	{
	}

	void OnClick(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
};

} // namespace dconSoft
