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
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2007-02-08 DRC Created
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CARBHelpCommandLine : public CCommandLineInfo
{
public:
	CARBHelpCommandLine()
		: m_Decode(false)
	{}
	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
	bool m_Decode;
};

void CARBHelpCommandLine::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag)
	{
		CString arg(lpszParam);
		if (0 == arg.CompareNoCase(_T("decode")))
			m_Decode = true;
	}
	else
		CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

class CARBHelpApp : public CWinApp
{
public:
	CARBHelpApp();
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CARBHelpApp, CWinApp)
END_MESSAGE_MAP()

CARBHelpApp::CARBHelpApp()
{
}

CARBHelpApp theApp;

BOOL CARBHelpApp::InitInstance()
{
	CWinApp::InitInstance();
	SetRegistryKey(_T("dcon Software"));

	CARBHelpCommandLine cmdInfo;
	ParseCommandLine(cmdInfo);

	CDlgARBHelp dlg(cmdInfo.m_Decode);
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}
