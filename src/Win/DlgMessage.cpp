/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgMessage class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessage dialog

CDlgMessage::CDlgMessage(
		CString const& msg,
		UINT caption,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgMessage::IDD, pParent)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rMsg(0,0,0,0)
	, m_rClose(0,0,0,0)
{
	if (0 != caption)
		m_Caption.LoadString(caption);
	//{{AFX_DATA_INIT(CDlgMessage)
	m_Message = msg;
	//}}AFX_DATA_INIT
	m_Message.Replace(_T("\n"), _T("\r\n"));
}


void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessage)
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	DDX_Control(pDX, IDOK, m_ctrlClose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgMessage)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessage message handlers

BOOL CDlgMessage::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), TRUE);	// Set big icon
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);	// Set small icon

	if (!m_Caption.IsEmpty())
		SetWindowText(m_Caption);

	GetWindowRect(m_rWin);
	GetClientRect(m_rDlg);
	m_ctrlMessage.GetWindowRect(m_rMsg);
	ScreenToClient(m_rMsg);
	m_ctrlClose.GetWindowRect(m_rClose);
	ScreenToClient(m_rClose);

	m_ctrlClose.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgMessage::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rWin.Width();
	lpMMI->ptMinTrackSize.y = m_rWin.Height();
	CDlgBaseDialog::OnGetMinMaxInfo(lpMMI);
}


void CDlgMessage::OnSize(UINT nType, int cx, int cy)
{
	CDlgBaseDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ctrlMessage.GetSafeHwnd()))
	{
		m_ctrlClose.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rClose.left), cy - (m_rDlg.Height() - m_rClose.bottom) - m_rClose.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlMessage.SetWindowPos(NULL,
			0, 0,
			cx - (m_rDlg.Width() - m_rMsg.Width()), cy - (m_rDlg.Height() - m_rMsg.Height()),
			SWP_NOZORDER | SWP_NOMOVE);
	}
}
