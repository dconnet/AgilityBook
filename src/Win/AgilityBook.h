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
	afx_msg void OnAppAbout();
	afx_msg void OnHelpSplash();
	afx_msg void OnHelpUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CAgilityBookApp theApp;

// Some global functions.
extern void ExpandAll(CTreeCtrl& ctrl, HTREEITEM hItem, UINT code);
extern void UpdateVersion(bool bVerbose);
