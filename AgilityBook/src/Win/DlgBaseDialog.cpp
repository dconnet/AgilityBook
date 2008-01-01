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
 * @brief Base class for all dialogs.
 * @author David Connet
 *
 * Revision History
 * @li 2004-05-31 DRC Created.
 */

#include "stdafx.h"
#include <afxpriv.h>
#include "AgilityBook.h"
#include "DlgBaseDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBaseDialog dialog

IMPLEMENT_DYNAMIC(CDlgBaseDialog, CDialog)

CDlgBaseDialog::CDlgBaseDialog(
		LPCTSTR lpszTemplateName,
		CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd)
{
	//{{AFX_DATA_INIT(CDlgBaseDialog)
	//}}AFX_DATA_INIT
}


CDlgBaseDialog::CDlgBaseDialog(
		UINT nIDTemplate,
		CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
}


void CDlgBaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBaseDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBaseDialog, CDialog)
	//{{AFX_MSG_MAP(CDlgBaseDialog)
	ON_WM_HELPINFO()
	ON_COMMAND(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBaseDialog message handlers

BOOL CDlgBaseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgBaseDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (!ShowContextHelp(theApp.LanguageManager(), pHelpInfo))
		OnHelp();
	return TRUE;
}


void CDlgBaseDialog::OnHelp()
{
	SendMessage(WM_COMMANDHELP);
}
