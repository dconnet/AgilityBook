#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2008-11-22 DRC Moved GetDispInfo accessors into base class.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include <wx/string.h>
#include <wx/listctrl.h>


class CListData
{
public:
	CListData();
	virtual ~CListData();
	/// If iCol == -1, return all columns
	virtual std::wstring OnNeedText(long iCol) const = 0;
	/// By default, just uses OnNeedText to fill in text.
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
};
typedef std::tr1::shared_ptr<CListData> CListDataPtr;


/// Provide a way to refresh items easily by using a common tree data class
class CTreeData : public wxTreeItemData
{
public:
	CTreeData();
	virtual ~CTreeData();
	virtual std::wstring OnNeedText() const = 0;
};
