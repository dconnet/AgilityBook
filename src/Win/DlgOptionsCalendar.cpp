/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2009-01-11 DRC Fixed a bug that added an 8th day to week.
 * @li 2006-11-22 DRC Turned this into Calendar only, created new Print page.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "DlgOptionsCalendar.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"


CDlgOptionsCalendar::CDlgOptionsCalendar(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	//, m_ctrlDayOfWeek(false)
	//, m_ctrlCalEntries(false)
{
	/*
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
	m_pageCal.m_nOpeningNear = CAgilityBookOptions::CalendarOpeningNear();
	if (0 > m_pageCal.m_nOpeningNear)
	{
		m_pageCal.m_bOpeningNear = FALSE;
		m_pageCal.m_nOpeningNear = 0;
	}
	else
		m_pageCal.m_bOpeningNear = TRUE;
	m_pageCal.m_nClosingNear = CAgilityBookOptions::CalendarClosingNear();
	if (0 > m_pageCal.m_nClosingNear)
	{
		m_pageCal.m_bClosingNear = FALSE;
		m_pageCal.m_nClosingNear = 0;
	}
	else
		m_pageCal.m_bClosingNear = TRUE;
	m_pageCal.m_DayOfWeek = static_cast<int>(CAgilityBookOptions::GetFirstDayOfWeek());
	m_pageCal.m_bAutoDelete = CAgilityBookOptions::AutoDeleteCalendarEntries() ? TRUE : FALSE;
	m_pageCal.m_bHideOld = CAgilityBookOptions::ViewAllCalendarEntries() ? FALSE : TRUE;
	m_pageCal.m_Days = CAgilityBookOptions::DaysTillEntryIsPast();
	m_pageCal.m_bHideOverlapping = CAgilityBookOptions::HideOverlappingCalendarEntries() ? TRUE : FALSE;
	m_pageCal.m_bOpening = CAgilityBookOptions::ViewAllCalendarOpening() ? TRUE : FALSE;
	m_pageCal.m_bClosing = CAgilityBookOptions::ViewAllCalendarClosing() ? TRUE : FALSE;
	CAgilityBookOptions::GetCalendarFontInfo(m_pageCal.m_fontCalViewInfo);
	*/
}


void CDlgOptionsCalendar::Save()
{
	/*
	if (!m_pageCal.m_bOpeningNear)
		m_pageCal.m_nOpeningNear = -1;
	CAgilityBookOptions::SetCalendarOpeningNear(m_pageCal.m_nOpeningNear);
	if (!m_pageCal.m_bClosingNear)
		m_pageCal.m_nClosingNear = -1;
	CAgilityBookOptions::SetCalendarClosingNear(m_pageCal.m_nClosingNear);
	CAgilityBookOptions::SetFirstDayOfWeek(static_cast<ARBDate::DayOfWeek>(m_pageCal.m_DayOfWeek));
	CAgilityBookOptions::SetAutoDeleteCalendarEntries(m_pageCal.m_bAutoDelete ? true : false);
	CAgilityBookOptions::SetViewAllCalendarEntries(m_pageCal.m_bHideOld ? false : true);
	CAgilityBookOptions::SetDaysTillEntryIsPast(m_pageCal.m_Days);
	CAgilityBookOptions::SetHideOverlappingCalendarEntries(m_pageCal.m_bHideOverlapping ? true : false);
	CAgilityBookOptions::SetViewAllCalendarOpening(m_pageCal.m_bOpening ? true : false);
	CAgilityBookOptions::SetViewAllCalendarClosing(m_pageCal.m_bClosing ? true : false);
	CAgilityBookOptions::SetCalendarFontInfo(m_pageCal.m_fontCalViewInfo);
	*/
}

#pragma message PRAGMA_MESSAGE("TODO: Implement CDlgOptionsCalendar")
#if 0
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
	DDX_Control(pDX, IDC_OPT_CAL_DAY_OF_WEEK, m_ctrlDayOfWeek);
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

	int idx;
	assert(ARBDate::eSunday == 0);
	LCTYPE days[] = {
		LOCALE_SDAYNAME7, // Start with sunday
		LOCALE_SDAYNAME1,
		LOCALE_SDAYNAME2,
		LOCALE_SDAYNAME3,
		LOCALE_SDAYNAME4,
		LOCALE_SDAYNAME5,
		LOCALE_SDAYNAME6
	};
	LANGID id = theApp.LanguageManager().CurrentLanguage();
	for (idx = 0; idx < 7; ++idx)
	{
		CString str;
		GetLocaleInfo(id, days[idx], str.GetBuffer(80), 80);
		str.ReleaseBuffer();
		m_ctrlDayOfWeek.AddString(str);
	}
	m_ctrlDayOfWeek.SetCurSel(m_DayOfWeek);

	idx = m_ctrlCalEntries.AddString(Localization()->CalendarNotEntered().c_str());
	m_ctrlCalEntries.SetItemData(idx, CAgilityBookOptions::eCalColorNotEntered);
	idx = m_ctrlCalEntries.AddString(Localization()->CalendarPlanning().c_str());
	m_ctrlCalEntries.SetItemData(idx, CAgilityBookOptions::eCalColorPlanning);
	CString data;
	data.LoadString(IDS_COL_OPENING);
	data = wxT("  ") + data;
	idx = m_ctrlCalEntries.AddString(data);
	m_ctrlCalEntries.SetItemData(idx, CAgilityBookOptions::eCalColorOpening);
	data.LoadString(IDS_COL_CLOSING);
	data = wxT("  ") + data;
	idx = m_ctrlCalEntries.AddString(data);
	m_ctrlCalEntries.SetItemData(idx, CAgilityBookOptions::eCalColorClosing);
	idx = m_ctrlCalEntries.AddString(Localization()->CalendarEntered().c_str());
	m_ctrlCalEntries.SetItemData(idx, CAgilityBookOptions::eCalColorEntered);

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

			CString data;
			data = Localization()->CalendarNotEntered().c_str();
			data += wxT(" Text");
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorNotEntered));
			pDC->DrawText(data, r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			data = Localization()->CalendarPlanning().c_str();
			data += wxT(" Text");
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPlanning));
			pDC->DrawText(data, r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			data.LoadString(IDS_COL_OPENING);
			data += wxT(" Text");
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorOpening));
			pDC->DrawText(data, r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			data.LoadString(IDS_COL_CLOSING);
			data += wxT(" Text");
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorClosing));
			pDC->DrawText(data, r, DT_NOPREFIX|DT_TOP);

			r.top += tm.tmHeight;
			data = Localization()->CalendarEntered().c_str();
			data += wxT(" Text");
			pDC->SetTextColor(CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorEntered));
			pDC->DrawText(data, r, DT_NOPREFIX|DT_TOP);

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
#endif
