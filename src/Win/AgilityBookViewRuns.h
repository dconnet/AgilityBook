#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewRuns class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-08-30 DRC Added the ability to copy entries to the clipboard.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include <vector>
#include "CommonView.h"
#include "DlgFind.h"
#include "ListCtrl.h"
class CAgilityBookDoc;
class CAgilityBookViewRuns;
class CAgilityBookViewRunsData;

class CFindRuns : public IFindCallback
{
public:
	CFindRuns(CAgilityBookViewRuns* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search() const;
private:
	CAgilityBookViewRuns* m_pView;
};

class CAgilityBookViewRuns : public CListView2, public ICommonView
{
	friend class CAgilityBookViewRunsData;
	friend class CFindRuns;
protected: // create from serialization only
	CAgilityBookViewRuns();
	DECLARE_DYNCREATE(CAgilityBookViewRuns)

public:
	CAgilityBookDoc* GetDocument() const;

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;

private:
	CAgilityBookViewRunsData* GetItemData(int index) const;
	void SetupColumns();
	void LoadData();

protected:
	virtual void GetPrintLine(int nItem, CStringArray& line); // CListView2 override
	//{{AFX_VIRTUAL(CAgilityBookViewRuns)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookViewRuns();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	std::vector<int> m_Columns;
	bool m_bSuppressSelect;
	CFindRuns m_Callback;

// Generated message map functions
protected:
	//{{AFX_MSG(CAgilityBookViewRuns)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindNext();
	afx_msg void OnEditFindPrevious();
	afx_msg void OnUpdateAgilityEditRun(CCmdUI* pCmdUI);
	afx_msg void OnAgilityEditRun();
	afx_msg void OnUpdateAgilityNewTitle(CCmdUI* pCmdUI);
	afx_msg void OnAgilityNewTitle();
	afx_msg void OnUpdateAgilityNewTrial(CCmdUI* pCmdUI);
	afx_msg void OnAgilityNewTrial();
	afx_msg void OnUpdateAgilityNewRun(CCmdUI* pCmdUI);
	afx_msg void OnAgilityNewRun();
	afx_msg void OnUpdateAgilityDeleteRun(CCmdUI* pCmdUI);
	afx_msg void OnAgilityDeleteRun();
	afx_msg void OnViewCustomize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewRuns::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
