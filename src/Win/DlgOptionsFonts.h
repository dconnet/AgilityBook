#pragma once

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
 * @brief interface of the CDlgOptionsFonts class
 * @author David Connet
 *
 * Revision History
 * @li 2003-08-09 DRC Created, moved fonts from calendar page.
 */

#include "AgilityBookOptions.h"

class CDlgOptionsFonts : public CPropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNCREATE(CDlgOptionsFonts)
public:
	CDlgOptionsFonts();
	~CDlgOptionsFonts();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsFonts)
	enum { IDD = IDD_VIEW_OPTIONS_FONTS };
	CStatic	m_ctrlFontGeneralPrint;
	CStatic	m_ctrlFontCalViewDate;
	CStatic	m_ctrlFontCalViewText;
	CStatic	m_ctrlFontCalPrintDate;
	CStatic	m_ctrlFontCalPrintText;
	//}}AFX_DATA
	CFontInfo m_fontGeneralPrintInfo;
	CFontInfo m_fontDateInfo[2];
	CFontInfo m_fontTextInfo[2];
	CFont m_fontGeneralPrint;
	CFont m_fontDate[2];
	CFont m_fontText[2];

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsFonts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgOptionsFonts)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontGeneralPrint();
	afx_msg void OnFontCalViewDate();
	afx_msg void OnFontCalViewText();
	afx_msg void OnFontCalPrintDate();
	afx_msg void OnFontCalPrintText();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
