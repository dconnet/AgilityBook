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

		static LPCTSTR sc_Allow(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789"));
		if (m_FilterName != m_FilterName.SpanIncluding(sc_Allow))
		{
			pDX->PrepareCtrl(IDC_OPT_FILTER_NAMES);
			AfxMessageBox(_T("A filter name may contain only letters, numbers and imbedded spaces."), MB_ICONWARNING);
			pDX->Fail();
		}

		if (m_FilterName != tmp)
			UpdateData(FALSE);
		FillControls(true);
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

void CDlgOptionsFilter::FillControls(bool bSave)
{
	if (bSave)
	{
		m_CurFilter.bAllDates = false;
		if (0 == m_ViewDates
		|| (0 != m_ViewDates && !m_bDateStart && !m_bDateEnd))
		{
			m_CurFilter.bAllDates = true;
		}
		m_CurFilter.dateStartDate = ARBDate(m_timeStart.GetTime());
		m_CurFilter.bStartDate = m_bDateStart ? true : false;
		m_CurFilter.dateEndDate = ARBDate(m_timeEnd.GetTime());
		m_CurFilter.bEndDate = m_bDateEnd ? true : false;
		// Names are kept in sync via setdispinfo
		m_CurFilter.bViewAllNames = false;
		if (0 == m_ViewNames
		|| (0 != m_ViewNames && m_CurFilter.nameFilter.size()))
		{
			m_CurFilter.bViewAllNames = true;
		}
		m_CurFilter.calView.clear();
		if (m_bNotEntered)
			m_CurFilter.calView.AddNotEntered();
		if (m_bPlanning)
			m_CurFilter.calView.AddPlanning();
		if (m_bEntered)
			m_CurFilter.calView.AddEntered();
		// Venues are kept in sync via setdispinfo
		m_CurFilter.bViewAllVenues = false;
		if (0 == m_ViewVenues
		|| (0 != m_ViewVenues && 0 == m_CurFilter.venueFilter.size()))
		{
			m_CurFilter.bViewAllVenues = true;
		}
		switch (m_ViewQs)
		{
		default:
		case 0:
			m_CurFilter.eRuns = CFilterOptions::eViewRunsAll;
			break;
		case 1:
			m_CurFilter.eRuns = CFilterOptions::eViewRunsQs;
			break;
		case 2:
			m_CurFilter.eRuns = CFilterOptions::eViewRunsNQs;
			break;
		}
	}
	else
	{
		std::set<ARBString> names;
		m_pDoc->GetAllTrainingLogNames(names);

		m_ViewDates = m_CurFilter.bAllDates ? 0 : 1;
		if (m_CurFilter.dateStartDate.IsValid())
			m_timeStart = m_CurFilter.dateStartDate.GetDate();
		else if (!m_CurFilter.bAllDates)
			m_timeStart = CTime::GetCurrentTime();
		m_bDateStart = m_CurFilter.bStartDate ? TRUE : FALSE;
		if (m_CurFilter.dateEndDate.IsValid())
			m_timeEnd = m_CurFilter.dateEndDate.GetDate();
		else if (!m_CurFilter.bAllDates)
			m_timeEnd = CTime::GetCurrentTime();
		m_bDateEnd = m_CurFilter.bEndDate ? TRUE : FALSE;

		m_ViewNames = m_CurFilter.bViewAllNames ? 0 : 1;
		m_ctrlNames.DeleteAllItems();
		std::set<ARBString>::iterator iter;
		for (iter = m_CurFilter.nameFilter.begin(); iter != m_CurFilter.nameFilter.end(); )
		{
			if (names.end() == names.find((*iter)))
				iter = m_CurFilter.nameFilter.erase(iter);
			else
				++iter;
		}
		if (0 == names.size())
		{
			m_ViewNames = 0;
			GetDlgItem(IDC_OPT_FILTER_LOG_NAME_SELECTED)->EnableWindow(FALSE);
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
				if (m_CurFilter.nameFilter.end() != m_CurFilter.nameFilter.find((*iter)))
					m_ctrlNames.SetChecked(hItem, true);
			}
		}

		m_bNotEntered = m_CurFilter.calView.ViewNotEntered();
		m_bPlanning = m_CurFilter.calView.ViewPlanning();
		m_bEntered = m_CurFilter.calView.ViewEntered();

		m_ViewVenues = m_CurFilter.bViewAllVenues ? 0 : 1;
		m_ctrlVenue.DeleteAllItems();
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
			if (Find(pVenue->GetName(), _T(""), _T("")))
				m_ctrlVenue.SetChecked(hVenue, true);
			//m_ctrlVenue.SetItemData(hVenue,
			//	reinterpret_cast<LPARAM>(
			//		new CListPtrData<ARBConfigVenuePtr>(pVenue)));
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
				if (Find(pVenue->GetName(), pDiv->GetName(), _T("")))
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

		switch (m_CurFilter.eRuns)
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
		for (std::vector<CFilterOptionData>::iterator iter = m_Filters.begin();
			iter != m_Filters.end();
			++iter)
		{
			CFilterOptionData const& data = *iter;
			int idx = m_ctrlFilters.AddString(data.filterName.c_str());
			if (data.filterName == m_CurFilterName)
			{
				m_FilterName = m_CurFilterName.c_str();
				m_ctrlFilters.SetCurSel(idx);
			}
		}
		UpdateData(FALSE);
		UpdateControls();
	}
}

bool CDlgOptionsFilter::Find(
		ARBString const& venue,
		ARBString const& div,
		ARBString const& level) const
{
	for (std::vector<CVenueFilter>::const_iterator iter = m_CurFilter.venueFilter.begin();
		iter != m_CurFilter.venueFilter.end();
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
			m_CurFilter.venueFilter.push_back(filter);
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
	FillControls(false);
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
	m_CurFilter.nameFilter.clear();
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
				m_CurFilter.nameFilter.insert((LPCTSTR)str);
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
	m_CurFilter.venueFilter.clear();
	FillFilter(TVI_ROOT, _T(""));
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
		str.TrimRight();
		str.TrimLeft();
		m_CurFilterName = (LPCSTR)str;
		for (std::vector<CFilterOptionData>::iterator iFilter = m_Filters.begin();
			iFilter != m_Filters.end();
			++iFilter)
		{
			if ((*iFilter).filterName == m_CurFilterName)
			{
				m_CurFilter = (*iFilter);
				FillControls(false);
				break;
			}
		}
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
		m_CurFilterName = (LPCTSTR)m_FilterName;
		m_CurFilter.filterName = m_CurFilterName;
		std::set<ARBString>::iterator iter = m_DeleteFilters.find(m_CurFilterName);
		if (iter != m_DeleteFilters.end())
			m_DeleteFilters.erase(iter);
		std::vector<CFilterOptionData>::iterator iFilter;
		for (iFilter = m_Filters.begin(); iFilter != m_Filters.end(); ++iFilter)
		{
			if ((*iFilter).filterName == m_CurFilterName)
			{
				(*iFilter) = m_CurFilter;
				break;
			}
		}
		if (iFilter == m_Filters.end())
			m_Filters.push_back(m_CurFilter);
	}
	else
		MessageBeep(0);
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
			m_CurFilterName = (LPCTSTR)m_FilterName;
			std::vector<CFilterOptionData>::iterator iFilter;
			for (iFilter = m_Filters.begin(); iFilter != m_Filters.end(); ++iFilter)
			{
				if ((*iFilter).filterName == m_CurFilterName)
				{
					m_Filters.erase(iFilter);
					break;
				}
			}
			m_DeleteFilters.insert((LPCTSTR)m_FilterName);
			m_ctrlFilters.DeleteString(idx);
			m_CurFilterName.clear();
			m_FilterName.Empty();
		}
	}
}
