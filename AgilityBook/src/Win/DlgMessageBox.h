#pragma once

/*
 * Copyright � 2004 David Connet. All Rights Reserved.
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
 * @brief Message box with a details button.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-11 DRC Created
 */

/**
 * Callback to support the Details button.
 */
class IMessageBoxCallback
{
public:
	virtual ~IMessageBoxCallback();
	virtual void OnDetails(CWnd* pParent) = 0;
};

extern int AfxMessageBox2(UINT inIDText,
	UINT inType = MB_OK,
	IMessageBoxCallback* inCallback = NULL,
	CWnd* inParent = NULL);
extern int AfxMessageBox2(CString const& inText,
	UINT inType = MB_OK,
	IMessageBoxCallback* inCallback = NULL,
	CWnd* inParent = NULL);
