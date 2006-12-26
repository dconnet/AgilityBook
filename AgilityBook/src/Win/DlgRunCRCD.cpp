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
 * @brief implementation of the CDlgRunCRCD class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-05-04 DRC Added ability to suppress metafile.
 * @li 2004-04-27 DRC Added some error recovery.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunCRCD.h"

#include "AgilityBookOptions.h"
#include "ARBDogRun.h"
#include "Base64.h"
#include "ClipBoard.h"
#include "DlgCRCDViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD dialog

CDlgRunCRCD::CDlgRunCRCD(ARBDogRunPtr pRun)
	: CDlgBasePropertyPage(CDlgRunCRCD::IDD)
	, m_Run(pRun)
	, m_metaFile(NULL)
	, m_metaFileBack(NULL)
	, m_ViewText(false)
	, m_Insert(true)
{
	//{{AFX_DATA_INIT(CDlgRunCRCD)
	//}}AFX_DATA_INIT
}

CDlgRunCRCD::~CDlgRunCRCD()
{
	DeleteMetaFile();
}

void CDlgRunCRCD::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunCRCD)
	DDX_Control(pDX, IDC_RUNCRCD_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_RUNCRCD_VIEW, m_ctrlView);
	DDX_Control(pDX, IDC_RUNCRCD_COPY, m_ctrlInsert);
	DDX_Control(pDX, IDC_RUNCRCD_IMAGE, m_ctrlImage);
	DDX_Control(pDX, IDC_RUNCRCD_DESC, m_ctrlText);
	DDX_Control(pDX, IDC_RUNCRCD_CRCD, m_ctrlCRCD);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRunCRCD, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgRunCRCD)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RUNCRCD_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_RUNCRCD_VIEW, OnView)
	ON_BN_CLICKED(IDC_RUNCRCD_COPY, OnCopy)
	ON_BN_CLICKED(IDC_RUNCRCD_IMAGE, OnImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgRunCRCD::DeleteMetaFile()
{
	if (!m_metaFile && m_metaFileBack)
		m_metaFile = m_metaFileBack;
	if (m_metaFile)
	{
		DeleteEnhMetaFile(m_metaFile);
		m_metaFile = NULL;
		m_metaFileBack = NULL;
	}
}

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
	CString str;
	if (m_ViewText)
	{
		str.LoadString(IDS_CRCD_BTN_VIEWIMAGE);
		m_ctrlView.SetWindowText(str);
		m_ctrlText.ShowWindow(SW_SHOW);
		m_ctrlCRCD.ShowWindow(SW_HIDE);
	}
	else
	{
		str.LoadString(IDS_CRCD_BTN_VIEWTEXT);
		m_ctrlView.SetWindowText(str);
		m_ctrlText.ShowWindow(SW_HIDE);
		m_ctrlCRCD.ShowWindow(SW_SHOW);
	}
	if (m_Insert)
	{
		str.LoadString(IDS_CRCD_BTN_INSERTCOURSE);
		m_ctrlInsert.SetWindowText(str);
	}
	else
	{
		str.LoadString(IDS_CRCD_BTN_CLEARCOURSE);
		m_ctrlInsert.SetWindowText(str);
	}
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

void CDlgRunCRCD::SetCRCDData()
{
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		UINT nSize = GetEnhMetaFileBits(m_metaFile, 0, NULL);
		LPBYTE bits = new BYTE[nSize+1];
		GetEnhMetaFileBits(m_metaFile, nSize, bits);
		Base64 encode;
		ASSERT(sizeof(BYTE) == sizeof(char));
#ifdef UNICODE
		CString tmp(encode.Encode(reinterpret_cast<char const*>(bits), nSize).c_str());
		ARBString moreBits = tmp;
#else
		ARBString moreBits = encode.Encode(reinterpret_cast<char const*>(bits), nSize);
#endif
		m_Run->SetCRCDMetaFile(moreBits);
		delete [] bits;
	}
	else
		m_Run->SetCRCDMetaFile(_T(""));
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunCRCD message handlers

BOOL CDlgRunCRCD::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlCRCD.GetWindowRect(m_rCRCDwin);
	m_ctrlCRCD.GetClientRect(m_rCRCDclient);
	m_ctrlCRCD.MapWindowPoints(this, m_rCRCDclient);
	CString str = m_Run->GetCRCD().c_str();
	str.Replace(_T("\n"), _T("\r\n"));
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
#ifdef UNICODE
		std::string data = CStringA(m_Run->GetCRCDMetaFile().c_str());
		if (decode.Decode(data, pOutput, len))
#else
		if (decode.Decode(m_Run->GetCRCDMetaFile(), pOutput, len))
#endif
		{
			m_metaFile = SetEnhMetaFileBits(static_cast<UINT>(len), reinterpret_cast<LPBYTE>(pOutput));
			m_metaFileBack = m_metaFile;
			if (m_metaFile)
			{
				m_ViewText = false;
				AdjustCRCD();
				m_ctrlCRCD.SetEnhMetaFile(m_metaFile);
			}
		}
	}
	int nSetCheck = CAgilityBookOptions::GetIncludeCRCDImage() ? 1 : 0;
	if (m_metaFile)
		nSetCheck = 1;
	m_ctrlImage.SetCheck(nSetCheck);
	SetView();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgRunCRCD::OnCtlColor(
		CDC* pDC,
		CWnd* pWnd,
		UINT nCtlColor)
{
	HBRUSH hbr = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_RUNCRCD_CRCD)
		hbr = GetSysColorBrush(COLOR_WINDOW);
	else
		hbr = CDlgBasePropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
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
		if (!m_ctrlImage.GetCheck())
			bMeta = FALSE;
		if (bText || bMeta)
		{
			CClipboardDataReader clpData;
			if (clpData.Open())
			{
				m_ctrlText.SetWindowText(_T(""));
				DeleteMetaFile();
				if (bText)
				{
					m_ViewText = true;
					CString str;
					clpData.GetData(str);
					str.TrimRight();
					str.TrimLeft();
					// We do the replace since CRCD3 has "\n\nhdrs\r\netc"
					// First standardize to \n, store it, then replace the
					// other way for windows display.
					str.Replace(_T("\r\n"), _T("\n"));
					m_Run->SetCRCD((LPCTSTR)str);
					str.Replace(_T("\n"), _T("\r\n"));
					m_ctrlText.SetWindowText(str);
					if (0 < str.GetLength())
						m_Insert = false;
					// Only create the metafile if we pasted text. Otherwise
					// we could end up with a non-CRCD metafile. It's still
					// possible this may not be CRCD data - the user can
					// just clear it then.
					if (bMeta)
					{
						HENHMETAFILE hData = reinterpret_cast<HENHMETAFILE>(GetClipboardData(CF_ENHMETAFILE));
						m_metaFile = CopyEnhMetaFile(hData, NULL);
						m_metaFileBack = m_metaFile;
						SetCRCDData();
						AdjustCRCD();
						m_ctrlCRCD.SetEnhMetaFile(m_metaFile);
						if (m_metaFile)
						{
							m_ViewText = false;
							m_Insert = false;
						}
					}
				}
				SetView();
			}
		}
	}
	else
	{
		m_Insert = true;
		m_ViewText = true;
		m_ctrlText.SetWindowText(_T(""));
		DeleteMetaFile();
		m_Run->SetCRCD(_T(""));
		m_Run->SetCRCDMetaFile(_T(""));
		SetView();
	}
}

void CDlgRunCRCD::OnImage()
{
	CAgilityBookOptions::SetIncludeCRCDImage(0 != m_ctrlImage.GetCheck());
	if (m_ctrlImage.GetCheck())
	{
		if (!m_metaFile && m_metaFileBack)
			m_metaFile = m_metaFileBack;
	}
	else
	{
		if (m_metaFile)
		{
			m_metaFileBack = m_metaFile;
			m_metaFile = NULL;
		}
	}
	AdjustCRCD();
	m_ctrlCRCD.SetEnhMetaFile(m_metaFile);
	SetCRCDData();
	if (m_metaFile)
	{
		m_ViewText = false;
		m_Insert = false;
	}
	else
		m_ViewText = true;
	SetView();
}
