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
 * @brief implementation of the CDlgHelpPage1 class
 * @author David Connet
 *
 * Revision History
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageEncode.h"

#include <set>
#include <sstream>
#include "ARBHelp.h"
#include "ARBTypes.h"
#include "DlgARBHelp.h"
#include "..\Win\VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dump registry information

static void PrintIndent(int inIndent, otstringstream& outData)
{
	for (int indent = 0; indent < inIndent; ++indent)
		outData << ' ';
}

#define BUFF_SIZE 1024
static TCHAR g_szBuff[BUFF_SIZE];
static TCHAR g_szBuff2[BUFF_SIZE];

static void DumpRegistry(
	HKEY hKey,
	int inIndent,
	otstringstream& outData)
{
	if (!hKey)
		return;

	DWORD i;
	DWORD dwLen;
	// Enumerate the subkeys, recursively.
	for (i = 0; ; ++i)
	{
		dwLen = BUFF_SIZE;
		// A key name max size is 255.
		if (ERROR_SUCCESS != RegEnumKeyEx(hKey, i, g_szBuff, &dwLen, NULL, NULL, NULL, NULL))
			break;
		HKEY hSubKey;
		if (ERROR_SUCCESS == RegOpenKeyEx(hKey, g_szBuff, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hSubKey))
		{
			PrintIndent(inIndent, outData);
			outData << g_szBuff << _T("\r\n");
			DumpRegistry(hSubKey, inIndent + 1, outData);
			RegCloseKey(hSubKey);
		}
	}

	// Enumerate the key values.
	for (i = 0; ; ++i)
	{
		dwLen = BUFF_SIZE;
		DWORD type;
		if (ERROR_SUCCESS != RegEnumValue(hKey, i, g_szBuff, &dwLen, NULL, &type, NULL, NULL))
		{
			break;
		}
		bool bOk = false;
		TCHAR* pType = _T("??");
		switch (type)
		{
		default:
			break;
		case REG_BINARY:
			pType = _T("BINARY");
			break;
		//REG_DWORD_LITTLE_ENDIAN == REG_DWORD
		case REG_DWORD_BIG_ENDIAN:
			pType = _T("DWORD_BIG_ENDIAN");
			break;
		case REG_LINK:
			pType = _T("LINK");
			break;
		case REG_MULTI_SZ:
			pType = _T("MULTI_SZ");
			{
				dwLen = BUFF_SIZE;
				DWORD dwRet = RegQueryValueEx(hKey, g_szBuff, NULL, &type, reinterpret_cast<LPBYTE>(g_szBuff2), &dwLen);
				if (ERROR_SUCCESS == dwRet)
				{
					bOk = true;
					TCHAR* pBuffer = g_szBuff2;
					PrintIndent(inIndent, outData);
					outData << g_szBuff << '(' << pType << _T(")\r\n");
					while (pBuffer && *pBuffer)
					{
						PrintIndent(inIndent+1, outData);
						outData << pBuffer << _T("\r\n");
						pBuffer += lstrlen(pBuffer) + 1;
					}
				}
			}
			break;
		case REG_NONE:
			pType = _T("NONE");
			break;
#if _MSC_VER >= 1300
		case REG_QWORD:
		//REG_QWORD_LITTLE_ENDIAN == REG_QWORD
			pType = _T("QWORD");
			break;
#endif
		case REG_DWORD:
			pType = _T("DWORD");
			{
				DWORD dwVal;
				dwLen = sizeof(dwVal);
				if (ERROR_SUCCESS == RegQueryValueEx(hKey, g_szBuff, NULL, &type, (LPBYTE)&dwVal, &dwLen))
				{
					bOk = true;
					PrintIndent(inIndent, outData);
					outData << g_szBuff << '(' << pType << _T("): ") << dwVal << _T("\r\n");
				}
			}
			break;
		case REG_EXPAND_SZ:
		case REG_SZ:
			if (REG_EXPAND_SZ == type)
				pType = _T("EXPAND_SZ");
			else
				pType = _T("SZ");
			{
				dwLen = BUFF_SIZE;
				DWORD dwRet = RegQueryValueEx(hKey, g_szBuff, NULL, &type, reinterpret_cast<LPBYTE>(g_szBuff2), &dwLen);
				if (ERROR_SUCCESS == dwRet || ERROR_MORE_DATA == dwRet)
				{
					bOk = true;
					PrintIndent(inIndent, outData);
					outData << g_szBuff << '(' << pType << _T("): ") << g_szBuff2;
					if (ERROR_MORE_DATA == dwRet)
						outData << _T("...");
					outData << _T("\r\n");
				}
			}
			break;
		}
		if (!bOk)
		{
			PrintIndent(inIndent, outData);
			outData << g_szBuff << '(' << pType << _T(")\r\n");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Replacement for VC8 CFile::GetStatus
// The MFC version is not properly trapping a bad CTime value, which causes
// GetStatus to throw an exception.

// Defined in "Microsoft Visual Studio 8\VC\atlmfc\src\mfc\afximpl.h"
BOOL AFXAPI AfxFullPath(__out_z LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

BOOL GetLocalStatus(LPCSTR lpszFileName, CFileStatus& rStatus)
{
	ASSERT( lpszFileName != NULL );

	if ( lpszFileName == NULL )
	{
		return FALSE;
	}

	if ( lstrlen(lpszFileName) >= _MAX_PATH )
	{
		ASSERT(FALSE); // MFC requires paths with length < _MAX_PATH
		return FALSE;
	}

	// attempt to fully qualify path first
	if (!AfxFullPath(rStatus.m_szFullName, lpszFileName))
	{
		rStatus.m_szFullName[0] = '\0';
		return FALSE;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPSTR)lpszFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	VERIFY(FindClose(hFind));

	// strip attribute of NORMAL bit, our API doesn't have a "normal" bit.
	rStatus.m_attribute = (BYTE)
		(findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL);

	// get just the low DWORD of the file size
	ASSERT(findFileData.nFileSizeHigh == 0);
	rStatus.m_size = (LONG)findFileData.nFileSizeLow;

	// convert times as appropriate
	if (CTime::IsValidFILETIME(findFileData.ftCreationTime))
	{
		try
		{
			rStatus.m_ctime = CTime(findFileData.ftCreationTime);
		}
		catch (COleException* pe)
		{
			pe->Delete();
			rStatus.m_ctime = CTime();
		}
	}
	else
	{
		rStatus.m_ctime = CTime();
	}

	if (CTime::IsValidFILETIME(findFileData.ftLastAccessTime))
	{
		try
		{
			rStatus.m_atime = CTime(findFileData.ftLastAccessTime);
		}
		catch (COleException* pe)
		{
			pe->Delete();
			rStatus.m_atime = CTime();
		}
	}
	else
	{
		rStatus.m_atime = CTime();
	}

	if (CTime::IsValidFILETIME(findFileData.ftLastWriteTime))
	{
		try
		{
			rStatus.m_mtime = CTime(findFileData.ftLastWriteTime);
		}
		catch (COleException* pe)
		{
			pe->Delete();
			rStatus.m_mtime = CTime();
		}
	}
	else
	{
		rStatus.m_mtime = CTime();
	}

	if (rStatus.m_ctime.GetTime() == 0)
		rStatus.m_ctime = rStatus.m_mtime;

	if (rStatus.m_atime.GetTime() == 0)
		rStatus.m_atime = rStatus.m_mtime;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Process all files. Use a callback to allow custom processing.

class CProcessFile
{
public:
	CProcessFile(CStatic& inDirectory)
		: m_Directory(inDirectory)
	{
	}
	void UpdateDirectory(CString const& inDir)
	{
		m_Directory.SetWindowText(inDir);
		m_Directory.UpdateWindow();
	}

	virtual bool OnFile(CString const& inFileName, CFileStatus const& inStatus) = 0;

protected:
	CStatic& m_Directory;
};

// Recurse directory
static void SearchFor(CString const& inFullPath, CProcessFile& inCallback)
{
	CString tmpPath(inFullPath);
	int i;
	while (-1 != (i = tmpPath.Find('/')))
		tmpPath.SetAt(i, '\\');
	inCallback.UpdateDirectory(tmpPath);
	TCHAR c = tmpPath[tmpPath.GetLength()-1];
	if ('\\' != c)
		tmpPath += '\\';
	CString tmp(tmpPath);
	tmpPath += _T("*.*");
	WIN32_FIND_DATA find;
	HANDLE hFile = FindFirstFile(tmpPath, &find);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		TRACE(_T("FindFirstFile failed on '%s'\n"), (LPCTSTR)inFullPath);
		return;
	}
	tmpPath = tmp;

	bool bContinue = true;
	CFileStatus status;
	do
	{
		if (0 == lstrcmp(find.cFileName, _T("."))
		|| 0 == lstrcmp(find.cFileName, _T("..")))
			continue;
		tmp = tmpPath;
		tmp += find.cFileName;
		if (GetLocalStatus(tmp, status))
		{
			// It's a directory.
			if (CFile::directory & status.m_attribute)
			{
				SearchFor(tmp, inCallback);
			}
			else
			{
				bContinue = inCallback.OnFile(tmp, status);
			}
		}
	} while (bContinue && FindNextFile(hFile, &find));
	FindClose(hFile);
}

/////////////////////////////////////////////////////////////////////////////

static void DumpFileStatus(
	CString path,
	int inIndent,
	otstringstream& outData)
{
	PrintIndent(inIndent, outData);
	outData << _T("Files in ") << (LPCTSTR)path << _T("\r\n");

	path += _T("\\");
	CString tmp(path);
	tmp += _T("*.*");
	WIN32_FIND_DATA find;
	HANDLE hFile = FindFirstFile(tmp, &find);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		TRACE(_T("FindFirstFile failed on '%s'\n"), (LPCTSTR)tmp);
		return;
	}

	CFileStatus status;
	do
	{
		if (0 == lstrcmp(find.cFileName, _T("."))
		|| 0 == lstrcmp(find.cFileName, _T("..")))
			continue;
		tmp = path;
		tmp += find.cFileName;
		if (GetLocalStatus(tmp, status))
		{
			PrintIndent(inIndent, outData);
			// It's a directory.
			if (CFile::directory & status.m_attribute)
			{
				outData
					<< _T("Directory [ignored]: ")
					<< (LPCTSTR)tmp
					<< _T("\r\n");
			}
			else
			{
				outData
					<< _T("File: ")
					<< (LPCTSTR)tmp
					<< _T("\r\n");
				HINSTANCE inst = LoadLibrary(tmp);
				if (inst)
				{
					CVersionNum ver(inst);
					if (ver.Valid())
					{
						PrintIndent(inIndent+1, outData);
						outData << _T("FileName: ")
							<< (LPCTSTR)ver.GetFileName()
							<< _T("\r\n");
						PrintIndent(inIndent+1, outData);
						outData << _T("ProductName: ")
							<< (LPCTSTR)ver.GetProductName()
							<< _T("\r\n");
						PrintIndent(inIndent+1, outData);
						outData << _T("Version: ")
							<< (LPCTSTR)ver.GetVersionString()
							<< _T("\r\n");
						PrintIndent(inIndent+1, outData);
						outData << _T("LangID: ")
							<< ver.GetLangID()
							<< _T("\r\n");
					}
				}
			}
		}
	} while (FindNextFile(hFile, &find));
	FindClose(hFile);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage1 property page

IMPLEMENT_DYNAMIC(CDlgHelpPage1, CPropertyPage)

CDlgHelpPage1::CDlgHelpPage1(CDlgARBHelp* pParent)
	: CPropertyPage(CDlgHelpPage1::IDD)
	, m_Parent(pParent)
{
	//{{AFX_DATA_INIT(CDlgHelpPage1)
	//}}AFX_DATA_INIT
}

CDlgHelpPage1::~CDlgHelpPage1()
{
}

void CDlgHelpPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelpPage1)
	DDX_Control(pDX, IDC_TEXT, m_ctrlText);
	DDX_Control(pDX, IDC_TEXT2, m_ctrlText2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgHelpPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgHelpPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHelpPage1 message handlers

BOOL CDlgHelpPage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString msg;
	UINT ids[3] = {IDS_INTRODUCTION, IDS_GATHER, IDS_WHATNEXT};
	for (int i = 0; i < 3; ++i)
	{
		CString tmp;
		tmp.LoadString(ids[i]);
		msg += tmp;
		msg += _T("\r\n\r\n");
	}
	m_ctrlText.SetWindowText(msg);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgHelpPage1::OnSetActive()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	psheet->SetWizardButtons(PSWIZB_NEXT);
	return __super::OnSetActive();
}

class CProcessARBs : public CProcessFile
{
public:
	CProcessARBs(CDlgARBHelp* pParent, CStatic& inCtrlMsg, CString const& inMsg, CStatic& inCtrlText)
		: CProcessFile(inCtrlText)
		, m_Parent(pParent)
		, m_ctrlMsg(inCtrlMsg)
		, m_Msg(inMsg)
	{
	}
	virtual bool OnFile(CString const& inFileName, CFileStatus const& inStatus);

private:
	CDlgARBHelp* m_Parent;
	CStatic& m_ctrlMsg;
	CString const& m_Msg;
};

bool CProcessARBs::OnFile(CString const& inFileName, CFileStatus const& /*inStatus*/)
{
	int pos = inFileName.Find(_T(".arb"));
	if (0 <= pos)
	{
		// Include files that end in .arb or contain '.arb.', but not ending in '.lnk'
		if ((inFileName.GetLength() == pos + 4)
		|| (inFileName[pos+4] == '.' && inFileName.Right(4).CompareNoCase(_T(".lnk"))))
		{
			m_Parent->SetARBFileStatus(inFileName);
			CString tmp;
			tmp.FormatMessage(IDS_FILES_FOUND, (int)m_Parent->GetARBFiles().size());
			CString m(m_Msg);
			m += tmp;
			m_ctrlMsg.SetWindowText(m);
		}
	}
	return true;
}

LRESULT CDlgHelpPage1::OnWizardNext()
{
	CWaitCursor wait;
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	// SetWizardButtons does a postmessage. I need it updated now! (lparam is buttons)
	psheet->SendMessage(PSM_SETWIZBUTTONS, 0, 0);

	CString msg, tmp;

	// Read registry settings
	tmp.LoadString(IDS_TASK_REGISTRY);
	msg += tmp;
	m_ctrlText.SetWindowText(msg);
	static struct
	{
		HKEY hRegKey;
		bool bProcessOpenCmd;
		LPCTSTR pRootString;
		LPCTSTR pRegPath;
	} const sc_Reg[] =
	{
		{HKEY_CURRENT_USER, false, _T("HKCU\\"), _T("Software\\dcon Software\\Agility Record Book")},
		{HKEY_CLASSES_ROOT, false, _T("HKCR\\"), _T(".arb")},
		{HKEY_CLASSES_ROOT, true, _T("HKCR\\"), _T("AgilityBook.Document")},
		{HKEY_CLASSES_ROOT, true, _T("HKCR\\"), _T("Applications\\AGILIT~1.EXE")},
		{HKEY_CLASSES_ROOT, true, _T("HKCR\\"), _T("Applications\\AgilityBook.exe")},
		{NULL, NULL}
	};
	CString regInfo;
	for (int index = 0; sc_Reg[index].hRegKey; ++index)
	{
		otstringstream data;
		HKEY hSubKey;
		if (ERROR_SUCCESS == RegOpenKeyEx(sc_Reg[index].hRegKey, sc_Reg[index].pRegPath, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hSubKey))
		{
			data << sc_Reg[index].pRootString << sc_Reg[index].pRegPath << _T("\r\n");
			DumpRegistry(hSubKey, 1, data);
			HKEY hOpenKey;
			if (sc_Reg[index].bProcessOpenCmd)
			{
				if (ERROR_SUCCESS == RegOpenKeyEx(hSubKey, _T("shell\\open\\command"), 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hOpenKey))
				{
					DWORD type = REG_SZ;
					DWORD dwLen = BUFF_SIZE;
					DWORD dwRet = RegQueryValueEx(hOpenKey, g_szBuff, NULL, &type, reinterpret_cast<LPBYTE>(g_szBuff2), &dwLen);
					if (ERROR_SUCCESS == dwRet || ERROR_MORE_DATA == dwRet)
					{
						if (ERROR_MORE_DATA == dwRet)
						{
							tmp.LoadString(IDS_ERR_BUFFER_TOO_SMALL);
							PrintIndent(1, data);
							data << (LPCTSTR)tmp << _T("\r\n");
						}
						else
						{
							CString path(g_szBuff2);
							if (path[0] == '"')
							{
								path = path.Mid(1);
								int pos = path.Find('"');
								if (0 < pos)
									path = path.Left(pos);
							}
							else
							{
								int pos = path.Find(' ');
								if (0 < pos)
									path = path.Left(pos);
							}
							int pos = path.ReverseFind('\\');
							if (0 < pos)
							{
								path = path.Left(pos);
							}
							DumpFileStatus(path, 1, data);
						}
					}
					// read default
					RegCloseKey(hOpenKey);
				}
			}
			RegCloseKey(hSubKey);
		}
		else
		{
			tmp.FormatMessage(IDS_CANNOT_READ, sc_Reg[index].pRegPath);
			data << (LPCTSTR)tmp << _T("\r\n");
		}
		regInfo += data.str().c_str();
	}
	m_Parent->AddRegistryInfo(regInfo);

	tmp.LoadString(IDS_TASK_FILES);
	msg += _T("\r\n\r\n");
	msg += tmp;
	m_ctrlText.SetWindowText(msg);
	std::set<CString> drives;
	drives.insert(_T("C:")); // Search C:
	HKEY hRecent;
	// Also add any drives that are in the MRU list.
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\dcon Software\\Agility Record Book\\Recent File List"), 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hRecent))
	{
		DWORD i;
		// Enumerate the subkeys, recursively.
		for (i = 0; ; ++i)
		{
			DWORD dwLen = BUFF_SIZE;
			// A key name max size is 255.
			if (ERROR_SUCCESS != RegEnumKeyEx(hRecent, i, g_szBuff, &dwLen, NULL, NULL, NULL, NULL))
				break;
			HKEY hSubKey;
			if (ERROR_SUCCESS == RegOpenKeyEx(hRecent, g_szBuff, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hSubKey))
			{
				if (g_szBuff[1] == ':')
				{
					g_szBuff[1] = NULL;
					CString tmpBuff(g_szBuff);
					tmpBuff.MakeUpper();
					drives.insert(tmpBuff);
				}
				RegCloseKey(hSubKey);
			}
		}
		RegCloseKey(hRecent);
	}
	for (std::set<CString>::iterator i = drives.begin(); i != drives.end(); ++i)
	{
		tmp.FormatMessage(IDS_SEARCHINGDRIVE, (*i)[0]);
		msg += tmp;
		CProcessARBs search(m_Parent, m_ctrlText, msg, m_ctrlText2);
		m_ctrlText.SetWindowText(msg);
		SearchFor(*i, search);
	}
	m_ctrlText2.SetWindowText(_T(""));

	if (0 == m_Parent->GetARBFiles().size())
		return IDD_PAGE_ENCODE_FINISH;
	else
		return 0;
}
