/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief Clipboard functions for copying data to/from clipboard.
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

#include "stdafx.h"
#include <sstream>
#include "AgilityBook.h"
#include "ClipBoard.h"

#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

bool CopyDataToClipboard(
		UINT clpFmt,
		Element const& tree,
		CString const& txtForm)
{
	if (!AfxGetMainWnd()->OpenClipboard())
		return false;
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return false;
	}
	std::ostringstream out;
	tree.SaveXML(out);
	std::string data = out.str();

	// alloc mem block & copy text in
	HGLOBAL temp = GlobalAlloc(GHND, data.length()+1);
	if (NULL != temp)
	{
		LPTSTR str = reinterpret_cast<LPTSTR>(GlobalLock(temp));
		lstrcpy(str, data.c_str());
		GlobalUnlock(reinterpret_cast<void*>(temp));
		// send data to clipbard
		SetClipboardData(clpFmt, temp);

		if (!txtForm.IsEmpty())
		{
			temp = GlobalAlloc(GHND, txtForm.GetLength()+1);
			if (NULL != temp)
			{
				LPTSTR str = reinterpret_cast<LPTSTR>(GlobalLock(temp));
				lstrcpy(str, (LPCTSTR)txtForm);
				GlobalUnlock(reinterpret_cast<void*>(temp));
				// send data to clipbard
				SetClipboardData(CF_TEXT, temp);
			}
		}
	}

	CloseClipboard();
	return true;
}

bool GetDataFromClipboard(
		UINT clpFmt,
		Element& tree)
{
	bool bOk = false;
	if (IsClipboardFormatAvailable(clpFmt))
	{
		if (!AfxGetMainWnd()->OpenClipboard())
			return false;
		tree.clear();
		HANDLE hData = GetClipboardData(clpFmt);
		CString data(reinterpret_cast<LPCTSTR>(GlobalLock(hData)));
		GlobalUnlock(hData);
		CloseClipboard();
		std::string err;
		bOk = tree.LoadXMLBuffer((LPCSTR)data, data.GetLength(), err);
		if (!bOk && 0 < err.length())
		{
			AfxMessageBox(err.c_str(), MB_ICONEXCLAMATION);
		}
	}
	return bOk;
}
