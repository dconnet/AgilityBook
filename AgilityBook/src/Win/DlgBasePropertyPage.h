#pragma once

/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @brief Base class for all property pages.
 * @author David Connet
 *
 * Revision History
 * @li 2004-05-31 DRC Created.
 */

class CDlgBasePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgBasePropertyPage)
public:
#if _MSC_VER < 1300
	CDlgBasePropertyPage(
			UINT nIDTemplate,
			UINT nIDCaption = 0);
	CDlgBasePropertyPage(
			LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0);
#else
	explicit CDlgBasePropertyPage(
			UINT nIDTemplate,
			UINT nIDCaption = 0,
			DWORD dwSize = sizeof(PROPSHEETPAGE)
	);
	explicit CDlgBasePropertyPage(
			LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0,
			DWORD dwSize = sizeof(PROPSHEETPAGE)
	);
	CDlgBasePropertyPage(
			UINT nIDTemplate,
			UINT nIDCaption,
			UINT nIDHeaderTitle,
			UINT nIDHeaderSubTitle = 0,
			DWORD dwSize = sizeof(PROPSHEETPAGE)
	);
	CDlgBasePropertyPage(
			LPCTSTR lpszTemplateName,
			UINT nIDCaption,
			UINT nIDHeaderTitle,
			UINT nIDHeaderSubTitle = 0,
			DWORD dwSize = sizeof(PROPSHEETPAGE)
	);
#endif
	~CDlgBasePropertyPage();

private:
// Dialog Data
	//{{AFX_DATA(CDlgBasePropertyPage)
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDlgBasePropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgBasePropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
