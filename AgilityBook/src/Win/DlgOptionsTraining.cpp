/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsTraining class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsTraining.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsTraining property page

IMPLEMENT_DYNAMIC(CDlgOptionsTraining, CPropertyPage)

CDlgOptionsTraining::CDlgOptionsTraining(CAgilityBookDoc* pDoc)
	: CPropertyPage(CDlgOptionsTraining::IDD)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CDlgOptionsTraining)
	m_ViewNames = -1;
	m_ViewDates = -1;
	m_bDateStart = FALSE;
	m_timeStart = 0;
	m_bDateEnd = FALSE;
	m_timeEnd = 0;
	//}}AFX_DATA_INIT
}

CDlgOptionsTraining::~CDlgOptionsTraining()
{
}

void CDlgOptionsTraining::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsTraining)
	DDX_Radio(pDX, IDC_OPTIONS_VENUES_ALL, m_ViewNames);
	DDX_Control(pDX, IDC_OPTIONS_VENUES, m_ctrlNames);
	DDX_Radio(pDX, IDC_OPTIONS_DATES_ALL, m_ViewDates);
	DDX_Control(pDX, IDC_OPTIONS_DATE_START_CHECK, m_ctrlDateStartCheck);
	DDX_Check(pDX, IDC_OPTIONS_DATE_START_CHECK, m_bDateStart);
	DDX_Control(pDX, IDC_OPTIONS_DATE_START, m_ctrlDateStart);
	DDX_DateTimeCtrl(pDX, IDC_OPTIONS_DATE_START, m_timeStart);
	DDX_Control(pDX, IDC_OPTIONS_DATE_END_CHECK, m_ctrlDateEndCheck);
	DDX_DateTimeCtrl(pDX, IDC_OPTIONS_DATE_END, m_timeEnd);
	DDX_Check(pDX, IDC_OPTIONS_DATE_END_CHECK, m_bDateEnd);
	DDX_Control(pDX, IDC_OPTIONS_DATE_END, m_ctrlDateEnd);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsTraining, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsTraining)
	ON_BN_CLICKED(IDC_OPTIONS_DATES_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_DATES_RANGE, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_VENUES_ALL, OnViewUpdate)
	ON_BN_CLICKED(IDC_OPTIONS_VENUES_SELECTED, OnViewUpdate)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_OPTIONS_VENUES, OnSetdispinfoNames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgOptionsTraining::UpdateControls()
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
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsTraining message handlers

BOOL CDlgOptionsTraining::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	std::set<std::string> names;
	std::set<std::string>::iterator iter;
	m_pDoc->GetAllTrainingLogNames(names);
	for (iter = m_filterNames.begin(); iter != m_filterNames.end(); )
	{
		if (names.end() == names.find((*iter)))
			iter = m_filterNames.erase(iter);
		else
			++iter;
	}
	if (0 == names.size())
	{
		m_ViewNames = 0;
		GetDlgItem(IDC_OPTIONS_VENUES_SELECTED)->EnableWindow(FALSE);
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
			if (m_filterNames.end() != m_filterNames.find((*iter)))
				m_ctrlNames.SetChecked(hItem, true);
		}
	}

	UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsTraining::OnViewUpdate() 
{
	UpdateData(TRUE);
	UpdateControls();
}

void CDlgOptionsTraining::OnSetdispinfoNames(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_filterNames.clear();
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
				m_filterNames.insert((LPCSTR)str);
		}
		hItem = m_ctrlNames.GetNextItem(hItem, TVGN_NEXT);
	}
	*pResult = 0;
}
