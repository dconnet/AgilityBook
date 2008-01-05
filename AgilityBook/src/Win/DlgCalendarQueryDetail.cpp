/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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

#include "ARBConfig.h"
#include "DlgQueryDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendarQueryDetail dialog

CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
		ARBConfig const& inConfig,
		std::map<tstring, tstring> const& inLocCodes,
		std::map<tstring, tstring> const& inVenueCodes,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCalendarQueryDetail::IDD, pParent)
	, m_ctrlLocations(false)
	, m_ctrlVenues(false)
	, m_EditCodes(true)
	, m_Config(inConfig)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations()
	, m_Venues()
{
}

CDlgCalendarQueryDetail::CDlgCalendarQueryDetail(
		ARBConfig const& inConfig,
		std::map<tstring, tstring> const& inLocCodes,
		std::vector<tstring> const& inSelectedLocCodes,
		std::map<tstring, tstring> const& inVenueCodes,
		std::vector<tstring> const& inSelectedVenueCodes,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCalendarQueryDetail::IDD, pParent)
	, m_ctrlLocations(false)
	, m_ctrlVenues(false)
	, m_EditCodes(false)
	, m_Config(inConfig)
	, m_LocCodes(inLocCodes)
	, m_VenueCodes(inVenueCodes)
	, m_Locations(inSelectedLocCodes)
	, m_Venues(inSelectedVenueCodes)
{
}


void CDlgCalendarQueryDetail::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCalendarQueryDetail)
	DDX_Control(pDX, IDC_QUERY_LOCATIONS, m_ctrlLocations);
	DDX_Control(pDX, IDC_QUERY_LOCCODE_NEW, m_ctrlNewLoc);
	DDX_Control(pDX, IDC_QUERY_LOCCODE_EDIT, m_ctrlEditLoc);
	DDX_Control(pDX, IDC_QUERY_LOCCODE_DELETE, m_ctrlDeleteLoc);
	DDX_Control(pDX, IDC_QUERY_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_QUERY_VENUE_NEW, m_ctrlNewVenue);
	DDX_Control(pDX, IDC_QUERY_VENUE_EDIT, m_ctrlEditVenue);
	DDX_Control(pDX, IDC_QUERY_VENUE_DELETE, m_ctrlDeleteVenue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCalendarQueryDetail, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgCalendarQueryDetail)
	ON_NOTIFY(NM_DBLCLK, IDC_QUERY_LOCATIONS, OnNMDblclkQueryLocations)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUERY_LOCATIONS, OnLvnItemchangedQueryLocations)
	ON_NOTIFY(NM_DBLCLK, IDC_QUERY_VENUES, OnNMDblclkQueryVenues)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUERY_VENUES, OnLvnItemchangedQueryVenues)
	ON_BN_CLICKED(IDC_QUERY_LOCCODE_NEW, OnNewLocationCode)
	ON_BN_CLICKED(IDC_QUERY_LOCCODE_EDIT, OnEditLocationCode)
	ON_BN_CLICKED(IDC_QUERY_LOCCODE_DELETE, OnDeleteLocationCode)
	ON_BN_CLICKED(IDC_QUERY_VENUE_NEW, OnNewVenueCode)
	ON_BN_CLICKED(IDC_QUERY_VENUE_EDIT, OnEditVenueCode)
	ON_BN_CLICKED(IDC_QUERY_VENUE_DELETE, OnDeleteVenueCode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgCalendarQueryDetail::UpdateButtons()
{
	BOOL bSelected = (1 == m_ctrlLocations.GetSelectedCount());
	m_ctrlEditLoc.EnableWindow(bSelected);
	m_ctrlDeleteLoc.EnableWindow(bSelected);
	bSelected = (1 == m_ctrlVenues.GetSelectedCount());
	m_ctrlEditVenue.EnableWindow(bSelected);
	m_ctrlDeleteVenue.EnableWindow(bSelected);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendarQueryDetail message handlers

BOOL CDlgCalendarQueryDetail::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP;
	if (!m_EditCodes)
	{
		styles |= LVS_EX_CHECKBOXES;
		m_ctrlNewLoc.ShowWindow(SW_HIDE);
		m_ctrlEditLoc.ShowWindow(SW_HIDE);
		m_ctrlDeleteLoc.ShowWindow(SW_HIDE);
		m_ctrlNewVenue.ShowWindow(SW_HIDE);
		m_ctrlEditVenue.ShowWindow(SW_HIDE);
		m_ctrlDeleteVenue.ShowWindow(SW_HIDE);
	}
	m_ctrlLocations.SetExtendedStyle(m_ctrlLocations.GetExtendedStyle() | styles);
	m_ctrlVenues.SetExtendedStyle(m_ctrlVenues.GetExtendedStyle() | styles);

	CString str;
	str.LoadString(IDS_COL_CODE);
	m_ctrlLocations.InsertColumn(0, str);
	str.LoadString(IDS_COL_LOCATION);
	m_ctrlLocations.InsertColumn(1, str);
	str.LoadString(IDS_COL_CODE);
	m_ctrlVenues.InsertColumn(0, str);
	str.LoadString(IDS_COL_VENUE);
	m_ctrlVenues.InsertColumn(1, str);

	std::map<tstring, tstring>::const_iterator i;
	for (i = m_LocCodes.begin(); i != m_LocCodes.end(); ++i)
	{
		int idx = m_ctrlLocations.InsertItem(m_ctrlLocations.GetItemCount(), i->first.c_str());
		m_ctrlLocations.SetItemText(idx, 1, i->second.c_str());
		if (!m_EditCodes)
		{
			if (m_Locations.end() != std::find(m_Locations.begin(), m_Locations.end(), i->first))
				m_ctrlLocations.SetCheck(idx, TRUE);
		}
	}
	m_ctrlLocations.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlLocations.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	for (i = m_VenueCodes.begin(); i != m_VenueCodes.end(); ++i)
	{
		int idx = m_ctrlVenues.InsertItem(m_ctrlVenues.GetItemCount(), i->first.c_str());
		m_ctrlVenues.SetItemText(idx, 1, i->second.c_str());
		if (!m_EditCodes)
		{
			if (m_Venues.end() != std::find(m_Venues.begin(), m_Venues.end(), i->first))
				m_ctrlVenues.SetCheck(idx, TRUE);
		}
	}
	m_ctrlVenues.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlVenues.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	UpdateButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCalendarQueryDetail::OnNMDblclkQueryLocations(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	OnEditLocationCode();
	*pResult = 0;
}


void CDlgCalendarQueryDetail::OnLvnItemchangedQueryLocations(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}


void CDlgCalendarQueryDetail::OnNMDblclkQueryVenues(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	OnEditVenueCode();
	*pResult = 0;
}


void CDlgCalendarQueryDetail::OnLvnItemchangedQueryVenues(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}


void CDlgCalendarQueryDetail::OnNewLocationCode()
{
	CDlgQueryDetail dlg(_T(""), _T(""), this);
	if (IDOK == dlg.DoModal())
	{
		int idx = m_ctrlLocations.InsertItem(m_ctrlLocations.GetItemCount(), dlg.GetCode().c_str());
		m_ctrlLocations.SetItemText(idx, 1, dlg.GetName().c_str());
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnEditLocationCode()
{
	int idx = m_ctrlLocations.GetSelection();
	if (0 <= idx)
	{
		CDlgQueryDetail dlg(m_ctrlLocations.GetItemText(idx, 0), m_ctrlLocations.GetItemText(idx, 1), this);
		if (IDOK == dlg.DoModal())
		{
			m_ctrlLocations.SetItemText(idx, 0, dlg.GetCode().c_str());
			m_ctrlLocations.SetItemText(idx, 1, dlg.GetName().c_str());
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::OnDeleteLocationCode()
{
	int idx = m_ctrlLocations.GetSelection();
	if (0 <= idx)
	{
		m_ctrlLocations.DeleteItem(idx);
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnNewVenueCode()
{
	CDlgQueryDetail dlg(_T(""), _T(""), this, &m_Config);
	if (IDOK == dlg.DoModal())
	{
		tstring code = dlg.GetCode();
		tstring name = dlg.GetName();
		if (!code.empty())
		{
			int idx = m_ctrlVenues.InsertItem(m_ctrlVenues.GetItemCount(), code.c_str());
			m_ctrlVenues.SetItemText(idx, 1, name.c_str());
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::OnEditVenueCode()
{
	int idx = m_ctrlVenues.GetSelection();
	if (0 <= idx)
	{
		CDlgQueryDetail dlg(m_ctrlVenues.GetItemText(idx, 0), m_ctrlVenues.GetItemText(idx, 1), this, &m_Config);
		if (IDOK == dlg.DoModal())
		{
			m_ctrlVenues.SetItemText(idx, 0, dlg.GetCode().c_str());
			m_ctrlVenues.SetItemText(idx, 1, dlg.GetName().c_str());
			UpdateButtons();
		}
	}
}


void CDlgCalendarQueryDetail::OnDeleteVenueCode()
{
	int idx = m_ctrlVenues.GetSelection();
	if (0 <= idx)
	{
		m_ctrlVenues.DeleteItem(idx);
		UpdateButtons();
	}
}


void CDlgCalendarQueryDetail::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	if (m_EditCodes)
	{
		m_LocCodes.clear();
		m_VenueCodes.clear();

		int idx;
		int nItems = m_ctrlLocations.GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			CString str1 = m_ctrlLocations.GetItemText(idx, 0);
			CString str2 = m_ctrlLocations.GetItemText(idx, 1);
			m_LocCodes[(LPCTSTR)str1] = (LPCTSTR)str2;
		}

		nItems = m_ctrlVenues.GetItemCount();
		for (idx = 0; idx < nItems; ++idx)
		{
			CString str1 = m_ctrlVenues.GetItemText(idx, 0);
			CString str2 = m_ctrlVenues.GetItemText(idx, 1);
			m_VenueCodes[(LPCTSTR)str1] = (LPCTSTR)str2;
		}
	}

	else
	{
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
	}

	CDlgBaseDialog::OnOK();
}
