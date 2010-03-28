/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CColumnOrder class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-28 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "ColumnOrder.h"

#include "ARBTypes.h"
#include <wx/config.h>

/////////////////////////////////////////////////////////////////////////////
// CColumnOrder
//  m_order is a list of the column numbers in the order of importance
//  m_bDescending indicates the sort order
//   - this not ordered like m_order is, it is column specific.

CColumnOrder::CColumnOrder(wxString const& pItem)
	: m_Item(pItem)
	, m_nColumns(0)
	, m_bDefaultDescending(false)
	, m_order(NULL)
	, m_bDescending(NULL)
{
}


CColumnOrder::~CColumnOrder()
{
	delete [] m_order;
	delete [] m_bDescending;
}


bool CColumnOrder::Initialize(int nColumns)
{
	if (0 < m_nColumns)
	{
		delete [] m_order;
		delete [] m_bDescending;
	}
	m_nColumns = nColumns;
	if (0 < m_nColumns)
	{
		m_order = new int[m_nColumns];
		m_bDescending = new bool[m_nColumns];
		for (int i = 0; i < m_nColumns; ++i)
		{
			m_order[i] = i;
			m_bDescending[i] = m_bDefaultDescending;
		}
	}
	bool rc = false;
	// Load last settings.
	wxString item = wxT("Sorting/") + m_Item + wxT("Order");
	wxString str = wxConfig::Get()->Read(item, wxT(""));
	int i;
	for (i = 0; i < m_nColumns && !str.IsEmpty(); ++i)
	{
		rc = true;
		m_order[i] = tstringUtil::ToCLong(str);
		int n = str.Find(',');
		if (n > 0)
			str = str.Mid(n+1);
		else
			str.Empty();
	}
	item = wxT("Sorting/") + m_Item + wxT("Sort");
	str = wxConfig::Get()->Read(item, wxT(""));
	for (i = 0; i < m_nColumns && !str.IsEmpty(); ++i)
	{
		rc = true;
		m_bDescending[i] = tstringUtil::ToCLong(str) == 0 ? false : true;
		int n = str.Find(',');
		if (n > 0)
			str = str.Mid(n+1);
		else
			str.Empty();
	}
	return rc;
}


void CColumnOrder::Save()
{
	{
		wxString str;
		for (int i = 0; i < m_nColumns; ++i)
		{
			if (0 < i)
				str << wxT(",");
			str << m_order[i];
		}
		wxString item = wxT("Sorting/") + m_Item + wxT("Order");
		wxConfig::Get()->Write(item, str);
	}
	{
		wxString str;
		for (int i = 0; i < m_nColumns; ++i)
		{
			if (0 < i)
				str << wxT(",");
			str << static_cast<int>(m_bDescending[i]);
		}
		wxString item = wxT("Sorting/") + m_Item + wxT("Sort");
		wxConfig::Get()->Write(item, str);
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
			m_order[j+1] = m_order[j];
		m_order[0] = tmp;
	}
	return true;
}
