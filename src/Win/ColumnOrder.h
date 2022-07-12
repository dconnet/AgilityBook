#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CColumnOrder class
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-01-28 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include <assert.h>
#include <string>


namespace dconSoft
{


class CColumnOrder
{
	DECLARE_NO_COPY_IMPLEMENTED(CColumnOrder)
public:
	CColumnOrder(std::wstring const& pItem);
	bool Initialize(int nColumns); // Returns true if saved settings restored
	void Save();

	bool SetColumnOrder(int nNewFirstColumn);
	int GetSize() const
	{
		return m_nColumns;
	}
	int FindColumnOrder(int column) const; // Gives index in order for a given column.
	int GetColumnAt(int index) const
	{
		assert(m_order && 0 <= index && m_nColumns > index);
		return m_order[index];
	}
	bool IsDescending(int column) const
	{
		assert(m_bDescending && 0 <= column && m_nColumns > column);
		return m_bDescending[column];
	}

private:
	wxString m_Item;
	int m_nColumns;
	bool m_bDefaultDescending;
	std::unique_ptr<int[]> m_order;
	std::unique_ptr<bool[]> m_bDescending;
};

} // namespace dconSoft
