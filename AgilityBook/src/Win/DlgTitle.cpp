/*
 * Copyright © 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-11 DRC Added option to suppress "skipped" titles.
 * @li 2004-01-26 DRC The wrong name was saved into the ARBDogTitle object.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTitle.h"

#include "ARBConfig.h"
#include "ARBDogTitle.h"
#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTitle dialog

// If pTitle is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgTitle::CDlgTitle(const ARBConfig& config, ARBDogTitleList& titles, ARBDogTitle* pTitle, CWnd* pParent)
	: CDialog(CDlgTitle::IDD, pParent)
	, m_Config(config)
	, m_Titles(titles)
	, m_pTitle(pTitle)
	, m_bInit(false)
{
	//{{AFX_DATA_INIT(CDlgTitle)
	//}}AFX_DATA_INIT
}

void CDlgTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTitle)
	DDX_Control(pDX, IDC_DATE, m_ctrlDate);
	DDX_Control(pDX, IDC_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_RECEIVED, m_ctrlReceived);
	DDX_Control(pDX, IDC_HIDE_OLD, m_ctrlHide);
	DDX_Control(pDX, IDC_DESC, m_ctrlDesc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTitle, CDialog)
	//{{AFX_MSG_MAP(CDlgTitle)
	ON_CBN_SELCHANGE(IDC_VENUES, OnSelchangeVenues)
	ON_CBN_SELCHANGE(IDC_TITLES, OnSelchangeTitles)
	ON_BN_CLICKED(IDC_HIDE_OLD, OnBnClickedHideOld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTitle message handlers

BOOL CDlgTitle::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bInit = true;
	for (ARBConfigVenueList::const_iterator iter = m_Config.GetVenues().begin();
		iter != m_Config.GetVenues().end();
		++iter)
	{
		ARBConfigVenue* pVenue = (*iter);
		int index = m_ctrlVenues.AddString(pVenue->GetName().c_str());
		m_ctrlVenues.SetItemDataPtr(index, pVenue);
		if (m_pTitle && m_pTitle->GetVenue() == pVenue->GetName())
		{
			m_ctrlVenues.SetCurSel(index);
			OnSelchangeVenues();
		}
	}
	if (m_pTitle && m_pTitle->GetReceived())
		m_ctrlReceived.SetCheck(1);
	else
		m_ctrlReceived.SetCheck(0);
	m_ctrlHide.SetCheck(CAgilityBookOptions::GetHideRequiredTitles() ? 1 : 0);
	if (m_pTitle)
	{
		const ARBDate& date = m_pTitle->GetDate();
		CTime t(date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0, 0);
		m_ctrlDate.SetTime(&t);
	}
	else
		m_ctrlDesc.SetWindowText("");
	m_bInit = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTitle::OnSelchangeVenues()
{
	m_ctrlTitles.ResetContent();
	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR != index)
	{
		const ARBConfigVenue* pVenue = reinterpret_cast<const ARBConfigVenue*>(m_ctrlVenues.GetItemDataPtr(index));
		ASSERT(pVenue);
		for (ARBConfigDivisionList::const_iterator iter = pVenue->GetDivisions().begin();
			iter != pVenue->GetDivisions().end();
			++iter)
		{
			ARBConfigDivision* pDiv = (*iter);
			// Names of titles that are required for titles we have but
			// haven't earned yet (For instance, I started at level3 in CPE)
			std::set<std::string> inUseRequired;
			if (CAgilityBookOptions::GetHideRequiredTitles())
			{
				// This is the set of names to be processed
				std::set<std::string> todo;
				// First load the list with existing titles
				for (ARBDogTitleList::const_iterator iterDogTitle = m_Titles.begin();
					iterDogTitle != m_Titles.end();
					++iterDogTitle)
				{
					const std::string& name = (*iterDogTitle)->GetName();
					const ARBConfigTitle* pTitle = pDiv->GetTitles().FindTitle(name);
					if (pTitle)
						todo.insert(name);
				}
				// For each name, add all of its requirements to the inUse and
				// todo list. Pop off the head and continue until the list is done.
				while (0 < todo.size())
				{
					const ARBConfigTitle* pTitle = pDiv->GetTitles().FindTitle(*(todo.begin()));
					todo.erase(todo.begin());
					if (pTitle)
					{
						ARBConfigTitleRequiresList::const_iterator iterRequires;
						for (iterRequires = pTitle->GetRequires().begin();
							iterRequires != pTitle->GetRequires().end();
							++iterRequires)
						{
							inUseRequired.insert(*iterRequires);
							todo.insert(*iterRequires);
						}
					}
				}
				// Finally, go thru the inUse list and remove any titles we
				// have already earned.
				for (std::set<std::string>::iterator iterSet = inUseRequired.begin();
					iterSet != inUseRequired.end();)
				{
					if (m_Titles.NumTitlesInUse(pVenue->GetName(), *iterSet))
						iterSet = inUseRequired.erase(iterSet);
					else
						++iterSet;
				}
			}

			for (ARBConfigTitleList::const_iterator iterTitle = pDiv->GetTitles().begin();
				iterTitle != pDiv->GetTitles().end();
				++iterTitle)
			{
				ARBConfigTitle* pTitle = (*iterTitle);
				// Suppress any titles we already have.
				if (0 == m_Titles.NumTitlesInUse(pVenue->GetName(), pTitle->GetName())
				|| (m_pTitle && m_pTitle->GetName() == pTitle->GetName()))
				{
					bool bShow = true;
					if (CAgilityBookOptions::GetHideRequiredTitles())
					{
						for (std::set<std::string>::iterator iterSet = inUseRequired.begin();
							iterSet != inUseRequired.end();
							++iterSet)
						{
							if (pTitle->GetName() == *iterSet)
							{
								bShow = false;
								break;
							}
						}
					}
					if (bShow)
					{
						int idx = m_ctrlTitles.AddString(pTitle->GetCompleteName().c_str());
						m_ctrlTitles.SetItemDataPtr(idx, pTitle);
						if (m_bInit && m_pTitle && m_pTitle->GetName() == pTitle->GetName())
						{
							m_ctrlTitles.SetCurSel(idx);
							OnSelchangeTitles();
						}
					}
				}
			}
		}
	}
}

void CDlgTitle::OnSelchangeTitles() 
{
	int index = m_ctrlTitles.GetCurSel();
	CString str;
	if (CB_ERR != index)
	{
		const ARBConfigTitle* pTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlTitles.GetItemDataPtr(index));
		str = pTitle->GetDescription().c_str();
	}
	str.Replace("\n", "\r\n");
	m_ctrlDesc.SetWindowText(str);
}

void CDlgTitle::OnBnClickedHideOld()
{
	bool bHide = m_ctrlHide.GetCheck() == 1 ? true : false;
	CAgilityBookOptions::SetHideRequiredTitles(bHide);
	m_bInit = true; // Make sure title is reset
	OnSelchangeVenues();
	m_bInit = false;
}

void CDlgTitle::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CTime time;
	m_ctrlDate.GetTime(time);

	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlVenues);
		return;
	}
	const ARBConfigVenue* pVenue = reinterpret_cast<const ARBConfigVenue*>(m_ctrlVenues.GetItemDataPtr(index));
	ASSERT(NULL != pVenue);

	index = m_ctrlTitles.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlTitles);
		return;
	}
	const ARBConfigTitle* pTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlTitles.GetItemDataPtr(index));
	ASSERT(NULL != pTitle);

	CString name;
	m_ctrlTitles.GetLBText(index, name);

	ARBDogTitle* title = new ARBDogTitle();
	ARBDate date(time.GetYear(), time.GetMonth(), time.GetDay());
	title->SetDate(date);
	title->SetVenue(pVenue->GetName());
	title->SetName(pTitle->GetName());
	title->SetReceived(m_ctrlReceived.GetCheck()==1);
	if (m_pTitle)
		*m_pTitle = *title;
	else
		m_Titles.AddTitle(title);
	title->Release();
	CDialog::OnOK();
}
