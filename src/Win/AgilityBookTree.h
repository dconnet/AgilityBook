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
 * @brief interface of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-28 DRC Implemented Find/FindNext.
 * @li 2003-08-30 DRC Added GetPrintLine to allow future differences between
 *                    printing and viewing (already in the listctrl)
 * @li 2003-08-28 DRC Added printing.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include <vector>
#include "CommonView.h"
#include "DlgFind.h"
class ARBBase;
class ARBDog;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookDoc;
class CAgilityBookTree;
class CAgilityBookTreeData;
struct CTreePrintData;
struct CVenueFilter;

class CFindTree : public IFindCallback
{
public:
	CFindTree(CAgilityBookTree* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	void FillTree(HTREEITEM hItem) const;
	HTREEITEM GetNextItem() const;
	CAgilityBookTree* m_pView;
	mutable std::vector<HTREEITEM> m_Items;
	mutable std::vector<HTREEITEM>::const_iterator m_Iter;
};

class CAgilityBookTree : public CTreeView, public ICommonView
{
	friend class CFindTree;
	friend class CAgilityBookTreeData;
protected: // create from serialization only
	CAgilityBookTree();
	DECLARE_DYNCREATE(CAgilityBookTree)

// Attributes
public:
	CAgilityBookDoc* GetDocument() const;
	CAgilityBookTreeData* GetCurrentTreeItem() const;
	CAgilityBookTreeData* FindData(HTREEITEM hItem, ARBBase const* pBase) const;
	CAgilityBookTreeData* FindData(HTREEITEM hItem, ARBDog const* pDog) const;
	CAgilityBookTreeData* FindData(HTREEITEM hItem, ARBDogTrial const* pTrial) const;
	CAgilityBookTreeData* FindData(HTREEITEM hItem, ARBDogRun const* pRun) const;
	HTREEITEM InsertDog(ARBDog* pDog, bool bSelect = false);
	HTREEITEM InsertTrial(ARBDogTrial* pTrial, HTREEITEM hParent);
	HTREEITEM InsertRun(ARBDogTrial* pTrial, ARBDogRun* pRun, HTREEITEM hParent);
	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;
	virtual bool GetMessage2(CString& msg) const;

// Overrides
	//{{AFX_VIRTUAL(CAgilityBookTree)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// Only CAgilityBookTreeData should be calling this.
	CString GetPrintLine(HTREEITEM hItem) const;
private:
	void LoadData();
	CAgilityBookTreeData* GetItemData(HTREEITEM hItem) const;
	void PrintLine(CDC* pDC, CTreePrintData *pData, HTREEITEM hItem, int indent) const;
	std::vector<int> m_Columns[3];
	bool m_bReset;
	bool m_bSuppressSelect;
	CFindTree m_Callback;
	ARBDog* m_pDog;

// Generated message map functions
protected:
	//{{AFX_MSG(CAgilityBookTree)
	afx_msg void OnDestroy();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateDogCmd(CCmdUI* pCmdUI);
	afx_msg BOOL OnDogCmd(UINT id);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindNext();
	afx_msg void OnEditFindPrevious();
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	afx_msg void OnExpand();
	afx_msg void OnUpdateExpandAll(CCmdUI* pCmdUI);
	afx_msg void OnExpandAll();
	afx_msg void OnUpdateCollapse(CCmdUI* pCmdUI);
	afx_msg void OnCollapse();
	afx_msg void OnUpdateCollapseAll(CCmdUI* pCmdUI);
	afx_msg void OnCollapseAll();
	afx_msg void OnViewCustomize();
	//}}AFX_MSG
	afx_msg LRESULT OnDelayedMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookTree::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
