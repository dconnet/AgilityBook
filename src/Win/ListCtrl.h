#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 */

/////////////////////////////////////////////////////////////////////////////

class CListCtrl2 : public CListCtrl
{
// Construction
public:
	CListCtrl2();
	int GetSelection();
	void SetSelection(int index, bool bEnsureVisible = false);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrl2)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrl2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrl2)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CListView2 : public CListView
{
	DECLARE_DYNCREATE(CListView2)
// Construction
public:
	CListView2();
	int GetSelection();
	void SetSelection(int index, bool bEnsureVisible = false);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListView2)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListView2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListView2)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
