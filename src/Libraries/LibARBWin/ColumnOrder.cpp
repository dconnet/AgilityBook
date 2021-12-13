/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CColumnOrder class
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-28 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "LibARBWin/ColumnOrder.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/RegItemsBase.h"
#include <wx/config.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CColumnOrder
//  m_order is a list of the column numbers in the order of importance
//  m_bDescending indicates the sort order
//   - this not ordered like m_order is, it is column specific.

CColumnOrder::CColumnOrder(std::wstring const& pItem)
	: m_Item(pItem.c_str())
	, m_nColumns(0)
	, m_bDefaultDescending(false)
	, m_order(nullptr)
	, m_bDescending(nullptr)
{
}


bool CColumnOrder::Initialize(int nColumns)
{
	if (0 < m_nColumns)
	{
		m_order.reset();
		m_bDescending.reset();
	}
	m_nColumns = nColumns;
	if (0 < m_nColumns)
	{
		m_order = std::make_unique<int[]>(m_nColumns);
		m_bDescending = std::make_unique<bool[]>(m_nColumns);
		for (int i = 0; i < m_nColumns; ++i)
		{
			m_order[i] = i;
			m_bDescending[i] = m_bDefaultDescending;
		}
	}
	bool rc = false;
	// Load last settings.
	wxString str = wxConfig::Get()->Read(CFG_SORTING_ORDER(m_Item.wx_str()), L"");
	int i;
	for (i = 0; i < m_nColumns && !str.IsEmpty(); ++i)
	{
		rc = true;
		m_order[i] = StringUtil::ToCLong(StringUtil::stringW(str));
		int n = str.Find(',');
		if (n > 0)
			str = str.Mid(n + 1);
		else
			str.Empty();
	}
	str = wxConfig::Get()->Read(CFG_SORTING_SORT(m_Item.wx_str()), L"");
	for (i = 0; i < m_nColumns && !str.IsEmpty(); ++i)
	{
		rc = true;
		m_bDescending[i] = StringUtil::ToCLong(StringUtil::stringW(str)) == 0 ? false : true;
		int n = str.Find(',');
		if (n > 0)
			str = str.Mid(n + 1);
		else
			str.Empty();
	}
	return rc;
}


void CColumnOrder::Save()
{
	{
		fmt::wmemory_buffer str;
		for (int i = 0; i < m_nColumns; ++i)
		{
			if (0 < i)
				fmt::format_to(std::back_inserter(str), L"{}", L",");
			fmt::format_to(std::back_inserter(str), L"{}", m_order[i]);
		}
		wxConfig::Get()->Write(CFG_SORTING_ORDER(m_Item.wx_str()), fmt::to_string(str).c_str());
	}
	{
		fmt::wmemory_buffer str;
		for (int i = 0; i < m_nColumns; ++i)
		{
			if (0 < i)
				fmt::format_to(std::back_inserter(str), L"{}", L",");
			fmt::format_to(std::back_inserter(str), L"{}", static_cast<int>(m_bDescending[i]));
		}
		wxConfig::Get()->Write(CFG_SORTING_SORT(m_Item.wx_str()), fmt::to_string(str).c_str());
	}
}


int CColumnOrder::FindColumnOrder(int column) const
{
	for (int i = 0; i < m_nColumns; ++i)
		if (m_order[i] == column)
			return i;
	return -1;
}


bool CColumnOrder::SetColumnOrder(int nNewFirstColumn)
{
	int index = FindColumnOrder(nNewFirstColumn);
	if (-1 == nNewFirstColumn)
		return false;
	// if the newcol is 1st, then flip the sort order.
	if (0 == index)
		m_bDescending[nNewFirstColumn] = !m_bDescending[nNewFirstColumn];
	// Otherwise, shift the cols and put the new one 1st.
	else
	{
		int tmp = m_order[index];
		for (int j = index - 1; j >= 0; --j)
			m_order[j + 1] = m_order[j];
		m_order[0] = tmp;
	}
	return true;
}
