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
 * @brief implementation of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2006-11-22 DRC Turned this into Calendar only, created new Print page.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsCalendar.h"

#include "ARBAgilityRecordBook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static const struct
{
	CAgilityBookOptions::CalendarColorItem item;
	LPCTSTR text;
} sc_CalItems[] =
{
	{CAgilityBookOptions::eCalColorNotEntered, CALENDAR_NOTENTERED},
	{CAgilityBookOptions::eCalColorPlanning,   CALENDAR_PLANNING},
	{CAgilityBookOptions::eCalColorOpening,    _T("  Opening")},
	{CAgilityBookOptions::eCalColorClosing,    _T("  Closing")},
	{CAgilityBookOptions::eCalColorEntered,    CALENDAR_ENTERED},
};
static const int sc_CalItemsCount = sizeof(sc_CalItems) / sizeof(sc_CalItems[0]);

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar property page

IMPLEMENT_DYNAMIC(CDlgOptionsCalendar, CDlgBasePropertyPage)


CDlgOptionsCalendar::CDlgOptionsCalendar()
	: CDlgBasePropertyPage(CDlgOptionsCalendar::IDD)
	, m_ctrlCalEntries(false)
{
	//{{AFX_DATA_INIT(CDlgOptionsCalendar)
	m_DayOfWeek = -1;
	m_bAutoDelete = FALSE;
	m_bHideOld = FALSE;
	m_Days = 0;
	m_bHideOverlapping = FALSE;
	m_bOpeningNear = TRUE;
	m_nOpeningNear = -1;
	m_bClosingNear = TRUE;
	m_nClosingNear = -1;
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
	DDX_Check(pDX, IDC_OPT_CAL_WARN_OPENNEAR, m_bOpeningNear);
	DDX_Control(pDX, IDC_OPT_CAL_OPENNEAR, m_ctrlOpeningNear);
	DDX_Text(pDX, IDC_OPT_CAL_OPENNEAR, m_nOpeningNear);
	DDX_Control(pDX, IDC_OPT_CAL_COLOR_OPENNEAR, m_ctrlOpeningNearColor);
	DDX_Control(pDX, IDC_OPT_CAL_COLOR_OPENNEAR_SET, m_ctrlOpeningNearSet);
	DDX_Check(pDX, IDC_OPT_CAL_WARN_CLOSENEAR, m_bClosingNear);
	DDX_Control(pDX, IDC_OPT_CAL_CLOSENEAR, m_ctrlClosingNear);
	DDX_Text(pDX, IDC_OPT_CAL_CLOSENEAR, m_nClosingNear);
	DDX_Control(pDX, IDC_OPT_CAL_COLOR_CLOSENEAR, m_ctrlClosingNearColor);
	DDX_Control(pDX, IDC_OPT_CAL_COLOR_CLOSENEAR_SET, m_ctrlClosingNearSet);
	DDX_CBIndex(pDX, IDC_OPT_CAL_DAY_OF_WEEK, m_DayOfWeek);
	DDX_Check(pDX, IDC_OPT_CAL_AUTO_DELETE, m_bAutoDelete);
	DDX_Check(pDX, IDC_OPT_CAL_HIDE_OLD, m_bHideOld);
	DDX_Text(pDX, IDC_OPT_CAL_OLD_ENTRY_DAYS, m_Days);
	DDX_Check(pDX, IDC_OPT_CAL_HIDE_OVERLAP, m_bHideOverlapping);
	DDX_Check(pDX, IDC_OPT_CAL_OPENING, m_bOpening);
	DDX_Check(pDX, IDC_OPT_CAL_CLOSING, m_bClosing);
	DDX_Control(pDX, IDC_OPT_CAL_ENTRIES, m_ctrlCalEntries);
	DDX_Control(pDX, IDC_OPT_CAL_COLOR, m_ctrlColor);
	DDX_Control(pDX, IDC_OPT_CAL_VIEW, m_ctrlCalView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOptionsCalendar, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsCalendar)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_OPT_CAL_ENTRIES, OnSelchangeCalEntries)
	ON_BN_CLICKED(IDC_OPT_CAL_WARN_OPENNEAR, OnCalNear)
	ON_BN_CLICKED(IDC_OPT_CAL_COLOR_OPENNEAR_SET, OnCalColorOpeningNear)
	ON_BN_CLICKED(IDC_OPT_CAL_WARN_CLOSENEAR, OnCalNear)
	ON_BN_CLICKED(IDC_OPT_CAL_COLOR_CLOSENEAR_SET, OnCalColorClosingNear)
	ON_BN_CLICKED(IDC_OPT_CAL_COLOR_SET, OnCalColors)
	ON_BN_CLICKED(IDC_OPT_CAL_FONT, OnFontCalView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgOptionsCalendar::UpdateControls()
{
	if (m_bOpeningNear)
	{
		m_ctrlOpeningNear.EnableWindow(TRUE);
		m_ctrlOpeningNearSet.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlOpeningNear.EnableWindow(FALSE);
		m_ctrlOpeningNearSet.EnableWindow(FALSE);
	}
	if (m_bClosingNear)
	{
		m_ctrlClosingNear.EnableWindow(TRUE);
		m_ctrlClosingNearSet.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlClosingNear.EnableWindow(FALSE);
		m_ctrlClosingNearSet.EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar message handlers

BOOL CDlgOptionsCalendar::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_fontCalViewInfo.CreateFont(m_fontCalView);
	m_ctrlCalView.SetFont(&m_fontCalView);
	for (int i = 0; i < sc_CalItemsCount; ++i)
	{
		int idx = m_ctrlCalEntries.AddString(sc_CalItems[i].text);
		m_ctrlCalEntries.SetItemData(idx, sc_CalItems[i].item);
	}
	m_ctrlCalEntries.SetCurSel(0);
	UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOptionsCalendar::OnDrawItem(
		int nIDCtl,
		LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect r;
	switch (nIDCtl)
	{
	case IDC_OPT_CAL_COLOR_OPENNEAR:
		m_ctrlOpeningNearColor.GetClientRect(&r);
		pDC->SetBkColor(CAgilityBookOptions::CalendarOpeningNearColor());
		pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
		break;
	case IDC_OPT_CAL_COLOR_CLOSENEAR:
		m_ctrlClosingNearColor.GetClientRect(&r);
		pDC->SetBkColor(CAgilityBookOptions::CalendarClosingNearColor());
		pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
		break;
	case IDC_OPT_CAL_COLOR:
		{
			int idx = m_ctrlCalEntries.GetCurSel();
			if (0 <= idx)
			{
				m_ctrlColor.GetClientRect(&r);
				CAgilityBookOptions::CalendarColorItem item = static_cast<CAgilityBookOptions::CalendarColorItem>(m_ctrlCalEntries.GetItemData(idx));
				pDC->SetBkColor(CAgilityBookOptions::CalendarColor(item));
				pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);
			}
		}
		break;
	case IDC_OPT_CAL_VIEW:
		{
			m_ctrlCalView.GetClientRect(&r);
			pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
			pDC->ExtTextOut(0, 0, ETO_OPAQUE, r, NULL, 0, NULL);

			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

			CFont* pOldFont = pDC->SelectObject(&m_fontCalView);
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);

			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorNotEntered));
			pDC->DrawText(CALENDAR_NOTENTERED _T(" Text"), r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPlanning));
			pDC->DrawText(CALENDAR_PLANNING _T(" Text"), r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorOpening));
			pDC->DrawText(_T("Opening Text"), r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorClosing));
			pDC->DrawText(_T("Closing Text"), r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorEntered));
			pDC->DrawText(CALENDAR_ENTERED _T(" Text"), r, DT_NOPREFIX|DT_TOP);

			pDC->SelectObject(pOldFont);
		}
		break;
	}
}


void CDlgOptionsCalendar::OnSelchangeCalEntries()
{
	UpdateData(TRUE);
	m_ctrlColor.Invalidate();
	m_ctrlCalView.Invalidate();
}


void CDlgOptionsCalendar::OnCalNear()
{
	UpdateData(TRUE);
	UpdateControls();
}


void CDlgOptionsCalendar::OnCalColorOpeningNear()
{
	UpdateData(TRUE);
	CColorDialog dlg(CAgilityBookOptions::CalendarOpeningNearColor(), CC_ANYCOLOR, this);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookOptions::SetCalendarOpeningNearColor(dlg.GetColor());
		m_ctrlOpeningNearColor.Invalidate();
	}
}


void CDlgOptionsCalendar::OnCalColorClosingNear()
{
	UpdateData(TRUE);
	CColorDialog dlg(CAgilityBookOptions::CalendarClosingNearColor(), CC_ANYCOLOR, this);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookOptions::SetCalendarClosingNearColor(dlg.GetColor());
		m_ctrlClosingNearColor.Invalidate();
	}
}


void CDlgOptionsCalendar::OnCalColors()
{
	UpdateData(TRUE);
	int idx = m_ctrlCalEntries.GetCurSel();
	if (0 > idx)
		return;
	CAgilityBookOptions::CalendarColorItem item = static_cast<CAgilityBookOptions::CalendarColorItem>(m_ctrlCalEntries.GetItemData(idx));
	CColorDialog dlg(CAgilityBookOptions::CalendarColor(item), CC_ANYCOLOR, this);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookOptions::SetCalendarColor(item, dlg.GetColor());
		m_ctrlColor.Invalidate();
		m_ctrlCalView.Invalidate();
	}
}


void CDlgOptionsCalendar::OnFontCalView()
{
	UpdateData(TRUE);
	LOGFONT logFont;
	m_fontCalView.GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontCalViewInfo.CreateFont(dlg, m_fontCalView);
		m_ctrlCalView.SetFont(&(m_fontCalView));
		m_ctrlCalView.Invalidate();
	}
}
