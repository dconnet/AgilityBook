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
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include <sstream>
#include "ARBHelp.h"
#include "BinaryData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static LONG GetRegKey(
		HKEY key,
		LPCTSTR subkey,
		LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		lstrcpy(retdata,data);
		RegCloseKey(hkey);
	}
	return retval;
}

static bool GotoURL(CString const& url)
{
	CWaitCursor wait;

	// First try ShellExecute()
	// Use INT_PTR instead of 'int' for compatibility with VC7.
	// Removes the '4311' warning message.
	INT_PTR result = reinterpret_cast<INT_PTR>(ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW));

	// If it failed, get the .htm regkey and lookup the program
	if (result <= HINSTANCE_ERROR)
	{
		TCHAR key[MAX_PATH + MAX_PATH];
		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
		{
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
			{
				TCHAR* pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL)
				{	// No quotes found
					pos = _tcsstr(key, _T("%1"));	// Check for %1, without quotes
					if (pos == NULL)				// No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';				// Remove the parameter
				}
				else
					*pos = '\0';					// Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);

				USES_CONVERSION;
				result = WinExec(T2A(key), SW_SHOW);
			}
		}
	}

	if (result <= HINSTANCE_ERROR)
	{
		CString str;
		str.Format(_T("Unable to start mail program (err=%x)"), result);
		AfxMessageBox(str, MB_ICONSTOP | MB_OK);
		return false;
	}
	else
		return true;
}

IMPLEMENT_DYNAMIC(CDlgARBHelp, CPropertySheet)

#pragma warning ( push )
#pragma warning ( disable : 4355 )
CDlgARBHelp::CDlgARBHelp(bool decode)
	: CPropertySheet(_T(""), NULL, 0)
	, m_Decode(decode)
	, m_pageEncode(this)
	, m_pageFiles(this)
	, m_pageEncodeFinish(this)
	, m_pageDecode()
	, m_pageDecodeFinish()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetWizardMode();

	if (m_Decode)
	{
		AddPage(&m_pageDecode);
		AddPage(&m_pageDecodeFinish);
	}
	else
	{
		AddPage(&m_pageEncode);
		AddPage(&m_pageFiles);
		AddPage(&m_pageEncodeFinish);
	}
}
#pragma warning ( pop )

BEGIN_MESSAGE_MAP(CDlgARBHelp, CPropertySheet)
	//{{AFX_MSG_MAP(CDlgARBHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgARBHelp::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CDlgARBHelp::AddSysInfo(tstring const& inData)
{
	m_SysInfo.clear();
	if (!inData.empty())
		m_SysInfo = inData;
}


void CDlgARBHelp::AddRegistryInfo(LPCTSTR inData)
{
	m_RegInfo.clear();
	if (inData)
		m_RegInfo = inData;
}

void CDlgARBHelp::SetARBFileStatus(CString const& inFileName, bool bInclude)
{
	m_IncFile[inFileName] = bInclude;
}

// Make sure this is synchronized (order of encoding) with
// DlgPageDecode (decoder)
void CDlgARBHelp::SendIt()
{
	// Scoping done to minimize memory usage.
	tstring arbData;
	{
		otstringstream clp;

		{
			tstring data;
			BinaryData::EncodeString(m_SysInfo, data);
			clp << _T("\r\n") << STREAM_SYSTEM_BEGIN << _T("\r\n")
				<< data
				<< _T("\r\n") << STREAM_SYSTEM_END << _T("\r\n");
		}

		{
			tstring data;
			BinaryData::EncodeString(m_RegInfo, data);
			clp << _T("\r\n") << STREAM_REGISTRY_BEGIN << _T("\r\n")
				<< data
				<< _T("\r\n") << STREAM_REGISTRY_END << _T("\r\n");
		}

		for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
		{
			clp << _T("\r\n") << (LPCTSTR)(*iFile).first;
			if ((*iFile).second)
			{
				CStringA filename((*iFile).first);
#if _MSC_VER >= 1400
				FILE* file;
				fopen_s(&file, (LPCSTR)filename, "rb");
#else
				FILE* file = fopen((LPCSTR)filename, "rb");
#endif
				if (file)
				{
					tstring data;
					BinaryData::Encode(file, data);
					fclose(file);
					clp << _T("\r\n") << STREAM_FILE_BEGIN << _T("\r\n")
						<< data
						<< _T("\r\n") << STREAM_FILE_END << _T("\r\n");
				}
				else
					clp << _T(": Error: Cannot read file\r\n");
			}
			else
			{
				clp << _T(": Skipped\r\n");
			}
		}

		arbData = clp.str();
	}
	CString clpData;
	{
		tstring data;
		BinaryData::EncodeString(arbData, data);
		otstringstream clp;
		clp << _T("\r\n") << STREAM_DATA_BEGIN << _T("\r\n")
			<< data
			<< _T("\r\n") << STREAM_DATA_END << _T("\r\n");
		clpData = clp.str().c_str();
	}

	if (AfxGetMainWnd()->OpenClipboard())
	{
		EmptyClipboard();
		HGLOBAL temp = GlobalAlloc(GHND, sizeof(TCHAR)*(clpData.GetLength()+1));
		if (NULL != temp)
		{
			void* pData = GlobalLock(temp);
			memcpy(pData, (LPCTSTR)clpData, sizeof(TCHAR)*(clpData.GetLength()+1));
			GlobalUnlock(temp);
			// send data to clipbard
#ifdef UNICODE
			SetClipboardData(CF_UNICODETEXT, temp);
#else
			SetClipboardData(CF_TEXT, temp);
#endif
		}
		CloseClipboard();

		CString str;
		str.LoadString(IDS_MAIL_ADDRESS);
		GotoURL(str);
	}
	else
	{
		AfxMessageBox(IDS_CLIPBOARD_FAILED, MB_ICONSTOP);
	}
}
