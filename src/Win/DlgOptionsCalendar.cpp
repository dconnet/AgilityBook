/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @li 2003-07-31 DRC Allow screen fonts for printer font selection. Also, the
 *                    wrong font was created for the printer date font.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsCalendar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar property page

IMPLEMENT_DYNCREATE(CDlgOptionsCalendar, CPropertyPage)

CDlgOptionsCalendar::CDlgOptionsCalendar() : CPropertyPage(CDlgOptionsCalendar::IDD)
{
	//{{AFX_DATA_INIT(CDlgOptionsCalendar)
	m_DayOfWeek = -1;
	m_bAutoDelete = FALSE;
	m_bHideOld = FALSE;
	m_Days = 0;
	m_bHideOverlapping = FALSE;
	m_sizeX = 0;
	m_sizeY = 0;
	//}}AFX_DATA_INIT
}

CDlgOptionsCalendar::~CDlgOptionsCalendar()
{
}

void CDlgOptionsCalendar::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsCalendar)
	DDX_CBIndex(pDX, IDC_DAY_OF_WEEK, m_DayOfWeek);
	DDX_Check(pDX, IDC_AUTO_DELETE, m_bAutoDelete);
	DDX_Check(pDX, IDC_HIDE_OLD, m_bHideOld);
	DDX_Text(pDX, IDC_OLD_ENTRY_DAYS, m_Days);
	DDX_Check(pDX, IDC_HIDE_OVERLAP, m_bHideOverlapping);
	DDX_Text(pDX, IDC_SIZE_X, m_sizeX);
	DDV_MinMaxInt(pDX, m_sizeX, 10, 1000);
	DDX_Text(pDX, IDC_SIZE_Y, m_sizeY);
	DDV_MinMaxInt(pDX, m_sizeY, 10, 1000);
	DDX_Control(pDX, IDC_FONT_DATE_TEXT, m_ctrlFontDate);
	DDX_Control(pDX, IDC_FONT_TEXT_TEXT, m_ctrlFontText);
	DDX_Control(pDX, IDC_FONT2_DATE_TEXT, m_ctrlFont2Date);
	DDX_Control(pDX, IDC_FONT2_TEXT_TEXT, m_ctrlFont2Text);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsCalendar, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsCalendar)
	ON_BN_CLICKED(IDC_FONT_DATE, OnFontDate)
	ON_BN_CLICKED(IDC_FONT_TEXT, OnFontText)
	ON_BN_CLICKED(IDC_FONT2_DATE, OnFont2Date)
	ON_BN_CLICKED(IDC_FONT2_TEXT, OnFont2Text)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsCalendar message handlers

BOOL CDlgOptionsCalendar::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	for (int i = 0; i < 2; ++i)
	{
		m_fontDateInfo[i].CreateFont(m_fontDate[i]);
		m_fontTextInfo[i].CreateFont(m_fontText[i]);
	}
	m_ctrlFontDate.SetFont(&(m_fontDate[0]));
	m_ctrlFontText.SetFont(&(m_fontText[0]));
	m_ctrlFont2Date.SetFont(&(m_fontDate[1]));
	m_ctrlFont2Text.SetFont(&(m_fontText[1]));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsCalendar::OnFontDate() 
{
	LOGFONT logFont;
	m_fontDate[0].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontDateInfo[0].CreateFont(dlg, m_fontDate[0]);
		m_ctrlFontDate.SetFont(&(m_fontDate[0]));
	}
}

void CDlgOptionsCalendar::OnFontText() 
{
	LOGFONT logFont;
	m_fontText[0].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontTextInfo[0].CreateFont(dlg, m_fontText[0]);
		m_ctrlFontText.SetFont(&(m_fontText[0]));
	}
}

void CDlgOptionsCalendar::OnFont2Date() 
{
	LOGFONT logFont;
	m_fontDate[1].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS|CF_PRINTERFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontDateInfo[1].CreateFont(dlg, m_fontDate[1]);
		m_ctrlFontDate.SetFont(&(m_fontDate[1]));
	}
}

void CDlgOptionsCalendar::OnFont2Text() 
{
	LOGFONT logFont;
	m_fontText[1].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS|CF_PRINTERFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontTextInfo[1].CreateFont(dlg, m_fontText[1]);
		m_ctrlFontText.SetFont(&(m_fontText[1]));
	}
}
