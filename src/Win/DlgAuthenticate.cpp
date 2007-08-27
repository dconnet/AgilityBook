/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief Web authentication
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-03 DRC Created
 */

#include "stdafx.h"
#include "resource.h"
#include "DlgAuthenticate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAuthenticate dialog

IMPLEMENT_DYNAMIC(CDlgAuthenticate, CDialog)

CString CDlgAuthenticate::m_Password; // So we remember the password for the life of the program


CDlgAuthenticate::CDlgAuthenticate(CString& userName, CWnd* pParent)
	: CDialog(CDlgAuthenticate::IDD, pParent)
	, m_userName(userName)
{
	//{{AFX_DATA_INIT(CDlgAuthenticate)
	//}}AFX_DATA_INIT
}


void CDlgAuthenticate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAuthenticate)
	DDX_Text(pDX, IDC_AUTH_USERNAME, m_Name);
	DDX_Text(pDX, IDC_AUTH_PASSWORD, m_Password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAuthenticate, CDialog)
	//{{AFX_MSG_MAP(CDlgAuthenticate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgAuthenticate::OnOK()
{
	CDialog::OnOK();
	m_userName = m_Name;
}
