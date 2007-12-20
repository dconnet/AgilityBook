#pragma once

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
 * @brief main header file for the AgilityBook application
 * @author David Connet
 *
 * Revision History
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 */

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// These are needed in order to generate a decent pragma message
#define STRING2(x) #x
#define STRING(x) STRING2(x)
//#pragma message ( __FILE__ "(" STRING(__LINE__) ") : message" )
#define PRAGMA_MESSAGE(x) ( __FILE__ "(" STRING(__LINE__) ") : " x )


#include "resource.h"       // main symbols
#include "LanguageManager.h"
#include "UpdateInfo.h"

class CAgilityBookApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CAgilityBookApp();

	CUpdateInfo& UpdateInfo()						{return m_UpdateInfo;}
	CLanguageManager const& LanguageManager() const	{return m_LangMgr;}

	/**
	 * Override the WinHelp api so we can map it to HtmlHelp.
	 * We are not re-mapping the cmd ids - most of the WinHelp cmds are
	 * <i>not</i> the same as HtmlHelp. It is recommended that when you
	 * need to call help, that you use the HtmlHelp api. Overriding this
	 * api allows MFC contexthelp to work properly.
	 */
	virtual void WinHelp(
			DWORD_PTR dwData,
			UINT nCmd = HELP_CONTEXT);

	void SetStatusText(
			CString const& msg,
			bool bFiltered);
	void SetStatusText2(CString const& msg);

private:
	CUpdateInfo m_UpdateInfo;
	CLanguageManager m_LangMgr;

// Overrides
	//{{AFX_VIRTUAL(CAgilityBookApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	//{{AFX_MSG(CAgilityBookApp)
	afx_msg void OnHelpContents();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelpSplash();
	afx_msg void OnHelpSysinfo();
	afx_msg void OnPrintBlankRuns();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CAgilityBookApp theApp;

// Some global functions.
class CAgilityBookDoc;

/**
 * Make popup menus use the ON_UPDATE_COMMAND_UI handlers
 * @param pTarget Window messages should be propagated to
 * @param pPopupMenu OnInitMenuPopup parameter
 * @param nIndex OnInitMenuPopup parameter
 * @param bSysMenu OnInitMenuPopup parameter
 */
extern void InitMenuPopup(
		CCmdTarget* pTarget,
		CMenu* pPopupMenu,
		UINT nIndex,
		BOOL bSysMenu);

/**
 * Show context help.
 * @param langMgr Language Manager
 * @param pHelpInfo OnHelpInfo parameter.
 */
extern bool ShowContextHelp(CLanguageManager const& langMgr, HELPINFO* pHelpInfo);

/**
 * Use ShellExecute to start a program.
 * @param pCmd Command to run.
 */
extern void RunCommand(TCHAR const* const pCmd);

/**
 * Expand all items in a tree control.
 * @param ctrl Tree control
 * @param hItem Item from which to expand all items
 * @param code Expand/Collapse code
 */
extern void ExpandAll(
		CTreeCtrl& ctrl,
		HTREEITEM hItem,
		UINT code);

/**
 * Get the file status of a file. This works around a bug in MFC with CTime.
 * @param lpszFileName File to check.
 * @param rStatus Status structure.
 */
extern BOOL GetLocalStatus(
		LPCTSTR lpszFileName,
		CFileStatus& rStatus);

/**
 * Print blank pages to enter runs on.
 * @param inConfig Configuration
 * @param inDog Dog's runs to print.
 * @param inRuns Runs to print, if empty, print blank pages.
 * @return Printed?
 */
#include "PointsData.h"
extern bool PrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns);


/**
 * Separate a line into substrings. Very simply routine that has no escape chars
 * @param inSep Separator character
 * @param inStr String to separate
 * @param outFields Separated pieces
 * @return Number of fields
 */
extern size_t BreakLine(
		char inSep,
		tstring inStr,
		std::vector<tstring>& outFields);
