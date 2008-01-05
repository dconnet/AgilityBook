/*
 * Copyright © 2006-2008 David Connet. All Rights Reserved.
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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "stdafx.h"
#include "ComboBox.h"

#include "ARBConfigVenue.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBox2

IMPLEMENT_DYNAMIC(CComboBox2, CComboBox)

CComboBox2::CComboBox2(bool bAutoDelete)
	: m_bAutoDelete(bAutoDelete)
{
}


CComboBox2::~CComboBox2()
{
}


CListData* CComboBox2::GetData(int index) const
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
		return reinterpret_cast<CListData*>(GetItemDataPtr(index));
	else
		return NULL;
}


void CComboBox2::SetData(int index, CListData* inData)
{
	if (0 <= index && index < GetCount() && m_bAutoDelete)
	{
		CListData* pData = GetData(index);
		if (pData)
			delete pData;
		SetItemDataPtr(index, inData);
	}
}


BEGIN_MESSAGE_MAP(CComboBox2, CComboBox)
	//{{AFX_MSG_MAP(CComboBox2)
	ON_WM_DESTROY()
	ON_MESSAGE(CB_RESETCONTENT, OnResetContent)
	ON_MESSAGE(CB_DELETESTRING, OnDeleteString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBox2 message handlers

void CComboBox2::OnDestroy()
{
	ResetContent();
	CComboBox::OnDestroy();
}


LRESULT CComboBox2::OnResetContent(WPARAM, LPARAM)
{
	if (m_bAutoDelete)
	{
		for (int i = GetCount() - 1; i >= 0; --i)
		{
			CListData* pData = GetData(i);
			SetItemDataPtr(i, NULL);
			delete pData;
		}
	}
	return Default();
}


LRESULT CComboBox2::OnDeleteString(WPARAM wParam, LPARAM)
{
	if (m_bAutoDelete)
	{
		int index = static_cast<int>(wParam);
		CListData* pData = GetData(index);
		SetItemDataPtr(index, NULL);
		delete pData;
	}
	return Default();
}


/////////////////////////////////////////////////////////////////////////////
// CVenueComboBox

IMPLEMENT_DYNAMIC(CVenueComboBox, CComboBox2)

CVenueComboBox::CVenueComboBox(bool bAutoDelete)
	: CComboBox2(bAutoDelete)
{
}


CVenueComboBox::~CVenueComboBox()
{
}


bool CVenueComboBox::Initialize(
		ARBConfigVenueList const& venues,
		tstring const& inSelectVenue,
		bool useLongName)
{
	bool bSelectionSet = false;
	for (ARBConfigVenueList::const_iterator iterVenue = venues.begin();
		iterVenue != venues.end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		int index;
		if (useLongName)
			index = AddString(pVenue->GetLongName().c_str());
		else
			index = AddString(pVenue->GetName().c_str());
		if (m_bAutoDelete)
			SetData(index, new CListPtrData<ARBConfigVenuePtr>(pVenue));
		if (pVenue->GetName() == inSelectVenue)
		{
			bSelectionSet = true;
			SetCurSel(index);
		}
	}
	return bSelectionSet;
}


BEGIN_MESSAGE_MAP(CVenueComboBox, CComboBox2)
	//{{AFX_MSG_MAP(CVenueComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVenueComboBox message handlers
