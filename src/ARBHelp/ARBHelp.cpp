// ARBHelp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DlgARBHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "arbzlib.lib")

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
