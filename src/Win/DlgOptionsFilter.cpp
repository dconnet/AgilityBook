/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsFilter.h"

#include "ARBConfig.h"
#include "ARBConfigVenue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFilter property page

IMPLEMENT_DYNAMIC(CDlgOptionsFilter, CPropertyPage)

CDlgOptionsFilter::CDlgOptionsFilter(const ARBConfig& config)
	: CPropertyPage(CDlgOptionsFilter::IDD)
	, m_Config(config)
{
	//{{AFX_DATA_INIT(CDlgOptionsFilter)
	m_ViewDates = -1;
	m_bDateStart = FALSE;
	m_timeStart = 0;
	m_timeEnd = 0;
	m_bDateEnd = FALSE;
	m_ViewVenues = -1;
	m_ViewQs = -1;
	//}}AFX_DATA_INIT
}

CDlgOptionsFilter::~CDlgOptionsFilter()
{
}

void CDlgOptionsFilter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsFilter)
	DDX_Radio(pDX, IDC_OPTIONS_DATES_ALL, m_ViewDates);
	DDX_Control(pDX, IDC_OPTIONS_DATE_START_CHECK, m_ctrlDateStartCheck);
	DDX_Check(pDX, IDC_OPTIONS_DATE_START_CHECK, m_bDateStart);
	DDX_Control(pDX, IDC_OPTIONS_DATE_START, m_ctrlDateStart);
	DDX_DateTimeCtrl(pDX, IDC_OPTIONS_DATE_START, m_timeStart);
	DDX_Control(pDX, IDC_OPTIONS_DATE_END_CHECK, m_ctrlDateEndCheck);
	DDX_DateTimeCtrl(pDX, IDC_OPTIONS_DATE_END, m_timeEnd);
	DDX_Check(pDX, IDC_OPTIONS_DATE_END_CHECK, m_bDateEnd);
	DDX_Control(pDX, IDC_OPTIONS_DATE_END, m_ctrlDateEnd);
	DDX_Radio(pDX, IDC_OPTIONS_VENUES_ALL, m_ViewVenues);
	DDX_Control(pDX, IDC_OPTIONS_VENUES, m_ctrlVenue);
	DDX_Radio(pDX, IDC_OPTIONS_RUNS_ALL, m_ViewQs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsFilter, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsFilter)
	ON_BN_CLICKED(IDC_OPTIONS_DATES_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_DATES_RANGE, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_VENUES_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_VENUES_SELECTED, OnViewUpdate)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_OPTIONS_VENUES, OnSetdispinfoVenues)
	ON_BN_CLICKED(IDC_OPTIONS_RUNS_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_RUNS_Q, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_RUNS_NON_Q, OnViewUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

bool CDlgOptionsFilter::Find(
	const std::string& venue,
	const std::string& div,
	const std::string& level) const
{
	for (std::vector<CVenueFilter>::const_iterator iter = m_VenueFilter.begin();
		iter != m_VenueFilter.end();
		++iter)
	{
		if ((*iter).venue == venue
		&& (0 == div.length() || (0 < div.length() && (*iter).division == div))
		&& (0 == level.length() || (0 < level.length() && (*iter).level == level)))
			return true;
	}
	return false;
}

void CDlgOptionsFilter::FillFilter(HTREEITEM hItem, CString path)
{
	if (TVI_ROOT != hItem)
	{
		// Don't worry about enabled. Things are only "disabled" if the parent
		// checkbox is not set. If that's not set, we won't be calling down
		// into this.
		if (m_ctrlVenue.IsCheckVisible(hItem))
		{
			if (!m_ctrlVenue.GetChecked(hItem))
				return;
			if (!path.IsEmpty())
				path += "/";
			path += m_ctrlVenue.GetItemText(hItem);
		}
	}
	HTREEITEM hChildItem = m_ctrlVenue.GetNextItem(hItem, TVGN_CHILD);
	if (NULL == hChildItem)
	{
		// We're at a leaf...
		CStringArray rawFilter;
		int pos;
		while (0 <= (pos = path.Find('/')))
		{
			rawFilter.Add(path.Left(pos));
			path = path.Mid(pos+1);
		}
		rawFilter.Add(path);
		if (0 < rawFilter.GetSize())
		{
			CVenueFilter filter;
			switch (rawFilter.GetSize())
			{
			default:
			case 3:
				filter.level = (LPCSTR)rawFilter[2];
				// fallthru
			case 2:
				filter.division = (LPCSTR)rawFilter[1];
				// fallthru
			case 1:
				filter.venue = (LPCSTR)rawFilter[0];
			}
			m_VenueFilter.push_back(filter);
		}
	}
	else
	{
		while (NULL != hChildItem)
		{
			FillFilter(hChildItem, path);
			hChildItem = m_ctrlVenue.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

void CDlgOptionsFilter::UpdateControls()
{
	if (0 == m_ViewDates)
	{
		m_ctrlDateStartCheck.EnableWindow(FALSE);
		m_ctrlDateStart.EnableWindow(FALSE);
		m_ctrlDateEndCheck.EnableWindow(FALSE);
		m_ctrlDateEnd.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlDateStartCheck.EnableWindow(TRUE);
		m_ctrlDateStart.EnableWindow(TRUE);
		m_ctrlDateEndCheck.EnableWindow(TRUE);
		m_ctrlDateEnd.EnableWindow(TRUE);
	}
	if (0 == m_ViewVenues)
	{
		m_ctrlVenue.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlVenue.EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFilter message handlers

BOOL CDlgOptionsFilter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	for (ARBConfigVenueList::const_iterator iter = m_Config.GetVenues().begin(); iter != m_Config.GetVenues().end(); ++iter)
	{
		ARBConfigVenue* pVenue = (*iter);
		HTREEITEM hVenue = m_ctrlVenue.InsertItem(
			pVenue->GetName().c_str(),
			TVI_ROOT,
			TVI_LAST);
		m_ctrlVenue.ShowCheckbox(hVenue, true);
		if (Find(pVenue->GetName(), "", ""))
			m_ctrlVenue.SetChecked(hVenue, true);
		m_ctrlVenue.SetItemData(hVenue, reinterpret_cast<LPARAM>(pVenue));
		for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
			iterDiv != pVenue->GetDivisions().end();
			++iterDiv)
		{
			ARBConfigDivision* pDiv = *iterDiv;
			HTREEITEM hDiv = m_ctrlVenue.InsertItem(
				pDiv->GetName().c_str(),
				hVenue,
				TVI_LAST);
			m_ctrlVenue.ShowCheckbox(hDiv, true);
			if (Find(pVenue->GetName(), pDiv->GetName(), ""))
			{
				m_ctrlVenue.SetChecked(hDiv, true);
				m_ctrlVenue.EnsureVisible(hDiv);
			}
			for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
				iterLevel != pDiv->GetLevels().end();
				++iterLevel)
			{
				ARBConfigLevel* pLevel = *iterLevel;
				HTREEITEM hLevel = m_ctrlVenue.InsertItem(
					pLevel->GetName().c_str(),
					hDiv,
					TVI_LAST);
				if (0 < pLevel->GetSubLevels().size())
				{
					m_ctrlVenue.ShowCheckbox(hLevel, false);
					for (ARBConfigSubLevelList::const_iterator iterSubLevel = pLevel->GetSubLevels().begin();
						iterSubLevel != pLevel->GetSubLevels().end();
						++iterSubLevel)
					{
						ARBConfigSubLevel* pSubLevel = *iterSubLevel;
						HTREEITEM hSubLevel = m_ctrlVenue.InsertItem(
							pSubLevel->GetName().c_str(),
							hLevel,
							TVI_LAST);
						m_ctrlVenue.ShowCheckbox(hSubLevel, true);
						if (Find(pVenue->GetName(), pDiv->GetName(), pSubLevel->GetName()))
						{
							m_ctrlVenue.SetChecked(hSubLevel, true);
							m_ctrlVenue.EnsureVisible(hSubLevel);
						}
					}
				}
				else
				{
					m_ctrlVenue.ShowCheckbox(hLevel, true);
					if (Find(pVenue->GetName(), pDiv->GetName(), pLevel->GetName()))
					{
						m_ctrlVenue.SetChecked(hLevel, true);
						m_ctrlVenue.EnsureVisible(hLevel);
					}
				}
			}
		}
	}

	UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsFilter::OnViewUpdate() 
{
	UpdateData(TRUE);
	UpdateControls();
}

void CDlgOptionsFilter::OnSetdispinfoVenues(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	m_VenueFilter.clear();
	FillFilter(TVI_ROOT, "");
	*pResult = 0;
}
