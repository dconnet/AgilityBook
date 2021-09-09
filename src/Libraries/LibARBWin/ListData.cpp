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
 * 2021-09-09 Add support for EnableCheckBoxes.
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-01-01 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
 */

#include "stdafx.h"
#include "LibARBWin/ListData.h"

#include "ARBCommon/StringUtil.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CListData::CListData()
{
}


CListData::~CListData()
{
}


int CListData::OnCompare(CListDataPtr const& item, long iCol) const
{
	return 0;
}


void CListData::OnNeedListItem(long iCol, wxListItem& info) const
{
	info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
	info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
}


bool CListData::OnNeedCheck() const
{
	return false;
}


CTreeData::CTreeData()
{
}


CTreeData::~CTreeData()
{
}
