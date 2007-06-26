/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2007-06-25 DRC Allow "1" as the start for recurring titles.
 * @li 2006-05-08 DRC Changing the date unselected a title.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-26 DRC The wrong name was saved into the ARBDogTitle object.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTitle.h"

#include "ARBConfig.h"
#include "ARBDogTitle.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTitle dialog

// If pTitle is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgTitle::CDlgTitle(
		ARBConfig const& config,
		ARBDogTitleList& titles,
		ARBDogTitlePtr pTitle,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgTitle::IDD, pParent)
	, m_ctrlVenues(true)
	, m_ctrlTitles(true)
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
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTitle)
	DDX_Control(pDX, IDC_TITLE_EARNED, m_ctrlEarned);
	DDX_Control(pDX, IDC_TITLE_DATE, m_ctrlDate);
	DDX_Control(pDX, IDC_TITLE_HIDDEN, m_ctrlHidden);
	DDX_Control(pDX, IDC_TITLE_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_TITLE_TITLES, m_ctrlTitles);
	DDX_Control(pDX, IDC_TITLE_RECEIVED, m_ctrlReceived);
	DDX_Control(pDX, IDC_TITLE_DESC, m_ctrlDesc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTitle, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgTitle)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TITLE_DATE, OnDtnDatetimechangeTitleDate)
	ON_BN_CLICKED(IDC_TITLE_EARNED, OnBnClickedEarned)
	ON_CBN_SELCHANGE(IDC_TITLE_VENUES, OnSelchangeVenues)
	ON_CBN_SELCHANGE(IDC_TITLE_TITLES, OnSelchangeTitles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

ARBConfigVenuePtr CDlgTitle::GetVenueData(int index) const
{
	CListData* pData = m_ctrlVenues.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigVenuePtr>*>(pData)->GetData();
}

ARBConfigTitlePtr CDlgTitle::GetTitleData(int index) const
{
	CListData* pData = m_ctrlTitles.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigTitlePtr>*>(pData)->GetData();
}

ARBDate CDlgTitle::GetDate()
{
	ARBDate date;
	if (m_ctrlEarned.GetCheck() == 1)
	{
		CTime time;
		m_ctrlDate.GetTime(time);
		date = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
	return date;
}

void CDlgTitle::FillTitles()
{
	ARBConfigTitlePtr pSelTitle;
	int index = m_ctrlTitles.GetCurSel();
	if (CB_ERR != index)
		pSelTitle = GetTitleData(index);
	m_ctrlTitles.ResetContent();
	index = m_ctrlVenues.GetCurSel();
	if (CB_ERR != index)
	{
		ARBDate date = GetDate();
		ARBConfigVenuePtr pVenue = GetVenueData(index);
		ASSERT(pVenue);
		for (ARBConfigTitleList::const_iterator iterTitle = pVenue->GetTitles().begin();
			iterTitle != pVenue->GetTitles().end();
			++iterTitle)
		{
			ARBConfigTitlePtr pTitle = (*iterTitle);
			// Suppress any titles we already have.
			if (0 < pTitle->GetMultiple()
			|| 0 == m_Titles.NumTitlesInUse(pVenue->GetName(), pTitle->GetName())
			|| (m_pTitle && m_pTitle->GetRawName() == pTitle->GetName()))
			{
				if (pTitle->IsValidOn(date))
				{
					int idx = m_ctrlTitles.AddString(pTitle->GetCompleteName().c_str());
					m_ctrlTitles.SetData(idx,
						new CListPtrData<ARBConfigTitlePtr>(pTitle));
					if ((m_bInit && m_pTitle && m_pTitle->GetRawName() == pTitle->GetName())
					|| (!m_bInit && pSelTitle && pSelTitle->GetName() == pTitle->GetName()))
					{
						m_ctrlTitles.SetCurSel(idx);
						FillTitleInfo();
					}
				}
			}
		}
	}
}

void CDlgTitle::FillTitleInfo()
{
	int index = m_ctrlTitles.GetCurSel();
	CString str;
	if (CB_ERR != index)
	{
		ARBConfigTitlePtr pTitle = GetTitleData(index);
		str = pTitle->GetDescription().c_str();
	}
	str.Replace(_T("\n"), _T("\r\n"));
	m_ctrlDesc.SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgTitle message handlers

BOOL CDlgTitle::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_bInit = true;
	for (ARBConfigVenueList::const_iterator iter = m_Config.GetVenues().begin();
		iter != m_Config.GetVenues().end();
		++iter)
	{
		ARBConfigVenuePtr pVenue = (*iter);
		int index = m_ctrlVenues.AddString(pVenue->GetName().c_str());
		m_ctrlVenues.SetData(index,
			new CListPtrData<ARBConfigVenuePtr>(pVenue));
		if (m_pTitle && m_pTitle->GetVenue() == pVenue->GetName())
		{
			m_ctrlVenues.SetCurSel(index);
			FillTitles();
		}
	}
	int nEarned = 1;
	int nHidden = 0;
	int nReceived = 0;
	if (m_pTitle)
	{
		if (m_pTitle->GetDate().IsValid())
		{
			CTime t = m_pTitle->GetDate().GetDate();
			m_ctrlDate.SetTime(&t);
		}
		else
		{
			nEarned = 0;
			m_ctrlDate.EnableWindow(FALSE);
			m_ctrlReceived.EnableWindow(FALSE);
		}
		if (m_pTitle->IsHidden())
			nHidden = 1;
		if (m_pTitle->GetReceived())
			nReceived = 1;
	}
	else
		m_ctrlDesc.SetWindowText(_T(""));
	m_ctrlEarned.SetCheck(nEarned);
	m_ctrlHidden.SetCheck(nHidden);
	m_ctrlReceived.SetCheck(nReceived);
	m_bInit = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTitle::OnDtnDatetimechangeTitleDate(NMHDR* pNMHDR, LRESULT* pResult)
{
	//LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	FillTitles();
	*pResult = 0;
}

void CDlgTitle::OnBnClickedEarned()
{
	if (m_ctrlEarned.GetCheck() == 1)
	{
		m_ctrlDate.EnableWindow(TRUE);
		m_ctrlReceived.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlDate.EnableWindow(FALSE);
		m_ctrlReceived.EnableWindow(FALSE);
	}
	FillTitles();
}

void CDlgTitle::OnSelchangeVenues()
{
	FillTitles();
}

void CDlgTitle::OnSelchangeTitles() 
{
	FillTitleInfo();
}

void CDlgTitle::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlVenues);
		return;
	}
	ARBConfigVenuePtr pVenue = GetVenueData(index);
	ASSERT(pVenue);

	index = m_ctrlTitles.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlTitles);
		return;
	}
	ARBConfigTitlePtr pTitle = GetTitleData(index);
	ASSERT(pTitle);

	CString name;
	m_ctrlTitles.GetLBText(index, name);

	ARBDate date = GetDate();
	bool bHidden = (m_ctrlHidden.GetCheck() == 1);
	bool bReceived = (m_ctrlReceived.GetCheck() == 1);
	if (m_ctrlEarned.GetCheck() != 1)
	{
		bHidden = true;
		bReceived = false;
	}

	short instance = 0;
	if (0 < pTitle->GetMultiple())
	{
		if (m_pTitle && m_pTitle->GetRawName() == pTitle->GetName())
			instance = m_pTitle->GetInstance();
		else
		{
			instance = pTitle->GetMultiple();
			short next = m_Titles.FindMaxInstance(pVenue->GetName(), pTitle->GetName()) + 1;
			if (next > instance || 1 == next)
				instance = next;
		}
	}

	ARBDogTitlePtr title(ARBDogTitle::New());
	title->SetDate(date);
	title->SetHidden(bHidden);
	title->SetVenue(pVenue->GetName());
	title->SetName(pTitle->GetName(), instance, pTitle->GetMultiple() == 1);
	title->SetReceived(bReceived);
	if (m_pTitle)
	{
		bool bSort = false;
		if (m_pTitle->GetDate() != title->GetDate())
			bSort = true;
		*m_pTitle = *title;
		if (bSort)
			m_Titles.sort();
	}
	else
		m_Titles.AddTitle(title);
	CDlgBaseDialog::OnOK();
}
