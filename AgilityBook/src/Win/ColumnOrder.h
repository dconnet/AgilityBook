#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief interface of the CColumnOrder class
 * @author David Connet
 *
 * Revision History
 */

class CColumnOrder
{
public:
	CColumnOrder(LPCTSTR pItem);
	~CColumnOrder();
	bool Initialize(int nColumns); // Returns true if saved settings restored
	void Save();

	bool SetColumnOrder(int nNewFirstColumn);
	int GetSize() const;
	int FindColumnOrder(int column) const; // Gives index in order for a given column.
	int GetColumnAt(int index) const;
	bool IsDescending(int column) const;
private:
	CString m_Item;
	int m_nColumns;
	bool m_bDefaultDescending;
	int *m_order;
	bool *m_bDescending;
};

inline int CColumnOrder::GetSize() const
{
	return m_nColumns;
}

inline int CColumnOrder::GetColumnAt(int index) const
{
	ASSERT(m_order && 0 <= index && m_nColumns > index);
	return m_order[index];
}

inline bool CColumnOrder::IsDescending(int column) const
{
	ASSERT(m_bDescending && 0 <= column && m_nColumns > column);
	return m_bDescending[column];
}
