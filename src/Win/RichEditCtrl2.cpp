/*
 * Copyright © 2005-2006 David Connet. All Rights Reserved.
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
 * @brief Autodetect URLs and open a window when clicked.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-30 DRC Created
 */

#include "stdafx.h"
#include <shellapi.h>
#include "RichEditCtrl2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrl2 control

CRichEditCtrl2::CRichEditCtrl2()
{
}

BEGIN_MESSAGE_MAP(CRichEditCtrl2, CRichEditCtrl)
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
END_MESSAGE_MAP()

BOOL CRichEditCtrl2::Create(
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID)
{
	if (!CRichEditCtrl::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;
	Setup();
	return TRUE;
}

BOOL CRichEditCtrl2::CreateEx(
		DWORD dwExStyle,
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID)
{
	if (!CRichEditCtrl::CreateEx(dwExStyle, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	Setup();
	return TRUE;
}

void CRichEditCtrl2::PreSubclassWindow()
{
	CRichEditCtrl::PreSubclassWindow();
	Setup();
}

void CRichEditCtrl2::Setup()
{
	// A readonly richedit has a white background.
	if (GetStyle() & ES_READONLY)
		SetBackgroundColor(FALSE, ::GetSysColor(COLOR_3DFACE));
	SetEventMask(GetEventMask() | ENM_LINK);
	SetAutoURLDetect(TRUE);
}

void CRichEditCtrl2::OnEnLink(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	ENLINK* pEnLink = reinterpret_cast<ENLINK*>(pNMHDR);
	*pResult = 0;

	switch (pEnLink->msg)
	{
	case WM_LBUTTONDOWN:
		{
			CWaitCursor WaitCursor;
			CHARRANGE CharRange;
			GetSel(CharRange);
			SetSel(pEnLink->chrg);
			CString URL = GetSelText();
			SetSel(CharRange);
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(sei));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_FLAG_DDEWAIT;
			sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
			//sei.lpVerb = NULL;
			//sei.lpVerb = _T("opennew");
			sei.lpVerb = _T("open");
			//sei.lpVerb = _T("edit");
			sei.lpFile = (LPCTSTR)URL;
			sei.lpParameters = NULL;
			sei.lpDirectory = NULL;
			sei.nShow = SW_SHOWNORMAL;
			ShellExecuteEx(&sei);
			*pResult = 1;
		}
		break;

	case WM_LBUTTONUP:
		*pResult = 1;
		break;

	default:
		break;
	}
}
