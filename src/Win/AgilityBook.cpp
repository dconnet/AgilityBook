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
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2004-07-20 DRC Changed the updating to auto-update configurations also.
 *                    Moved the user-request updates to the document.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 * @li 2004-03-05 DRC Added check-for-updates feature.
 * @li 2003-12-07 DRC When opening the last opened file fails, open a new doc.
 */

#include "stdafx.h"
#include <afxinet.h>
#include "AgilityBook.h"
#include "MainFrm.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include "AgilityBookDoc.h"
#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"
#include "AgilityBookViewTraining.h"
#include "Element.h"
#include "HyperLink.h"
#include "Splash.h"
#include "TabView.h"
#include "VersionNum.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

#ifdef _DEBUG
#pragma comment(lib, "xerces-c_2D.lib")
#else
#pragma comment(lib, "xerces-c_2.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Some global functions.

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
		popup.pszText = "";
		popup.pt = pt;
		popup.clrForeground = GetSysColor(COLOR_INFOTEXT);
		popup.clrBackground = GetSysColor(COLOR_INFOBK);
		popup.rcMargins.left = -1;
		popup.rcMargins.top = -1;
		popup.rcMargins.right = -1;
		popup.rcMargins.bottom = -1;
		popup.pszFont = "Arial, 8, ascii, , , ";

		CString name(AfxGetApp()->m_pszHelpFilePath);
		name += "::/Help/AgilityBook.txt";
		::HtmlHelp(hwnd, name, HH_DISPLAY_TEXT_POPUP, (DWORD_PTR)&popup);
	}
	return bOk;
}

void RunCommand(char const* pCmd)
{
	if (pCmd)
	{
		INT_PTR result = reinterpret_cast<INT_PTR>(ShellExecute(NULL, _T("open"), pCmd, NULL, NULL, SW_SHOW));
		if (result <= HINSTANCE_ERROR)
		{
			CString str;
			switch (result)
			{
			case 0:
				str = "The operating system is out of memory or resources.";
				break;
			case SE_ERR_PNF:
				str = "The specified path was not found.";
				break;
			case SE_ERR_FNF:
				str = "The specified file was not found.";
				break;
			case ERROR_BAD_FORMAT:
				str = "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
				break;
			case SE_ERR_ACCESSDENIED:
				str = "The operating system denied access to the specified file.";
				break;
			case SE_ERR_ASSOCINCOMPLETE:
				str = "The filename association is incomplete or invalid.";
				break;
			case SE_ERR_DDEBUSY:
				str = "The DDE transaction could not be completed because other DDE transactions were being processed.";
				break;
			case SE_ERR_DDEFAIL:
				str = "The DDE transaction failed.";
				break;
			case SE_ERR_DDETIMEOUT:
				str = "The DDE transaction could not be completed because the request timed out.";
				break;
			case SE_ERR_DLLNOTFOUND:
				str = "The specified dynamic-link library was not found.";
				break;
			case SE_ERR_NOASSOC:
				str = "There is no application associated with the given filename extension.";
				break;
			case SE_ERR_OOM:
				str = "There was not enough memory to complete the operation.";
				break;
			case SE_ERR_SHARE:
				str = "A sharing violation occurred. ";
				break;
			default:
				str.Format(_T("Unknown Error (%d) occurred."), result);
				break;
			}
			str = "Unable to open " + str;
			AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
		}
	}
}

void ExpandAll(CTreeCtrl& ctrl, HTREEITEM hItem, UINT code)
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

bool ReadHttpFile(CString const& inURL, CString& outData)
{
	CWaitCursor wait;
	outData.Empty();
	try
	{
		CInternetSession session("my version");
		CStdioFile* pFile = session.OpenURL(inURL);
		if (pFile)
		{
			char buffer[1025];
			UINT nChars;
			while (0 < (nChars = pFile->Read(buffer, sizeof(buffer)-1)))
			{
				buffer[nChars] = 0;
				outData += buffer;
			}
			pFile->Close();
			delete pFile;
		}
		session.Close();
	}
	catch (CInternetException* ex)
	{
		ex->Delete();
		outData.Empty();
	}
	return (outData.GetLength() > 0);
}

// The format of the version.txt file is:
// line 1:
//  "ARB Version n1.n2.n3.n4"
// line 2-n: xml (see below)

/**
 * Check the version against the web.
 * @param outData Version info from the web.
 * @param bVerbose Show information dialogs
 * @param bOnOpenDoc We're calling this during a file open.
 * @return <0 failure, 0 Update occurred, >0 Version ok
 */
static int UpdateVersion(CString& outData, bool bVerbose, bool bOnOpenDoc)
{
	CString url;
	url.LoadString(IDS_HELP_UPDATE);
	url += "/version.txt";
	if (!ReadHttpFile(url, outData))
	{
		if (bVerbose)
		{
			CSplashWnd::HideSplashScreen();
			AfxMessageBox(IDS_UPDATE_UNKNOWN, MB_ICONEXCLAMATION);
		}
		return -1;
	}

	// Pretend the version is up-to-date.
	// Don't worry about the splash screen after this.
	if (bOnOpenDoc)
		return 1;

	ARBDate today = ARBDate::Today();
	CVersionNum verNew(outData);
	CVersionNum verThis;
	ASSERT(verThis.Valid());
	if (!verNew.Valid())
	{
		if (bVerbose)
			AfxMessageBox(IDS_UPDATE_UNKNOWN, MB_ICONEXCLAMATION);
		return -1;
	}

	// If the version has changed, don't bother checking the config.
	if (verThis < verNew)
	{
		AfxGetApp()->WriteProfileString("Settings", "lastVerCheck", today.GetString(ARBDate::eDashYMD).c_str());
		CString ver;
		ver.FormatMessage(IDS_UPDATE_NEWER_VERSION, (LPCTSTR)verNew.GetVersionString());
		if (IDYES == AfxMessageBox(ver, MB_ICONQUESTION | MB_YESNO))
		{
			CString url;
			url.LoadString(IDS_ABOUT_LINK_SOURCEFORGE);
			int nTab = url.Find('\t');
			if (0 < nTab)
				url = url.Mid(nTab+1);
			CHyperLink::GotoURL(url);
		}
		return 0;
	}
	AfxGetApp()->WriteProfileString("Settings", "lastVerCheck", today.GetString(ARBDate::eDashYMD).c_str());
	return 1;
}

void UpdateVersion()
{
	CString data;
	UpdateVersion(data, false, true);
}

void UpdateVersion(CAgilityBookDoc* pDoc, bool bOnOpenDoc)
{
	CString data;
	if (0 >= UpdateVersion(data, true, bOnOpenDoc))
		return;
	// Only continue if we parsed the version.txt file AND the version
	// is up-to-date.

	// Skip the first line, that's the version.
	int n = data.Find('\n');
	if (0 < n)
		data = data.Mid(n+1);
	else
		data.Empty();
	if (data.IsEmpty())
		return;
	// The rest of the file is xml:
	// <Data>
	//   <Config ver="1" file="file">data</Config>
	// </Data>
	bool bUpToDate = true;
	Element tree;
	std::string errMsg;
	if (!tree.LoadXMLBuffer((LPCSTR)data, data.GetLength(), errMsg))
	{
		bUpToDate = false;
		CString msg("Failed to load 'version.txt'.");
		if (0 < errMsg.length())
		{
			msg += "\n\n";
			msg += errMsg.c_str();
		}
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
	}
	// If the parse was successful, check for the posted config version.
	else if (tree.GetName() == "Data")
	{
		int nConfig = tree.FindElement("Config");
		if (0 <= nConfig)
		{
			Element& config = tree.GetElement(nConfig);
			short ver;
			std::string file;
			if (Element::eFound == config.GetAttrib("ver", ver)
			&& Element::eFound == config.GetAttrib("file", file))
			{
				// Cool! New config!
				if (ver > pDoc->GetConfig().GetVersion())
				{
					bUpToDate = false;
					std::string note = config.GetValue();
					CString msg("The configuration has been updated. Would you like to merge the new one with your data?");
					if (0 < note.length())
					{
						// If the info contains a note, append it.
						// A note will often give a brief description of things
						// the user must do. For instance, from v4->5, USDAA
						// titling pts were removed from Tournament Jumpers and
						// Snooker to allow for non-titling runs. In case the
						// user saved some that way, we need to warn them.
						msg += "\n\n";
						msg += note.c_str();
					}
					CSplashWnd::HideSplashScreen();
					if (IDYES == AfxMessageBox(msg, MB_ICONQUESTION | MB_YESNO))
					{
						// Load the config.
						CString url;
						url.LoadString(IDS_HELP_UPDATE);
						url += "/";
						url += file.c_str();
						CString strConfig;
						if (ReadHttpFile(url, strConfig))
						{
							Element tree;
							std::string errMsg;
							if (!tree.LoadXMLBuffer((LPCSTR)strConfig, strConfig.GetLength(), errMsg))
							{
								CString msg("Failed to load '");
								msg += url;
								msg += "'.";
								if (0 < errMsg.length())
								{
									msg += "\n\n";
									msg += errMsg.c_str();
								}
								AfxMessageBox(msg, MB_ICONEXCLAMATION);
							}
							else if (tree.GetName() == "DefaultConfig")
							{
								strConfig.Empty();
								ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
								tree.GetAttrib(ATTRIB_BOOK_VERSION, version);
								int nConfig = tree.FindElement(TREE_CONFIG);
								if (0 <= nConfig)
								{
									ARBAgilityRecordBook book;
									if (!book.GetConfig().Load(tree.GetElement(nConfig), version, errMsg))
									{
										if (0 < errMsg.length())
											AfxMessageBox(errMsg.c_str(), MB_ICONWARNING);
									}
									else
									{
										pDoc->ImportConfiguration(book.GetConfig());
										pDoc->SetModifiedFlag(TRUE);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (bUpToDate && !bOnOpenDoc)
		AfxMessageBox(IDS_UPDATE_CURRENT, MB_ICONINFORMATION);
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookApp

BEGIN_MESSAGE_MAP(CAgilityBookApp, CWinApp)
	//{{AFX_MSG_MAP(CAgilityBookApp)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP_SPLASH, OnHelpSplash)
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
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	CWinApp::InitInstance();

	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (XMLException const& toCatch)
	{
		CString msg;
		msg.LoadString(IDS_XERCES_ERROR);
		char *pStr = XMLString::transcode(toCatch.getMessage());
		msg += pStr;
		delete [] pStr;
		AfxMessageBox(msg, MB_ICONSTOP);
		return FALSE;
	}

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_DATE_CLASSES | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

#if defined (_MSC_VER) && (_MSC_VER < 1300) // 1300 is VC7
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// We need at least 800x600 (the event(run) dialog is big!)
	if (800 > GetSystemMetrics(SM_CXFULLSCREEN)
	|| 600 > GetSystemMetrics(SM_CYFULLSCREEN))
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
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTabView));
	AddDocTemplate(pDocTemplate);

	m_pDocTemplateTree = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAgilityBookTree));
	m_pDocTemplateRuns = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAgilityBookViewRuns));
	m_pDocTemplatePoints = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAgilityBookViewPoints));
	m_pDocTemplateCal = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAgilityBookViewCalendar));
	m_pDocTemplateTraining = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAgilityBookDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAgilityBookViewTraining));

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(FALSE);

	// Enable Html help
#if _MSC_VER >= 1300
	EnableHtmlHelp();
#endif
	char* chmFile = _tcsdup(m_pszHelpFilePath);
	lstrcpy(&chmFile[lstrlen(chmFile)-3], "chm");
	m_pszHelpFilePath = chmFile;

	// Should we open the last open file?
	bool bOpeningLast = false;
	if (CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
	{
		// Don't open it if the shift key is down.
		if (0 <= GetKeyState(VK_SHIFT))
		{
			CString strFile = GetProfileString("Settings", "LastFile", _T(""));
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
		if (!CFile::GetStatus(cmdInfo.m_strFileName, rStatus)
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
	if (AfxGetApp()->GetProfileInt("Settings", "autoCheck", 1))
	{
		CString ver = AfxGetApp()->GetProfileString("Settings", "lastVerCheck", "");
		ARBDate date = ARBDate::FromString((LPCSTR)ver, ARBDate::eDashYYYYMMDD);
		if (date.IsValid())
		{
			ARBDate today = ARBDate::Today();
			date += 30;
			if (date < today)
				UpdateVersion();
		}
	}
	return TRUE;
}

int CAgilityBookApp::ExitInstance()
{
	// Close any open HTML Help windows
	::HtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);

	delete m_pDocTemplateTree;
	delete m_pDocTemplateRuns;
	delete m_pDocTemplatePoints;
	delete m_pDocTemplateCal;
	delete m_pDocTemplateTraining;
	XMLPlatformUtils::Terminate();
	return CWinApp::ExitInstance();
}

void CAgilityBookApp::WinHelp(DWORD_PTR dwData, UINT nCmd)
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
	CSplashWnd::ShowSplashScreen(AfxGetMainWnd(), false);
}

BOOL CAgilityBookApp::PreTranslateMessage(MSG* pMsg)
{
	// The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
	return CWinApp::PreTranslateMessage(pMsg);
}
