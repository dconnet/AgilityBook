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
 * 2020-12-07 Add OnCompare for use with CReportListHeader.
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-01-01 Ported to wxWidgets.
 * 2008-11-22 Moved GetDispInfo accessors into base class.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
 */

#include "LibwxARBWin.h"

#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/treectrl.h>


class CListData;
typedef std::shared_ptr<CListData> CListDataPtr;
class ARBWIN_API CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CListData)
public:
	CListData();
	virtual ~CListData();
	/// If iCol == -1, return all columns
	virtual std::wstring OnNeedText(long iCol) const = 0;
	/// By default, just uses OnNeedText to fill in text.
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
	/// Calling when clicking on sort header.
	/// Ascending/descending will be handled by the sort routine.
	virtual int OnCompare(CListDataPtr item, long iCol) const;
};


/// Provide a way to refresh items easily by using a common tree data class
class ARBWIN_API CTreeData : public wxTreeItemData
{
	DECLARE_NO_COPY_IMPLEMENTED(CTreeData)
public:
	CTreeData();
	~CTreeData();
	virtual std::wstring OnNeedText() const = 0;
};
