/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgCRCDViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-06 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgCRCDViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCRCDViewer dialog

CDlgCRCDViewer::CDlgCRCDViewer(
		HENHMETAFILE inMetaFile,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCRCDViewer::IDD, pParent)
	, m_metaFile(inMetaFile)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rCRCDwin(0,0,0,0)
	, m_rCRCDclient(0,0,0,0)
	, m_rOK(0,0,0,0)
{
	//{{AFX_DATA_INIT(CDlgCRCDViewer)
	//}}AFX_DATA_INIT
}

void CDlgCRCDViewer::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCRCDViewer)
	DDX_Control(pDX, IDC_VIEWER_CRCD, m_ctrlCRCD);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCRCDViewer, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgCRCDViewer)
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgCRCDViewer::AdjustCRCD()
{
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		CRect rBounds(header.rclBounds.left,
			header.rclBounds.top,
			header.rclBounds.right,
			header.rclBounds.bottom);
		CRect r;
		m_ctrlCRCD.GetClientRect(r);
		double bounds1 = static_cast<double>(rBounds.Width()) / rBounds.Height();
		double crcd1 = static_cast<double>(r.Width()) / r.Height();
		double bounds2 = static_cast<double>(rBounds.Height()) / rBounds.Width();
		if (bounds1 < crcd1)
		{
			r.right = r.left + static_cast<LONG>(r.Height() * bounds1);
		}
		else
		{
			r.bottom = r.top + static_cast<LONG>(r.Width() * bounds2);
		}
		int x = (m_rCRCDwin.Width() - m_rCRCDclient.Width()) / 2;
		int y = (m_rCRCDwin.Height() - m_rCRCDclient.Height()) / 2;
		m_ctrlCRCD.SetWindowPos(NULL, 0, 0, r.Width()+x, r.Height()+y, SWP_NOZORDER | SWP_NOMOVE);
		m_ctrlCRCD.Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCRCDViewer message handlers

BOOL CDlgCRCDViewer::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);	// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);	// Set small icon

	GetWindowRect(m_rWin);
	GetClientRect(m_rDlg);
	m_ctrlCRCD.GetWindowRect(m_rCRCDwin);
	ScreenToClient(m_rCRCDwin);
	m_ctrlCRCD.GetClientRect(m_rCRCDclient);
	m_ctrlCRCD.MapWindowPoints(this, m_rCRCDclient);
	m_ctrlOK.GetWindowRect(m_rOK);
	ScreenToClient(m_rOK);

	AdjustCRCD();
	m_ctrlCRCD.SetEnhMetaFile(m_metaFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgCRCDViewer::OnCtlColor(
		CDC* pDC,
		CWnd* pWnd,
		UINT nCtlColor)
{
	HBRUSH hbr = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_VIEWER_CRCD)
		hbr = GetSysColorBrush(COLOR_WINDOW);
	else
		hbr = CDlgBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CDlgCRCDViewer::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rWin.Width();
	lpMMI->ptMinTrackSize.y = m_rWin.Height();
	CDlgBaseDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgCRCDViewer::OnSize(
		UINT nType,
		int cx,
		int cy)
{
	CDlgBaseDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ctrlCRCD.GetSafeHwnd()))
	{
		m_ctrlOK.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rOK.left), m_rOK.top,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlCRCD.SetWindowPos(NULL,
			0, 0,
			cx - (m_rDlg.Width() - m_rCRCDwin.Width()), cy - (m_rDlg.Height() - m_rCRCDwin.Height()),
			SWP_NOZORDER | SWP_NOMOVE);
		AdjustCRCD();
	}
}
