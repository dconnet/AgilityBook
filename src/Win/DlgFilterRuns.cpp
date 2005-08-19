/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgFilterRuns class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Separated options and filters.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgFilterRuns.h"

#include "ARBConfig.h"
#include "ARBConfigVenue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterRuns property page

IMPLEMENT_DYNAMIC(CDlgFilterRuns, CDlgBasePropertyPage)

CDlgFilterRuns::CDlgFilterRuns(ARBConfig const& config)
	: CDlgBasePropertyPage(CDlgFilterRuns::IDD)
	, m_Config(config)
{
	//{{AFX_DATA_INIT(CDlgFilterRuns)
	m_ViewVenues = -1;
	m_ViewQs = -1;
	//}}AFX_DATA_INIT
}

CDlgFilterRuns::~CDlgFilterRuns()
{
}

void CDlgFilterRuns::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFilterRuns)
	DDX_Radio(pDX, IDC_FILTER_RUN_VENUES_ALL, m_ViewVenues);
	DDX_Control(pDX, IDC_FILTER_RUN_VENUES, m_ctrlVenue);
	DDX_Radio(pDX, IDC_FILTER_RUN_RUNS_ALL, m_ViewQs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgFilterRuns, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgFilterRuns)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_FILTER_RUN_VENUES_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_FILTER_RUN_VENUES_SELECTED, OnViewUpdate)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_FILTER_RUN_VENUES, OnSetdispinfoVenues)
	ON_BN_CLICKED(IDC_FILTER_RUN_RUNS_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_FILTER_RUN_RUNS_Q, OnViewUpdate)
	ON_BN_CLICKED(IDC_FILTER_RUN_RUNS_NON_Q, OnViewUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

bool CDlgFilterRuns::Find(
		std::string const& venue,
		std::string const& div,
		std::string const& level) const
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

void CDlgFilterRuns::FillFilter(
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

void CDlgFilterRuns::UpdateControls()
{
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
// CDlgFilterRuns message handlers

BOOL CDlgFilterRuns::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();

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

BOOL CDlgFilterRuns::OnHelpInfo(HELPINFO* pHelpInfo)
{
	ShowContextHelp(pHelpInfo);
	return TRUE;
}

void CDlgFilterRuns::OnViewUpdate() 
{
	UpdateData(TRUE);
	UpdateControls();
}

void CDlgFilterRuns::OnSetdispinfoVenues(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	//TV_DISPINFO* pTVDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	// We could probably do this by just watching what changed. but this
	// is way easier - no state to keep track of!
	m_VenueFilter.clear();
	FillFilter(TVI_ROOT, "");
	*pResult = 0;
}
