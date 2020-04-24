#pragma once

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
 * 2013-08-23 Created
 */

#include "LibwxARBWin.h"

#ifdef WIN32
#include <shobjidl.h>

#else
// Allow non-windows compilation.
typedef void* HWND;
typedef enum
{
	TBPF_NOPROGRESS = 0,
	TBPF_INDETERMINATE = 0x1,
	TBPF_NORMAL = 0x2,
	TBPF_ERROR = 0x4,
	TBPF_PAUSED = 0x8
} TBPFLAG;
typedef unsigned int ULONGLONG;
#endif

class CTaskbarProgress;
typedef std::shared_ptr<CTaskbarProgress> CTaskbarProgressPtr;

class ARBWIN_API CTaskbarProgress
{
	DECLARE_NO_COPY_IMPLEMENTED(CTaskbarProgress)
protected:
	CTaskbarProgress()
	{
	}

public:
	static CTaskbarProgressPtr Get(HWND hwnd);
	virtual ~CTaskbarProgress();

	virtual bool SetProgressState(TBPFLAG tbpFlags) = 0;
	virtual bool SetProgressValue(ULONGLONG ullCompleted, ULONGLONG ullTotal) = 0;
};
