/*
 * Copyright © 2004-2008 David Connet. All Rights Reserved.
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
 * @brief Base class for Property sheets
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-05 DRC Created
 */

#include "stdafx.h"
#include <afxpriv.h>
#include "AgilityBook.h"
#include "DlgBaseSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBaseSheet dialog

IMPLEMENT_DYNAMIC(CDlgBaseSheet, CPropertySheet)

CDlgBaseSheet::CDlgBaseSheet(
		UINT nIDCaption,
		CWnd* pParentWnd,
		UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_HASHELP;
}


CDlgBaseSheet::CDlgBaseSheet(
		LPCTSTR pszCaption,
		CWnd* pParentWnd,
		UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_HASHELP;
}


CDlgBaseSheet::~CDlgBaseSheet()
{
}


BEGIN_MESSAGE_MAP(CDlgBaseSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDlgBaseSheet)
	ON_WM_HELPINFO()
	ON_COMMAND(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBaseSheet message handlers

BOOL CDlgBaseSheet::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (!ShowContextHelp(theApp.LanguageManager(), pHelpInfo))
		OnHelp();
	return TRUE;
}


void CDlgBaseSheet::OnHelp()
{
	SendMessage(WM_COMMANDHELP);
}
