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
 * @brief implementation of the CDlgOptionsCalendar class
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
#include "DlgOptionsCalendar.h"
#include ".\dlgoptionscalendar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar property page

IMPLEMENT_DYNCREATE(CDlgOptionsCalendar, CDlgBasePropertyPage)

CDlgOptionsCalendar::CDlgOptionsCalendar()
	: CDlgBasePropertyPage(CDlgOptionsCalendar::IDD)
{
	//{{AFX_DATA_INIT(CDlgOptionsCalendar)
	m_DayOfWeek = -1;
	m_bAutoDelete = FALSE;
	m_bHideOld = FALSE;
	m_Days = 0;
	m_bHideOverlapping = FALSE;
	m_sizeX = 0;
	m_sizeY = 0;
	m_bNormal = TRUE;
	m_bOpening = FALSE;
	m_bClosing = FALSE;
	//}}AFX_DATA_INIT
}

CDlgOptionsCalendar::~CDlgOptionsCalendar()
{
}

void CDlgOptionsCalendar::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsCalendar)
	DDX_CBIndex(pDX, IDC_OPTIONS_CAL_DAY_OF_WEEK, m_DayOfWeek);
	DDX_Check(pDX, IDC_OPTIONS_CAL_AUTO_DELETE, m_bAutoDelete);
	DDX_Check(pDX, IDC_OPTIONS_CAL_HIDE_OLD, m_bHideOld);
	DDX_Text(pDX, IDC_OPTIONS_CAL_OLD_ENTRY_DAYS, m_Days);
	DDX_Check(pDX, IDC_OPTIONS_CAL_HIDE_OVERLAP, m_bHideOverlapping);
	DDX_Text(pDX, IDC_OPTIONS_CAL_SIZE_X, m_sizeX);
	DDV_MinMaxInt(pDX, m_sizeX, 10, 1000);
	DDX_Text(pDX, IDC_OPTIONS_CAL_SIZE_Y, m_sizeY);
	DDV_MinMaxInt(pDX, m_sizeY, 10, 1000);
	DDX_Check(pDX, IDC_OPTIONS_CAL_NORMAL, m_bNormal);
	DDX_Check(pDX, IDC_OPTIONS_CAL_OPENING, m_bOpening);
	DDX_Control(pDX, IDC_OPTIONS_CAL_COLOR_OPEN, m_Opening);
	DDX_Check(pDX, IDC_OPTIONS_CAL_CLOSING, m_bClosing);
	DDX_Control(pDX, IDC_OPTIONS_CAL_COLOR_CLOSE, m_Closing);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsCalendar, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsCalendar)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_OPTIONS_CAL_COLOR_OPEN_SET, OnOptionsCalColorOpen)
	ON_BN_CLICKED(IDC_OPTIONS_CAL_COLOR_CLOSE_SET, OnOptionsCalColorClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar message handlers

BOOL CDlgOptionsCalendar::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsCalendar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CStatic* pCtrl = NULL;
	COLORREF color = 0;
	switch (nIDCtl)
	{
	case IDC_OPTIONS_CAL_COLOR_OPEN:
		pCtrl = &m_Opening;
		color = CAgilityBookOptions::CalendarOpeningColor();
		break;
	case IDC_OPTIONS_CAL_COLOR_CLOSE:
		pCtrl = &m_Closing;
		color = CAgilityBookOptions::CalendarClosingColor();
		break;
	}
	if (pCtrl)
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CRect r;
		pCtrl->GetClientRect(&r);
		dc.SetBkColor(color);
		dc.ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
		dc.Detach();
	}
}

void CDlgOptionsCalendar::OnOptionsCalColorOpen()
{
	CColorDialog dlg(CAgilityBookOptions::CalendarOpeningColor(), CC_ANYCOLOR, this);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookOptions::SetCalendarOpeningColor(dlg.GetColor());
		m_Opening.Invalidate();
	}
}

void CDlgOptionsCalendar::OnOptionsCalColorClose()
{
	CColorDialog dlg(CAgilityBookOptions::CalendarClosingColor(), CC_ANYCOLOR, this);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookOptions::SetCalendarClosingColor(dlg.GetColor());
		m_Closing.Invalidate();
	}
}
