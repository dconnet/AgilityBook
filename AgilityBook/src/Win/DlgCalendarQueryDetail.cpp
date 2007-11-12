/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2007-10-05 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgCalendarQueryDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendarQueryDetail dialog

CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
		std::map<tstring, tstring> const& inLocCodes,
		std::vector<tstring> const& inSelectedLocCodes,
		std::map<tstring, tstring> const& inVenueCodes,
		std::vector<tstring> const& inSelectedVenueCodes,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCalendarQueryDetail::IDD, pParent)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations(inSelectedLocCodes)
	, m_Venues(inSelectedVenueCodes)
{
	//{{AFX_DATA_INIT(CDlgCalendarQueryDetail)
	//}}AFX_DATA_INIT
}


void CDlgCalendarQueryDetail::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCalendarQueryDetail)
	DDX_Control(pDX, IDC_QUERY_LOCATIONS, m_ctrlLocations);
	DDX_Control(pDX, IDC_QUERY_VENUES, m_ctrlVenues);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCalendarQueryDetail, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgCalendarQueryDetail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendarQueryDetail message handlers

BOOL CDlgCalendarQueryDetail::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlLocations.SetExtendedStyle(m_ctrlLocations.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlVenues.SetExtendedStyle(m_ctrlVenues.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	m_ctrlLocations.InsertColumn(0, _T("Code"));
	m_ctrlLocations.InsertColumn(1, _T("Location"));
	m_ctrlVenues.InsertColumn(0, _T("Code"));
	m_ctrlVenues.InsertColumn(1, _T("ARB Code"));

	std::map<tstring, tstring>::const_iterator i;
	for (i = m_LocCodes.begin(); i != m_LocCodes.end(); ++i)
	{
		int idx = m_ctrlLocations.InsertItem(m_ctrlLocations.GetItemCount(), i->first.c_str());
		m_ctrlLocations.SetItemText(idx, 1, i->second.c_str());
		if (m_Locations.end() != std::find(m_Locations.begin(), m_Locations.end(), i->first))
			m_ctrlLocations.SetCheck(idx, TRUE);
	}
	m_ctrlLocations.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlLocations.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	for (i = m_VenueCodes.begin(); i != m_VenueCodes.end(); ++i)
	{
		int idx = m_ctrlVenues.InsertItem(m_ctrlVenues.GetItemCount(), i->first.c_str());
		m_ctrlVenues.SetItemText(idx, 1, i->second.c_str());
		if (m_Venues.end() != std::find(m_Venues.begin(), m_Venues.end(), i->first))
			m_ctrlVenues.SetCheck(idx, TRUE);
	}
	m_ctrlVenues.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlVenues.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCalendarQueryDetail::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	m_Locations.clear();
	m_Venues.clear();

	int idx;
	int nItems = m_ctrlLocations.GetItemCount();
	for (idx = 0; idx < nItems; ++idx)
	{
		if (m_ctrlLocations.GetCheck(idx))
		{
			CString str = m_ctrlLocations.GetItemText(idx, 0);
			m_Locations.push_back((LPCTSTR)str);
		}
	}

	nItems = m_ctrlVenues.GetItemCount();
	for (idx = 0; idx < nItems; ++idx)
	{
		if (m_ctrlVenues.GetCheck(idx))
		{
			CString str = m_ctrlVenues.GetItemText(idx, 0);
			m_Venues.push_back((LPCTSTR)str);
		}
	}

	CDlgBaseDialog::OnOK();
}
