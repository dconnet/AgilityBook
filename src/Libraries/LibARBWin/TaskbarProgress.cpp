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
 * 2018-10-11 Moved to Win LibARBWin
 * 2013-09-12 Remove use of ATL (can now use VC-express to compile)
 * 2013-08-23 Created
 */

#include "stdafx.h"
#include "LibARBWin/TaskbarProgress.h"

#include "ARBCommon/ARBMisc.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

#ifdef WIN32

class CTaskbarProgressImpl : public CTaskbarProgress
{
	CTaskbarProgressImpl() = delete;
	DECLARE_NO_COPY_IMPLEMENTED(CTaskbarProgressImpl)
public:
	CTaskbarProgressImpl(HWND hwnd)
		: m_hWnd(hwnd)
		, m_pTaskbarList(nullptr)
	{
		if (IsWin7OrBetter())
			::CoCreateInstance(
				CLSID_TaskbarList,
				nullptr,
				CLSCTX_ALL,
				__uuidof(ITaskbarList3),
				(void**)&m_pTaskbarList);
	}

	~CTaskbarProgressImpl()
	{
		if (m_pTaskbarList)
			m_pTaskbarList->Release();
	}

	bool SetProgressState(TBPFLAG tbpFlags) override
	{
		return (m_pTaskbarList && SUCCEEDED(m_pTaskbarList->SetProgressState(m_hWnd, tbpFlags)));
	}
	bool SetProgressValue(ULONGLONG ullCompleted, ULONGLONG ullTotal) override
	{
		return (m_pTaskbarList && SUCCEEDED(m_pTaskbarList->SetProgressValue(m_hWnd, ullCompleted, ullTotal)));
	}

	HWND m_hWnd;
	ITaskbarList3* m_pTaskbarList;
};

#endif // WIN32

/////////////////////////////////////////////////////////////////////////////

CTaskbarProgressPtr CTaskbarProgress::Get(HWND hwnd)
{
#ifdef WIN32
	CTaskbarProgressPtr pTaskbar = std::make_shared<CTaskbarProgressImpl>(hwnd);
	return pTaskbar;
#else
	return nullptr;
#endif
}


CTaskbarProgress::~CTaskbarProgress()
{
}
