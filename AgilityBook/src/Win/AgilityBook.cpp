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
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "MainFrm.h"

#include "AboutDlg.h"
#include "AgilityBookDoc.h"
#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"
#include "AgilityBookViewTraining.h"
#include "TabView.h"

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

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookApp

BEGIN_MESSAGE_MAP(CAgilityBookApp, CWinApp)
	//{{AFX_MSG_MAP(CAgilityBookApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
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
	CWinApp::InitInstance();

	try
	{
		XMLPlatformUtils::Initialize();
	}
    catch (const XMLException& toCatch)
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
	icc.dwICC = ICC_DATE_CLASSES;
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
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Should we open the last open file?
	if (CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
	{
		CString strFile = GetProfileString("Settings", "LastFile", _T(""));
		if (!strFile.IsEmpty())
			cmdInfo.ParseParam(strFile, FALSE, TRUE);
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
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CAgilityBookApp::ExitInstance()
{
	delete m_pDocTemplateTree;
	delete m_pDocTemplateRuns;
	delete m_pDocTemplatePoints;
	delete m_pDocTemplateCal;
	delete m_pDocTemplateTraining;
	XMLPlatformUtils::Terminate();
	return CWinApp::ExitInstance();
}

// CAgilityBookApp message handlers

void CAgilityBookApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
