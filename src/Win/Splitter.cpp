/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief Propagate messages into all views.
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "Splitter.h"

#include "AgilityBookTree.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewPoints.h"
#include "AgilityBookViewRuns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitter

IMPLEMENT_DYNAMIC(CSplitter, CSplitterWnd)

BEGIN_MESSAGE_MAP(CSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSplitter::CSplitter()
{
}


CSplitter::~CSplitter()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers

// Route messages into ALL child views, not just the active one.
BOOL CSplitter::OnCmdMsg(
		UINT nID,
		int nCode,
		void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo)
{
	int nRows = GetRowCount();
	int nCols = GetColumnCount();
	for (int row = 0; row < nRows; ++row)
	{
		for (int col = 0; col < nCols; ++col)
		{
			CWnd* pane = GetPane(row, col);
			if (pane && pane->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
		}
	}
	return CSplitterWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
