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

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgBasePropertyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBasePropertyPage property page

IMPLEMENT_DYNAMIC(CDlgBasePropertyPage, CPropertyPage)

#if _MSC_VER < 1300
CDlgBasePropertyPage::CDlgBasePropertyPage(
		UINT nIDTemplate,
		UINT nIDCaption)
	: CPropertyPage(nIDTemplate, nIDCaption)
{
	m_psp.dwFlags |= PSP_HASHELP;
}
CDlgBasePropertyPage::CDlgBasePropertyPage(
		LPCTSTR lpszTemplateName,
		UINT nIDCaption)
	: CPropertyPage(lpszTemplateName, nIDCaption)
{
	m_psp.dwFlags |= PSP_HASHELP;
}

#else
CDlgBasePropertyPage::CDlgBasePropertyPage(
		UINT nIDTemplate,
		UINT nIDCaption,
		DWORD dwSize)
	: CPropertyPage(nIDTemplate, nIDCaption, dwSize)
{
	m_psp.dwFlags |= PSP_HASHELP;
	//{{AFX_DATA_INIT(CDlgBasePropertyPage)
	//}}AFX_DATA_INIT
}
CDlgBasePropertyPage::CDlgBasePropertyPage(
		LPCTSTR lpszTemplateName,
		UINT nIDCaption,
		DWORD dwSize)
	: CPropertyPage(lpszTemplateName, nIDCaption, dwSize)
{
	m_psp.dwFlags |= PSP_HASHELP;
}
CDlgBasePropertyPage::CDlgBasePropertyPage(
		UINT nIDTemplate,
		UINT nIDCaption,
		UINT nIDHeaderTitle,
		UINT nIDHeaderSubTitle,
		DWORD dwSize)
	: CPropertyPage(nIDTemplate, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle, dwSize)
{
	m_psp.dwFlags |= PSP_HASHELP;
}
CDlgBasePropertyPage::CDlgBasePropertyPage(
		LPCTSTR lpszTemplateName,
		UINT nIDCaption,
		UINT nIDHeaderTitle,
		UINT nIDHeaderSubTitle,
		DWORD dwSize)
	: CPropertyPage(lpszTemplateName, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle, dwSize)
{
	m_psp.dwFlags |= PSP_HASHELP;
}
#endif

CDlgBasePropertyPage::~CDlgBasePropertyPage()
{
}

void CDlgBasePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBasePropertyPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgBasePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgBasePropertyPage)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBasePropertyPage message handlers

BOOL CDlgBasePropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgBasePropertyPage::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (!ShowContextHelp(pHelpInfo))
		OnHelp();
	return TRUE;
}
