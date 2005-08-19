/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgFilterDate class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgFilterDate.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterDate property page

IMPLEMENT_DYNAMIC(CDlgFilterDate, CDlgBasePropertyPage)

CDlgFilterDate::CDlgFilterDate()
	: CDlgBasePropertyPage(CDlgFilterDate::IDD)
{
	//{{AFX_DATA_INIT(CDlgFilterDate)
	m_ViewDates = -1;
	m_bDateStart = FALSE;
	m_timeStart = 0;
	m_bDateEnd = FALSE;
	m_timeEnd = 0;
	//}}AFX_DATA_INIT
}

CDlgFilterDate::~CDlgFilterDate()
{
}

void CDlgFilterDate::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFilterDate)
	DDX_Radio(pDX, IDC_FILTER_DATE_ALL, m_ViewDates);
	DDX_Control(pDX, IDC_FILTER_DATE_START_CHECK, m_ctrlDateStartCheck);
	DDX_Check(pDX, IDC_FILTER_DATE_START_CHECK, m_bDateStart);
	DDX_Control(pDX, IDC_FILTER_DATE_START, m_ctrlDateStart);
	DDX_DateTimeCtrl(pDX, IDC_FILTER_DATE_START, m_timeStart);
	DDX_Control(pDX, IDC_FILTER_DATE_END_CHECK, m_ctrlDateEndCheck);
	DDX_DateTimeCtrl(pDX, IDC_FILTER_DATE_END, m_timeEnd);
	DDX_Check(pDX, IDC_FILTER_DATE_END_CHECK, m_bDateEnd);
	DDX_Control(pDX, IDC_FILTER_DATE_END, m_ctrlDateEnd);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgFilterDate, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgFilterDate)
	ON_BN_CLICKED(IDC_FILTER_DATE_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_FILTER_DATE_RANGE, OnViewUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgFilterDate::UpdateControls()
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
}

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterDate message handlers

BOOL CDlgFilterDate::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();

	UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFilterDate::OnViewUpdate() 
{
	UpdateData(TRUE);
	UpdateControls();
}
