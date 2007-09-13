// ARBHelpDlg.cpp : implementation file
//

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
	, m_pagePage1(this, decode)
	, m_pagePage2(this)
	, m_pagePage3(this)
	, m_pageEdit()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetWizardMode();

	AddPage(&m_pagePage1);
	if (m_Decode)
	{
		AddPage(&m_pageEdit);
	}
	else
	{
		AddPage(&m_pagePage2);
		AddPage(&m_pagePage3);
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
// DlgHelpPageEdit (decoder)
void CDlgARBHelp::SendIt()
{
	// Scoping done to minimize memory usage.
	ARBString arbData;
	{
		ARBostringstream clp;

		{
			ARBString data;
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
#if _MSC_VER >= 1400
				FILE* file;
				fopen_s(&file, (LPCTSTR)(*iFile).first, "rb");
#else
				FILE* file = fopen((LPCTSTR)(*iFile).first, "rb");
#endif
				if (file)
				{
					ARBString data;
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
		ARBString data;
		BinaryData::EncodeString(arbData, data);
		ARBostringstream clp;
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
			SetClipboardData(CF_TEXT, temp);
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
