/*
 * Copyright © 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgRunCRCD class
 * @author David Connet
 *
 * Revision History
 * @li 2004-04-27 DRC Added some error recovery.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunCRCD.h"

#include "ARBDogRun.h"
#include "Base64.h"
#include "DlgCRCDViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD dialog

CDlgRunCRCD::CDlgRunCRCD(ARBDogRun* pRun)
	: CPropertyPage(CDlgRunCRCD::IDD)
	, m_Run(pRun)
	, m_metaFile(NULL)
	, m_ViewText(false)
	, m_Insert(true)
{
	//{{AFX_DATA_INIT(CDlgRunCRCD)
	//}}AFX_DATA_INIT
}

CDlgRunCRCD::~CDlgRunCRCD()
{
	if (m_metaFile)
	{
		DeleteEnhMetaFile(m_metaFile);
		m_metaFile = NULL;
	}
}

void CDlgRunCRCD::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunCRCD)
	DDX_Control(pDX, IDC_RUNCRCD_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_RUNCRCD_VIEW, m_ctrlView);
	DDX_Control(pDX, IDC_RUNCRCD_COPY, m_ctrlInsert);
	DDX_Control(pDX, IDC_RUNCRCD_DESC, m_ctrlText);
	DDX_Control(pDX, IDC_RUNCRCD_CRCD, m_ctrlCRCD);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRunCRCD, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgRunCRCD)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RUNCRCD_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_RUNCRCD_VIEW, OnView)
	ON_BN_CLICKED(IDC_RUNCRCD_COPY, OnCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgRunCRCD::AdjustCRCD()
{
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		CRect rBounds(header.rclBounds.left,
			header.rclBounds.top,
			header.rclBounds.right,
			header.rclBounds.bottom);
		CRect r(m_rCRCDclient);
		double bounds1 = static_cast<double>(rBounds.Width()) / rBounds.Height();
		double crcd1 = static_cast<double>(m_rCRCDclient.Width()) / m_rCRCDclient.Height();
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
	}
}

void CDlgRunCRCD::SetView()
{
	if (m_ViewText)
	{
		m_ctrlView.SetWindowText("View Image");
		m_ctrlText.ShowWindow(SW_SHOW);
		m_ctrlCRCD.ShowWindow(SW_HIDE);
	}
	else
	{
		m_ctrlView.SetWindowText("View Text");
		m_ctrlText.ShowWindow(SW_HIDE);
		m_ctrlCRCD.ShowWindow(SW_SHOW);
	}
	if (m_Insert)
		m_ctrlInsert.SetWindowText("Insert Course");
	else
		m_ctrlInsert.SetWindowText("Clear Course");
	if (m_metaFile)
	{
		m_ctrlEdit.EnableWindow(TRUE);
		m_ctrlView.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlView.EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD message handlers

BOOL CDlgRunCRCD::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_ctrlCRCD.GetWindowRect(m_rCRCDwin);
	m_ctrlCRCD.GetClientRect(m_rCRCDclient);
	m_ctrlCRCD.MapWindowPoints(this, m_rCRCDclient);
	CString str = m_Run->GetCRCD().c_str();
	str.Replace("\n", "\r\n");
	m_ctrlText.SetWindowText(str);
	if (0 < str.GetLength())
		m_Insert = false;
	else
		m_Insert = true;
	m_ViewText = true;
	if (0 < m_Run->GetCRCDMetaFile().length())
	{
		Base64 decode;
		char* pOutput;
		size_t len;
		if (decode.Decode(m_Run->GetCRCDMetaFile(), pOutput, len))
		{
			m_metaFile = SetEnhMetaFileBits(static_cast<UINT>(len), reinterpret_cast<LPBYTE>(pOutput));
			if (m_metaFile)
			{
				m_ViewText = false;
				AdjustCRCD();
				m_ctrlCRCD.SetEnhMetaFile(m_metaFile);
			}
		}
	}
	SetView();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgRunCRCD::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_RUNCRCD_CRCD)
		hbr = GetSysColorBrush(COLOR_WINDOW);
	else
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CDlgRunCRCD::OnEdit()
{
	if (m_metaFile)
	{
		CDlgCRCDViewer viewer(m_metaFile, this);
		viewer.DoModal();
	}
}

void CDlgRunCRCD::OnView()
{
	m_ViewText = !m_ViewText;
	SetView();
}

void CDlgRunCRCD::OnCopy()
{
	if (m_Insert)
	{
		BOOL bText = ::IsClipboardFormatAvailable(CF_TEXT);
		BOOL bMeta = ::IsClipboardFormatAvailable(CF_ENHMETAFILE);
		if (bText || bMeta)
		{
			if (AfxGetMainWnd()->OpenClipboard())
			{
				m_ctrlText.SetWindowText("");
				if (m_metaFile)
				{
					DeleteEnhMetaFile(m_metaFile);
					m_metaFile = NULL;
				}
				if (bText)
				{
					m_ViewText = true;
					HANDLE hData = GetClipboardData(CF_TEXT);
					CString str((LPCTSTR)GlobalLock(hData));
					GlobalUnlock(hData);
					m_ctrlText.SetWindowText(str);
					str.TrimRight();
					str.TrimLeft();
					str.Replace("\r\n", "\n");
					m_Run->SetCRCD((LPCTSTR)str);
					if (0 < str.GetLength())
						m_Insert = false;
					// Only create the metafile if we pasted text. Otherwise
					// we could end up with a non-CRCD metafile. It's still
					// possible this may not be CRCD data - the user can
					// just clear it then.
					if (bMeta)
					{
						HENHMETAFILE hData = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
						m_metaFile = CopyEnhMetaFile(hData, NULL);
						ENHMETAHEADER header;
						GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
						UINT nSize = GetEnhMetaFileBits(m_metaFile, 0, NULL);
						LPBYTE bits = new BYTE[nSize+1];
						GetEnhMetaFileBits(m_metaFile, nSize, bits);
						Base64 encode;
						ASSERT(sizeof(BYTE) == sizeof(char));
						std::string moreBits = encode.Encode(reinterpret_cast<char const*>(bits), nSize);
						m_Run->SetCRCDMetaFile(moreBits);
						delete [] bits;
						AdjustCRCD();
						m_ctrlCRCD.SetEnhMetaFile(m_metaFile);
						if (m_metaFile)
						{
							m_ViewText = false;
							m_Insert = false;
						}
					}
				}
				CloseClipboard();
				SetView();
			}
		}
	}
	else
	{
		m_Insert = true;
		m_ViewText = true;
		m_ctrlText.SetWindowText("");
		if (m_metaFile)
		{
			DeleteEnhMetaFile(m_metaFile);
			m_metaFile = NULL;
		}
		m_Run->SetCRCD("");
		m_Run->SetCRCDMetaFile("");
		SetView();
	}
}
