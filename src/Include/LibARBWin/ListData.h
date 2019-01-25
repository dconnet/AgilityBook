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
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-01-01 Ported to wxWidgets.
 * 2008-11-22 Moved GetDispInfo accessors into base class.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
 */

#include <wx/string.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>


class CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CListData)
public:
	CListData();
	virtual ~CListData();
	/// If iCol == -1, return all columns
	virtual std::wstring OnNeedText(long iCol) const = 0;
	/// By default, just uses OnNeedText to fill in text.
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
};
typedef std::shared_ptr<CListData> CListDataPtr;


/// Provide a way to refresh items easily by using a common tree data class
class CTreeData : public wxTreeItemData
{
	DECLARE_NO_COPY_IMPLEMENTED(CTreeData)
public:
	CTreeData();
	~CTreeData();
	virtual std::wstring OnNeedText() const = 0;
};
