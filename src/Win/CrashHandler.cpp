/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief Trap information about unhandled exceptions.
 * @author David Connet
 *
 * See Bugslayer by John Robbins in MSJ Aug 1998.
 *
 * Revision History
 * @li 2004-10-22 DRC Created
 */

#include "stdafx.h"
#include "CrashHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static bool s_bLetItCrash = false;

static LPCTSTR GetFaultReason(LPEXCEPTION_POINTERS ExceptionInfo)
{
	static char const* pReason = "Yikes! We're goin' down!";
	return pReason;
}

static LPCTSTR GetFirstStackTraceString(LPEXCEPTION_POINTERS ExceptionInfo)
{
	return NULL;
}

static LPCTSTR GetNextStackTraceString(LPEXCEPTION_POINTERS ExceptionInfo)
{
	return NULL;
}

/**
 * This function is passed to SetUnhandledExceptionFilter().
 * Note, this function will NOT be called if you are running under a debugger.
 */
LONG WINAPI CrashHandler(LPEXCEPTION_POINTERS ExceptionInfo)
{
	LPCTSTR pReason = GetFaultReason(ExceptionInfo);
	AfxMessageBox(pReason, MB_ICONSTOP);
	for (LPCTSTR pStack = GetFirstStackTraceString(ExceptionInfo);
		pStack;
		pStack = GetNextStackTraceString(ExceptionInfo))
	{
		// Dump info
	}
	if (s_bLetItCrash)
		return EXCEPTION_CONTINUE_SEARCH;
	else
		return EXCEPTION_EXECUTE_HANDLER;
}
