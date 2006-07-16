/*
 * Copyright © 2006 David Connet. All Rights Reserved.
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
 * @brief Let user specify how to sort events within a venue.
 * @author David Connet
 *
 * Revision History
 * @li 2006-07-16 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgPointsViewSort.h"

#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPointsViewSort dialog

CDlgPointsViewSort::CDlgPointsViewSort(CWnd* pParent)
	: CDlgBaseDialog(CDlgPointsViewSort::IDD, pParent)
	, m_ctrlPrimary(false)
	, m_ctrlSecondary(false)
	, m_ctrlTertiary(false)
{
	//{{AFX_DATA_INIT(CDlgPointsViewSort)
	//}}AFX_DATA_INIT
	CAgilityBookOptions::GetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);
}

void CDlgPointsViewSort::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPointsViewSort)
	DDX_Control(pDX, IDC_POINTS_VIEW_SORT_C1, m_ctrlPrimary);
	DDX_Control(pDX, IDC_POINTS_VIEW_SORT_C2, m_ctrlSecondary);
	DDX_Control(pDX, IDC_POINTS_VIEW_SORT_C3, m_ctrlTertiary);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgPointsViewSort, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgPointsViewSort)
	ON_CBN_SELCHANGE(IDC_POINTS_VIEW_SORT_C1, OnCbnSelchangePrimary)
	ON_CBN_SELCHANGE(IDC_POINTS_VIEW_SORT_C2, OnCbnSelchangeSecondary)
	ON_CBN_SELCHANGE(IDC_POINTS_VIEW_SORT_C3, OnCbnSelchangeTertiary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

static int AddItem(CComboBox2& ctrl, CAgilityBookOptions::PointsViewSort item, CAgilityBookOptions::PointsViewSort inSelect)
{
	int idx = CB_ERR;
	switch (item)
	{
	case CAgilityBookOptions::ePointsViewSortDivision:
		idx = ctrl.AddString("Division");
		ctrl.SetItemData(idx, item);
		break;
	case CAgilityBookOptions::ePointsViewSortLevel:
		idx = ctrl.AddString("Level");
		ctrl.SetItemData(idx, item);
		break;
	case CAgilityBookOptions::ePointsViewSortEvent:
		idx = ctrl.AddString("Event");
		ctrl.SetItemData(idx, item);
		break;
	}
	if (item == inSelect && CB_ERR != idx)
		ctrl.SetCurSel(idx);
	return idx;
}

void CDlgPointsViewSort::FillPrimary()
{
	m_ctrlPrimary.ResetContent();
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortDivision, m_Primary);
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortLevel, m_Primary);
	AddItem(m_ctrlPrimary, CAgilityBookOptions::ePointsViewSortEvent, m_Primary);
	FillSecondary();
}

void CDlgPointsViewSort::FillSecondary()
{
	m_ctrlSecondary.ResetContent();
	if (m_Primary != CAgilityBookOptions::ePointsViewSortDivision)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortDivision, m_Secondary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortLevel)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortLevel, m_Secondary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortEvent)
		AddItem(m_ctrlSecondary, CAgilityBookOptions::ePointsViewSortEvent, m_Secondary);
	FillTertiary();
}

void CDlgPointsViewSort::FillTertiary()
{
	m_ctrlTertiary.ResetContent();
	if (m_Primary != CAgilityBookOptions::ePointsViewSortDivision
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortDivision)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortDivision, m_Tertiary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortLevel
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortLevel)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortLevel, m_Tertiary);
	if (m_Primary != CAgilityBookOptions::ePointsViewSortEvent
	&& m_Secondary != CAgilityBookOptions::ePointsViewSortEvent)
		AddItem(m_ctrlTertiary, CAgilityBookOptions::ePointsViewSortEvent, m_Tertiary);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPointsViewSort message handlers

BOOL CDlgPointsViewSort::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	FillPrimary();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPointsViewSort::OnCbnSelchangePrimary()
{
	int idx = m_ctrlPrimary.GetCurSel();
	if (CB_ERR == idx)
		m_Primary = CAgilityBookOptions::ePointsViewSortUnknown;
	else
		m_Primary = static_cast<CAgilityBookOptions::PointsViewSort>(m_ctrlPrimary.GetItemData(idx));
	FillSecondary();
}

void CDlgPointsViewSort::OnCbnSelchangeSecondary()
{
	int idx = m_ctrlSecondary.GetCurSel();
	if (CB_ERR == idx)
		m_Secondary = CAgilityBookOptions::ePointsViewSortUnknown;
	else
		m_Secondary = static_cast<CAgilityBookOptions::PointsViewSort>(m_ctrlSecondary.GetItemData(idx));
	FillTertiary();
}

void CDlgPointsViewSort::OnCbnSelchangeTertiary()
{
	int idx = m_ctrlTertiary.GetCurSel();
	if (CB_ERR == idx)
		m_Tertiary = CAgilityBookOptions::ePointsViewSortUnknown;
	else
		m_Tertiary = static_cast<CAgilityBookOptions::PointsViewSort>(m_ctrlTertiary.GetItemData(idx));
}

void CDlgPointsViewSort::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	int index = m_ctrlPrimary.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlPrimary);
		return;
	}

	index = m_ctrlSecondary.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlSecondary);
		return;
	}

	index = m_ctrlTertiary.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlTertiary);
		return;
	}

	CAgilityBookOptions::SetPointsViewSort(m_Primary, m_Secondary, m_Tertiary);

	CDlgBaseDialog::OnOK();
}
