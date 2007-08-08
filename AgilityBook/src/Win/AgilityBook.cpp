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
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2005-02-10 DRC There was a problem initializing RICHED*.DLL on Win98.
 * @li 2004-07-20 DRC Changed the updating to auto-update configurations also.
 *                    Moved the user-request updates to the document.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 * @li 2004-03-05 DRC Added check-for-updates feature.
 * @li 2003-12-07 DRC When opening the last opened file fails, open a new doc.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "CrashHandler.h"
#include "MainFrm.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewHtml.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"
#include "AgilityBookViewTraining.h"
#include "DlgMessage.h"
#include "Element.h"
#include "Splash.h"
#include "TabView.h"

#include <xercesc/util/XercesVersion.hpp>
#if _XERCES_VERSION < 20200
#error Minimum version of Xerces is 2.2.
#elif _XERCES_VERSION > 20700
#pragma message ( "Warning: Untested version of Xerces" )
#endif
#pragma message ( "Compiling with Xerces " XERCES_FULLVERSIONDOT )
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
XERCES_CPP_NAMESPACE_USE
// Note, see XercesVersion.hpp for how to use the version macros.
// Currently, we've used versions 2.2 and 2.7. There were no source code
// changes needed between the two, hence we haven't needed to do any
// funny stuff!
#ifdef XML_LIBRARY
	#ifdef _DEBUG
		#define XERCES_LIB	"xerces-c_static_2D.lib"
	#else
		#define XERCES_LIB	"xerces-c_static_2.lib"
	#endif
#else
	#ifdef _DEBUG
		#define XERCES_LIB	"xerces-c_2D.lib"
	#else
		#define XERCES_LIB	"xerces-c_2.lib"
	#endif
#endif
#pragma message ( "Linking with " XERCES_LIB )
#pragma comment(lib, XERCES_LIB)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Some global functions.

void InitMenuPopup(CCmdTarget* pTarget, CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CCmdUI cmdUI;
	cmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (UINT n = 0; n < cmdUI.m_nIndexMax; ++n)
	{
		cmdUI.m_nIndex = n;
		cmdUI.m_nID = pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
		cmdUI.m_pMenu = pPopupMenu;
		cmdUI.DoUpdate(pTarget, FALSE);
	}
}

bool ShowContextHelp(HELPINFO* pHelpInfo)
{
	bool bOk = false;
	if (pHelpInfo->dwContextId > 0)
	{
		bOk = true;
		HWND hwnd = NULL;
		POINT pt = pHelpInfo->MousePos;
		if (HELPINFO_WINDOW == pHelpInfo->iContextType)
		{
			hwnd = static_cast<HWND>(pHelpInfo->hItemHandle);
			RECT r;
			::GetWindowRect(hwnd, &r);
			pt.x = r.left + (r.right - r.left) / 2;
			pt.y = r.bottom;
		}
		HH_POPUP popup;
		memset(&popup, 0, sizeof(HH_POPUP));
		popup.cbStruct = sizeof(HH_POPUP);
		popup.hinst = 0;
		popup.idString = static_cast<UINT>(pHelpInfo->dwContextId);
		popup.pszText = _T("");
		popup.pt = pt;
		popup.clrForeground = GetSysColor(COLOR_INFOTEXT);
		popup.clrBackground = GetSysColor(COLOR_INFOBK);
		popup.rcMargins.left = -1;
		popup.rcMargins.top = -1;
		popup.rcMargins.right = -1;
		popup.rcMargins.bottom = -1;
		popup.pszFont = _T("Arial, 8, ascii, , , ");

		CString name(AfxGetApp()->m_pszHelpFilePath);
		name += _T("::/Help/AgilityBook.txt");
		::HtmlHelp(hwnd, name, HH_DISPLAY_TEXT_POPUP, (DWORD_PTR)&popup);
	}
	return bOk;
}

void RunCommand(TCHAR const* const pCmd)
{
	if (pCmd)
	{
		INT_PTR result = reinterpret_cast<INT_PTR>(ShellExecute(NULL, _T("open"), pCmd, NULL, NULL, SW_SHOW));
		if (result <= HINSTANCE_ERROR)
		{
			CString str;
			str.LoadString(IDS_UNABLE_TO_OPEN);
			ARBostringstream msg;
			msg << (LPCTSTR)str
				<< pCmd
				<< _T("\n");
			switch (result)
			{
			case 0:
				str.LoadString(IDS_SYSERR_MEMORY);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_PNF:
				str.LoadString(IDS_SYSERR_SE_ERR_PNF);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_FNF:
				str.LoadString(IDS_SYSERR_SE_ERR_FNF);
				msg << (LPCTSTR)str;
				break;
			case ERROR_BAD_FORMAT:
				str.LoadString(IDS_SYSERR_ERROR_BAD_FORMAT);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_ACCESSDENIED:
				str.LoadString(IDS_SYSERR_SE_ERR_ACCESSDENIED);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_ASSOCINCOMPLETE:
				str.LoadString(IDS_SYSERR_SE_ERR_ASSOCINCOMPLETE);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_DDEBUSY:
				str.LoadString(IDS_SYSERR_SE_ERR_DDEBUSY);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_DDEFAIL:
				str.LoadString(IDS_SYSERR_SE_ERR_DDEFAIL);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_DDETIMEOUT:
				str.LoadString(IDS_SYSERR_SE_ERR_DDETIMEOUT);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_DLLNOTFOUND:
				str.LoadString(IDS_SYSERR_SE_ERR_DLLNOTFOUND);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_NOASSOC:
				str.LoadString(IDS_SYSERR_SE_ERR_NOASSOC);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_OOM:
				str.LoadString(IDS_SYSERR_SE_ERR_OOM);
				msg << (LPCTSTR)str;
				break;
			case SE_ERR_SHARE:
				str.LoadString(IDS_SYSERR_SE_ERR_SHARE);
				msg << (LPCTSTR)str;
				break;
			default:
				str.FormatMessage(IDS_SYSERR_UNKNOWN, (long)result);
				msg << (LPCTSTR)str;
				break;
			}
			AfxMessageBox(msg.str().c_str(), MB_ICONEXCLAMATION | MB_OK);
		}
	}
}

void ExpandAll(
		CTreeCtrl& ctrl,
		HTREEITEM hItem,
		UINT code)
{
	if (NULL == hItem)
		return;
	if (TVI_ROOT != hItem)
		ctrl.Expand(hItem, code);
	HTREEITEM hChildItem = ctrl.GetNextItem(hItem, TVGN_CHILD);
	while (NULL != hChildItem)
	{
		ExpandAll(ctrl, hChildItem, code);
		hChildItem = ctrl.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

// Copy of CFile::GetStatus, changes are commented below.
// For some reason the timestamp on files can be totally messed up.
// It happens in the FILETIME to CTime conversion (throws a COleException)
// This has occurred with files on a NAS and CD.
// VC8 has changed, but still has problems - VC8 code has been merged below
BOOL GetLocalStatus(
		LPCTSTR lpszFileName,
		CFileStatus& rStatus)
{
	// Defined in "Microsoft Visual Studio 8\VC\atlmfc\src\mfc\afximpl.h"
#if _MSC_VER >= 1400
	BOOL AFXAPI AfxFullPath(__out_z LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
#else
	BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
#endif

#if _MSC_VER >= 1400
	ASSERT( lpszFileName != NULL );
#endif

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
	HANDLE hFind = FindFirstFile((LPTSTR)lpszFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	VERIFY(FindClose(hFind));

	// strip attribute of NORMAL bit, our API doesn't have a "normal" bit.
// CHANGE: Added '0xff' due to runtime cast check option
	rStatus.m_attribute = (BYTE)
		(0xff & (findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL));

	// get just the low DWORD of the file size
	ASSERT(findFileData.nFileSizeHigh == 0);
	rStatus.m_size = (LONG)findFileData.nFileSizeLow;

	// convert times as appropriate
// CHANGE: handle COleException
#if _MSC_VER >= 1400
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

#else
	//rStatus.m_ctime = CTime(findFileData.ftCreationTime);
	//rStatus.m_atime = CTime(findFileData.ftLastAccessTime);
	//rStatus.m_mtime = CTime(findFileData.ftLastWriteTime);
	CTime* times[3]; times[0] = &rStatus.m_ctime; times[1] = &rStatus.m_atime; times[2] = &rStatus.m_mtime;
	FILETIME* ft[3]; ft[0] = &findFileData.ftCreationTime; ft[1] = &findFileData.ftLastAccessTime; ft[2] = &findFileData.ftLastWriteTime;
	for (int i = 0; i < 3; ++i)
	{
		try
		{
			*(times[i]) = CTime(*(ft[i]));
		}
		catch (COleException* pe)
		{
			pe->Delete();
			*(times[i]) = 0;
		}
	}
#endif

	if (rStatus.m_ctime.GetTime() == 0)
		rStatus.m_ctime = rStatus.m_mtime;

	if (rStatus.m_atime.GetTime() == 0)
		rStatus.m_atime = rStatus.m_mtime;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookApp

BEGIN_MESSAGE_MAP(CAgilityBookApp, CWinApp)
	//{{AFX_MSG_MAP(CAgilityBookApp)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP_SPLASH, OnHelpSplash)
	ON_COMMAND(ID_HELP_SYSINFO, OnHelpSysinfo)
	ON_COMMAND(ID_FILE_PRINT_BLANK_RUNS, OnPrintBlankRuns)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// CAgilityBookApp construction

CAgilityBookApp::CAgilityBookApp()
{
}

// The one and only CAgilityBookApp object
CAgilityBookApp theApp;

// CAgilityBookApp initialization
BOOL CAgilityBookApp::InitInstance()
{
	if (!AfxOleInit())
	{
		AfxMessageBox(_T("ERROR: Cannot initialize COM"), MB_ICONSTOP);
		return FALSE;
	}

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	// Note: ICC_WIN95_CLASSES == ICC_LISTVIEW_CLASSES ICC_TREEVIEW_CLASSES
	//  ICC_BAR_CLASSES ICC_TAB_CLASSES ICC_UPDOWN_CLASS ICC_PROGRESS_CLASS
	//  ICC_HOTKEY_CLASS ICC_ANIMATE_CLASS
	icc.dwICC = ICC_DATE_CLASSES | ICC_WIN95_CLASSES;
	if (!InitCommonControlsEx(&icc))
	{
		AfxMessageBox(_T("ERROR: Unable to initialize the common controls."), MB_ICONSTOP);
		return FALSE;
	}

	CWinApp::InitInstance();

	// Calling AfxInitRichEdit() is not sufficient for older systems.
	// All that will do is load RICHED32.DLL. On newer systems, RICHED32
	// automatically loads RICHED20 also. But not on Win98. So specifically
	// load that too [with AfxInitRichEdit2].
#if _MSC_VER < 1300
	// VC6
	if (!AfxInitRichEdit())
	{
		AfxMessageBox(_T("ERROR: Unable to initialize RICHED32.DLL. Please see 'http://support.microsoft.com/default.aspx?scid=kb;en-us;218838' This may address the problem."), MB_ICONSTOP);
		return FALSE;
	}
	if (NULL == LoadLibrary(_T("RICHED20.DLL")))
#else
	if (!AfxInitRichEdit2())
#endif
	{
		AfxMessageBox(_T("ERROR: Unable to initialize RICHED20.DLL. Please see 'http://support.microsoft.com/default.aspx?scid=kb;en-us;218838' This may address the problem."), MB_ICONSTOP);
		return FALSE;
	}

	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (XMLException const& toCatch)
	{
		CString msg;
		msg.LoadString(IDS_XERCES_ERROR);
#ifdef UNICODE
		msg += toCatch.getMessage();
#else
		char* pStr = XMLString::transcode(toCatch.getMessage());
		msg += pStr;
		delete [] pStr;
#endif
		AfxMessageBox(msg, MB_ICONSTOP);
		return FALSE;
	}

#if _MSC_VER < 1300
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// We need at least 800x600 (the event(run) dialog is big!)
	if (800 > GetSystemMetrics(SM_CXSCREEN)
	|| 600 > GetSystemMetrics(SM_CYSCREEN))
	{
		if (IDNO == AfxMessageBox(IDS_MIN_RESOLUTION, MB_ICONSTOP | MB_YESNO))
			return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need

	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("dcon Software"));
	InitCrashHandler(GetAppRegistryKey());
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash && CAgilityBookOptions::AutoShowSplashScreen());

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTabView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(FALSE);

	// Enable Html help
#if _MSC_VER >= 1300
	EnableHtmlHelp();
#endif
	TCHAR* chmFile = _tcsdup(m_pszHelpFilePath);
	lstrcpy(&chmFile[lstrlen(chmFile)-3], _T("chm"));
	m_pszHelpFilePath = chmFile;

	// Should we open the last open file?
	bool bOpeningLast = false;
	if (CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
	{
		// Don't open it if the shift key is down.
		if (0 <= GetKeyState(VK_SHIFT))
		{
			CString strFile = GetProfileString(_T("Settings"), _T("LastFile"), _T(""));
			if (!strFile.IsEmpty())
			{
				bOpeningLast = true;
				cmdInfo.ParseParam(strFile, FALSE, TRUE);
			}
		}
	}
	// If a file is being opened, verify it exists first!
	// This catches both the case where the remembered last file (above)
	// doesn't exist and the case where the user types a command line where
	// it doesn't exist.
	if (CCommandLineInfo::FileOpen == cmdInfo.m_nShellCommand)
	{
		CFileStatus rStatus;
		if (!GetLocalStatus(cmdInfo.m_strFileName, rStatus)
		|| 0 == rStatus.m_size)
		{
			cmdInfo.m_strFileName = _T("");
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
		}
	}

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
	{
		// If the last document failed to open, just open a new file.
		if (bOpeningLast
		&& cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
		{
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
			if (!ProcessShellCommand(cmdInfo))
				return FALSE;
		}
		else
			return FALSE;
	}
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Check for updates every 30 days.
	if (AfxGetApp()->GetProfileInt(_T("Settings"), _T("autoCheck"), 1))
	{
		CString ver = AfxGetApp()->GetProfileString(_T("Settings"), _T("lastVerCheck"), _T(""));
		ARBDate date = ARBDate::FromString((LPCTSTR)ver, ARBDate::eDashYYYYMMDD);
		if (date.IsValid())
		{
			ARBDate today = ARBDate::Today();
			date += 30;
			if (date < today)
				m_UpdateInfo.AutoUpdateProgram();
		}
	}
	return TRUE;
}

int CAgilityBookApp::ExitInstance()
{
	// Close any open HTML Help windows
	::HtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
	XMLPlatformUtils::Terminate();
	CleanupCrashHandler();
	return CWinApp::ExitInstance();
}

void CAgilityBookApp::WinHelp(
		DWORD_PTR dwData,
		UINT nCmd)
{
#if _MSC_VER < 1300
	::HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(), m_pszHelpFilePath, nCmd, dwData);
#else
	HtmlHelp(dwData, nCmd);
#endif
}

// CAgilityBookApp message handlers

void CAgilityBookApp::OnHelpContents()
{
	WinHelp(0, HH_DISPLAY_TOC);
}

void CAgilityBookApp::OnHelpIndex()
{
	WinHelp(0, HH_DISPLAY_INDEX);
}

void CAgilityBookApp::OnHelpSplash()
{
	BOOL bEnabled = CSplashWnd::IsSplashScreenEnabled();
	CSplashWnd::EnableSplashScreen(TRUE);
	CSplashWnd::ShowSplashScreen(AfxGetMainWnd(), false);
	CSplashWnd::EnableSplashScreen(bEnabled);
}

BOOL CAgilityBookApp::PreTranslateMessage(MSG* pMsg)
{
	// The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
	return CWinApp::PreTranslateMessage(pMsg);
}

void CAgilityBookApp::OnHelpSysinfo()
{
	ARBostringstream info;

	// Windows version
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(os);
	GetVersionEx(&os);
	switch (os.dwPlatformId)
	{
	default: // Win32s
		info << _T("Windows32s ")
			<< os.dwMajorVersion
			<< '.'
			<< os.dwMinorVersion
			<< ' '
			<< os.szCSDVersion
			<< std::endl;
		break;
	case VER_PLATFORM_WIN32_WINDOWS: // Win95/98
		if (0 == os.dwMinorVersion)
			info << _T("Windows 95 ")
				<< os.dwMajorVersion
				<< '.'
				<< os.dwMinorVersion
				<< '.'
				<< int(LOWORD(os.dwBuildNumber))
				<< ' '
				<< os.szCSDVersion
				<< std::endl;
		else
			info << _T("Windows 98 ")
				<< os.dwMajorVersion
				<< '.'
				<< os.dwMinorVersion
				<< '.'
				<< int(LOWORD(os.dwBuildNumber))
				<< ' '
				<< os.szCSDVersion
				<< std::endl;
		break;
	case VER_PLATFORM_WIN32_NT: // NT/Win2000
		info << _T("Windows ")
			<< os.dwMajorVersion
			<< '.'
			<< os.dwMinorVersion
			<< '.'
			<< os.dwBuildNumber
			<< ' '
			<< os.szCSDVersion
			<< std::endl;
		break;
	}

	CString badVer;
	badVer.LoadString(IDS_BAD_VERSION);

	// Me.
	{
		CVersionNum ver;
		info << (LPCTSTR)ver.GetFileName();
		info << ": ";
		if (ver.Valid())
			info << (LPCTSTR)ver.GetVersionString();
		else
			info << (LPCTSTR)badVer;
		info << std::endl;
	}

	// Common controls.
	HINSTANCE hCommCtrl = LoadLibrary(_T("comctl32.dll"));
	if (hCommCtrl)
	{
		CVersionNum ver(hCommCtrl);
		info << (LPCTSTR)ver.GetFileName();
		info << ": ";
		if (ver.Valid())
			info << (LPCTSTR)ver.GetVersionString();
		else
			info << (LPCTSTR)badVer;
		info << std::endl;
		FreeLibrary(hCommCtrl);
		hCommCtrl = NULL;
	}

	// Internet Explorer.
	HINSTANCE hShellDocObj = LoadLibrary(_T("shdocvw.dll"));
	if (hShellDocObj)
	{
		CVersionNum ver(hShellDocObj);
		info << (LPCTSTR)ver.GetFileName();
		info << ": ";
		if (ver.Valid())
			info << (LPCTSTR)ver.GetVersionString();
		else
			info << (LPCTSTR)badVer;
		info << std::endl;
		FreeLibrary(hShellDocObj);
		hShellDocObj = NULL;
	}

	CString msg(info.str().c_str());
	CDlgMessage dlg(msg);
	dlg.DoModal();
}

void CAgilityBookApp::OnPrintBlankRuns()
{
	std::vector<RunInfo> runs;
	PrintRuns(NULL, ARBDogPtr(), runs);
}
