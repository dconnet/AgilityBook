#pragma once

/*
 * Copyright © 2006-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgOptionsPrint class
 * @author David Connet
 *
 * Revision History
 * @li 2006-11-22 DRC Moved printing to new page.
 */

#include "AgilityBookOptions.h"
#include "ComboBox.h"
#include "DlgBasePropertyPage.h"

class CDlgOptionsPrint : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNAMIC(CDlgOptionsPrint)
public:
	CDlgOptionsPrint();
	~CDlgOptionsPrint();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsPrint)
	enum { IDD = IDD_VIEW_OPTIONS_PRINTING };
	CStatic	m_ctrlFontPrint;
	double m_Left;
	double m_Right;
	double m_Top;
	double m_Bottom;
	//}}AFX_DATA
	CFontInfo m_fontPrintInfo;
	CFont m_fontPrint;

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsPrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgOptionsPrint)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
