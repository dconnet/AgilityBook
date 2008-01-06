/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief Read clipboard and decode data.
 * @author David Connet
 *
 * Revision History
 * @li 2007-02-08 DRC Created
 */

#include "stdafx.h"
#include "DlgPageDecodeFinish.h"

#include <fstream>
#include <ios>
#include "ARBHelp.h"
#include "ARBTypes.h"
#include "BinaryData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPageDecodeFinish property page

IMPLEMENT_DYNAMIC(CDlgPageDecodeFinish, CPropertyPage)

CDlgPageDecodeFinish::CDlgPageDecodeFinish()
	: CPropertyPage(CDlgPageDecodeFinish::IDD)
{
	//{{AFX_DATA_INIT(CDlgPageDecodeFinish)
	//}}AFX_DATA_INIT
}

CDlgPageDecodeFinish::~CDlgPageDecodeFinish()
{
}

void CDlgPageDecodeFinish::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPageDecodeFinish)
	DDX_Control(pDX, IDC_EDIT, m_ctrlEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgPageDecodeFinish, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgPageDecodeFinish)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPageDecodeFinish message handlers

BOOL CDlgPageDecodeFinish::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWaitCursor wait;
	if (OpenClipboard())
	{
		CString data;
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			data = CStringA(reinterpret_cast<char const*>(GlobalLock(hData)));
			GlobalUnlock(hData);
		}
		CloseClipboard();

		// Make sure this is synchronized (order of decoding) with
		// DlgARBHelp (encoder)
		int pos = data.Find(STREAM_DATA_BEGIN);
		if (0 <= pos)
		{
			data = data.Mid(pos + lstrlen(STREAM_DATA_BEGIN));
			pos = data.Find(STREAM_DATA_END);
			if (0 <= pos)
				data = data.Left(pos);
			data.TrimLeft();
			data.TrimRight();

			tstring dataIn((LPCTSTR)data);
			data.Empty();
			tstring dataOut;
			BinaryData::DecodeString(dataIn, dataOut);
			dataIn.clear();
			data = dataOut.c_str();
			dataOut.clear();
			data.TrimLeft();
			data.TrimRight();

			otstringstream editData;
			editData << _T("Any temporary files created will be deleted upon closing this window.\r\n\r\n");

			pos = data.Find(STREAM_REGISTRY_BEGIN);
			if (0 <= pos)
			{
				int posEnd = data.Find(STREAM_REGISTRY_END);
				if (pos < posEnd)
				{
					int posData = pos + lstrlen(STREAM_REGISTRY_BEGIN);
					// Dump the preceding data.
					editData << (LPCTSTR)data.Left(posData) << _T("\r\n");
					// Trim preceding
					data = data.Mid(posData);
					data.TrimLeft();
					// Get the data to decode
					posEnd = data.Find(STREAM_REGISTRY_END); // Recompute - we just changed the string
					dataIn = (LPCTSTR)data.Left(posEnd);
					// Strip that from main data.
					data = data.Mid(posEnd + lstrlen(STREAM_REGISTRY_END));
					data.TrimLeft();
					// Now decode
					BinaryData::DecodeString(dataIn, dataOut);
					dataIn.clear();
					editData << dataOut << STREAM_REGISTRY_END << _T("\r\n\r\n");
					dataOut.clear();
				}
			}
			while ((0 <= (pos = data.Find(STREAM_FILE_BEGIN))))
			{
				int posEnd = data.Find(STREAM_FILE_END);
				if (0 < posEnd && posEnd > pos)
				{
					int posData = pos + lstrlen(STREAM_FILE_BEGIN);
					// Dump the preceding data (but not identifier.
					editData << (LPCTSTR)data.Left(pos); // New line included
					// Trim preceding
					data = data.Mid(posData);
					data.TrimLeft();
					// Get the data to decode
					posEnd = data.Find(STREAM_FILE_END); // Recompute - we just changed the string
					dataIn = (LPCTSTR)data.Left(posEnd);
					// Strip that from main data.
					data = data.Mid(posEnd + lstrlen(STREAM_FILE_END));
					data.TrimLeft();
					// Now decode
					BinaryData::DecodeString(dataIn, dataOut);
					dataIn.clear();
					// Generate a temp file name
					CString tempdir;
					LPTSTR pDir = tempdir.GetBuffer(MAX_PATH+1);
					GetTempPath(MAX_PATH, pDir);
					tempdir.ReleaseBuffer();
					CString tempname;
					LPTSTR pFile = tempname.GetBuffer(MAX_PATH);
					GetTempFileName(tempdir, _T("arb"), 0, pFile);
					tempname.ReleaseBuffer();
					m_TmpFiles.push_back(tempname);
					std::ofstream output((LPCTSTR)tempname, std::ios::out | std::ios::binary);
					output.exceptions(std::ios_base::badbit);
					if ((output.rdstate() & std::ios::failbit))
					{
						editData << STREAM_FILE_BEGIN << _T("\r\n")
							<< dataOut
							<< STREAM_FILE_END << _T("\r\n\r\n");
					}
					else
					{
						output << dataOut;
						output.close();
						editData << _T("File written to: ") << (LPCTSTR)tempname << _T("\r\n\r\n");
					}
					dataOut.clear();
				}
			}
			editData << (LPCTSTR)data;

			m_ctrlEdit.SetWindowText(editData.str().c_str());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgPageDecodeFinish::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_FINISH);
	return __super::OnSetActive();
}

static void ClearFile(CString const& inFile)
{
	DeleteFile(inFile);
}

void CDlgPageDecodeFinish::OnDestroy()
{
	CPropertyPage::OnDestroy();

	std::for_each(m_TmpFiles.begin(), m_TmpFiles.end(), ClearFile);
}
