/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsFonts class
 * @author David Connet
 *
 * Revision History
 * @li 2003-08-09 DRC Created, moved fonts from calendar page.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgOptionsFonts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFonts property page

IMPLEMENT_DYNCREATE(CDlgOptionsFonts, CPropertyPage)

CDlgOptionsFonts::CDlgOptionsFonts() : CPropertyPage(CDlgOptionsFonts::IDD)
{
	//{{AFX_DATA_INIT(CDlgOptionsFonts)
	//}}AFX_DATA_INIT
}

CDlgOptionsFonts::~CDlgOptionsFonts()
{
}

void CDlgOptionsFonts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptionsFonts)
	DDX_Control(pDX, IDC_FONT_GENERAL_PRINT_TEXT, m_ctrlFontGeneralPrint);
	DDX_Control(pDX, IDC_FONT_CAL_VIEW_DATE_TEXT, m_ctrlFontCalViewDate);
	DDX_Control(pDX, IDC_FONT_CAL_VIEW_TEXT_TEXT, m_ctrlFontCalViewText);
	DDX_Control(pDX, IDC_FONT_CAL_PRINT_DATE_TEXT, m_ctrlFontCalPrintDate);
	DDX_Control(pDX, IDC_FONT_CAL_PRINT_TEXT_TEXT, m_ctrlFontCalPrintText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgOptionsFonts, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgOptionsFonts)
	ON_BN_CLICKED(IDC_FONT_GENERAL_PRINT, OnFontGeneralPrint)
	ON_BN_CLICKED(IDC_FONT_CAL_VIEW_DATE, OnFontCalViewDate)
	ON_BN_CLICKED(IDC_FONT_CAL_VIEW_TEXT, OnFontCalViewText)
	ON_BN_CLICKED(IDC_FONT_CAL_PRINT_DATE, OnFontCalPrintDate)
	ON_BN_CLICKED(IDC_FONT_CAL_PRINT_TEXT, OnFontCalPrintText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptionsFonts message handlers

BOOL CDlgOptionsFonts::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_fontGeneralPrintInfo.CreateFont(m_fontGeneralPrint);
	for (int i = 0; i < 2; ++i)
	{
		m_fontDateInfo[i].CreateFont(m_fontDate[i]);
		m_fontTextInfo[i].CreateFont(m_fontText[i]);
	}
	m_ctrlFontGeneralPrint.SetFont(&m_fontGeneralPrint);
	m_ctrlFontCalViewDate.SetFont(&(m_fontDate[0]));
	m_ctrlFontCalViewText.SetFont(&(m_fontText[0]));
	m_ctrlFontCalPrintDate.SetFont(&(m_fontDate[1]));
	m_ctrlFontCalPrintText.SetFont(&(m_fontText[1]));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptionsFonts::OnFontGeneralPrint()
{
	LOGFONT logFont;
	m_fontGeneralPrint.GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontGeneralPrintInfo.CreateFont(dlg, m_fontGeneralPrint);
		m_ctrlFontGeneralPrint.SetFont(&(m_fontGeneralPrint));
	}
}

void CDlgOptionsFonts::OnFontCalViewDate() 
{
	LOGFONT logFont;
	m_fontDate[0].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontDateInfo[0].CreateFont(dlg, m_fontDate[0]);
		m_ctrlFontCalViewDate.SetFont(&(m_fontDate[0]));
	}
}

void CDlgOptionsFonts::OnFontCalViewText() 
{
	LOGFONT logFont;
	m_fontText[0].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontTextInfo[0].CreateFont(dlg, m_fontText[0]);
		m_ctrlFontCalViewText.SetFont(&(m_fontText[0]));
	}
}

void CDlgOptionsFonts::OnFontCalPrintDate() 
{
	LOGFONT logFont;
	m_fontDate[1].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS|CF_PRINTERFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontDateInfo[1].CreateFont(dlg, m_fontDate[1]);
		m_ctrlFontCalViewDate.SetFont(&(m_fontDate[1]));
	}
}

void CDlgOptionsFonts::OnFontCalPrintText() 
{
	LOGFONT logFont;
	m_fontText[1].GetLogFont(&logFont);
	CFontDialog dlg(&logFont, CF_SCREENFONTS|CF_PRINTERFONTS);
	if (IDOK == dlg.DoModal())
	{
		m_fontTextInfo[1].CreateFont(dlg, m_fontText[1]);
		m_ctrlFontCalViewText.SetFont(&(m_fontText[1]));
	}
}
