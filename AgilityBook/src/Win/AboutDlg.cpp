/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @li 2003-09-17 DRC Added a 'check for updates' control.
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static bool GetVersionInfo(CString& name, CString& version, WORD inwLangID = 0, WORD inwCharSet = 0)
{
	name.Empty();
	version.Empty();

	// Get the filename and other info...
	CString appFName;
	LPTSTR tp = appFName.GetBuffer(MAX_PATH);
	::GetModuleFileName(NULL, tp, MAX_PATH);
	appFName.ReleaseBuffer();
#ifndef _UNICODE
	appFName.OemToAnsi();
#endif

	// Get the version information size for allocate the buffer
	LPTSTR pAppName = appFName.GetBuffer(0);
	DWORD fvHandle;
	DWORD dwSize = ::GetFileVersionInfoSize(pAppName, &fvHandle);
	if (0 == dwSize)
	{
		appFName.ReleaseBuffer();
		return false;
	}

	// Allocate buffer and retrieve version information
	BYTE* pFVData = new BYTE[dwSize];
	if (!::GetFileVersionInfo(pAppName, fvHandle, dwSize, pFVData))
	{
		appFName.ReleaseBuffer();
		return false;
	}
	appFName.ReleaseBuffer();
	// Retrieve the language and character-set identifier
	UINT nQuerySize;
	DWORD* pTransBlock;
	if (!::VerQueryValue(pFVData, _T("\\VarFileInfo\\Translation"), reinterpret_cast<void**>(&pTransBlock), &nQuerySize))
	{
		delete [] pFVData;
		pFVData = NULL;
		return false;
	}
	DWORD dwData = 0;
	if (nQuerySize > 4 && (0 != inwLangID || 0 != inwCharSet))
	{
		bool bOk = false;
		for (DWORD offset = 0; !bOk && offset < nQuerySize; offset += sizeof(DWORD))
		{
			dwData = pTransBlock[0];
			if (inwLangID == LOWORD(dwData) && inwCharSet == HIWORD(dwData))
				bOk = true;
		}
		if (!bOk)
			dwData = pTransBlock[0];
	}
	else
		dwData = pTransBlock[0];
	// Swap the words so wsprintf will print the lang-codepage in
	// the correct format.
	//  Hi-order: IBM code page
	//  Lo-order: Microsoft language id
	WORD wLangID = LOWORD(dwData);
	WORD wCodePage = HIWORD(dwData);

	// Cache the fixed info.
	VS_FIXEDFILEINFO* pffi;
	if (!::VerQueryValue(pFVData, _T("\\"), reinterpret_cast<void**>(&pffi), &nQuerySize))
	{
		delete [] pFVData;
		pFVData = NULL;
		return false;
	}
	// Date didn't get set. Get the create date.
	if (0 == pffi->dwFileDateLS && 0 == pffi->dwFileDateMS)
	{
		HANDLE hFile = CreateFile(appFName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile)
		{
			FILETIME ft;
			GetFileTime(hFile, &ft, NULL, NULL);
			CloseHandle(hFile);
			pffi->dwFileDateLS = ft.dwLowDateTime;
			pffi->dwFileDateMS = ft.dwHighDateTime;
		}
	}

	// Get the real name.
	TCHAR subBlockName[255];
	wsprintf(subBlockName, _T("\\StringFileInfo\\%04hx%04hx\\ProductName"), wLangID, wCodePage);
	void* pValue = NULL;
	UINT vSize;
	::VerQueryValue(pFVData, subBlockName, &pValue, &vSize);
	name = reinterpret_cast<LPCTSTR>(pValue);

	version.Format(_T("%hd.%hd.%hd.%hd"),
		HIWORD(pffi->dwProductVersionMS),
		LOWORD(pffi->dwProductVersionMS),
		HIWORD(pffi->dwProductVersionLS),
		LOWORD(pffi->dwProductVersionLS));
	delete [] pFVData;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
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
	DDX_Control(pDX, IDC_ABOUT_UPDATE, m_ctrlUpdate);
	DDX_Control(pDX, IDC_ABOUT_TEXT, m_ctrlText);
	DDX_Control(pDX, IDC_ABOUT_LINK1, m_ctrlLink1);
	DDX_Control(pDX, IDC_ABOUT_LINK2, m_ctrlLink2);
	DDX_Control(pDX, IDC_ABOUT_LINK3, m_ctrlLink3);
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Generate the about text.
	CString name, version;
	CString info(_T("?"));
	if (GetVersionInfo(name, version))
	{
		SetWindowText(name);
		info.FormatMessage(IDS_ABOUT_TEXT, (LPCTSTR)version, (LPCTSTR)name);
	}
	m_ctrlText.SetWindowText(info);

	struct
	{
		UINT id;
		CHyperLink* pCtrl;
		bool bMove;
	} idControls[] = {
		{IDS_ABOUT_UPDATE, &m_ctrlUpdate, false},
		{IDS_ABOUT_LINK1, &m_ctrlLink1, true},
		{IDS_ABOUT_LINK2, &m_ctrlLink2, true},
		{IDS_ABOUT_LINK3, &m_ctrlLink3, true}
	};
	int nControls = sizeof(idControls) / sizeof(idControls[0]);
	for (int i = 0; i < nControls; ++i)
	{
		name.LoadString(idControls[i].id);
		CString url;
		int nTab = name.Find('\t');
		if (0 < nTab)
		{
			url = name.Mid(nTab+1);
			name = name.Left(nTab);
		}
		else
			url = name;
		idControls[i].pCtrl->SetWindowText(name);
		idControls[i].pCtrl->SetURL(url);
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
			for (int i = 0; i < nControls; ++i)
			{
				if (idControls[i].bMove)
				{
					idControls[i].pCtrl->GetClientRect(rect);
					rect.OffsetRect(0, offsetY);
					idControls[i].pCtrl->MapWindowPoints(this, rect);
					idControls[i].pCtrl->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
		// And finally resize the dialog.
		GetWindowRect(rect);
		rect.right -= offsetX;
		rect.bottom += offsetY;
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
