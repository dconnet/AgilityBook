#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewCalendarList class
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include "CommonView.h"
#include "ListCtrl.h"
class ARBCalendar;
class CAgilityBookDoc;

class CAgilityBookViewCalendarList : public CListView2, public ICommonView
{
	friend class CAgilityBookViewCalendarData;
protected: // create from serialization only
	CAgilityBookViewCalendarList();
	DECLARE_DYNCREATE(CAgilityBookViewCalendarList)

public:
	CAgilityBookDoc* GetDocument() const;

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;

private:
	CAgilityBookViewCalendarData* GetItemData(int index) const;
	void LoadData();
	CImageList m_ImageList;
	int m_imgEmpty;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;

protected:
	virtual void GetPrintLine(int nItem, CStringArray& line); // CListView2 override
	//{{AFX_VIRTUAL(CAgilityBookViewCalendarList)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookViewCalendarList();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool m_bSuppressSelect;

// Generated message map functions
protected:
	//{{AFX_MSG(CAgilityBookViewCalendarList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateCalendarCreateEntry(CCmdUI* pCmdUI);
	afx_msg void OnCalendarCreateEntry();
	afx_msg void OnUpdateCalendarEdit(CCmdUI* pCmdUI);
	afx_msg void OnCalendarEdit();
	afx_msg void OnCalendarNew();
	afx_msg void OnUpdateCalendarDelete(CCmdUI* pCmdUI);
	afx_msg void OnCalendarDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewCalendarList::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
