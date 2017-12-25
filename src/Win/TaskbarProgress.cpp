/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Display progress in Win7 taskbar
 * @author David Connet
 *
 * Revision History
 * 2013-09-12 Remove use of ATL (can now use VC-express to compile)
 * 2013-08-23 Created
 */

#include "stdafx.h"
#include "TaskbarProgress.h"

#include "ARBCommon/ARBMisc.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

#ifdef WIN32

class CTaskbarProgressImpl : public CTaskbarProgress
{
public:
	CTaskbarProgressImpl(HWND hwnd)
		: m_hWnd(hwnd)
		, m_pTaskbarList(nullptr)
	{
		if (IsWin7OrBetter())
			::CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, __uuidof(ITaskbarList3), (void**)&m_pTaskbarList);
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
	return nullptr;
#endif
}


CTaskbarProgress::~CTaskbarProgress()
{
}
