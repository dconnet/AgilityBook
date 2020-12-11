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
#include "CheckListCtrl.h"

#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DPI.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/RegItemsBase.h"
#include "fmt/format.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


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
