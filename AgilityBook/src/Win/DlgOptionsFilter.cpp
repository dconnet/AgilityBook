/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsFilter.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBConfigVenue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFilter property page

IMPLEMENT_DYNAMIC(CDlgOptionsFilter, CDlgBasePropertyPage)

CDlgOptionsFilter::CDlgOptionsFilter(CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CDlgOptionsFilter::IDD)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CDlgOptionsFilter)
	m_ViewDates = -1;
	m_bDateStart = FALSE;
	m_timeStart = 0;
	m_bDateEnd = FALSE;
	m_timeEnd = 0;
	m_ViewNames = -1;
	m_bNotEntered = TRUE;
	m_bPlanning = TRUE;
	m_bEntered = TRUE;
	m_ViewVenues = -1;
	m_ViewQs = -1;
	//}}AFX_DATA_INIT
}

CDlgOptionsFilter::~CDlgOptionsFilter()
{
}

void CDlgOptionsFilter::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsFilter)
	DDX_Radio(pDX, IDC_OPT_FILTER_DATE_ALL, m_ViewDates);
	DDX_Control(pDX, IDC_OPT_FILTER_DATE_START_CHECK, m_ctrlDateStartCheck);
	DDX_Check(pDX, IDC_OPT_FILTER_DATE_START_CHECK, m_bDateStart);
	DDX_Control(pDX, IDC_OPT_FILTER_DATE_START, m_ctrlDateStart);
	DDX_DateTimeCtrl(pDX, IDC_OPT_FILTER_DATE_START, m_timeStart);
	DDX_Control(pDX, IDC_OPT_FILTER_DATE_END_CHECK, m_ctrlDateEndCheck);
	DDX_DateTimeCtrl(pDX, IDC_OPT_FILTER_DATE_END, m_timeEnd);
	DDX_Check(pDX, IDC_OPT_FILTER_DATE_END_CHECK, m_bDateEnd);
	DDX_Control(pDX, IDC_OPT_FILTER_DATE_END, m_ctrlDateEnd);
	DDX_Radio(pDX, IDC_OPT_FILTER_LOG_NAME_ALL, m_ViewNames);
	DDX_Control(pDX, IDC_OPT_FILTER_LOG_NAME, m_ctrlNames);
	DDX_Check(pDX, IDC_OPT_FILTER_CAL_NOT_ENTERED, m_bNotEntered);
	DDX_Check(pDX, IDC_OPT_FILTER_CAL_PLANNING, m_bPlanning);
	DDX_Check(pDX, IDC_OPT_FILTER_CAL_ENTERED, m_bEntered);
	DDX_Radio(pDX, IDC_OPT_FILTER_RUN_VENUES_ALL, m_ViewVenues);
	DDX_Control(pDX, IDC_OPT_FILTER_RUN_VENUES, m_ctrlVenue);
	DDX_Radio(pDX, IDC_OPT_FILTER_RUN_RUNS_ALL, m_ViewQs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsFilter, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsFilter)
	ON_WM_HELPINFO()
	ON_NOTIFY(TVN_SETDISPINFO, IDC_OPT_FILTER_LOG_NAME, OnSetdispinfoNames)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_OPT_FILTER_RUN_VENUES, OnSetdispinfoVenues)
	ON_BN_CLICKED(IDC_OPT_FILTER_DATE_ALL, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_DATE_RANGE, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_LOG_NAME_ALL, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_LOG_NAME_SELECTED, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_RUN_VENUES_ALL, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_RUN_VENUES_SELECTED, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_RUN_RUNS_ALL, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_RUN_RUNS_Q, OnUpdateFilters)
	ON_BN_CLICKED(IDC_OPT_FILTER_RUN_RUNS_NON_Q, OnUpdateFilters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CDlgOptionsFilter::Find(
		ARBString const& venue,
		ARBString const& div,
		ARBString const& level) const
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

void CDlgOptionsFilter::FillFilter(
		HTREEITEM hItem,
		CString path)
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
				path += _T("/");
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
				filter.level = (LPCTSTR)rawFilter[2];
				// fallthru
			case 2:
				filter.division = (LPCTSTR)rawFilter[1];
				// fallthru
			case 1:
				filter.venue = (LPCTSTR)rawFilter[0];
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
	if (0 == m_ViewNames)
	{
		m_ctrlNames.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlNames.EnableWindow(TRUE);
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
	CDlgBasePropertyPage::OnInitDialog();

	std::set<ARBString> names;
	std::set<ARBString>::iterator iter;
	m_pDoc->GetAllTrainingLogNames(names);
	for (iter = m_NameFilter.begin(); iter != m_NameFilter.end(); )
	{
		if (names.end() == names.find((*iter)))
			iter = m_NameFilter.erase(iter);
		else
			++iter;
	}
	if (0 == names.size())
	{
		m_ViewNames = 0;
		GetDlgItem(IDC_OPT_FILTER_LOG_NAME_SELECTED)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		for (iter = names.begin(); iter != names.end(); ++iter)
		{
			HTREEITEM hItem = m_ctrlNames.InsertItem(
				(*iter).c_str(),
				TVI_ROOT,
				TVI_LAST);
			m_ctrlNames.ShowCheckbox(hItem, true);
			if (m_NameFilter.end() != m_NameFilter.find((*iter)))
				m_ctrlNames.SetChecked(hItem, true);
		}
	}

	for (ARBConfigVenueList::const_iterator iter = m_pDoc->GetConfig().GetVenues().begin(); iter != m_pDoc->GetConfig().GetVenues().end(); ++iter)
	{
		ARBConfigVenue* pVenue = (*iter);
		HTREEITEM hVenue = m_ctrlVenue.InsertItem(
			pVenue->GetName().c_str(),
			TVI_ROOT,
			TVI_LAST);
		m_ctrlVenue.ShowCheckbox(hVenue, true);
		if (Find(pVenue->GetName(), _T(""), _T("")))
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
			if (Find(pVenue->GetName(), pDiv->GetName(), _T("")))
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

BOOL CDlgOptionsFilter::OnHelpInfo(HELPINFO* pHelpInfo)
{
	ShowContextHelp(pHelpInfo);
	return TRUE;
}

void CDlgOptionsFilter::OnSetdispinfoNames(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	m_NameFilter.clear();
	HTREEITEM hItem = m_ctrlNames.GetRootItem();
	while (NULL != hItem)
	{
		if (m_ctrlNames.IsCheckVisible(hItem)
		&& m_ctrlNames.GetChecked(hItem))
		{
			CString str = m_ctrlNames.GetItemText(hItem);
			str.TrimRight();
			str.TrimLeft();
			if (!str.IsEmpty())
				m_NameFilter.insert((LPCTSTR)str);
		}
		hItem = m_ctrlNames.GetNextItem(hItem, TVGN_NEXT);
	}
	*pResult = 0;
}

void CDlgOptionsFilter::OnSetdispinfoVenues(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	//TV_DISPINFO* pTVDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	m_VenueFilter.clear();
	FillFilter(TVI_ROOT, _T(""));
	*pResult = 0;
}

void CDlgOptionsFilter::OnUpdateFilters()
{
	UpdateData(TRUE);
	UpdateControls();
}
