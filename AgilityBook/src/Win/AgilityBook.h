#pragma once

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
 * @brief main header file for the AgilityBook application
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 */

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CAgilityBookApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CAgilityBookApp();

	/**
	 * Override the WinHelp api so we can map it to HtmlHelp.
	 * We are not re-mapping the cmd ids - most of the WinHelp cmds are
	 * <i>not</i> the same as HtmlHelp. It is recommended that when you
	 * need to call help, that you use the HtmlHelp api. Overriding this
	 * api allows MFC contexthelp to work properly.
	 */
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

private:
	CSingleDocTemplate* m_pDocTemplateTree;
	CSingleDocTemplate* m_pDocTemplateRuns;
	CSingleDocTemplate* m_pDocTemplatePoints;
	CSingleDocTemplate* m_pDocTemplateCal;
	CSingleDocTemplate* m_pDocTemplateTraining;

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CAgilityBookApp theApp;

// Some global functions.
class CAgilityBookDoc;
extern bool ShowContextHelp(HELPINFO* pHelpInfo);
extern void RunCommand(char const* pCmd);
extern void ExpandAll(CTreeCtrl& ctrl, HTREEITEM hItem, UINT code);
extern void UpdateVersion();
extern void UpdateVersion(CAgilityBookDoc* pDoc);
