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
 * @brief implementation of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgTitle.h"

#include "ARBConfig.h"
#include "ARBDogTitle.h"

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
	DDX_Control(pDX, IDC_DESC, m_ctrlDesc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTitle, CDialog)
	//{{AFX_MSG_MAP(CDlgTitle)
	ON_CBN_SELCHANGE(IDC_VENUES, OnSelchangeVenues)
	ON_CBN_SELCHANGE(IDC_TITLES, OnSelchangeTitles)
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
			for (ARBConfigTitleList::const_iterator iterTitle = pDiv->GetTitles().begin();
				iterTitle != pDiv->GetTitles().end();
				++iterTitle)
			{
				ARBConfigTitle* pTitle = (*iterTitle);
				// Suppress any titles we already have.
				if (0 == m_Titles.NumTitlesInUse(pVenue->GetName(), pTitle->GetName())
				|| (m_pTitle && m_pTitle->GetName() == pTitle->GetName()))
				{
					int idx = m_ctrlTitles.AddString(pTitle->GetName().c_str());
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

	CString venue;
	m_ctrlVenues.GetLBText(index, venue);
	index = m_ctrlTitles.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlTitles);
		return;
	}

	CString name;
	m_ctrlTitles.GetLBText(index, name);

	ARBDogTitle* title = new ARBDogTitle;
	ARBDate date(time.GetYear(), time.GetMonth(), time.GetDay());
	title->SetDate(date);
	title->SetVenue((LPCSTR)venue);
	title->SetName((LPCSTR)name);
	title->SetReceived(m_ctrlReceived.GetCheck()==1);
	if (m_pTitle)
		*m_pTitle = *title;
	else
		m_Titles.AddTitle(title);
	title->Release();
	CDialog::OnOK();
}
