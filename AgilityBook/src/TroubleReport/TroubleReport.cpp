/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief Generate a trouble report to be emailed.
 * @author David Connet
 *
 * Generate an email (via mailto:) with enough system information to attempt
 * to debug a problem.
 *
 * Revision History
 * @li 2004-10-22 DRC Created
 */

#include "stdafx.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CTroubleReportDlg dialog
class CTroubleReportDlg : public CDialog
{
// Construction
public:
	CTroubleReportDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TROUBLEREPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};

CTroubleReportDlg::CTroubleReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTroubleReportDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTroubleReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTroubleReportDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTroubleReportDlg message handlers

BOOL CTroubleReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE; // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CTroubleReportDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the
// user drags the minimized window.
HCURSOR CTroubleReportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////

class CTroubleCommands : public CCommandLineInfo
{
public:
	CTroubleCommands()
		: m_bFault(false)
	{}
	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
	bool m_bFault;
};

// /fault	Invoked from program fault
void CTroubleCommands::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
	if (bFlag)
	{
		CString param(lpszParam);
		if (0 == param.CompareNoCase(_T("fault")))
			m_bFault = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTroubleReportApp

class CTroubleReportApp : public CWinApp
{
public:
	CTroubleReportApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CTroubleReportApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CTroubleReportApp construction
CTroubleReportApp::CTroubleReportApp()
{
}

// The one and only CTroubleReportApp object
CTroubleReportApp theApp;

CString QueryKey(HKEY hParentKey, LPCTSTR inKeyName, int indent)
{
	CString retVal;
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(hParentKey, inKeyName, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hKey))
	{
		CString strIndent1("   ", indent);
		CString strIndent("   ", indent+1);
		DWORD numSubKeys = 0;
		DWORD maxSubKeyName = 0;
		DWORD numValues = 0;
		DWORD maxValueName = 0;
		DWORD maxValueData = 0;
		if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL,
			&numSubKeys, &maxSubKeyName,
			NULL,
			&numValues, &maxValueName, &maxValueData,
			NULL, NULL))
		{
			retVal += strIndent1;
			retVal += inKeyName;
			retVal += "\n";
			// Enumerate the subkeys, recursively.
			if (0 < numSubKeys)
			{
				TCHAR* nameSubKey = new TCHAR[maxSubKeyName+1];
				for (DWORD i = 0; i < numSubKeys; ++i)
				{
					DWORD nameSubKeyLen = maxSubKeyName + 1;
					if (ERROR_SUCCESS == RegEnumKeyEx(hKey, i,
						nameSubKey, &nameSubKeyLen, NULL, NULL, NULL, NULL))
					{
						retVal += QueryKey(hKey, nameSubKey, indent + 1);
					}
				}
				delete [] nameSubKey;
			}

			// Enumerate the key values.
			if (numValues)
			{
				TCHAR* valueName = new TCHAR[maxValueName + 1];
				TCHAR* valueData = new TCHAR[maxValueData + 2];
				for (DWORD i = 0; i < numValues; ++i)
				{
					valueName[0] = '\0';
					DWORD dwSize = maxValueName + 1;
					DWORD type;
					if (ERROR_SUCCESS == RegEnumValue(hKey, i,
						valueName,
						&dwSize,
						NULL,
						&type,
						NULL,
						NULL))
					{
						bool bOk = false;
						CString strType;
						switch (type)
						{
						default:
							strType = "??";
							break;
						case REG_BINARY:
							strType = "BINARY";
							break;
						//REG_DWORD_LITTLE_ENDIAN == REG_DWORD
						case REG_DWORD_BIG_ENDIAN:
							strType = "DWORD_BIG_ENDIAN";
							break;
						case REG_LINK:
							strType = "LINK";
							break;
						case REG_MULTI_SZ:
							strType = "MULTI_SZ";
							break;
						case REG_NONE:
							strType = "NONE";
							break;
						case REG_QWORD:
						//REG_QWORD_LITTLE_ENDIAN == REG_QWORD
							strType = "QWORD";
							break;
						case REG_DWORD:
							strType = "DWORD";
							{
								DWORD dwVal;
								dwSize = sizeof(dwVal);
								if (ERROR_SUCCESS == RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)&dwVal, &dwSize))
								{
									bOk = true;
									retVal += strIndent;
									retVal += valueName;
									CString tmp;
									tmp.Format("(%s): %ld\n",
										(LPCTSTR)strType,
										dwVal);
									retVal += tmp;
								}
							}
							break;
						case REG_EXPAND_SZ:
						case REG_SZ:
							if (REG_EXPAND_SZ == type)
								strType = "EXPAND_SZ";
							else
								strType = "SZ";
							dwSize = maxValueData + 2;
							if (ERROR_SUCCESS == RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)valueData, &dwSize))
							{
								bOk = true;
								retVal += strIndent;
								retVal += valueName;
								CString tmp;
								tmp.Format("(%s): ",
									(LPCTSTR)strType);
								retVal += tmp;
								retVal += valueData;
								retVal += "\n";
							}
							break;
						}
						if (!bOk)
						{
							retVal += strIndent;
							retVal += valueName;
							CString tmp;
							tmp.Format("(%s)\n",
								(LPCTSTR)strType);
							retVal += tmp;
						}
					}
				}
				delete [] valueName;
				delete [] valueData;
			}
		}
	}
	if (NULL != hKey)
		RegCloseKey(hKey);
	return retVal;
}

// CTroubleReportApp initialization
BOOL CTroubleReportApp::InitInstance()
{
	CWinApp::InitInstance();

	CTroubleCommands cmdInfo;
	ParseCommandLine(cmdInfo);

	HKEY hSoftKey = NULL;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software"), 0, KEY_READ, &hSoftKey))
	{
		CString regEntries = QueryKey(hSoftKey, _T("dcon Software"), 0);
		AfxMessageBox(regEntries);
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);

	CTroubleReportDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}
