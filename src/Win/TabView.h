#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 */

#include <vector>
#include "Splitter.h"

class CTabView : public CCtrlView
{
	DECLARE_DYNCREATE(CTabView)

public:
	CTabView();
	virtual ~CTabView();

	CAgilityBookDoc* GetDocument() const;
	CTabCtrl const& GetTabCtrl() const	{return *reinterpret_cast<CTabCtrl const*>(this);}
	CTabCtrl& GetTabCtrl()				{return *reinterpret_cast<CTabCtrl*>(this);}
	int GetItemCount() const			{return GetTabCtrl().GetItemCount();}
	int GetCurSel() const				{return GetTabCtrl().GetCurSel();}
	void SetCurSel(int index);

// Overrides
	//{{AFX_VIRTUAL(CTabView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
protected:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void SetActiveView();

	CSplitter m_splitterRuns;
	CSplitter m_splitterCal;
	std::vector<CWnd*> m_Panes;
	CView* m_pLastFocusRuns; // Last view with focus in splitter window.
	CView* m_pLastFocusCal; // Last view with focus in splitter window.

	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CTabView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
};

#ifndef _DEBUG  // debug version in TabView.cpp
inline CAgilityBookDoc* CTabView::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
