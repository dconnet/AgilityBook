/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Display progress in Win7 taskbar
 * @author David Connet
 *
 * Revision History
 * @li 2013-09-12 DRC Remove use of ATL (can now use VC-express to compile)
 * @li 2013-08-23 DRC Created
 */

#include "stdafx.h"
#include "TaskbarProgress.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


#ifdef WIN32

bool CheckOS(DWORD dwMajor, DWORD dwMinor, int op)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajor;
	osvi.dwMinorVersion = dwMinor;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

#pragma warning (push)
// warning C4244: 'argument' : conversion from 'int' to 'BYTE', possible loss of data
#pragma warning (disable : 4244)
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
#pragma warning (pop)

	return !!VerifyVersionInfo(&osvi, 
			VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
			dwlConditionMask);
}


inline bool IsWin7OrBetter()
{
	return CheckOS(6, 1, VER_GREATER_EQUAL);
}


class CTaskbarProgressImpl : public CTaskbarProgress
{
public:
	CTaskbarProgressImpl(HWND hwnd)
		: m_hWnd(hwnd)
		, m_pTaskbarList(NULL)
	{
		if (IsWin7OrBetter())
			::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, __uuidof(ITaskbarList3), (void**)&m_pTaskbarList);
	}

	virtual ~CTaskbarProgressImpl()
	{
		if (m_pTaskbarList)
			m_pTaskbarList->Release();
	}
	
	virtual bool SetProgressState(TBPFLAG tbpFlags)
	{
		return (m_pTaskbarList && SUCCEEDED(m_pTaskbarList->SetProgressState(m_hWnd, tbpFlags)));
	}
	virtual bool SetProgressValue(ULONGLONG ullCompleted, ULONGLONG ullTotal)
	{
		return (m_pTaskbarList && SUCCEEDED(m_pTaskbarList->SetProgressValue(m_hWnd, ullCompleted, ullTotal)));
	}

	ITaskbarList3* m_pTaskbarList;
	HWND m_hWnd;
};

#endif // WIN32

/////////////////////////////////////////////////////////////////////////////

CTaskbarProgress* CTaskbarProgress::Get(HWND hwnd)
{
#ifdef WIN32
	CTaskbarProgressImpl* pTaskbar = new CTaskbarProgressImpl(hwnd);
	return pTaskbar;
#else
	return NULL;
#endif
}


CTaskbarProgress::~CTaskbarProgress()
{
}
