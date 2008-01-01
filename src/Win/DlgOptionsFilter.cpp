/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-09-01 DRC Fix setting of 'After' (start) date filter.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
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
	, m_ctrlFilters(false)
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
	DDX_Control(pDX, IDC_OPT_FILTER_NAMES, m_ctrlFilters);
	DDX_CBString(pDX, IDC_OPT_FILTER_NAMES, m_FilterName);
	DDV_MaxChars(pDX, m_FilterName, 20);
	DDX_Control(pDX, IDC_OPT_FILTER_NAMES_SAVE, m_ctrlSaveName);
	DDX_Control(pDX, IDC_OPT_FILTER_NAMES_DELETE, m_ctrlDelName);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		CString tmp(m_FilterName);
		m_FilterName.TrimRight();
		m_FilterName.TrimLeft();
		if (m_FilterName != tmp)
			UpdateData(FALSE);

		if (0 == m_ViewDates
		|| (0 != m_ViewDates && !m_bDateStart && !m_bDateEnd))
		{
			m_FilterOptions.SetViewAllDates(true);
		}
		else
			m_FilterOptions.SetViewAllDates(false);
		m_FilterOptions.SetStartFilterDate(ARBDate(m_timeStart.GetTime()));
		m_FilterOptions.SetStartFilterDateSet(m_bDateStart ? true : false);
		m_FilterOptions.SetEndFilterDate(ARBDate(m_timeEnd.GetTime()));
		m_FilterOptions.SetEndFilterDateSet(m_bDateEnd ? true : false);
		// Names are kept in sync via setdispinfo
		m_FilterOptions.SetTrainingViewAllNames(0 == m_ViewNames ? true : false);
		CCalendarViewFilter calView = m_FilterOptions.FilterCalendarView();
		calView.Clear();
		if (m_bNotEntered)
			calView.AddNotEntered();
		if (m_bPlanning)
			calView.AddPlanning();
		if (m_bEntered)
			calView.AddEntered();
		m_FilterOptions.SetFilterCalendarView(calView);
		// Venues are kept in sync via setdispinfo
		m_FilterOptions.SetViewAllVenues(0 == m_ViewVenues ? true : false);
		switch (m_ViewQs)
		{
		default:
		case 0:
			m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsAll);
			break;
		case 1:
			m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsQs);
			break;
		case 2:
			m_FilterOptions.SetViewRuns(CFilterOptions::eViewRunsNQs);
			break;
		}
	}
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
	ON_CBN_SELCHANGE(IDC_OPT_FILTER_NAMES, OnSelchangeFilterNames)
	ON_BN_CLICKED(IDC_OPT_FILTER_NAMES_SAVE, OnBnClickedOptFilterNamesSave)
	ON_BN_CLICKED(IDC_OPT_FILTER_NAMES_DELETE, OnBnClickedOptFilterNamesDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgOptionsFilter::FillControls()
{
	std::set<tstring> allLogNames;
	m_pDoc->GetAllTrainingLogNames(allLogNames);

	m_ViewDates = m_FilterOptions.GetViewAllDates() ? 0 : 1;
	if (m_FilterOptions.GetStartFilterDate().IsValid())
		m_timeStart = m_FilterOptions.GetStartFilterDate().GetDate();
	else if (!m_FilterOptions.GetViewAllDates())
		m_timeStart = CTime::GetCurrentTime();
	m_bDateStart = m_FilterOptions.GetStartFilterDateSet() ? TRUE : FALSE;
	if (m_FilterOptions.GetEndFilterDate().IsValid())
		m_timeEnd = m_FilterOptions.GetEndFilterDate().GetDate();
	else if (!m_FilterOptions.GetViewAllDates())
		m_timeEnd = CTime::GetCurrentTime();
	m_bDateEnd = m_FilterOptions.GetEndFilterDateSet() ? TRUE : FALSE;

	m_ViewNames = m_FilterOptions.GetTrainingViewAllNames() ? 0 : 1;
	m_ctrlNames.DeleteAllItems();
	std::set<tstring> names;
	m_FilterOptions.GetTrainingFilterNames(names);
	bool bFix = false;
	for (std::set<tstring>::iterator iter = names.begin();
		iter != names.end(); )
	{
		if (allLogNames.end() == allLogNames.find((*iter)))
		{
			bFix = true;
			iter = names.erase(iter);
		}
		else
			++iter;
	}
	if (bFix)
		m_FilterOptions.SetTrainingFilterNames(names);
	if (0 == allLogNames.size())
	{
		m_ViewNames = 0;
		GetDlgItem(IDC_OPT_FILTER_LOG_NAME_SELECTED)->EnableWindow(FALSE);
	}
	else
	{
		for (std::set<tstring>::iterator iter = allLogNames.begin();
			iter != allLogNames.end();
			++iter)
		{
			HTREEITEM hItem = m_ctrlNames.InsertItem(
				(*iter).c_str(),
				TVI_ROOT,
				TVI_LAST);
			m_ctrlNames.ShowCheckbox(hItem, true);
			if (names.end() != names.find((*iter)))
				m_ctrlNames.SetChecked(hItem, true);
		}
	}

	m_bNotEntered = m_FilterOptions.FilterCalendarView().ViewNotEntered();
	m_bPlanning = m_FilterOptions.FilterCalendarView().ViewPlanning();
	m_bEntered = m_FilterOptions.FilterCalendarView().ViewEntered();

	m_ViewVenues = m_FilterOptions.GetViewAllVenues() ? 0 : 1;
	m_ctrlVenue.DeleteAllItems();
	if (0 < m_pDoc->GetConfig().GetVenues().size())
	{
		for (ARBConfigVenueList::const_iterator iVenue = m_pDoc->GetConfig().GetVenues().begin();
			iVenue != m_pDoc->GetConfig().GetVenues().end();
			++iVenue)
		{
			ARBConfigVenuePtr pVenue = (*iVenue);
			HTREEITEM hVenue = m_ctrlVenue.InsertItem(
				pVenue->GetName().c_str(),
				TVI_ROOT,
				TVI_LAST);
			m_ctrlVenue.ShowCheckbox(hVenue, true);
			if (m_FilterOptions.FilterExists(pVenue->GetName(), _T(""), _T("")))
				m_ctrlVenue.SetChecked(hVenue, true);
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++iterDiv)
			{
				ARBConfigDivisionPtr pDiv = *iterDiv;
				HTREEITEM hDiv = m_ctrlVenue.InsertItem(
					pDiv->GetName().c_str(),
					hVenue,
					TVI_LAST);
				m_ctrlVenue.ShowCheckbox(hDiv, true);
				if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), _T("")))
				{
					m_ctrlVenue.SetChecked(hDiv, true);
					m_ctrlVenue.EnsureVisible(hDiv);
				}
				for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
					iterLevel != pDiv->GetLevels().end();
					++iterLevel)
				{
					ARBConfigLevelPtr pLevel = *iterLevel;
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
							ARBConfigSubLevelPtr pSubLevel = *iterSubLevel;
							HTREEITEM hSubLevel = m_ctrlVenue.InsertItem(
								pSubLevel->GetName().c_str(),
								hLevel,
								TVI_LAST);
							m_ctrlVenue.ShowCheckbox(hSubLevel, true);
							if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), pSubLevel->GetName()))
							{
								m_ctrlVenue.SetChecked(hSubLevel, true);
								m_ctrlVenue.EnsureVisible(hSubLevel);
							}
						}
					}
					else
					{
						m_ctrlVenue.ShowCheckbox(hLevel, true);
						if (m_FilterOptions.FilterExists(pVenue->GetName(), pDiv->GetName(), pLevel->GetName()))
						{
							m_ctrlVenue.SetChecked(hLevel, true);
							m_ctrlVenue.EnsureVisible(hLevel);
						}
					}
				}
			}
		}
	}
	else
	{
		m_ViewVenues = 0;
		GetDlgItem(IDC_OPT_FILTER_RUN_VENUES_SELECTED)->EnableWindow(FALSE);
	}

	switch (m_FilterOptions.GetViewRuns())
	{
	default:
	case CFilterOptions::eViewRunsAll:
		m_ViewQs = 0;
		break;
	case CFilterOptions::eViewRunsQs:
		m_ViewQs = 1;
		break;
	case CFilterOptions::eViewRunsNQs:
		m_ViewQs = 2;
		break;
	}

	m_ctrlFilters.ResetContent();
	std::vector<tstring> filterNames;
	m_FilterOptions.GetAllFilterNames(filterNames);
	for (std::vector<tstring>::iterator iterName = filterNames.begin();
		iterName != filterNames.end();
		++iterName)
	{
		int idx = m_ctrlFilters.AddString((*iterName).c_str());
		if ((*iterName) == m_FilterOptions.GetCurrentFilter())
		{
			m_FilterName = m_FilterOptions.GetCurrentFilter().c_str();
			m_ctrlFilters.SetCurSel(idx);
		}
	}
	UpdateData(FALSE);
	UpdateControls();
}


void CDlgOptionsFilter::FillFilter(
		HTREEITEM hItem,
		CString path,
		std::vector<CVenueFilter>& outVenues)
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
			outVenues.push_back(filter);
		}
	}
	else
	{
		while (NULL != hChildItem)
		{
			FillFilter(hChildItem, path, outVenues);
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
		m_ctrlNames.EnableWindow(FALSE);
	else
		m_ctrlNames.EnableWindow(TRUE);
	if (0 == m_ViewVenues)
		m_ctrlVenue.EnableWindow(FALSE);
	else
		m_ctrlVenue.EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFilter message handlers

BOOL CDlgOptionsFilter::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	FillControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgOptionsFilter::OnHelpInfo(HELPINFO* pHelpInfo)
{
	ShowContextHelp(theApp.LanguageManager(), pHelpInfo);
	return TRUE;
}


void CDlgOptionsFilter::OnSetdispinfoNames(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	std::set<tstring> names;
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
				names.insert((LPCTSTR)str);
		}
		hItem = m_ctrlNames.GetNextItem(hItem, TVGN_NEXT);
	}
	m_FilterOptions.SetTrainingFilterNames(names);
	*pResult = 0;
}


void CDlgOptionsFilter::OnSetdispinfoVenues(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	//TV_DISPINFO* pTVDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	std::vector<CVenueFilter> venues;
	FillFilter(TVI_ROOT, _T(""), venues);
	m_FilterOptions.SetFilterVenue(venues);
	*pResult = 0;
}


void CDlgOptionsFilter::OnUpdateFilters()
{
	UpdateData(TRUE);
	UpdateControls();
}


void CDlgOptionsFilter::OnSelchangeFilterNames()
{
	int idx = m_ctrlFilters.GetCurSel();
	if (0 <= idx)
	{
		CString str;
		m_ctrlFilters.GetLBText(idx, str);
		m_FilterOptions.SetCurrentFilter((LPCTSTR)str);
		FillControls();
	}
	else
		MessageBeep(0);
}


void CDlgOptionsFilter::OnBnClickedOptFilterNamesSave()
{
	if (!UpdateData(TRUE))
		return;
	if (!m_FilterName.IsEmpty())
	{
		m_FilterOptions.AddFilter((LPCTSTR)m_FilterName);
		m_ctrlFilters.AddString(m_FilterName);
	}
	else
		AfxMessageBox(IDS_NAME_FILTER, MB_ICONEXCLAMATION);
}


void CDlgOptionsFilter::OnBnClickedOptFilterNamesDelete()
{
	if (!UpdateData(TRUE))
		return;
	if (!m_FilterName.IsEmpty())
	{
		int idx = m_ctrlFilters.FindStringExact(-1, m_FilterName);
		if (0 <= idx)
		{
			tstring name = (LPCTSTR)m_FilterName;
			m_FilterOptions.DeleteFilter(name);
			m_ctrlFilters.DeleteString(idx);
			m_FilterName.Empty();
			UpdateData(FALSE);
		}
	}
}
