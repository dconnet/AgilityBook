/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAboutDlg class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-05 DRC Moved versioninfo into new class, added new links/buttons.
 * @li 2004-03-02 DRC Added %VERSION% keyword in hotlinks.
 * @li 2003-09-17 DRC Added a 'check for updates' control.
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "AboutDlg.h"
#include "VersionNum.h"
#include ".\aboutdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_UPDATE, OnBnClickedUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT_TEXT, m_ctrlText);
	DDX_Control(pDX, IDC_ABOUT_LINK1, m_ctrlLink1);
	DDX_Control(pDX, IDC_ABOUT_LINK2, m_ctrlLink2);
	DDX_Control(pDX, IDC_ABOUT_LINK3, m_ctrlLink3);
	DDX_Control(pDX, IDC_ABOUT_LINK4, m_ctrlLink4);
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Generate the about text.
	CString version;
	CString info(_T("?"));
	CVersionNum ver;
	if (ver.Valid())
	{
		version = ver.GetVersionString();
		SetWindowText(ver.GetName());
		info.FormatMessage(IDS_ABOUT_TEXT, (LPCTSTR)version, (LPCTSTR)ver.GetName());
	}
	m_ctrlText.SetWindowText(info);

	struct
	{
		UINT id;			// Id of stringtable entry for url text.
		CHyperLink* pCtrl;	// Control
		bool bMove;			// Does this move on resize?
	} idLinkControls[] = {
		{IDS_ABOUT_LINK_YAHOO, &m_ctrlLink1, true},
		{IDS_ABOUT_LINK_EMAILHELP, &m_ctrlLink2, true},
		{IDS_ABOUT_LINK_ARB, &m_ctrlLink3, true},
		{IDS_ABOUT_LINK_SOURCEFORGE, &m_ctrlLink4, true}
	};
	int nLinkControls = sizeof(idLinkControls) / sizeof(idLinkControls[0]);
	struct
	{
		UINT id; // Dlgid of other controls to be moved.
	} idControls[] = {
		{IDC_ABOUT_TEXT2},
		{IDC_UPDATE}
	};
	int nControls = sizeof(idControls) / sizeof(idControls[0]);

	for (int i = 0; i < nLinkControls; ++i)
	{
		CString name;
		name.LoadString(idLinkControls[i].id);
		CString url;
		int nTab = name.Find('\t');
		if (0 < nTab)
		{
			url = name.Mid(nTab+1);
			name = name.Left(nTab);
		}
		else
			url = name;
		idLinkControls[i].pCtrl->SetWindowText(name);
		if (0 <= (nTab = url.Find("%VERSION%")))
		{
			CString tmp = url.Left(nTab) + version + url.Mid(nTab+9);
			url = tmp;
		}
		idLinkControls[i].pCtrl->SetURL(url);
	}

	// Re-size the text control and dialog as needed.

	// Get the current size of the text control. We get both the
	// window and client rects in order to determine the non-client
	// area. After determining the required space for the text, we
	// add the NC-area back in - otherwise the control will be sized
	// slightly too small.
	CSize szBorders(0, 0);
	CRect rect;
	m_ctrlText.GetClientRect(rect);
	CRect rWin;
	m_ctrlText.GetWindowRect(rWin);
	if (rWin.Height() != rect.Height() || rWin.Width() != rect.Width())
	{
		szBorders.cx = (rWin.Width() - rect.Width()) / 2;
		szBorders.cy = (rWin.Height() - rect.Height()) / 2;
	}
	CRect rectNeed(rect);
	// Now determine how much space this text needs.
	CFont* pFont = m_ctrlText.GetFont();
	CWindowDC dc(this);
	CFont* pOldFont = dc.SelectObject(pFont);
	DWORD dwFlags = DT_CALCRECT | DT_WORDBREAK;
	DWORD dwStyle = m_ctrlText.GetStyle();
	if (SS_CENTER & dwStyle)
		dwFlags |= DT_CENTER;
	else if (SS_RIGHT & dwStyle)
		dwFlags |= DT_RIGHT;
	if (SS_NOPREFIX & dwStyle)
		dwFlags |= DT_NOPREFIX;
	dc.DrawText(info, rectNeed, dwFlags);
	dc.SelectObject(pOldFont);
	rectNeed.InflateRect(szBorders);
	// The 5 just allows a little padding to the right of the text.
	// It looks too scrunched otherwise.
	rectNeed.right += 5;
	// Adjust the height.
	int offsetY = (rectNeed.Height() - rect.Height());
	if (0 > offsetY)
		offsetY = 0;
	rect.bottom += offsetY;
	// Should we shrink the width of the dialog?
	// Note, we only shrink the control.
	int offsetX = (rect.Width() - rectNeed.Width());
	if (0 < offsetX)
		rect.right -= offsetX;
	else
		offsetX = 0;
	if (0 != offsetX || 0 != offsetY)
	{
		m_ctrlText.MapWindowPoints(this, rect);
		m_ctrlText.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
		// Move the okay button.
		if (0 < offsetX)
		{
			CWnd* pCtrl = GetDlgItem(IDOK);
			pCtrl->GetClientRect(rect);
			rect.OffsetRect(-offsetX, 0);
			pCtrl->MapWindowPoints(this, rect);
			pCtrl->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		// Move some other controls
		if (0 < offsetY)
		{
			int i;
			for (i = 0; i < nLinkControls; ++i)
			{
				if (idLinkControls[i].bMove)
				{
					idLinkControls[i].pCtrl->GetClientRect(rect);
					rect.OffsetRect(0, offsetY);
					idLinkControls[i].pCtrl->MapWindowPoints(this, rect);
					idLinkControls[i].pCtrl->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
			for (i = 0; i < nControls; ++i)
			{
				CWnd* pCtrl = GetDlgItem(idControls[i].id);
				pCtrl->GetClientRect(rect);
				rect.OffsetRect(0, offsetY);
				pCtrl->MapWindowPoints(this, rect);
				pCtrl->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		// And finally resize the dialog.
		GetWindowRect(rect);
		rect.right -= offsetX;
		rect.bottom += offsetY;
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	GetDlgItem(IDOK)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnBnClickedUpdate()
{
	UpdateVersion(true);
}
