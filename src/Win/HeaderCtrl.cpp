/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief Sort header for subclassing in a list control.
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-24 DRC Created
 */

#include "stdafx.h"
#include "resource.h"
#include "HeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrl2

BEGIN_MESSAGE_MAP(CHeaderCtrl2, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrl2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CHeaderCtrl2::CHeaderCtrl2()
	: m_bInit(false)
{
	m_ImageList.Create(16, 16, ILC_MASK, 2, 0);
	CWinApp* app = AfxGetApp();
	m_sortAscending = m_ImageList.Add(app->LoadIcon(IDI_HEADER_UP));
	m_sortDescending = m_ImageList.Add(app->LoadIcon(IDI_HEADER_DOWN));
}

CHeaderCtrl2::~CHeaderCtrl2()
{
}

CHeaderCtrl2::SortOrder CHeaderCtrl2::GetSortOrder(int iCol) const
{
	if (0 > iCol || iCol >= GetItemCount())
		return eNoSort;
	HDITEM item;
	item.mask = HDI_FORMAT | HDI_IMAGE;
	GetItem(iCol, &item);
	if (item.fmt & HDF_IMAGE)
	{
		if (item.iImage == m_sortAscending)
			return eAscending;
		else if (item.iImage == m_sortDescending)
			return eDescending;
	}
	return eNoSort;
}

void CHeaderCtrl2::Sort(int iCol, SortOrder eOrder)
{
	if (0 > iCol || iCol >= GetItemCount())
		return;
	if (!m_bInit)
	{
		m_bInit = true;
		SetImageList(&m_ImageList);
#if _MSC_VER >= 1300
		SetBitmapMargin(0);
#endif
	}
	int nIndex = -1;
	switch (eOrder)
	{
	case eAscending: nIndex = m_sortAscending; break;
	case eDescending: nIndex = m_sortDescending; break;
	}
	HDITEM item;
	item.mask = HDI_FORMAT;
	if (0 <= nIndex)
		item.mask |= HDI_IMAGE;
	item.iImage = nIndex;
	item.fmt = HDF_LEFT | HDF_STRING;
	if (0 <= nIndex)
		item.fmt |= HDF_IMAGE;
	SetItem(iCol, &item);
}

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrl2 message handlers
