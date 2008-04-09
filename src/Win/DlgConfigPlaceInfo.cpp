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
 * @brief implementation of the CDlgConfigPlaceInfo class
 * @author David Connet
 *
 * Revision History
 * @li 2006-11-04 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigPlaceInfo.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigPlaceInfo dialog

CDlgConfigPlaceInfo::CDlgConfigPlaceInfo(
		std::vector<short> const& inExistingPlaces,
		short inPlace,
		double inValue,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigPlaceInfo::IDD, pParent)
	, m_Place(inPlace)
	, m_Value(inValue)
	, m_origPlace(inPlace)
	, m_origValue(inValue)
	, m_ExistingPlaces(inExistingPlaces)
{
	//{{AFX_DATA_INIT(CDlgConfigPlaceInfo)
	//}}AFX_DATA_INIT
}


void CDlgConfigPlaceInfo::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigPlaceInfo)
	DDX_Text(pDX, IDC_CONFIG_PLACEINFO_PLACE, m_Place);
	DDX_Text(pDX, IDC_CONFIG_PLACEINFO_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConfigPlaceInfo, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigPlaceInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigPlaceInfo message handlers

void CDlgConfigPlaceInfo::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	if (m_Place != m_origPlace)
	{
		if (m_ExistingPlaces.end() != std::find(m_ExistingPlaces.begin(), m_ExistingPlaces.end(), m_Place))
		{
			AfxMessageBox(IDS_CONFIG_PLACE_UNIQUE, MB_ICONWARNING);
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_PLACEINFO_PLACE));
			return;
		}
	}
	if (0 > m_Place)
	{
		AfxMessageBox(IDS_CONFIG_PLACE_TOOSMALL, MB_ICONWARNING);
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_PLACEINFO_PLACE));
		return;
	}
	if (0.0 > m_Value)
	{
		if (IDYES != AfxMessageBox(IDS_CONFIG_CHECK_VALUE, MB_ICONWARNING | MB_YESNO))
		{
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_PLACEINFO_PLACE));
			return;
		}
	}
	CDialog::OnOK();
}
